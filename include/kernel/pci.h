#pragma once
#include <stdint.h>
void init_pci();

void pci_check_function(uint8_t bus, uint8_t device, uint8_t function);

void pci_check_device(uint8_t bus, uint8_t device);

void pci_check_bus(uint8_t bus);
