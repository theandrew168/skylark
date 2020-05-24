#ifndef SKYLARK_FILE_H_INCLUDED
#define SKYLARK_FILE_H_INCLUDED

#include <stdbool.h>

enum file_status {
    FILE_OK = 0,
    FILE_ERROR,
};

bool file_exists(const char* path);
int file_size(const char* path, long* size);
int file_read(const char* path, void* dest, long size);

#endif
