/*
 * File: time_handler.h
 * Project: logger
 * Author: Anton Ihonen, anton.ihonen@gmail.com
 *
 * This module contains a time handler class
 * that can be used to fetch information about
 * the current time.
 *
 * Copyright (C) 2019. Anton Ihonen
 */

#ifndef TIME_HANDLER_H
#define TIME_HANDLER_H

#include "error.h"
#include <stdbool.h>
#include <time.h>

/* Determines whether thandler_get_wday gets the weekday as
mon, Mon, MON, monday, Monday or MONDAY. */
typedef enum { __WD_SHORT_SMALL, __WD_SHORT_FIRST_CAP, __WD_SHORT_ALL_CAPS,
	__WD_LONG_SMALL, __WD_LONG_FIRST_CAP, __WD_LONG_ALL_CAPS } __WDAY_FORMAT;

/* Determines whether thandler_get_mname gets the month name as
jan, Jan, JAN, january, January or JANUARY. */
typedef enum { __MN_SHORT_SMALL, __MN_SHORT_FIRST_CAP, __MN_SHORT_ALL_CAPS,
	__MN_LONG_SMALL, __MN_LONG_FIRST_CAP, __MN_LONG_ALL_CAPS} __MNAME_FORMAT;

/* Defines the properties of a time handler object. */
typedef struct
{
	/* Contains the result of the last call to localtime()
	received upon the last call to thandler_fetch_ltime(). */
	struct tm* _last_fetch;
	/* For testing purposes: */
	bool _is_testing;
} thandler_t;

LOG_ERROR
thandler_init(thandler_t* thandler);

LOG_ERROR
thandler_close(thandler_t* thandler);

LOG_ERROR
thandler_fetch_ltime(thandler_t* thandler);

LOG_ERROR
thandler_is_legal_state(thandler_t* thandler, bool* was_fetched);

LOG_ERROR
thandler_get_year(thandler_t* thandler, char* year);

LOG_ERROR
thandler_get_month(thandler_t* thandler, char* month);

LOG_ERROR
thandler_get_mday(thandler_t* thandler, char* mday);

LOG_ERROR
thandler_get_hours(thandler_t* thandler, char* hours);

LOG_ERROR
thandler_get_mins(thandler_t* thandler, char* mins);

LOG_ERROR
thandler_get_secs(thandler_t* thandler, char* secs);

LOG_ERROR
thandler_get_mname(thandler_t* thandler, char* mname, __MNAME_FORMAT format);

LOG_ERROR
thandler_get_wday(thandler_t* thandler, char* wday, __WDAY_FORMAT format);

#endif /* TIME_HANDLER_H */
