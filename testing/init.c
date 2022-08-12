#include "rotomos_abi.h"
#include <stddef.h>
char* message = "Hello, world!\r\n";
void* stdout = 0;
const size_t message_len = 15;
void start() {
    stdout = open("tty", 'a');
    write(stdout,message, message_len);
    exec("hello");
    for(;;) {}
}