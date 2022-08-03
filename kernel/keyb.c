#include <kernel/core.h>
#include <kernel/cpu.h>
#include <kernel/keyb.h>
#include <kernel/libc.h>
#include <stdbool.h>
#include <stdint.h>
const char keyscans[96] = "\0\e1234567890-=\b\tqwertyuiop[]\n""\x1E""asdfghjkl;'`\x0E\\zxcvbnm,./\x0F*\x1A \x11\xF1\xF2\xF3\xF5\xF5\xF6\xF7\xF8\xF9\xFA\x12\x13\x37\x38\x39-456+1230.\x00\x00\x00\xFB\xFC\x00\x00\x00\x00\x00\x00\x00";
const char keyscans_shift[96] = "\0\e!@#$%^&*()_+\b\tQWERTYUIOP{}\n""\x1E""ASDFGHJKL:\"~\x0E\\ZXCVBNM<>?\x0F*\x1A \x11\xF1\xF2\xF3\xF5\xF5\xF6\xF7\xF8\xF9\xFA\x12\x13\x37\x38\x39-456+1230.\x00\x00\x00\xFB\xFC\x00\x00\x00\x00\x00\x00\x00";
bool modleftshift = false; bool modrightshift = false;
bool modctrl = false; bool modalt = false;
bool capslock = false; bool numlock = false; bool scrlock = false;
char scandecode(uint8_t code) {
    if(code && code < 96) {
        return keyscans[code];
    }
    return 0;
}
char scandecode_shift(uint8_t code) {
    if(code && code < 96) {
        return keyscans_shift[code];
    }
    return 0;
}
char scan2ascii(uint8_t code) {
    char glyph;
    if(code > 128) {
        glyph = scandecode(code-128);
        if(glyph == 0x0E) { modleftshift = false; return 0; }
        else if(glyph == 0x0F) { modrightshift = false; return 0; }
        else if(glyph == 0x1E) { modctrl = false; return 0; }
        else if(glyph == 0x1A) { modalt = false; return 0; }
        return 0;
    } else {
        glyph = scandecode(code);
        if(glyph == 0x0E) { modleftshift = true; return 0; }
        else if(glyph == 0x0F) { modrightshift = true; return 0; }
        else if(glyph == 0x1E) { modctrl = true; return 0; }
        else if(glyph == 0x1A) { modalt = true; return 0; }
        else if(glyph == 0x11) { capslock = !capslock; return 0; }
        else if(glyph == 0x12) { numlock = !numlock; return 0; }
        else if(glyph == 0x13) { scrlock = !scrlock; return 0; }
        else {
            if(modleftshift ^ capslock) return scandecode_shift(code);
            else return glyph;
        }
    }
    return 0;
}

char* keyb_buffer;
int write_head,read_head;
int chars_remaining;

void init_keyb() {
    keyb_buffer = kmalloc(64);
    read_head = 0;
    write_head = 0;
    chars_remaining = 0;
    printf("Type away, your keyboard is now activated.\n");
    pic_clear_mask(1);
}

char keyb_readnext() {
    if(!chars_remaining) return 0;
    char ret = keyb_buffer[read_head++];
    read_head %= 64;
    chars_remaining--;
    return ret;
}

__attribute__ ((no_caller_saved_registers))
void keyb_handler() {
    char keyc = scan2ascii(inb(0x60));
    if(keyc && chars_remaining < 64) {
        keyb_buffer[write_head++] = keyc;
        write_head %= 64;
        chars_remaining++;
        printf("%c",keyc);
    }
    pic_eoi(1);
}