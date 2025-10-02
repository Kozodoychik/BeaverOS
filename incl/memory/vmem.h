#include <stdint.h>
#include <stdbool.h>

typedef struct {
    bool present            : 1;
    bool read_write         : 1;
    bool user_supervisor    : 1;
    bool write_through      : 1;
    bool cache_disable      : 1;
    bool accessed           : 1;
    uint8_t available       : 1;
    bool page_4mb           : 1;
    uint8_t available_high  : 4;
    uint32_t address        : 20;
} page_directory_entry;

typedef struct {
    bool present           : 1;
    bool read_write        : 1;
    bool user_supervisor   : 1;
    bool write_through     : 1;
    bool cache_disable     : 1;
    bool accessed          : 1;
    bool dirty             : 1;
    bool pat               : 1;
    bool global            : 1;
    uint8_t available      : 3;
    uint32_t address       : 20;
} page_table_entry;

void paging_setup();