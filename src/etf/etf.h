// hehe pragma once go brrrrrr
#pragma once

#include <stdlib.h>
#include <stddef.h>
#include <limits.h>
#include <string.h>

#include "const.h"

typedef struct etf_buf {
    char * buf;
    size_t len;
    size_t alloc_sz
} etf_buf;



typedef struct etf {
    etf_buf data;
    int cur;
} etf;

#define ETF_APPEND(des, src, length) \
    return etf_buf_write(des, (const char *)src, length)

int etf_buf_write(etf_buf * des, const char * src, size_t w_length);

int etf_append_version(etf_buf * des);

int etf_append_nil(etf_buf * des);

int etf_append_false(etf_buf * des);

int etf_append_true(etf_buf * des);

int etf_append_small_int(etf_buf * des, unsigned char data);

int etf_append_int(etf_buf * des, int32_t data);