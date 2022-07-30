#pragma once
#include <stddef.h>

//! @brief Write string to TTY
//! @param str A string.
//! @param len Its length.
//! @return Returns the same length if successful, otherwise returns 0.
int tty_write(const char* str, size_t len);