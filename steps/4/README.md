# Step 4

## user mode hello world.
step4ではuser modeでプログラムを実行しますが大きく2つのことについて説明します. 実行ファイルのロードとPhysical Memory Protection(=PMP)です. 実行ファイルのロードではELF形式のファイルの扱いとCPU cacheの操作について説明します. 

## Loading Executable
`riscv-probe/examples/user/user.c` も user modeでプログラムを実行しますがこのサンプルはmachine mode実行中にmodeをuserに切り替え, そのまま処理を継続しuser modeでプログラムを実行しています. しかし今回のstepではそれっぽく動作させるため, 1. user modeプログラムをメモリにロードし, 2. entry pointにuser modeで突入し, user modeでプログラムを実行します. プログラムをロードするために実行ファイルの形式であるELF形式について説明します.

### ELF
ELFは実行ファイル, 共有ライブラリ, オブジェクトファイルなどを格納するファイル形式の1つで, ELF Headerと必要に応じてProgram Header, Section Headerとそれらヘッダーから参照されるエントリで構成されます. 今回は実行ファイルに焦点を絞ってELFの説明をおこないます.
ELF形式の実行ファイルはELF Header, Program Header, Section Headerから構成されます[^1](1). `riscv-probe/libfemto/include/elf.h` に定義があるためこれを使用します.

#### ELF Header
```c
enum {
    EI_MAG0 = 0,
    EI_MAG1 = 1,
    EI_MAG2 = 2,
    EI_MAG3 = 3,
    EI_CLASS = 4,
    ...
};
enum {
    ELFCLASSNONE = 0,
    ELFCLASS32 = 1,
    ELFCLASS64 = 2,
    ELFCLASS128 = 3
};
typedef struct {
    Elf32_Byte  e_ident[EI_NIDENT];
    Elf32_Half  e_type;
    Elf32_Half  e_machine;
    Elf32_Word  e_version;
    Elf32_Addr  e_entry;
    Elf32_Off   e_phoff;
    Elf32_Off   e_shoff;
    Elf32_Word  e_flags;
    Elf32_Half  e_ehsize;
    Elf32_Half  e_phentsize;
    Elf32_Half  e_phnum;
    Elf32_Half  e_shentsize;
    Elf32_Half  e_shnum;
    Elf32_Half  e_shstrndx;
} Elf32_Ehdr;
```
重要なエントリのみ説明します.
`e_ident[EI_CLASS]` に `ELFCLASS32` あるいは `ELFCLASS64`が設定されています[^2](2). この値をもとにELF形式の32-bitあるいは64-bitを判定します. 今回は32-bitのRV32を扱うため`Elf32_Ehdr` を使用します.
`e_entry` はプログラムのメモリにレイアウトされた後の実行開始アドレスが格納されます.
`e_phoff`, `e_phentsize` そして `e_phnum` はProgram Headerのそれぞれファイルオフセット, サイズ, 数を表します. 同様に , `e_shoff`, `e_shentsize` そして `e_shnum` はSection Headerのそれぞれファイルオフセット, サイズ, 数を表します. `p_phentsize`, `e_shentsize` は `e_ident[EI_CLASS]` により32-bitあるいは64-bitが判定できればProgram HeaderあるいはSection Headerのサイズは決まるのでなくても問題ないエントリです.

#### Program Header
```c
typedef struct {
    Elf32_Word  p_type;
    Elf32_Off   p_offset;
    Elf32_Addr  p_vaddr;
    Elf32_Addr  p_paddr;
    Elf32_Word  p_filesz;
    Elf32_Word  p_memsz;
    Elf32_Word  p_flags;
    Elf32_Word  p_align;
} Elf32_Phdr;
```

### CPU Cache

## Physical Memory Protection


###### 1
Section HeaderはProgram Headerで参照されるセグメントをロードするためだけの目的では不要ですがobjdumpなどのtoolでは参照されます.

###### 2
`EI_ELFCLASS128` はわかりません.

Physical Memory Protectionは指定されるPhysical Address領域へのアクセスをhartごとに制限する仕組みで, machine modeではPhysical Addressへのアクセスは

Virtual Address Translaionで行われる

- riscv-probe/examples/user/user.c について
	- 同じコードをuser modeで実行してもいいけど, べつのプログラムをロードしたいよね.
- PMP+MMUを同時に使える risc-v の優位性. nested page tableが不要になるのが有利な点. ARMではMPU+MMUシステムは存在しないか? 要確認.
- PMPの説明を普通にする
- objcopy -> .o -> link. symbolをsed.
- RISC-Vのキャッシュはボードのマニュアル参照か
	- なぜキャッシュの話をしないといけないか図を書く. 

###### 5
[SiFive FE310-G000 Manual](https://sifive.cdn.prismic.io/sifive%2F4d063bf8-3ae6-4db6-9843-ee9076ebadf7_fe310-g000.pdf) `9.2 Interrupt Entry and Exit`



2-stage address translation

<https://www.embedded.com/design/mcus-processors-and-socs/4461129/Understanding-virtualization-facilities-in-the-ARMv8-processor-architecture>

<https://www.csd.uoc.gr/~hy428/reading/vm-support-ARM-may18-2016.pdf>

<https://news.mynavi.jp/article/20100928-arm_cortex-a15/>
