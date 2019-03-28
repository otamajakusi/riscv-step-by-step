#include <stdio.h>
#include <string.h>
#include "elf.h"
#include "arch/riscv/encoding.h"
#include "arch/riscv/machine.h"

static void dump_phdr(const Elf32_Phdr* phdr) {
	printf("%08x(p_type)\n",    phdr->p_type);
	printf("%08x(p_offset)\n",  phdr->p_offset);
	printf("%08x(p_vaddr)\n",   phdr->p_vaddr);
	printf("%08x(p_paddr)\n",   phdr->p_paddr);
	printf("%08x(p_filesz)\n",  phdr->p_filesz);
	printf("%08x(p_memsz)\n",   phdr->p_memsz);
	printf("%08x(p_flags)\n",   phdr->p_flags);
	printf("%08x(p_align)\n",   phdr->p_align);
}

static void dump(const Elf32_Ehdr* ehdr, const Elf32_Phdr* phdr) {
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
    for (int i = 0; i < ehdr->e_phnum; i ++) {
        printf("phdr %d----------\n", i);
        dump_phdr(phdr + i);
    }
}

static int ehdr_check(const Elf32_Ehdr* ehdr) {
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

int load_elf(void *dst, const void *src) {
    printf("dst %p, src %p\n", dst, src);
    const Elf32_Ehdr* ehdr = src;
    const Elf32_Phdr* phdr = (const Elf32_Phdr*)(ehdr + 1);
    dump(ehdr, phdr);
    if (ehdr_check(ehdr) < 0) {
        return -1;
    }
    for (int i = 0; i < ehdr->e_phnum; i ++) {
        if (phdr[i].p_type == PT_LOAD && phdr[i].p_filesz) {
            const void* from = src + phdr[i].p_offset;
            void* to = phdr[i].p_vaddr;
            printf("copying (%dth) %p --> %p (sz:%x)\n", i, from, to, phdr[i].p_filesz);
            memcpy(to, from, phdr[i].p_filesz);
        }
    }
    //asm volatile ("j %[entry]" : :[entry]"r"(entry));
	pmp_allow_all();
    typedef void (*fptr)(void);
    fptr entry = ehdr->e_entry;
    printf("entry %p\n", entry);
    mode_set_and_jump(PRV_U, entry);
    return 0;
}

