#include <kernel/cpu.h>
#include <kernel/fb.h>
#include <kernel/libc.h>
#include <kernel/mouse.h>
#include <kernel/pic.h>
#include <stdint.h>

int cursor_x,cursor_y;
uint32_t cursor_color=0;

void mouse_wait()
{
    uint64_t timeout = 100000;
    while (timeout--)
    {
        if (!(inb(0x64) & 2))
        {
            return;
        }
    }
}

void mouse_wait_in()
{
    uint64_t timeout = 100000;
    while (timeout--)
    {
        if (inb(0x64) & 1)
        {
            return;
        }
    }
}

void mouse_write(uint8_t byte) {
    mouse_wait();
    outb(0x64, 0xD4);
    mouse_wait();
    outb(0x60, byte);
}

uint8_t mouse_read() {
    mouse_wait_in();
    return inb(0x60);
}

void mouse_alive() {
    mouse_write(0xEB);
    mouse_read();
}


void init_mouse() {
    cursor_x=0;cursor_y=0;
    
    outb(0x64, 0xA8);

    mouse_wait();
    outb(0x64, 0x20);
    uint8_t status=(mouse_read() | 2) &~ 32;
    mouse_wait();
    outb(0x64, 0x60);
    mouse_wait();
    outb(0x60, status);

    mouse_write(0xF6);
    mouse_read();
    
    mouse_write(0xF4);
    mouse_read();

    pic_clear_mask(2);
    pic_clear_mask(12);
}

uint8_t mouse_cycle=0;
int mouse_ready=0;
char mouse_packet[4];

void process_mouse_packet() {
    if(!mouse_ready) return;
    if(mouse_packet[0] & 16) cursor_x -= 256 - mouse_packet[1];
    else cursor_x += mouse_packet[1];
    if(mouse_packet[0] & 32) cursor_y += 256 - mouse_packet[2];
    else cursor_y -= mouse_packet[2];
    if(cursor_x<0) cursor_x=0;
    if(cursor_y<0) cursor_y=0;
    if(cursor_x>=fb_width()) cursor_x=fb_width()-1;
    if(cursor_y>=fb_height()) cursor_y=fb_height()-1;
    mouse_ready=0;
}
uint8_t skip=1;
__attribute__ ((no_caller_saved_registers))
void mouse_handler() {
    cursor_color += 99999;
    process_mouse_packet();
    uint8_t data=inb(0x60);
    printf("(%X)",(uint64_t)data);
    if(skip) {
        skip=0;
        pic_eoi(12);
        return;
    }
    mouse_packet[mouse_cycle++] = data;
    mouse_cycle%=3;
    if(!mouse_cycle) mouse_ready=1;
    fb_plot((uint16_t)cursor_x, (uint16_t)cursor_y, cursor_color);
    pic_eoi(12);
}