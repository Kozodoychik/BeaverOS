#include <memory/vmem.h>

extern enable_paging(page_directory_entry*);

page_directory_entry page_directories[1024] __attribute__((aligned(4096)));

page_table_entry page_tables_0[1024] __attribute__((aligned(4096))); 

void init_page_directories() {
    for (int i = 0;i < 1024;i++) {
        page_directories[i].present = false;
        page_directories[i].read_write = true;
        page_directories[i].user_supervisor = false;
        page_directories[i].write_through = false;
        page_directories[i].cache_disable = false;
        page_directories[i].accessed = false;
        page_directories[i].available = 0;
        page_directories[i].available_high = 0;
        page_directories[i].page_4mb = false;
        page_directories[i].address = 0;
    }
}

void identity_paging(page_table_entry* pte, uint32_t size) {
    uint32_t addr = 0;

    for (;size > 0;addr+=4096,size-=4096,pte++) {
        pte->present = true;
        pte->address = addr >> 12;
    }
}

void paging_setup() {
    init_page_directories();
    identity_paging(page_tables_0, 0x400000);

    page_directories[0].address = ((uint32_t)page_tables_0) >> 12;
    page_directories[0].user_supervisor = false;
    page_directories[0].present = true;

    enable_paging(page_directories);
}