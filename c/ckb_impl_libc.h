#ifndef CKB_IMPL_LIBC_H_
#define CKB_IMPL_LIBC_H_

#define ERROR_FILE_NOTFOUND -101
#define ERROR_BUFFER_NOT_ENOUGH -102

#define MAX_FILENAME_LENGTH 256

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int ckb_mmap_cell(const char *cell_id, void* buf, size_t* len)
{
  char internal_buffer[MAX_FILENAME_LENGTH];
  snprintf(internal_buffer, MAX_FILENAME_LENGTH, "cells/%s/data", cell_id);

  FILE *fp = fopen(internal_buffer, "rb");
  if (fp == NULL) {
    return ERROR_FILE_NOTFOUND;
  }
  fseek(fp, 0, SEEK_END);
  size_t size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  if (*len < size) {
    *len = size;
    fclose(fp);
    return ERROR_BUFFER_NOT_ENOUGH;
  }

  *len = fread(buf, 1, size, fp);
  fclose(fp);
  return 0;
}

int ckb_input_cell_argc(const char *cell_id, int *argc)
{
  char internal_buffer[MAX_FILENAME_LENGTH];
  snprintf(internal_buffer, MAX_FILENAME_LENGTH, "cells/%s/argc", cell_id);

  FILE *fp = fopen(internal_buffer, "rb");
  if (fp == NULL) {
    return ERROR_FILE_NOTFOUND;
  }

  memset(internal_buffer, 0, MAX_FILENAME_LENGTH);
  size_t size = fread(internal_buffer, MAX_FILENAME_LENGTH - 1, 1, fp);
  fclose(fp);

  *argc = atoi(internal_buffer);

  return 0;
}

int ckb_input_cell_argv(const char *cell_id, int arg, char *buf, size_t* len)
{
  char internal_buffer[MAX_FILENAME_LENGTH];
  snprintf(internal_buffer, MAX_FILENAME_LENGTH, "cells/%s/argv%d", cell_id, arg);

  FILE *fp = fopen(internal_buffer, "rb");
  if (fp == NULL) {
    return ERROR_FILE_NOTFOUND;
  }
  fseek(fp, 0, SEEK_END);
  size_t size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  if (*len < size + 1) {
    *len = size + 1;
    fclose(fp);
    return ERROR_BUFFER_NOT_ENOUGH;
  }

  size = fread(buf, 1, size, fp);
  buf[size] = '\0';
  *len = size + 1;
  fclose(fp);
  return 0;
}

int __ckb_dump_data(const char *name, void *buf, size_t len) {
  FILE *fp = fopen(name, "wb");
  if (fp == NULL) {
    return ERROR_FILE_NOTFOUND;
  }

  fwrite(buf, 1, len, fp);
  fclose(fp);
  return 0;
}

#endif
