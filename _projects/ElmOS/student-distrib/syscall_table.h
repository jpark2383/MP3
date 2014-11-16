#ifndef _SYSCALL_TABLE
#define _SYSCALL_TABLE
#include "types.h"
#include "lib.h"
#include "systemcalls.h"


extern void syscall_handler(void);
extern int32_t halt (uint8_t status);
extern int32_t execute (const uint8_t* command);
extern int32_t read (int32_t fd, void* buf, int32_t nbytes);
extern int32_t write (int32_t fd, const void* buf, int32_t nbytes);
extern int32_t open (const uint8_t* filename);
extern int32_t close (int32_t fd);

#endif
