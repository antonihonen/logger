/*
 * File: string_util.h
 * Project: logger
 * Author: Anton Ihonen, anton.ihonen@gmail.com
 *
 * This module contains utilities that help
 * with handling strings.
 *
 * Copyright (C) 2019. Anton Ihonen
 */

#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#define __CLEAR_STRING(string) (string[0] = '\0')
#define __IS_EMPTY_STRING(string) (string[0] == '\0')

/* Writes the value of i to str with two digits.
Zero is used as prefix for i < 10, so 1
will turn into "01" and so on. */
void
__two_digit_int_to_str(int i, char* str);

/* Converts alphabetical characters in str to lowercase. */
void
__ascii_str_to_lower(char* str);

#endif /* STRING_UTIL_H */
