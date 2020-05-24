#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "file.h"

bool
file_exists(const char* path)
{
    FILE* fp = fopen(path, "r");
    if (fp == NULL) {
        return false;
    }

    fclose(fp);
    return true;
}

int
file_size(const char* path, long* size)
{
    FILE* fp = fopen(path, "r");
    if (fp == NULL) {
        return FILE_ERROR;
    }

    fseek(fp, 0, SEEK_END);
    *size = ftell(fp);

    fclose(fp);
    return FILE_OK;
}

int
file_read(const char* path, void* dest, long size)
{
    FILE* fp = fopen(path, "r");
    if (fp == NULL) {
        return FILE_ERROR;
    }

    long count = fread(dest, 1, size, fp);
    if (count != size) {
        fclose(fp);
        return FILE_ERROR;
    }

    fclose(fp);
    return FILE_OK;
}
