#include <kernel/idt.h>
#include <kernel/libc.h>
#include <kernel/panic.h>
#include <kernel/task.h>

extern "C" {

void HandleExceptWithCode(size_t code) {
    printf("Unhandled Exception: Error Code - %x", code);
    KePanic("<fatal exception>");
}
void HandleExceptNoCode() {
    printf("Unhandled Exception");
    KePanic("<fatal exception>");
}
void HandlePageFault(size_t error) {
    void* address;
    __asm__ volatile("movq %%cr2, %0":"=r"(address));
    printf("Page fault at %x (%X)", address, error);
    KePanic("<fatal exception>");
}
void HandleDoubleFault() {
    printf("Double fault");
    KePanic("<fatal exception>");
}
void HandleUD2(size_t address) {
    printf("Undefined Opcode at %x", address);
    KePanic("<fatal exception>");
}
void HandleIrq() {
    printf("Unknown IRQ recieved\n");
    PicEOI(16);
}
void HandlePit() {
    PicEOI(0);
    ContextSwitch();
}
void HandleKeyb() {
    PicEOI(1);
}

}