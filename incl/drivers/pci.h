#include <stdint.h>

#define PCI_CONFIG_ADDR 0xcf8
#define PCI_CONFIG_DATA 0xcfc

typedef struct {
    uint16_t vendor_id;
    uint16_t device_id;

    uint16_t command;
    uint16_t status;
    
    uint8_t revision_id;
    uint8_t prog_if;
    uint8_t subclass;
    uint8_t class_code;
    uint8_t cache_line_size;
    uint8_t latency_timer;
    uint8_t header_type;
    uint8_t bist;
    
    uint32_t bar0;
    uint32_t bar1;
    uint32_t bar2;
    uint32_t bar3;
    uint32_t bar4;
    uint32_t bar5;

    uint32_t cis_ptr;
    uint16_t subsystem_vendor_id;
    uint16_t subsystem_id;
    
    uint32_t exp_rom_base;
    uint8_t capabilities_ptr;

    uint8_t irq_line;
    uint8_t irq_pin;
    uint8_t min_grant;
    uint8_t max_latency;
} pci_device_header;

typedef struct {
    uint8_t bus;
    uint8_t device;
    uint8_t func;
    pci_device_header header;
} pci_device_descriptor;

uint16_t pci_config_read16(uint32_t bus, uint32_t device, uint32_t func, uint32_t reg);
uint32_t pci_config_read(uint32_t bus, uint32_t device, uint32_t func, uint32_t reg);

void pci_config_write(uint32_t bus, uint32_t device, uint32_t func, uint32_t reg, uint32_t value);

pci_device_header pci_get_header(uint32_t bus, uint32_t device, uint32_t func);
pci_device_descriptor pci_find_device_by_id(uint16_t vendor_id, uint16_t device_id);

void pci_print_devices();