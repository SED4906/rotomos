#include <kernel/fb.h>
#include <kernel/libc.h>
#include <kernel/panic.h>

__attribute__((noreturn))
void KePanic(const char* msg) {
    FbPrint(msg, strlen(msg));
    for(;;) KeLockUpForever();
}