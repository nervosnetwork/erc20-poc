#ifndef CKB_IMPL_SYSCALL_H_
#define CKB_IMPL_SYSCALL_H_

#define ERROR_FILE_NOTFOUND -101
#define ERROR_BUFFER_NOT_ENOUGH -102

#define MAX_FILENAME_LENGTH 256

#include "syscall.h"

#define SYS_ckb_mmap_cell 2049
#define SYS_ckb_input_cell_argc 2050
#define SYS_ckb_input_cell_argv 2051

int ckb_mmap_cell(const char *cell_id, void* buf, size_t* len)
{
  return syscall_errno(SYS_ckb_mmap_cell, cell_id, buf, len, 0, 0, 0);
}

int ckb_input_cell_argc(const char *cell_id, int *argc)
{
  return syscall_errno(SYS_ckb_input_cell_argc, cell_id, argc, 0, 0, 0, 0);
}

int ckb_input_cell_argv(const char *cell_id, int arg, char *buf, size_t* len)
{
  return syscall_errno(SYS_ckb_input_cell_argv, cell_id, arg, buf, len, 0, 0);
}

#endif
