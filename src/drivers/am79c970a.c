#include <drivers/am79c970a.h>
#include <drivers/pci.h>
#include <drivers/io.h>
#include <cpu/interrupts.h>
#include <io/printf.h>

#include <string.h>
#include <stdbool.h>


uint8_t tx_buffer[1520 * 8];
uint8_t rx_buffer[1520 * 8];

pcnet_tx_descriptor tx_descs[8];
pcnet_rx_descriptor rx_descs[8];

pcnet_init_block init_block;

uint8_t test_packet[128] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

pci_device_descriptor pcnet;
uint16_t io_base;

uint8_t mac[6];

bool init_done = false;

uint8_t rx_buffer_n = 0;
uint8_t tx_buffer_n = 0;

void pcnet_rap_write32(uint32_t value) {
    outl(io_base + PCNET_32_RAP, value);
}

void pcnet_rap_write16(uint16_t value) {
    outw(io_base + PCNET_16_RAP, value);
}

uint32_t pcnet_csr_read32(uint32_t i) {
    pcnet_rap_write32(i);
    return inl(io_base + PCNET_32_RDP);
}

uint16_t pcnet_csr_read16(uint16_t i) {
    pcnet_rap_write16(i);
    return inw(io_base + PCNET_16_RDP);
}

void pcnet_csr_write32(uint32_t i, uint32_t value) {
    pcnet_rap_write32(i);
    outl(io_base + PCNET_32_RDP, value);
}

void pcnet_csr_write16(uint16_t i, uint16_t value) {
    pcnet_rap_write16(i);
    outw(io_base + PCNET_16_RDP, value);
}

uint32_t pcnet_bcr_read32(uint32_t i) {
    pcnet_rap_write32(i);
    return inl(io_base + PCNET_32_BDP);
}

uint16_t pcnet_bcr_read16(uint16_t i) {
    pcnet_rap_write16(i);
    return inw(io_base + PCNET_16_BDP);
}

void pcnet_bcr_write32(uint32_t i, uint32_t value) {
    pcnet_rap_write32(i);
    outl(io_base + PCNET_32_BDP, value);
}

void pcnet_bcr_write16(uint16_t i, uint16_t value) {
    pcnet_rap_write16(i);
    outw(io_base + PCNET_16_BDP, value);
}

void pcnet_reset() {
    inw(io_base + PCNET_16_RESET);
    inl(io_base + PCNET_32_RESET);
}

void pcnet_detect() {
    pcnet = pci_find_device_by_id(0x1022, 0x2000);

    if (pcnet.bus == 0xff) {
        printk("AMD PCnet-PCI II (Am79C970A) not detected\n");
        return;
    }

    printk("AMD PCnet-PCI II (Am79C970A) detected at %x:%x:%x\n", pcnet.bus, pcnet.device, pcnet.func);
}

void pcnet_irq() {
    uint32_t status = pcnet_csr_read32(0);

    if (status & 0x100) init_done = true;
    
    printk("AMD PCnet IRQ!!! (CSR0: %x)\n", pcnet_csr_read32(0));
    
    if (status & 0x400) {
        printk("Received something! (n=%x)\n", rx_buffer_n);

        for (int i = 0; i < 12; i++) {
            printk("%x ", rx_buffer[i + (rx_buffer_n * 1520)]);
        }
        printk("\n");
        rx_descs[rx_buffer_n].own = 1;
        rx_buffer_n++;
        if (rx_buffer_n > 7) rx_buffer_n = 0;
    }

    if (status & 0x200) {
        printk("Send something!\n");

        tx_buffer_n++;
        if (tx_buffer_n > 7) tx_buffer_n = 0;
    }

    pcnet_csr_write32(0, pcnet_csr_read32(0) | 0x400);
    return;
}

void pcnet_init() {
    pcnet_detect();

    if (pcnet.bus == 0xff) return;

    io_base = pcnet.header.bar0 & 0xfffffffc;

    printk("PCnet I/O base: %x\n", io_base);
    printk("PCnet IRQ: %x\n", pcnet.header.irq_line);

    uint32_t conf = pci_config_read(pcnet.bus, pcnet.device, pcnet.func, 0x04);

    conf &= 0xffff0000;
    conf |= 0x05;

    pci_config_write(pcnet.bus, pcnet.device, pcnet.func, 0x04, conf);
    
    irq_register(pcnet.header.irq_line, pcnet_irq);

    pcnet_reset();

    outl(io_base + PCNET_32_RDP, 0);

    for (int i = 0; i < 6; i++) {
        mac[i] = (uint8_t)(inl(io_base + i) & 0xff);
    }

    printk("MAC: %x:%x:%x:%x:%x:%x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    uint32_t csr58 = pcnet_csr_read32(58);
    csr58 &= 0xff00;
    csr58 |= 2;
    pcnet_csr_write32(58, csr58);

    uint32_t bcr2 = pcnet_bcr_read32(2);
    bcr2 |= 0x02;
    pcnet_bcr_write32(2, bcr2);

    memset(tx_buffer, 0, sizeof(tx_buffer));
    memset(rx_buffer, 0, sizeof(rx_buffer));

    memset((uint8_t*)tx_descs, 0, sizeof(tx_descs));
    memset((uint8_t*)rx_descs, 0, sizeof(rx_descs));

    memset((uint8_t*)&init_block, 0, sizeof(init_block));

    uint16_t bcnt = -1520;
    bcnt &= 0x0fff;

    for (int i = 0; i < 8; i++) {
        rx_descs[i].buffer = (uint32_t)(rx_buffer) + (1520 * i);
        tx_descs[i].buffer = (uint32_t)(tx_buffer) + (1520 * i);
        
        rx_descs[i].bcnt = bcnt;
        rx_descs[i].must_be_f = 0xf;
        tx_descs[i].bcnt = bcnt;
        tx_descs[i].must_be_f = 0xf;
        
        rx_descs[i].own = 1;
    }

    init_block.rlen = 3;
    init_block.tlen = 3;

    init_block.rdra = (uint32_t)rx_descs;
    init_block.tdra = (uint32_t)tx_descs;

    memcpy((uint8_t*)((uint32_t)&init_block + 4), mac, 6);

    pcnet_csr_write32(1, (uint32_t)(&init_block) & 0xffff);
    pcnet_csr_write32(2, ((uint32_t)(&init_block) >> 16) & 0xffff);

    uint32_t csr0 = pcnet_csr_read32(0);
    csr0 |= 0b1 | (1 << 6);
    pcnet_csr_write32(0, csr0);

    uint32_t csr3 = pcnet_csr_read32(3);
    csr3 &= -0x700;
    pcnet_csr_write32(3, csr3);

    while(!init_done);

    csr0 = pcnet_csr_read32(0);
    csr0 &= -(0b101);
    csr0 |= 0b10;
    pcnet_csr_write32(0, csr0);

    memcpy((uint8_t*)((uint32_t)test_packet+6), mac, 6);

    memcpy(tx_buffer, test_packet, sizeof(test_packet));

    //tx_descs[0].bcnt = (uint16_t)(-(256)) & 0xfff;

    tx_descs[0].enp = 1;
    tx_descs[0].stp = 1;
    tx_descs[0].own = 1;

}