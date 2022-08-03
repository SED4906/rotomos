#include <kernel/core.h>
#include <kernel/libc.h>
#include <limine.h>
#include <4x6.h>

const uint64_t side_offh = 96;
const uint64_t side_offv = 72;
uint64_t gfx_terminal_col = side_offh/4;
uint64_t gfx_terminal_row = side_offv/6;

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

void fb_clear_screen() {
    if(!framebuffer_request.response) return;
    struct limine_framebuffer* fb=framebuffer_request.response->framebuffers[0];
    for(uint64_t i = 0;i<fb->width*fb->height;i++) {
        ((uint32_t*)fb->address)[i] = 0;
    }
}

void plotlinelow(uint64_t x0, uint64_t y0, uint64_t x1, uint64_t y1, uint32_t rgba) {
    int dx=x1-x0;
    int dy=y1-y0;
    int yi=1;
    if(dy<0) {
        yi=-1;
        dy=-dy;
    }
    int D=2*dy-dx;
    int y=y0;
    int xd = (x1-x0 > 0 ? 1 : -1);
    for(uint64_t x=x0;x!=x1;x+=xd) {
        fb_plot(x,y,rgba);
        if(D > 0) {
            y+=yi;
            D+=2*(dy-dx);
        }
        else {
            D+=2*dy;
        }
    }
}

void plotlinehigh(uint64_t x0, uint64_t y0, uint64_t x1, uint64_t y1, uint32_t rgba) {
    int dx=x1-x0;
    int dy=y1-y0;
    int xi=1;
    if(dx<0) {
        xi=-1;
        dx=-dx;
    }
    int D=2*dx-dy;
    int x=x0;
    int yd = (y1-y0 > 0 ? 1 : -1);
    for(uint64_t y=y0;y!=y1;y+=yd) {
        fb_plot(x,y,rgba);
        if(D > 0) {
            x+=xi;
            D+=2*(dx-dy);
        }
        else {
            D+=2*dx;
        }
    }
}

void fb_plot_line(uint64_t x0, uint64_t y0, uint64_t x1, uint64_t y1, uint32_t rgba) {
    int y=y1-y0;
    if(y<0) y=-y;
    int x=x1-x0;
    if(x<0) x=-x;
    if(y<x) {
        if(x0>x1) plotlinelow(x1,y1,x0,y0,rgba);
        else plotlinelow(x0,y0,x1,y1,rgba);
    } else {
        if(y0>y1) plotlinehigh(x1,y1,x0,y0,rgba);
        else plotlinehigh(x0,y0,x1,y1,rgba);
    }
}

void gfx_scroll_terminal() {
    if(!framebuffer_request.response) return;
    struct limine_framebuffer* fb=framebuffer_request.response->framebuffers[0];
    memmove(fb->address,&((uint32_t*)fb->address)[(fb->pitch >> 2)*6],fb->pitch*(fb->height-6));
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
    uint64_t x = gfx_terminal_col*4;
    uint64_t y = gfx_terminal_row*6;
    for(uint64_t i=0;i<len;i++)
    {
        char c=str[i];
        if(c == '\r') { x = side_offh; continue; }
        if(c == '\n') { x = side_offh; y += 6; continue; }
        if(c < 127 && c >= 32) {
            for(char k=0;k<6;k++)
            {
                unsigned char group = __4x6_bin[c*6+k];
                unsigned char p0 = group&0b11000000;
                unsigned char p1 = (group&0b00110000)<<2;
                unsigned char p2 = (group&0b00001100)<<4;
                unsigned char p3 = (group&0b00000011)<<6;
                if(p0) fb_plot(x,y+k,rgba(p0,p0,p0,p0));
                if(p1) fb_plot(x+1,y+k,rgba(p1,p1,p1,p1));
                if(p2) fb_plot(x+2,y+k,rgba(p2,p2,p2,p2));
                if(p3) fb_plot(x+3,y+k,rgba(p3,p3,p3,p3));
            }
            x+=4;
        }
        if(x>=fb->width-side_offh) {
            x=side_offh;
            y+=6;
        }
        if(y>=fb->height-side_offv) {
            y-=6;
            gfx_scroll_terminal();
        }
    }
    gfx_terminal_col = x/4;
    gfx_terminal_row = y/6;
    return len;
}