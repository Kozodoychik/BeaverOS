ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)

SECTIONS {
    . = 1M;

    .boot : {
        KEEP(*(.multiboot_header))
    }

    .text : {
        *(.text)
    }

}