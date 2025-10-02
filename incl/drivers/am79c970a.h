#include <stdint.h>

#define PCNET_16_RDP 0x10
#define PCNET_16_RAP 0x12
#define PCNET_16_RESET 0x14
#define PCNET_16_BDP 0x16

#define PCNET_32_RDP 0x10
#define PCNET_32_RAP 0x14
#define PCNET_32_RESET 0x18
#define PCNET_32_BDP 0x1c

typedef struct {
    uint16_t mode;
    uint8_t reserved : 4;
    uint8_t rlen : 4;
    uint8_t reserved2 : 4;
    uint8_t tlen : 4;
    uint64_t mac : 48;
    uint16_t reserved3;
    uint64_t ladr;
    uint32_t rdra;
    uint32_t tdra;
}__attribute__((packed)) pcnet_init_block;

typedef struct {
    uint32_t buffer;
    uint16_t bcnt : 12;
    uint8_t must_be_f : 4;
    uint8_t reserved;
    uint8_t enp : 1;
    uint8_t stp : 1;
    uint8_t buff : 1;
    uint8_t crc : 1;
    uint8_t oflo : 1;
    uint8_t fram : 1;
    uint8_t err : 1;
    uint8_t own : 1;
    uint16_t mcnt : 12;
    uint8_t must_be_zero : 4;
    uint8_t rpc;
    uint8_t rcc;
    uint32_t reserved2;
}__attribute__((packed)) pcnet_rx_descriptor;

typedef struct {
    uint32_t buffer;
    uint16_t bcnt : 12;
    uint8_t must_be_f : 4;
    uint8_t reserved;
    uint8_t enp : 1;
    uint8_t stp : 1;
    uint8_t def : 1;
    uint8_t one: 1;
    uint8_t more : 1;
    uint8_t add_no_fcs : 1;
    uint8_t err : 1;
    uint8_t own : 1;
    uint16_t trc : 4;
    uint8_t reserved2 : 4;
    uint16_t tdr : 10;
    uint8_t retry : 1;
    uint8_t lcar : 1;
    uint8_t lcol : 1;
    uint8_t ex_def : 1;
    uint8_t uflo : 1;
    uint8_t buff : 1;
    uint32_t reserved3;
}__attribute__((packed)) pcnet_tx_descriptor;

void pcnet_detect();
void pcnet_init();