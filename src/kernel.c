#include <stdint.h>
#include "stdio.h"
#include "cassert.h"
#include "memory/memory.h"
#include "vga/vga.h"
#include "hal.h"
#include "io/io.h"

#ifndef size_t
typedef unsigned int size_t;
#endif

void putsc(char* s, size_t count)
{
    for (int i = 0; count > i; i++)
    {
        putc(*s);
        s++;
    }
}

uint8_t* _bss_start;
uint8_t* __end;

struct RSDPDescriptor {
    char signature[8];
    uint8_t checksum;
    char OEMID[6];
    uint8_t revision;
    uint32_t rsdt_address;
} __attribute__((packed));

struct RSDPDescriptor20 {
    struct RSDPDescriptor firstPart;

    uint32_t length;
    uint64_t XsdtAddress;
    uint8_t ExtendedChecksum;
    uint8_t reserved[3];
} __attribute__((packed));

struct RSDPDescriptor* find_rsdp_in_ebda()
{
    // The EBDA starts at 0x000E0000 and can extend up to 0x000FFFFF.
    uint8_t* ebda_base = (uint8_t*)0x000E0000;
    uint8_t* ebda_end = (uint8_t*)0x000FFFFF;

    // The RSDP signature we are looking for
    char rsdp_signature[] = "RSD PTR ";

    // Search for the RSDP signature in the EBDA
    for (uint8_t* ptr = ebda_base; ptr < ebda_end - sizeof(struct RSDPDescriptor); ptr += 16)
    {
        if (memcmp(ptr, rsdp_signature, sizeof(rsdp_signature) - 1) == 0)
        {
            // Found the RSDP, validate checksum
            struct RSDPDescriptor* rsdp = (struct RSDPDescriptor*)ptr;
            uint8_t checksum = 0;
            for (size_t i = 0; i < sizeof(struct RSDPDescriptor); ++i)
            {
                checksum += ((uint8_t*)rsdp)[i];
            }

            // If the checksum is valid, return the pointer to the RSDP
            if (checksum == 0)
            {
                return rsdp;
            }
        }
    }

    // RSDP not found in EBDA
    return NULL;
}

struct ACPISDTHeader {
    char Signature[4];
    uint32_t Length;
    uint8_t Revision;
    uint8_t Checksum;
    char OEMID[6];
    char OEMTableID[8];
    uint32_t OEMRevision;
    uint32_t CreatorID;
    uint32_t CreatorRevision;
} __attribute__((packed));

#define MAX_SDTS 22
struct RSDT {
    struct ACPISDTHeader h;
    uint32_t PointerToOtherSDT[1];
};

#ifndef bool
typedef unsigned char bool;
#endif

#define true 1
#define false 0

struct MADT {
    char Signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char OEMID[6];
    char OEMTableID[8];
    uint32_t OEMRevision;
    uint32_t CreatorID;
    uint32_t CreatorRevision;
    uint32_t LocalAPICAddress;
    uint32_t Flags; 
} __attribute__((packed));

void *findFACP(void *RootSDT)
{
    struct RSDT *rsdt = (struct RSDT *) RootSDT;
    int entries = (rsdt->h.Length - sizeof(rsdt->h)) / 4;
 
    for (int i = 0; i < entries; i++)
    {
        struct ACPISDTHeader *h = (struct ACPISDTHeader *) rsdt->PointerToOtherSDT[i];
        if (!strncmp(h->Signature, "FACP", 4))
            return (void *) h;
    }
 
    // No FACP found
    return NULL;
}

struct MATD_APIC_HEADER {
    uint8_t entry_type;
    uint8_t record_length;
};

struct PL_APIC
{
    struct MATD_APIC_HEADER h;
    uint8_t ACPI_CPU_ID;
    uint8_t APIC_ID;
    uint32_t flags;
};

struct IO_APIC
{
    struct MATD_APIC_HEADER h;
    uint8_t IO_APIC_ID;
    uint8_t reserved;
    uint32_t IO_APIC_ADDR;
    uint32_t GLOBAL_SYSTEM_INTERRUPT_BASE;
};

struct IO_APIC_ISO
{
    struct MATD_APIC_HEADER h;
    uint8_t BUS_SOURCE;
    uint8_t IRQ_SOURCE;
    uint32_t GLOBAL_SYSTEM_INTERRUPT;
    uint16_t FLAGS;
};

struct IO_APIC_NMI_SOURCE
{
    struct MATD_APIC_HEADER h;
    uint8_t ACPI_CPU_ID;
    uint16_t FLAGS;
    uint8_t LINT;
};

struct LAPIC_ADDRESS_OVERRIDE
{
    uint16_t reserved;
    uint64_t LAPIC_PHYS_ADDR
};

struct PROCESSOR_LOCAL_X2APIC
{
    uint16_t reserved;
    uint32_t PROCESSOR_LOCAL_X2APIC_ID;
    uint32_t FLAGS;
    uint32_t ACPI_ID;
};

struct MATD
{
    struct ACPISDTHeader h;
    uint32_t lapic_addr;
    uint32_t flags;
};


void *findMATD(void *RootSDT)
{
    struct RSDT *rsdt = (struct RSDT *) RootSDT;
    int entries = (rsdt->h.Length - sizeof(rsdt->h)) / 4;
 
    for (int i = 0; i < entries; i++)
    {
        struct ACPISDTHeader *h = (struct ACPISDTHeader *) rsdt->PointerToOtherSDT[i];
        if (!strncmp(h->Signature, "APIC", 4))
            return (void *) h;
    }
 
    // No MATD found
    return NULL;
}

bool check_acpi_checksum(struct RSDT* _tableHeader)
{
    struct ACPISDTHeader *tableHeader = &_tableHeader->h;
    unsigned char sum = 0;
 
    for (int i = 0; i < tableHeader->Length; i++)
    {
        sum += ((char *) tableHeader)[i];
    }
 
    return sum == 0;
}

void print_acpi_header(struct RSDT* _header)
{
    struct ACPISDTHeader* header = &_header->h;

    char signature[5];
    memcpy(signature, header->Signature, 4);
    signature[4] = '\0';

    printf("RSDT Signature: %s\n", signature);
    printf("RSDT Length: %u\n", header->Length);
    printf("RSDT Revision: %u\n", header->Revision);
    printf("RSDT Checksum: %u\n", header->Checksum);
    
    char oem_id[7];
    memcpy(oem_id, header->OEMID, 6);
    oem_id[6] = '\0';
    printf("RSDT OEMID: ");
    putsc(oem_id, 6);
    printf("\n");

    char oem_table_id[9];
    memcpy(oem_table_id, header->OEMTableID, 8);
    oem_table_id[8] = '\0';
    printf("RSDT OEMTableID: ");
    putsc(oem_table_id, 8);
    printf("\n");

    printf("RSDT OEMRevision: %u\n", header->OEMRevision);
    printf("RSDT CreatorID: %u\n", header->CreatorID);
    printf("RSDT CreatorRevision: %u\n", header->CreatorRevision);
}

void kernel_main()
{
    clear_screen();
    disable_cursor();

    puts("Loading sillyOS!\n");

    // Clear bss section.
    memset(_bss_start, 0, __end - _bss_start);
    puts("BSS section got cleared.\n");

    HAL_Init();

    // Time to find it

    printf("Looking for RSDP...\n");
    struct RSDPDescriptor* rsdp = find_rsdp_in_ebda();

    if (rsdp)
    {
        printf("RSDP found at address: 0x%x\n", rsdp->rsdt_address);
    } else {
        printf("RSDP not found.\nHanging.\n");
        __asm__ __volatile__("cli");
        __asm__ __volatile__("hlt");
        for (;;);
    }

    {
        printf("RSDP Data:\n");

        printf("RSDP Signature: ");
        putsc(rsdp->signature, 8);
        putc('\n');

        puts("RSDP OEMID: ");
        putsc(rsdp->OEMID, sizeof(rsdp->OEMID));
        putc('\n');

        printf("RSDP checksum: %x\n", (int)rsdp->checksum);
        printf("RSDP Revision: %x\n", (int)rsdp->revision); 
    }

    printf("Loading RSDT...\n");

    struct RSDT* rsdt = (struct RSDT*)rsdp->rsdt_address;

    print_acpi_header(rsdt);

    //if (check_acpi_checksum(*rsdt) == true)
    //{
    //    printf("Invalid RSDT checksum.\n");
    //    for (;;);
    //}   

    // struct MADT *madt = findMADT(&rsdp);
    void* facp = findFACP(rsdt);
    if(facp == NULL)
    {
        printf("FACP not found.\n");
        for (;;);
    }

    void* matd = (struct *MATD)findMATD(rsdt);
    if (matd == NULL)
    {
        printf("MATD not found.\n");
        for (;;);
    }

    clear_screen();
    // print_acpi_header(matd);

    printf("Loading GUI!\n");


    for ( ;; );
}