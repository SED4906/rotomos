#include "rotomos_abi.h"
#include <stddef.h>
char* message = "Hello, world!\r\n";
const size_t message_len = 15;
void* stdout = 0;
void start() {
    stdout = open("tty",'a');
    write(stdout,message, message_len);
    close(stdout);
    exit();
}