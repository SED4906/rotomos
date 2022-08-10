#include <kernel/fifo.h>
#include <kernel/idt.h>
#include <kernel/keyb.h>
#include <kernel/libc.h>
#include <stdint.h>
const char keyscans[96] = "\0\e1234567890-=\b\tqwertyuiop[]\n""\x1E""asdfghjkl;'`\x0E\\zxcvbnm,./\x0F*\x1A \x11\xF1\xF2\xF3\xF5\xF5\xF6\xF7\xF8\xF9\xFA\x12\x13\x37\x38\x39-456+1230.\x00\x00\x00\xFB\xFC\x00\x00\x00\x00\x00\x00";
const char keyscans_shift[96] = "\0\e!@#$%^&*()_+\b\tQWERTYUIOP{}\n""\x1E""ASDFGHJKL:\"~\x0E\\ZXCVBNM<>?\x0F*\x1A \x11\xF1\xF2\xF3\xF5\xF5\xF6\xF7\xF8\xF9\xFA\x12\x13\x37\x38\x39-456+1230.\x00\x00\x00\xFB\xFC\x00\x00\x00\x00\x00\x00";
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

Fifo* keyb_fifo;
FifoHandle* keyb_fifo_handle;
char* keyb_fifo_name="keyboard";
void KeybInitialize() {
    keyb_fifo = CreateFifo(keyb_fifo_name);
    keyb_fifo_handle = FifoOpen(keyb_fifo);
    PicClear(1);
}

void KeybReceive(uint8_t scancode) {
    char keyc = scan2ascii(scancode);
    if(keyc) FifoWrite(keyb_fifo_handle,&keyc,1);
}

void KeybEcho() {
    char* c = new char{0};
    if(FifoRead(keyb_fifo_handle,c,1) == 1) printf("%c",*c);
}