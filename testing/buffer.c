#include "rotomos_abi.h"
#include <stddef.h>
const size_t required_len = 6;
void start() {
    int len=0;
    char* page=(char*)mmap((size_t)1<<39);
    if(!page) {write(1,":(\r\n",4);exit();}
    while (len<required_len)
    {
        len+=read(0,page+len,1);
    }
    write(1,page, required_len);
    munmap((size_t)page);
    exit();
}