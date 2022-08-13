#include <kernel/core.h>
#include <kernel/cpu.h>
#include <kernel/libc.h>
#include <kernel/mouse.h>
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

void init_mouse() {
    pic_clear_mask(2);
    pic_clear_mask(12);
    cursor_x=0;cursor_y=0;
    
    outb(0x64, 0xA8); // enabling the auxiliary device - mouse

    mouse_wait();
    outb(0x64, 0x20); // tells the keyboard controller that we want to send a command to the mouse
    mouse_wait_in();

    uint8_t status = inb(0x60) | 2;
    mouse_wait();
    outb(0x64, 0x60);
    mouse_wait();
    outb(0x60, status); // setting the correct bit is the "compaq" status byte

    mouse_write(0xF6);
    mouse_read();

    mouse_write(0xF4);
    mouse_read();
}

char mouse_cycle=0;
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
int skip=1;
__attribute__ ((no_caller_saved_registers))
void mouse_handler() {
    uint8_t data=inb(0x60);
    process_mouse_packet();
    if(skip) {
        skip=0;
        pic_eoi(12);
        return;
    }
    switch (mouse_cycle)
    {
    case 0:

        if ((data & 0b00001000) == 0)
            break;
        mouse_packet[0] = data;
        mouse_cycle++;
        break;
    case 1:

        mouse_packet[1] = data;
        mouse_cycle++;
        break;
    case 2:

        mouse_packet[2] = data;
        mouse_ready = 1;
        mouse_cycle = 0;
        break;
    }
    fb_plot((uint16_t)cursor_x, (uint16_t)cursor_y, 0xFFFFFFFF);
    pic_eoi(12);
}