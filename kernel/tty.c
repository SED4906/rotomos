#include <limine.h>
#include <kernel/tty.h>
#include <kernel/libc.h>

struct limine_terminal_request terminal_request = {
    .id = LIMINE_TERMINAL_REQUEST, .revision = 0,
    .response = 0
};

/// TTY Write
int tty_write(const char* str, size_t len) {
    if(!terminal_request.response) return 0;
    terminal_request.response->write(terminal_request.response->terminals[0], str, len);
    return len;
}