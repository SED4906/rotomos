#include <kernel/fb.h>
#include <kernel/libc.h>
#include <4x6.h>
#include <stdint.h>
#include <limine.h>

const uint64_t side_offh = 96;
const uint64_t side_offv = 72;
uint64_t fbtermcolumn = side_offh/4;
uint64_t fbtermrow = side_offv/6;

struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0, .response = 0
};

void FbPlot(uint16_t x, uint16_t y, uint32_t rgb) {
    if(!framebuffer_request.response) return;
    struct limine_framebuffer* fb=framebuffer_request.response->framebuffers[0];
    if(fb->width<x) return;
    if(fb->height<y) return;
    ((uint32_t*)fb->address)[x+y*(fb->pitch>>2)] = rgb;
}

uint32_t rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    unsigned int rgba = a;
    rgba += b << 8;
    rgba += g << 16;
    rgba += r << 24;
    return rgba;
}

void FbScroll() {
    if(!framebuffer_request.response) return;
    struct limine_framebuffer* fb=framebuffer_request.response->framebuffers[0];
    memmove(fb->address,&((uint32_t*)fb->address)[(fb->pitch >> 2)*6],fb->pitch*(fb->height-6));
}

extern "C" size_t FbPrint(const char * str, size_t len) {
    if(!framebuffer_request.response) return 0;
    struct limine_framebuffer* fb=framebuffer_request.response->framebuffers[0];
    uint64_t x = fbtermcolumn*4;
    uint64_t y = fbtermrow*6;
    for(uint64_t i=0;i<len;i++)
    {
        char c=str[i];
        if(c == '\r') { x = side_offh; continue; }
        if(c == '\n') { x = side_offh; y += 6; continue; }
        if(c < 127 && c >= 32) {
            for(char k=0;k<6;k++)
            {
                uint8_t group = __4x6_bin[c*6+k];
                uint8_t p0 = group&0b11000000;
                uint8_t p1 = (group&0b00110000)<<2;
                uint8_t p2 = (group&0b00001100)<<4;
                uint8_t p3 = (group&0b00000011)<<6;
                if(p0) FbPlot(x,y+k,rgba(p0,p0,p0,p0));
                if(p1) FbPlot(x+1,y+k,rgba(p1,p1,p1,p1));
                if(p2) FbPlot(x+2,y+k,rgba(p2,p2,p2,p2));
                if(p3) FbPlot(x+3,y+k,rgba(p3,p3,p3,p3));
            }
            x+=4;
        }
        if(x>=fb->width-side_offh) {
            x=side_offh;
            y+=6;
        }
        if(y>=fb->height-side_offv) {
            y-=6;
            FbScroll();
        }
    }
    fbtermcolumn = x/4;
    fbtermrow = y/6;
    return len;
}