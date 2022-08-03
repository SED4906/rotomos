#pragma once
#include <stdint.h>
char scan2ascii(uint8_t code);
void init_keyb();
char keyb_readnext();