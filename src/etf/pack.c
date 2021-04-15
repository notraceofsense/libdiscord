#include "etf.h"

int etf_buf_write(etf_buf * des, const char * src, size_t w_length) {
    char* buf = des->buf;
    size_t alloc_sz = des->alloc_sz;
    size_t length = des->len;

    if(w_length + length > alloc_sz) {
        alloc_sz = (w_length + length) * 2;
        buf = (char *)realloc(buf, alloc_sz);
        if(!buf) {
            return -1;
        }
        des->alloc_sz = alloc_sz;
    }

    memcpy(buf + length, src, w_length);
    length += w_length;

    des->buf = buf;
    des->length = length;
    return 0;
}

int etf_append_version(etf_buf * des) {
    static unsigned char src[1] = {FORMAT_VERSION};
    ETF_APPEND(des, src, 1);
}

int etf_append_nil(etf_buf * des) {
    static unsigned char src[5] = {SMALL_ATOM_EXT, 3, 'n', 'i', 'l'};
    ETF_APPEND(des, src, 5);
}

int etf_append_false(etf_buf * des) {
    static unsigned char src[7] = {SMALL_ATOM_EXT, 5, 'f', 'a', 'l', 's', 'e'};
    ETF_APPEND(des, src, 7);
}

int etf_append_true(etf_buf * des) {
    static unsigned char src[6] = {SMALL_ATOM_EXT, 4, 't', 'r', 'u', 'e'};
    ETF_APPEND(des, src, 6);
}

int etf_append_small_int(etf_buf * des, unsigned char data) {
    unsigned char src[2] = {SMALL_INTEGER_EXT, data};
    ETF_APPEND(des, src, 2);
}

int etf_append_int(etf_buf * des, int32_t data) {
    unsigned char src[5];
    src[0] = INTEGER_EXT;
    _ETF_STORE_32(src+1, data);
    ETF_APPEND(des, src, 5);
}
