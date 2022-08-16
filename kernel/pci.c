#include <kernel/cpu.h>
#include <kernel/libc.h>
#include <kernel/pci.h>
#include <stdint.h>

uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;
 
    // Create configuration address as per Figure 1
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
 
    // Write out the address
    outl(0xCF8, address);
    // Read in the data
    // (offset & 2) * 8) = 0 will choose the first word of the 32-bit register
    tmp = (uint16_t)((inl(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
    return tmp;
}

uint8_t pci_config_read_byte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint8_t tmp = 0;
 
    // Create configuration address as per Figure 1
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
 
    // Write out the address
    outl(0xCF8, address);
    // Read in the data
    // (offset & 2) * 8) = 0 will choose the first word of the 32-bit register
    tmp = (uint8_t)((inl(0xCFC) >> ((offset & 3) * 8)) & 0xFF);
    return tmp;
}

void pci_check_function(uint8_t bus, uint8_t device, uint8_t function) {
    uint8_t baseClass;
    uint8_t subClass;
    uint8_t secondaryBus;
    uint8_t progif;

    baseClass = pci_config_read_byte(bus, device, function, 11);
    subClass = pci_config_read_byte(bus, device, function, 10);
    progif = pci_config_read_byte(bus, device, function, 9);
    printf("%d:%d:%d (%d, %d, %d)\n", bus, device, function, baseClass, subClass, progif);
    if ((baseClass == 0x6) && (subClass == 0x4)) {
        secondaryBus = pci_config_read_byte(bus, device, function, 25);
        //printf("Other bus: %d\n",secondaryBus);
        pci_check_bus(secondaryBus);
    }
}

void pci_check_device(uint8_t bus, uint8_t device) {
    uint16_t vendor;
    uint8_t function=0;
    /* Try and read the first configuration register. Since there are no
     * vendors that == 0xFFFF, it must be a non-existent device. */
    if ((vendor = pci_config_read_word(bus, device, function, 0)) == 0xFFFF) return;
    printf("Found device (%d, %d)\n", bus, device);
    pci_check_function(bus, device, function);
    uint8_t headerType = pci_config_read_byte(bus, device, function, 14);
    if( (headerType & 0x80) != 0) {
        //printf("It's multi-function.\n");
        // It's a multi-function device, so check remaining functions
        for (function = 1; function < 8; function++) {
            if ((vendor = pci_config_read_word(bus, device, function, 0)) != 0xFFFF) {
                pci_check_function(bus, device, function);
            }
        }
    }
}

void pci_check_bus(uint8_t bus) {
    uint8_t device;

    for (device = 0; device < 32; device++) {
        //printf("Checking device %d\n", (uint64_t)device);
        pci_check_device(bus, device);
    }
}

void init_pci() {
    pci_check_bus(0);
}