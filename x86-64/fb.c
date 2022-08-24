#include <x86-64/fb.h>
#include <kernel/libc.h>
#include <limine.h>
#include <4x6.h>
#include <9x14.h>
#include <stddef.h>

const uint64_t side_offh = 90;
const uint64_t side_offv = 70;
uint64_t gfx_terminal_col = side_offh/9;
uint64_t gfx_terminal_row = side_offv/14;

struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0, .response = 0
};

void fb_plot(uint16_t x, uint16_t y, uint32_t rgb) {
    if(!framebuffer_request.response) return;
    struct limine_framebuffer* fb=framebuffer_request.response->framebuffers[0];
    if(fb->width<x) return;
    if(fb->height<y) return;
    ((uint32_t*)fb->address)[x+y*(fb->pitch>>2)] = rgb;
}

uint16_t fb_width() {
    if(!framebuffer_request.response) return 0;
    struct limine_framebuffer* fb=framebuffer_request.response->framebuffers[0];
    return (uint16_t)fb->width;
}

uint16_t fb_height() {
    if(!framebuffer_request.response) return 0;
    struct limine_framebuffer* fb=framebuffer_request.response->framebuffers[0];
    return (uint16_t)fb->height;
}

void fb_clear_screen() {
    if(!framebuffer_request.response) return;
    struct limine_framebuffer* fb=framebuffer_request.response->framebuffers[0];
    for(uint64_t i = 0;i<fb->width*fb->height;i++) {
        ((uint32_t*)fb->address)[i] = 0;
    }
}

void gfx_scroll_terminal() {
    if(!framebuffer_request.response) return;
    struct limine_framebuffer* fb=framebuffer_request.response->framebuffers[0];
    memmove(fb->address,&((uint32_t*)fb->address)[(fb->pitch >> 2)*14],fb->pitch*(fb->height-14));
}

unsigned int rgba(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    unsigned int rgba = a;
    rgba += b << 8;
    rgba += g << 16;
    rgba += r << 24;
    return rgba;
}

size_t fb_print_string(const char * str, size_t len) {
    if(!framebuffer_request.response) return 0;
    struct limine_framebuffer* fb=framebuffer_request.response->framebuffers[0];
    uint64_t x = gfx_terminal_col*9;
    uint64_t y = gfx_terminal_row*14;
    for(uint64_t i=0;i<len;i++)
    {
        char c=str[i];
        if(c == '\r') { x = side_offh; continue; }
        if(c == '\n') { x = side_offh; y += 14; continue; }
        if(c) {
            for(char k=0;k<14;k++)
            {
                for(char p=0;p<9;p++) {
                    size_t pos = p*2 + c*9*14*2 + k*9*2;
                    size_t byte = pos >> 3;
                    size_t bit = 6-(pos & 7);
                    char pixel = (bindata_bin[byte] & (3<<bit))>>bit;
                    if(pixel) fb_plot(x+p,y+k,rgba(pixel*64,pixel*64,pixel*64,pixel*64));
                }
            }
            x+=9;
        }
        if(x>=fb->width-side_offh) {
            x=side_offh;
            y+=14;
        }
        if(y>=fb->height-side_offv) {
            y-=14;
            gfx_scroll_terminal();
        }
    }
    gfx_terminal_col = x/9;
    gfx_terminal_row = y/14;
    return len;
}