#define LIBCO_C
#include "libco.h"
#include "settings.h"

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>

#ifdef __cplusplus
extern "C" {
#endif

static thread_local unsigned long co_active_buffer[64];
static thread_local cothread_t co_active_handle = 0;
void co_swap(cothread_t, cothread_t);

__asm__(
  ".text\n"
  ".align 4\n"
  ".type co_swap #function\n"
  "co_swap:\n"

  "mov x16,sp\n"
  "stp x16,x30,[x1]\n"
  "ldp x16,x30,[x0]\n"
  "mov sp,x16\n"
  "stp x19,x20,[x1,#16]\n"
  "ldp x19,x20,[x0,#16]\n"
  "stp x21,x22,[x1,#32]\n"
  "ldp x21,x22,[x0,#32]\n"
  "stp x23,x24,[x1,#48]\n"
  "ldp x23,x24,[x0,#48]\n"
  "stp x25,x26,[x1,#64]\n"
  "ldp x25,x26,[x0,#64]\n"
  "stp x27,x28,[x1,#80]\n"
  "ldp x27,x28,[x0,#80]\n"
  "str x29,[x1,#96]\n"
  "ldr x29,[x0,#96]\n"
  "stp d8,d9,[x1,#112]\n"
  "ldp d8,d9,[x0,#112]\n"
  "stp d10,d11,[x1,#128]\n"
  "ldp d10,d11,[x0,#128]\n"
  "stp d12,d13,[x1,#144]\n"
  "ldp d12,d13,[x0,#144]\n"
  "stp d14,d15,[x1,#160]\n"
  "ldp d14,d15,[x0,#160]\n"
  "br x30\n"

  ".size co_swap, .-co_swap\n"
);

cothread_t co_active() {
  if(!co_active_handle) co_active_handle = &co_active_buffer;
  return co_active_handle;
}

cothread_t co_derive(void* memory, unsigned int size, void (*entrypoint)(void)) {
  unsigned long* handle;
  if(!co_active_handle) co_active_handle = &co_active_buffer;

  if(handle = (unsigned long*)memory) {
    unsigned int offset = (size & ~15);
    unsigned long* p = (unsigned long*)((unsigned char*)handle + offset);
    handle[0]  = (unsigned long)p;           /* x16 (stack pointer) */
    handle[1]  = (unsigned long)entrypoint;  /* x30 (link register) */
    handle[12] = (unsigned long)p;           /* x29 (frame pointer) */
  }

  return handle;
}

cothread_t co_create(unsigned int size, void (*entrypoint)(void)) {
  void* memory = malloc(size);
  if(!memory) return (cothread_t)0;
  return co_derive(memory, size, entrypoint);
}

void co_delete(cothread_t handle) {
  free(handle);
}

void co_switch(cothread_t handle) {
  cothread_t co_previous_handle = co_active_handle;
  co_swap(co_active_handle = handle, co_previous_handle);
}

int co_serializable() {
  return 1;
}

#ifdef __cplusplus
}
#endif
