/*
 * File: log_types.h
 * Project: logger
 * Created: 2019-04-15
 * Author: Anton Ihonen, anton.ihonen@gmail.com
 *
 * This file defines various supporting data types and
 * related values used in the project.
 */

#ifndef LOG_ENUMS_H
#define LOG_ENUMS_H

typedef enum {
	TRACE,
	DEBUG,
	INFO,
	WARNING,
	ERROR,
	CRITICAL
} LOG_LEVEL;
#define LOG_VALID_LVL_COUNT 6
static const LOG_LEVEL VALID_LOG_LEVELS[6] =
	{ TRACE, DEBUG, INFO, WARNING, ERROR, CRITICAL };

typedef enum {
	REWRITE,
	ROTATE,
	IGNORE_EXCESS
} LOG_FILE_POLICY;
#define LOG_VALID_FILEPOL_COUNT 3
static const LOG_FILE_POLICY VALID_FILE_POLICIES[LOG_VALID_FILEPOL_COUNT] =
	{ REWRITE, ROTATE, IGNORE_EXCESS };

typedef enum {
	NO_BUFFERING,
	LINE_BUFFERING,
	FULL_BUFFERING
} LOG_BUFFERING_POLICY;
#define LOG_VALID_BUFPOL_COUNT 3
static const LOG_BUFFERING_POLICY VALID_BUF_POLICIES[LOG_VALID_BUFPOL_COUNT] =
	{ NO_BUFFERING, LINE_BUFFERING, FULL_BUFFERING };

typedef enum {
	BAD_LOG,
	BAD_LOG_LEVEL,
	BAD_BUF_POLICY,
	BAD_BUF_SIZE,
	BAD_BUFFER,
	BAD_FILE_POLICY,
	BAD_DIR,
	BAD_FILENAME,
	BAD_FILE_EXT,
	BAD_USER_BUFFER,
	BAD_ENTRY_FORMAT,
	BAD_ENTRY,
	BAD_PATH
} LOG_ERROR;

#endif // LOG_ENUMS_H