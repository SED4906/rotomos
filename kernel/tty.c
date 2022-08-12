#include <kernel/fs.h>
#include <kernel/tty.h>
#include <kernel/libc.h>

void init_tty() {
    create_fifo("tty");
}