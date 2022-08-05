#include "rotomos_abi.h"
#include <stddef.h>
char* message = "Hello, world!\r\n";
const size_t message_len = 15;
void start() {
    write(1,message, message_len);
    exec("hello");
    for(;;) {}
}