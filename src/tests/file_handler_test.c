/*
 * File: file_handler_test.h
 * Project: logger
 * Author: Anton Ihonen, anton.ihonen@gmail.com
 *
 * This module contains tests for the file handler module.
 *
 * Copyright (C) 2019. Anton Ihonen
 */

#include "file_handler_test.h"
#include <assert.h>

fhandler_t* fh;

static void init_test()
{
    printf("  init_test");

    if (fh) { fh_free(fh); }

    fh = fh_init("D:\\logs\\%(year)_%(month)_%(mday)", "log.log", 1024 * 1024 * 8, ROTATE, _IOFBF, 256, 0);
    assert(fh);
    assert(fh->_buf);
    assert(fh->_buf_cap == 256);
    fh_free(fh);

    fh = fh_init("D:\\logs\\%(year)_%(month)_%(mday)", "log.log", 1024 * 1024 * 8, ROTATE, _IOLBF, 0, 0);
    assert(fh);
    assert(fh->_buf);
    assert(fh->_buf_cap == BUFSIZ);
    fh_free(fh);

    fh = fh_init("D:\\logs\\%(year)_%(month)_%(mday)", "log.log", 0, REWRITE, _IONBF, 0, 0);
    assert(fh);
    assert(!fh->_buf);
    assert(fh->_buf_cap == 0);
    fh_free(fh);

    fh = fh_init("D:\\logs\\%(year)_%(month)_%(mday)", "log.log", 0, REWRITE, _IONBF, 64, 0);
    assert(fh);
    assert(!fh->_buf);
    assert(fh->_buf_cap == 0);
    fh_free(fh);

    printf(" -> OK\n");
}

static void fhandler_write_test()
{
    fh = fh_init("D:\\logs\\%(year)%(month)%(mday)", "log.log", 100, ROTATE, _IOLBF, 256, 0);
    format_t* ef = format_init("%(year)-%(month)-%(mday)  %(hour):%(min):%(sec)  %(LVL)  %(MSG)\n",
        _FORMAT_ENTRIES);
    char fmsg[512];
    format_entry(ef, fmsg, "Hello world!", L_CRITICAL);
    fh_fwrite(fh, fmsg);
    fh_free(fh);
    format_free(ef);
}

void run_file_handler_tests(char* test_set_title)
{
    fh = NULL;
    printf(test_set_title);
    init_test();
    fhandler_write_test();
}
