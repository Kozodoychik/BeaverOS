#include <drivers/pci.h>
#include <drivers/io.h>
#include <io/printf.h>
#include <stdbool.h>


uint16_t pci_config_read16(uint32_t bus, uint32_t device, uint32_t func, uint32_t reg) {
    uint32_t addr = (1 << 31) | (bus << 16) | (device << 11) | (func << 8) | (reg & 0xfc);
    
    outl(PCI_CONFIG_ADDR, addr);

    uint16_t data = (uint16_t)(inl(PCI_CONFIG_DATA) >> ((reg & 2) * 8) & 0xffff);
    return data;
}

uint32_t pci_config_read(uint32_t bus, uint32_t device, uint32_t func, uint32_t reg) {
    uint32_t addr = (1 << 31) | (bus << 16) | (device << 11) | (func << 8) | (reg & 0xfc);
    
    outl(PCI_CONFIG_ADDR, addr);

    uint32_t data = inl(PCI_CONFIG_DATA) >> ((reg & 2) * 8) & 0xffff;
    return data;
}

void pci_config_write(uint32_t bus, uint32_t device, uint32_t func, uint32_t reg, uint32_t value) {
    uint32_t addr = (1 << 31) | (bus << 16) | (device << 11) | (func << 8) | (reg & 0xfc);
    
    outl(PCI_CONFIG_ADDR, addr);
    outl(PCI_CONFIG_DATA, value);
}

pci_device_header pci_get_header(uint32_t bus, uint32_t device, uint32_t func) {
    pci_device_header d;

    d.vendor_id = pci_config_read16(bus, device, func, 0x00);
    d.device_id = pci_config_read16(bus, device, func, 0x02);

    d.command = pci_config_read16(bus, device, func, 0x04);
    d.status = pci_config_read16(bus, device, func, 0x06);

    d.revision_id = pci_config_read16(bus, device, func, 0x08) & 0xff;
    d.prog_if = pci_config_read16(bus, device, func, 0x08) >> 8;

    d.subclass = (uint8_t)(pci_config_read16(bus, device, func, 0x0a) & 0xff);
    d.class_code = (uint8_t)(pci_config_read16(bus, device, func, 0x0a) >> 8);

    d.cache_line_size = (uint8_t)(pci_config_read16(bus, device, func, 0x0c) & 0xff);
    d.latency_timer = (uint8_t)(pci_config_read16(bus, device, func, 0x0c) >> 8);

    d.header_type = (uint8_t)(pci_config_read16(bus, device, func, 0x0e) & 0xff);
    d.bist = (uint8_t)(pci_config_read16(bus, device, func, 0x0e) >> 8);

    d.bar0 = pci_config_read(bus, device, func, 0x10);
    d.bar1 = pci_config_read(bus, device, func, 0x14);
    d.bar2 = pci_config_read(bus, device, func, 0x18);
    d.bar3 = pci_config_read(bus, device, func, 0x1c);
    d.bar4 = pci_config_read(bus, device, func, 0x20);
    d.bar5 = pci_config_read(bus, device, func, 0x24);

    d.cis_ptr = pci_config_read(bus, device, func, 0x28);
    d.subsystem_vendor_id = pci_config_read16(bus, device, func, 0x2c);
    d.subsystem_id = pci_config_read16(bus, device, func, 0x2e);

    d.exp_rom_base = pci_config_read(bus, device, func, 0x30);

    d.capabilities_ptr = (uint8_t)(pci_config_read16(bus, device, func, 0x34) & 0xff);

    d.irq_line = (uint8_t)(pci_config_read16(bus, device, func, 0x3c) & 0xff);
    d.irq_pin = (uint8_t)(pci_config_read16(bus, device, func, 0x3c) >> 8);
    d.min_grant = (uint8_t)(pci_config_read16(bus, device, func, 0x3e) & 0xff);
    d.max_latency = (uint8_t)(pci_config_read16(bus, device, func, 0x3e) >> 8);

    return d;
}

pci_device_descriptor pci_find_device_by_id(uint16_t vendor_id, uint16_t device_id) {
    for (int bus = 0; bus < 8; bus++) {
        for (int device = 0; device < 8; device++) {
            bool mf = (bool)(pci_config_read16(bus, device, 0, 0x0e) & (1 << 7));
            for (int func = 0; func < (mf ? 8 : 1); func++) {
                pci_device_header d = pci_get_header(bus, device, func);
                if (d.vendor_id == vendor_id || d.device_id == device_id) {
                    pci_device_descriptor descr = {};

                    descr.bus = bus;
                    descr.device = device;
                    descr.func = func;
                    descr.header = d;

                    return descr;
                }
            }
        }
    }
    pci_device_descriptor empty;
    empty.bus = 0xff;
    return empty;
}

void pci_print_devices() {
    for (int bus = 0; bus < 8; bus++) {
        for (int device = 0; device < 8; device++) {
            bool mf = (bool)(pci_config_read16(bus, device, 0, 0x0e) & (1 << 7));
            for (int func = 0; func < (mf ? 8 : 1); func++) {
                pci_device_header d = pci_get_header(bus, device, func);
                if (d.vendor_id == 0 || d.vendor_id == 0xffff) continue;

                printk("PCI: %x:%x:%x vendor_id=%x device_id=%x\n", bus, device, func, d.vendor_id, d.device_id);
            }
        }
    }
}