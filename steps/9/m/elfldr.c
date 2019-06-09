#include <stdio.h>
#include <string.h>
#include "elfldr.h"
#include "vm.h"

//#define ENABLE_DUMP

static void dump_phdr(const Elf32_Phdr* phdr)
{
	printf("%08x(p_type)\n",    phdr->p_type);
	printf("%08x(p_offset)\n",  phdr->p_offset);
	printf("%08x(p_vaddr)\n",   phdr->p_vaddr);
	printf("%08x(p_paddr)\n",   phdr->p_paddr);
	printf("%08x(p_filesz)\n",  phdr->p_filesz);
	printf("%08x(p_memsz)\n",   phdr->p_memsz);
	printf("%08x(p_flags)\n",   phdr->p_flags);
	printf("%08x(p_align)\n",   phdr->p_align);
}

static void dump_ehdr(const Elf32_Ehdr* ehdr)
{
	printf("%02x%02x%02x%02x(e_ident)\n",   ehdr->e_ident[0],
                                            ehdr->e_ident[1],
                                            ehdr->e_ident[2],
                                            ehdr->e_ident[3]);
	printf("%04x(e_type)\n",                ehdr->e_type);
	printf("%04x(e_machine)\n",             ehdr->e_machine);
	printf("%08x(e_version)\n",             ehdr->e_version);
	printf("%08x(e_entry)\n",               ehdr->e_entry);
	printf("%08x(e_phoff)\n",               ehdr->e_phoff);
	printf("%08x(e_shoff)\n",               ehdr->e_shoff);
	printf("%08x(e_flags)\n",               ehdr->e_flags);
	printf("%04x(e_ehsize)\n",              ehdr->e_ehsize);
	printf("%04x(e_phentsize)\n",           ehdr->e_phentsize);
	printf("%04x(e_phnum)\n",               ehdr->e_phnum);
	printf("%04x(e_shentsize)\n",           ehdr->e_shentsize);
	printf("%04x(e_shnum)\n",               ehdr->e_shnum);
	printf("%04x(e_shstrndx)\n",            ehdr->e_shstrndx);
}

static void dump_elf(const Elf32_Ehdr* ehdr, const Elf32_Phdr* phdr)
{
    dump_ehdr(ehdr);
    for (int i = 0; i < ehdr->e_phnum; i ++) {
        printf("phdr %d----------\n", i);
        dump_phdr(phdr + i);
    }
}

static int check_phdr(const Elf32_Phdr* phdr, size_t size)
{
    if (phdr->p_type != PT_LOAD) {
        return 0; // do nothing
    }
    if (phdr->p_filesz > size ||
        phdr->p_offset >= size) {
        return -1; // illegal p_filesz or p_offset
    }
    if (phdr->p_vaddr + phdr->p_filesz < phdr->p_vaddr) {
        return -1; // wrapped around
    }
    if (phdr->p_offset + phdr->p_filesz < phdr->p_offset) {
        return -1; // wrapped around
    }
    // TODO: p_memsz should be checked.
    return 0;
}

static int check_ehdr(const Elf32_Ehdr* ehdr, size_t size)
{
    if (ehdr->e_ident[EI_CLASS] != ELFCLASS32) {
        printf("error: e_ident[EI_CLASS] %x\n", ehdr->e_ident[EI_CLASS]);
        return -1;
    }
    if (ehdr->e_type != ET_EXEC) {
        printf("error: e_type %x\n", ehdr->e_type);
        return -1;
    }
    if (ehdr->e_machine != EM_RISCV) {
        printf("error: e_machine %x\n", ehdr->e_machine);
        return -1;
    }
    const Elf32_Phdr* phdr = (const Elf32_Phdr*)(ehdr + 1);
    for (int i = 0; i < ehdr->e_phnum; i ++) {
        if (check_phdr(&phdr[i], size) < 0) {
            return -1;
        }
    }
    return 0;
}

void* load_elf(const void *elf, size_t size, uintptr_t pa)
{
    const Elf32_Ehdr* ehdr = check_elf(elf, size);
    const Elf32_Phdr* phdr = (const Elf32_Phdr*)(ehdr + 1);
    if (ehdr == NULL) {
        return NULL;
    }
    for (int i = 0; i < ehdr->e_phnum; i ++) {
        if (phdr[i].p_type == PT_LOAD && phdr[i].p_filesz) {
            const void* from = elf + phdr[i].p_offset;
            // FIXME: pa should have space of p_filesz.
            void* to = (void*)(phdr[i].p_vaddr + pa);
#if defined(ENABLE_DUMP)
            printf("copying (%dth) %p --> %p (sz:%x)\n", i, from, to, phdr[i].p_filesz);
#endif
            // FIXME:
            // make sure from and to addresses is not illegal.
            // make sure both W & E flags are not set.
            memcpy(to, from, phdr[i].p_filesz);
        }
    }
    // FIXME:
    // make sure e_entry is not illegal.
    return (void*)ehdr->e_entry;
}

const Elf32_Ehdr* check_elf(const void *elf, size_t size)
{
    const Elf32_Ehdr* ehdr = elf;
    const Elf32_Phdr* phdr = (const Elf32_Phdr*)(ehdr + 1);
#if defined(ENABLE_DUMP)
    dump_elf(ehdr, phdr);
#endif
    if (check_ehdr(ehdr, size) < 0) {
        return NULL;
    }
    return ehdr;
}

const Elf32_Phdr* get_phdr_from_va(
        const Elf32_Ehdr* ehdr, uint32_t va, int read, int write, int exec)
{
    const Elf32_Phdr* phdr = (const Elf32_Phdr*)(ehdr + 1);
    uint32_t flags = (read ? PF_R : 0) |
                     (write ? PF_W : 0) |
                     (exec ? PF_X : 0);
    for (int i = 0; i < ehdr->e_phnum; i ++) {
        if (phdr[i].p_type == PT_LOAD &&
            (phdr[i].p_flags & flags) == flags &&
            va >= phdr[i].p_vaddr &&
            va < phdr[i].p_vaddr + phdr[i].p_memsz) {
            return &phdr[i];
        }
    }
    return NULL;
}

// load page data from program segment located at va.
int load_program_segment(
        const Elf32_Ehdr* ehdr, const Elf32_Phdr* phdr,
        uint32_t va, const union sv32_pte* ptes1st,
        int validate_page)
{
    uint32_t va_page_aligned = va & ~(PAGE_SIZE - 1);
    uint32_t va_page_offset = (va - va_page_aligned) & ~(PAGE_SIZE - 1);
    uint32_t size = phdr->p_filesz - va_page_offset;
    size = size > PAGE_SIZE ? PAGE_SIZE : size;
    const void* from = (const char*)ehdr + phdr->p_offset + va_page_offset;
    uint64_t pa = va_to_pa(ptes1st, va, validate_page);
    void* to = (void*)(uintptr_t)(pa);
    printf("copying %p --> %p (sz:%x)\n", from, to, size);
    memcpy(to, from, size);
    return 0;
}
