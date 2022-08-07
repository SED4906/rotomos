#pragma once
#include <stddef.h>
#include <stdint.h>
void FbPlot(uint16_t x, uint16_t y, uint32_t rgb);
#ifdef __cplusplus
extern "C" {
#endif
size_t FbPrint(const char * str, size_t len);
#ifdef __cplusplus
}
#endif