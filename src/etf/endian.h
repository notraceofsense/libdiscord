#pragma once

// because yay endianness problems

#include <stdlib.h>
#include <stddef.h>
#include <endian.h>

#define _ETF_STORE_16(des, data) \
    do { uint16_t v = htobe16(data); memcpy(des, &v, 2);} while(0)

#define _ETF_STORE_32(des, data) \
    do { uint32_t v = htobe32(data); memcpy(des, &v, 4);} while(0)

#define _ETF_STORE_64(des, data) \
    do { uint64_t v = htobe64(data); memcpy(des, &v, 8);} while(0)