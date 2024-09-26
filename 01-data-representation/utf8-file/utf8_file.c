#include "utf8_file.h"
#include <unistd.h>
#include <string.h>

/*

                                             0xxxxxxx   1
                                    110xxxxx 10xxxxxx   2
                           1110xxxx 10xxxxxx 10xxxxxx   3
                  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx   4
         111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx   5
1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx   6
       |   |  |||
       |   |  |cp
       |   |  |
       |  remove
       |
      old
 */

// returns 0 - if succeeded, 1 - otherwise
int utf8_remove_byte(char* str, size_t bytes) {
    if (bytes == 1) {
        return 1;
    }

    if (bytes == 2) {
        if ((str[0] & 0x1F) == 0) {
            str[0] = (str[1] & 0x7F) | ((str[0] & 1) << 6);
            str[1] = 0x00;
            return 0;
        }
        return 1;
    }

    size_t cp_sz = 8 - bytes;
    size_t old_sz = 7 - bytes;
    uint8_t cp = (1 << cp_sz) - 1;
    uint8_t remove = (1 << 6) - 1 - cp;
    uint8_t old = (1 << old_sz) - 1;
    char buf[7];
    if ((str[0] & old) == 0 && (str[1] & remove) == 0) {
        memcpy(buf + 1, str + 2, bytes - 2);
        buf[bytes] = 0x00;
        buf[0] = (((1 << (bytes - 1)) - 1) << (9 - bytes)) | (str[1] & cp);
        memcpy(str, buf, bytes);
        return 0;
    }

    return 1;
}

// returns the amount of bytes in code without overloads
size_t utf8_remove_overloads(char* str, size_t bytes) {
    while (utf8_remove_byte(str, bytes) == 0) {
        --bytes;
    }
    return bytes;
}

ssize_t utf8_symbol_len(char symbol) {
    if ((symbol & BYTE_1) == BYTE_0) {
        return 1;
    }
    if ((symbol & BYTE_3) == BYTE_2) {
        return 2;
    }
    if ((symbol & BYTE_4) == BYTE_3) {
        return 3;
    }
    if ((symbol & BYTE_5) == BYTE_4) {
        return 4;
    }
    if ((symbol & BYTE_6) == BYTE_5) {
        return 5;
    }
    if ((symbol & BYTE_7) == BYTE_6) {
        return 6;
    }

    fprintf(stderr, "symbol len error\n");
    exit(1);
}

// returns amount of bytes of read symbol, -1 if error
ssize_t utf8_read_symbol(utf8_file_t* f, char* dest) {
    char buf[6];
    int ok = read(f->fd, buf, 1);
    if (ok == -1) {
        return -1;
    }
    if (ok == 0) {
        *dest = 0x00;
        return 0;
    }
    size_t bytes = utf8_symbol_len(*buf);
    if (read(f->fd, buf + 1, bytes - 1) == -1) {
        return -1;
    }
    bytes = utf8_remove_overloads(buf, bytes);
    memcpy(dest, buf, bytes);
    return bytes;
}

int utf8_read(utf8_file_t* f, uint32_t* res, size_t count) {
    char* tmp = (char*) res;
    ssize_t bytes;
    size_t cnt = 0;
    for (; cnt < count; ++cnt) {
        bytes = utf8_read_symbol(f, tmp);
        if (bytes == -1) {
            fprintf(stderr, "UTF8 read error");
            return -1;
        }
        if (bytes == 0) {
            *tmp = 0x00;
            break;
        }
        tmp += bytes;
    }
    return cnt;
}

int utf8_write(utf8_file_t* f, const uint32_t* str, size_t count) {
    char* tmp = (char*) str;
    char buf[6];
    size_t given_bytes;
    size_t bytes;
    size_t cnt = 0;

    for (; cnt < count; ++cnt) {
        if (*tmp == 0x00) {
            break;
        }
        given_bytes = utf8_symbol_len(*tmp);
        memcpy(buf, tmp, given_bytes);
        bytes = utf8_remove_overloads(buf, given_bytes);
        if (write(f->fd, tmp, bytes) == -1) {
            return -1;
        }
        tmp += given_bytes;
    }
    return cnt;
}

utf8_file_t* utf8_fromfd(int fd) {
    utf8_file_t* f = malloc(sizeof(utf8_file_t));
    f->fd = fd;
    return f;
}
