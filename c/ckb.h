#ifndef CKB_H_
#define CKB_H_

#include <stddef.h>

int ckb_mmap_cell(const char *cell_id, void* buf, size_t* len);
int ckb_input_cell_argc(const char *cell_id, int *argc);
int ckb_input_cell_argv(const char *cell_id, int arg, char *buf, size_t* len);

#if defined(CKB_IMPL_LIBC)
#include "ckb_impl_libc.h"
#elif defined(CKB_IMPL_SYSCALL)
#include "ckb_impl_syscall.h"
#else
#error "Please select a CKB implementation!"
#endif

#endif
