#include "utf8_file.h"

#include <errno.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
//    char buf[5];
//    buf[0] = 0b11110000;
//    buf[1] = 0b10000001;
//    buf[2] = 0b10111111;
//    buf[3] = 0b10111111;
////    buf[4] = 0x00;
//    printf("%d\n\n", (int)utf8_remove_overloads(buf, 4));
//    for (size_t i = 0; i < 4; ++i) {
//        printf(BYTE_TO_BINARY_PATTERN" ", BYTE_TO_BINARY(buf[i]));
//    }
//    return 0;

    int fd_in = open(argv[1], O_RDONLY);
    if (fd_in < 0) {
        return 1;
    }


    int fd_out = open(argv[2], O_WRONLY | O_CREAT, 0777);
    if (fd_out < 0) {
        return 1;
    }


    utf8_file_t* fin = utf8_fromfd(fd_in);
    if (fin == NULL) {
        return 1;
    }

    utf8_file_t* fout = utf8_fromfd(fd_out);
    if (fout == NULL) {
        return 1;
    }

    for (;;) {
        uint32_t buf[512];
        int res = utf8_read(fin, buf, sizeof(buf) / sizeof(wchar_t));
        if (res == 0) {
            break;
        }
        if (res < 0) {
            fprintf(stderr, "utf8_read failed: %s", strerror(errno));
            return 1;
        }

        int count = res;
        uint32_t* curr = &buf[0];
        while (count > 0) {
            res = utf8_write(fout, curr, res);
            if (res <= 0) {
                fprintf(stderr, "utf8_write failed: %s", strerror(errno));
                return 1;
            }
            count -= res;
            curr += res;
        }
    }

    return 0;
}
