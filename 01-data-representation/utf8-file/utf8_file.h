#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#define BUF_SIZE 1 << 20
#define WORD_CAP 4


#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0')


enum code_len : uint8_t {
    BYTE_0 = 0x00,
    BYTE_1 = 0x80,
    BYTE_2 = 0xC0,
    BYTE_3 = 0xE0,
    BYTE_4 = 0xF0,
    BYTE_5 = 0xF8,
    BYTE_6 = 0xFC,
    BYTE_7 = 0xFE,
};

size_t utf8_remove_overloads(char* str, size_t bytes);

typedef struct {
    int fd;
} utf8_file_t;

int utf8_write(utf8_file_t* f, const uint32_t* str, size_t count);
int utf8_read(utf8_file_t* f, uint32_t* res, size_t count);
utf8_file_t* utf8_fromfd(int fd);
