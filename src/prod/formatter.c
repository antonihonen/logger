/*
 * File: formatter.c
 * Project: logger
 * Author: Anton Ihonen, anton.ihonen@gmail.com
 *
 * Copyright (C) 2019. Anton Ihonen
 */

#include "formatter.h"
#include "string_util.h"
#include "time_handler.h"
#include <assert.h>
#include <malloc.h>
#include <stdbool.h>
#include <string.h>

/* Helper function prototypes. */

static inline const __FM_HANDLER const get_fm_handler(__FM_ID id);
void __fm_as_str(const char*, char*, size_t* const);
__FM_ID __identify_fm(const char* const, size_t* const);
void __expand_fm(char*, char*, thandler_t*, char*, LOG_LEVEL, size_t*, size_t*);
void __format_str(char*, char*, thandler_t*, char*, LOG_LEVEL);
static bool __is_valid_fn_form(const char*);
static bool __is_valid_e_form(const char*);

 /* File name formatter functions. */

fn_format_t*
fnf_init(char* format)
{
	assert(format);
	fn_format_t* new_fnf = malloc(sizeof(fn_format_t));
	thandler_t* new_th = th_init();

	if (new_fnf && new_th
		&& fnf_set_format(new_fnf, format))
	{
		new_fnf->_th = new_th;
		return new_fnf;
	}

	/* Memory allocation failed or format was invalid. Clean up
	and return NULL. */
	if (new_fnf) { free(new_fnf); }
	return NULL;
}

bool
fnf_set_format(fn_format_t* fnf, char* format)
{
	assert(fnf);
	assert(format);
	if (!__is_valid_fn_form(format)) { return false; }

	strcpy(fnf->_form, format);
	__clear_str(fnf->_exp_form);

	return true;
}

void
fnf_format(fn_format_t* fnf, char* formatted_filename)
{
	assert(fnf);
	assert(formatted_filename);

	__format_str(fnf->_form, formatted_filename,
		fnf->_th, NULL, __L_NO_LEVEL);
}

size_t
fnf_fn_max_len(fn_format_t* fnf)
{
	/* TODO: Implement. */
	return 0;
}

void
fnf_close(fn_format_t* fnf)
{
	assert(fnf);
	th_close(fnf->_th);
	free(fnf);
}

/* Entry formatter functions. */

e_format_t*
ef_init(char* format)
{
	assert(format);
	e_format_t* new_ef = malloc(sizeof(e_format_t));
	thandler_t* new_th = th_init();

	if (new_ef && new_th && ef_set_format(new_ef, format))
	{
		new_ef->_th = new_th;
		return new_ef;
	}

	/* Memory allocation failed or format was invalid. Clean up
	and return NULL. */
	if (new_ef) { free(new_ef); }
	return NULL;
}

bool
ef_set_format(e_format_t* ef, const char* const format)
{
	assert(ef); assert(format);

	if (!__is_valid_e_form(format)) { return false; }

	strcpy(ef->_form, format);
	__clear_str(ef->_exp_form);
	return true;
}

void
ef_format(e_format_t* ef, char* msg, LOG_LEVEL lvl, char* formatted_entry)
{
	assert(ef); assert(msg); assert(formatted_entry);

	__format_str(ef->_form, formatted_entry,
		ef->_th, msg, lvl);
}

void
ef_close(e_format_t* ef)
{
	assert(ef);
	th_close(ef->_th);
	free(ef);
}

/* Helper functions. */

/* Returns a pointer to the macro handler corresponding with id. */
const __FM_HANDLER const get_fm_handler(__FM_ID id)
{
	return __FM_TABLE[id].handler;
}

/* Checks that macro begins with __FM_BEGIN_INDIC
followed by __FM_LEFT_DELIM and ends with __FM_RIGHT_DELIM,
then writes the string between the two delimiters to dest,
adding the null terminator. Macro_len will indicate how
many characters there were from __FM_BEGIN_INDIC to
__FM_RIGHT_DELIM (inclusive) - the formatter can jump that many
characters in the format string since those characters
have been handled. If the macro doesn't conform to the rule
stated above, only __FM_BEGIN_INDIC will be written into
dest (and macro_len will be 1).
*/
void
__fm_as_str(const char* macro, char* dest, size_t* const macro_len)
{
	assert(macro); assert(dest); assert(macro_len);
	assert(*macro == __FM_BEGIN_INDIC);

	/* Save the initial state of macro_as_str. */
	char* macro_as_str_initial = dest;

	/* Indicates if __FM_RIGHT_DELIM has been encountered in the macro
	string. */
	bool has_ended = false;
	/* Indicates if __FM_LEFT_DELIM was encountered as the second char. */
	bool has_begun = false;
	/* Indicates whether the macro conforms to the format described in
	format_macro.h, except that in terms of this function the macro
	body doesn't need to be a valid one. */
	bool has_valid_format = true;
	*macro_len = 0;

	/* Read the macro string one character at a time until the macro
	either ends or is found to be invalid. */
	while (!has_ended)
	{
		switch (*macro)
		{
		case __FM_LEFT_DELIM:
			/* Only one __FM_LEFT_DELIM can be contained
			by a valid macro sequence and it must
			be the second character. */
			if (has_begun || *macro_len != 1)
			{
				has_valid_format = false;
				has_ended = true;
			}
			has_begun = true;
			break;
		case __FM_RIGHT_DELIM:
			/* __FM_LEFT_DELIM must come before __FM_RIGHT_DELIM */
			if (!has_begun) { has_valid_format = false; }
			has_ended = true;
			break;
		case __FM_BEGIN_INDIC:
			/* A valid macro cannot contain __FM_BEGIN_INDIC
			unless it's the first char. */
			if (*macro_len > 0)
			{
				has_valid_format = false;
				has_ended = true;
			}
			break;
		case '\0':
			/* End of macro string. */
			if (!has_begun || !has_ended) { has_valid_format = false; }
			has_ended = true;
			break;
		default:
			/* Only the characters forming the body of the macro
			is written into the macro_as_str buffer. */
			*dest = *macro;
			++dest;
			break;
		}

		if (!has_valid_format)
		{
			/* Invalid format so write __FM_BEGIN_INDIC in the first
			char of the macro string. */
			*macro_as_str_initial = __FM_BEGIN_INDIC;
			dest = macro_as_str_initial + 1;
			/* Set length to 0 as if the last character,
			which caused the macro to be found invalid,
			was the first one. */
			*macro_len = 0;
		}

		++*macro_len;

		if (has_ended)
		{
			/* Finish the macro string by appending null. */
			*dest = '\0';
		}

		/* Move on to the next char of the macro. */
		++macro;
	}
}

/* Figures what the macro indicated by macro_start is
and writes the corresponding __FM_ID to macro_id.
Macro_form_len will contain the length of the macro format,
i.e. how many characters the macro occupies in the
format string, not including null terminator.
*/
__FM_ID
__identify_fm(const char* const macro, size_t* const macro_len)
{
	assert(macro); assert(macro_len);
	assert(*macro == __FM_BEGIN_INDIC);

	/* Get the macro as a string as well as the number of
	characters the macro sequence consists of. */
	char macro_as_str[__MAX_FM_S_LEN];
	__fm_as_str(macro, macro_as_str, macro_len);

	if (macro_len == 1 && strncmp(macro_as_str, "%", 1) == 0)
	{
		return __FM_NO_MACRO;
	}

	/* Iterate through __FORMAT_MACROS and see if one of them
	matches. The indexes of the macro strings in __FORMAT_MACROS
	match their respective __FM_IDs so i == __FM_ID. */
	for (size_t i = 0; i < __FM_COUNT; ++i)
	{
		if (strcmp(macro_as_str, __FM_TABLE[i].str) == 0)
		{
			/* Match. */
			return i;
		}
	}

	/* No match. */
	return __FM_NO_MACRO;
}

/* Expands the format macro in the macro string
and writes the result to dest without adding the
null terminator. Thandler will be used to expand
time-related macros. Msg contains the log message
entered by the user, if any. Lvl contains the
log level of the message, if any. Macro_len
will contain the number of characters the macro
format consisted of, exp_macro_len the same but
for the expanded macro. If the macro format is invalid,
only __FM_BEGIN_INDIC is written to dest (macro_len
and exp_macro_len will be 1). */
void
__expand_fm(char* macro, char* dest, thandler_t* th,
	char* msg, LOG_LEVEL lvl, size_t* macro_len, size_t* exp_macro_len)
{
	assert(macro); assert(dest); assert(macro_len);
	assert(*macro == __FM_BEGIN_INDIC);
	/* Local time must have been fetched prior to call to this function. */
	assert(th_has_legal_state(th));

	/* Figure out which macro is in question. */
	__FM_ID macro_id = __identify_fm(macro, macro_len);

	if (macro_id != __FM_NO_MACRO)
	{
		/* Valid macro detected. Get and call the handler, it will
		expand the macro. */
		get_fm_handler(macro_id)(th, dest, lvl, msg, exp_macro_len);
	}
	else
	{
		/* The "macro" was incomplete. Don't expand. */
		*macro_len = 0;
		*exp_macro_len = 0;
	}
}

/* Expands all macros in the format buffer, writing the expanded
string to dest. Thandler will be used to expand time-related
macros. Msg contains the log message entered by the user,
if any. Lvl contains the log level of the message, if any.
Msg and lvl may be omitted when this function is used to
format a file name, since they are meaningless in that context. */
void
__format_str(char* format, char* dest, thandler_t* th, char* msg, LOG_LEVEL lvl)
{
	assert(format); assert(dest); assert(th);
	/* If message is given, logging level must also be given (this function
	was called by the entry formatter) - if not, logging level must not be
	given either (called by filename formatter). */
	assert((msg && lvl != __L_NO_LEVEL) || (!msg && lvl == __L_NO_LEVEL));

	/* Fetch the local time for time-related macros so they all
	refer to the same point in time. */
	th_fetch_ltime(th);

	while (*format != '\0')
	{
		if (*format != __FM_BEGIN_INDIC)
		{
			/* No macro begin encountered. */
			*dest = *format;
			++dest;
			++format;
		}
		else
		{
			/* Macro begin encountered. */

			/* Unexpanded macro length. */
			size_t macro_len = 0;
			/* Expanded macro length. */
			size_t exp_macro_len = 0;
			/* Expand the macro. */
			__expand_fm(format, dest, th, msg, lvl, &macro_len, &exp_macro_len);

			if (macro_len == 0 && exp_macro_len == 0)
			{
				/* False alarm, no valid macro found. */
				*dest = *format;
				macro_len = 1;
				exp_macro_len = 1;
			}
			format += macro_len;
			dest += exp_macro_len;
		}
	}
	/* Add the null terminator which was excluded in the
	while loop. */
	assert(*format == '\0');
	*dest = *format;
}

/* Returns true if format is valid, i.e. contains no illegal
macros (some macros can never be in the file name). */
bool __is_valid_fn_form(const char* format)
{
	assert(format);
	size_t max_len = 0;

	while (*format != '\0')
	{
		size_t macro_len = 1;
		size_t exp_macro_len = 1;
		if (*format == __FM_BEGIN_INDIC)
		{
			__FM_ID id = __identify_fm(format, &macro_len);
			if (id != __FM_NO_MACRO) { exp_macro_len = __FM_TABLE[id].max_len; }
			switch (id)
			{
			case __FM_MSG:
			case __FM_LVL_N:
			case __FM_LVL_F:
			case __FM_LVL_A:
				return false;
			}
		}
		format += macro_len;
		max_len += exp_macro_len;
	}

	if (max_len < __MAX_FILENAME_SIZE - 1) { return true; }

	return false;
}

/* Returns true if format is valid, i.e. contains no illegal macros.
Currently no restrictions. */
bool __is_valid_e_form(const char* format)
{
	assert(format);
	size_t max_len = 0;

	while (*format != '\0')
	{
		size_t macro_len = 1;
		size_t exp_macro_len = 1;
		if (*format == __FM_BEGIN_INDIC)
		{
			__FM_ID id = __identify_fm(format, &macro_len);
			if (id != __FM_NO_MACRO) { exp_macro_len = __FM_TABLE[id].max_len; }
		}
		format += macro_len;
		max_len += exp_macro_len;
	}

	if (max_len < __MAX_ENTRY_SIZE - 1) { return true; }

	return false;
}
