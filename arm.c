#define LIBCO_C
#include "libco.h"
#include "settings.h"

#include <assert.h>
#include <stdlib.h>
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
  ".arm\n"
  "co_swap:\n"
  "mov r3, sp\n"
  "stmia r1!, {r4-r11}\n"
  "stmia r1!, {r3, lr}\n"
  "ldmia r0!, {r4-r11}\n"
  "ldmia r0!, {r3, lr}\n"
  "mov sp, r3\n"
  "bx lr\n"

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
    handle[8] = (unsigned long)p;
    handle[9] = (unsigned long)entrypoint;
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
