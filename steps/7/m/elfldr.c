#include <stdio.h>
#include <string.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#include "elf.h"
#pragma GCC diagnostic pop

//#define ENABLE_DUMP

#if defined(ENABLE_DUMP)
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
#endif

static int check_ehdr(const Elf32_Ehdr* ehdr)
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
    return 0;
}

void* load_elf(const void *src)
{
    const Elf32_Ehdr* ehdr = src;
    const Elf32_Phdr* phdr = (const Elf32_Phdr*)(ehdr + 1);
#if defined(ENABLE_DUMP)
    dump_elf(ehdr, phdr);
#endif
    if (check_ehdr(ehdr) < 0) {
        return NULL;
    }
    for (int i = 0; i < ehdr->e_phnum; i ++) {
        if (phdr[i].p_type == PT_LOAD && phdr[i].p_filesz) {
            const void* from = src + phdr[i].p_offset;
            void* to = (void*)phdr[i].p_vaddr;
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
