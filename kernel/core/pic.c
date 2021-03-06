#include <kernel/cpu.h>
#include <kernel/core.h>
#include <kernel/libc.h>
#include <stdint.h>

//! @brief Disallow IRQs from specified line. 
//! @param IRQline An IRQ line number.
void pic_set_mask(unsigned char IRQline) {
    uint16_t port;
    uint8_t value;
 
    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) | (1 << IRQline);
    outb(port, value);        
}

//! @brief Allow IRQs from specified line. 
//! @param IRQline An IRQ line number.
void pic_clear_mask(unsigned char IRQline) {
    uint16_t port;
    uint8_t value;
 
    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) & ~(1 << IRQline);
    outb(port, value);        
}

//! @brief Send end of interrupt signal to PIC.
//! @param irq An IRQ line number.
void pic_eoi(unsigned char irq)
{
	if(irq >= 8)
		outb(PIC2_COMMAND,PIC_EOI);
 
	outb(PIC1_COMMAND,PIC_EOI);
}

//! @brief Set up PIC to use specified vectors.
//! @param offset1 Master PIC base vector.
//! @param offset2 Slave PIC base vector.
void init_pic(int offset1, int offset2)
{
	unsigned char a1, a2;
 
	a1 = inb(PIC1_DATA);                        // save masks
	a2 = inb(PIC2_DATA);
 
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	outb(PIC1_DATA, offset1);                 // ICW2: Master PIC vector offset
	outb(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset
	outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	outb(PIC1_DATA, ICW4_8086);
	outb(PIC2_DATA, ICW4_8086);
	outb(PIC1_DATA, a1);   // restore saved masks.
	outb(PIC2_DATA, a2);
}

//! @brief Sets the divider and mode of the PIT.
//! @param frequency A frequency in hertz.
void init_pit(size_t frequency) {
    size_t reload = 1193182 / frequency;
    outb(0x43, 0x36);
    outb(0x40, reload&0xFF);
    outb(0x40, (reload>>8)&0xFF);
    printf("Initialized PIT at frequency %dHz\n", frequency);
}