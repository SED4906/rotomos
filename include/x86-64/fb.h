#pragma once
#include <stddef.h>
#include <stdint.h>
void fb_plot(uint16_t x, uint16_t y, uint32_t rgb);

void fb_plot_line(uint64_t x0, uint64_t y0, uint64_t x1, uint64_t y1, uint32_t rgba);

void fb_draw_rotom_logo(uint64_t offset_x,uint64_t offset_y);

void fb_draw_rotom_text(uint64_t offset_x,uint64_t offset_y);

size_t fb_print_string(const char * str, size_t len);

uint16_t fb_width();

uint16_t fb_height();
