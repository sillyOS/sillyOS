ENTRY(kernel_main)
OUTPUT_FORMAT("binary")
phys = 0x9000;

SECTIONS
{
    . = phys;

    
    .text               : { __text_start = .;       *(.text)    }
    .data               : { __data_start = .;       *(.data)    }
    .rodata             : { __rodata_start = .;     *(.rodata)  }
    .bss                : { __bss_start = .;        *(.bss)     }
    
    __end = .;
}

