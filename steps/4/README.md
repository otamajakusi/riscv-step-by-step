# Step 4

## user mode hello world.
step4ではuser modeでプログラムを実行しますが大きく2つのことについて説明します. 実行ファイルのロードとPhysical Memory Protection(=PMP)です. 実行ファイルのロードではELF形式のファイルの扱いとCPU cacheの操作について説明します. 

## Loading Executable
`riscv-probe/examples/user/user.c` も user modeでプログラムを実行しますがこのサンプルはmachine mode実行中にmodeをuserに切り替え, そのまま処理を継続しuser modeでプログラムを実行しています. しかし今回のstepではそれっぽく動作させるため, 1. user modeプログラムをメモリにロードし, 2. entry pointにuser modeで突入し, user modeでプログラムを実行します. プログラムをロードするために実行ファイルの形式であるELF形式について説明します.

### ELF
ELFは実行ファイル, 共有ライブラリ, オブジェクトファイルなどを格納するファイル形式の1つで, ELF Headerと必要に応じてProgram Header, Section Headerとそれらヘッダーから参照されるエントリで構成されます. 今回は実行ファイルに焦点を絞ってELFの説明をおこないます.
ELF形式の実行ファイルはELF Header, Program Header, Section Headerから構成されます[^1](1). `riscv-probe/libfemto/include/elf.h` にある定義を使用します.

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
`e_ident[EI_CLASS]` には `ELFCLASS32` あるいは `ELFCLASS64`が格納されます[^2](2). この値をもとにELF形式の32-bitあるいは64-bitを判定します. 今回は32-bitのRV32を扱うため`Elf32_Ehdr` を使用します.
`e_entry` にはプログラムがメモリにレイアウトされた後の実行開始アドレスが格納されます.
`e_phoff`, `e_phentsize` そして `e_phnum` にはProgram Headerのそれぞれファイルオフセット, サイズ, 数が格納されます. 同様に , `e_shoff`, `e_shentsize` そして `e_shnum` にはSection Headerのそれぞれファイルオフセット, サイズそして数が格納されます[^3](3).

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
Program HeaderはELF Header e_phoffで示されるファイルオフセットに配置されますが, 通常e_phoffははELF Headerの終わりを示します. Program Headerはプログラム実行に必要なエントリ(=プログラムセグメント)の情報が格納されているためこれをパースしてそのプログラムセグメントをメモリにレイアウトすればプログラムのロードは完了となります.
`p_type` にはこのプログラムセグメントのタイプ情報が格納されます. 実行プログラムのロード対象となるタイプは `PT_LOAD` です.
`p_offset` にはこのプログラムセグメントのファイルオフセットが格納されます.
`p_vaddr` にはこのプログラムセグメントをロードするアドレス, `p_filesz` にはそのサイズがそれぞれ格納されます. `p_memsz` にはプログラムセグメントに必要とされるサイズが格納されます. 以下に `p_vaddr`, `p_filesz` そして `p_memsz` の関係を示します.

```text
        +--------+ ^
        |        | |
        |  zero  | | p_memsz (プログラムセグメントに必要なサイズ)
        |        | |
        +--------+ | ^
        |        | | |
        |  data  | | | p_filesz (ELFからロード(=コピー)が必要なサイズ)
        |        | | |
p_vaddr +--------+ v v
```

ELF形式の実行ファイルのロードについてまとめると, 1. ELF Header`e_ident[EI_CLASS]` など確認, 2. ELF Header `e_phoff`, `e_phnum` からProgram Headerを取り出し, 3. Program Header `p_type` が `PT_LOAD` であることを確認, 4. メモリ領域 `max(p_filesz, p_memsz)` を `p_vaddr` アドレスから確保, 5. ELFファイル `p_offset` オフセットからアドレス `p_vaddr` にサイズ `p_filesz` コピーする. 6. `3, 4, 5`をすべての Program Headerに対して実行する.
Section Headerはプログラムのロードに不要なため説明を省略します.
readelfを使用してProgram Headerを見てみることとします.

```bash
$ riscv32-unknown-elf-readelf -l steps/3/m.elf

Elf file type is EXEC (Executable file)
Entry point 0x80000000
There are 3 program headers, starting at offset 52

Program Headers:
  Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
  LOAD           0x000094 0x80000000 0x80000000 0x00764 0x00764 R E 0x4
  LOAD           0x0007f8 0x80000764 0x80000764 0x008fc 0x008fc RW  0x4
  LOAD           0x001100 0x80001060 0x80001060 0x00000 0x01010 RW  0x10

 Section to Segment mapping:
  Segment Sections...
   00     .text .rodata
   01     .data
   02     .bss
```

`steps/3/m.elf` は3つのProgram Headerから構成されていることがわかります[^4](4). Offset, VirtAddr, FileSiz, MemSizはそれぞれ上で説明した, p_offset, p_vaddr, p_filesz, p_memszに対応します. Flgは1番目のプログラムセグメントが `RE`(Read and Execute), 2番目と3番目のプログラムセグメントが `RW`(Read and Write) であることがわかります. 下の`Section to Segment mapping:` と書かれた部分にはプログラムセグメントに格納されるデータの種別が書かれています. `.text` は命令, `.rodata`は読み込み専用のデータ, `.data`は読み書きを行うデータ, `.bss` は初期値0のデータです. `.text`, `.rodata` は1番目のプログラムセグメントなので `RE`, `.data` は2番目のプログラムセグメントなので `RW`, 同様に `.bss` も`RW`で配置されます.

さて, ELFをロードする方法は以上となりますがどこからELFファイルをロードするか考えなくてはなりません. このシステムは現時点で外部からデータを取得できません. 今回はプログラム内部にuser modeプログラムを保持し, そこからELFをロードする方法をとります.

ディレクトリ構成説明する

```bash
$ riscv32-unknown-elf-objcopy \
    -I binary -O elf32-littleriscv -B riscv m.elf m.elf.o \
    --redefine-sym _binary_m_elf_start=m_start \
    --redefine-sym _binary_m_elf_end=m_end
```




### CPU Cache

## Physical Memory Protection


###### 1
Section HeaderはProgram Headerで参照されるセグメントをロードするためだけの目的では不要ですがobjdumpなどのtoolでは参照されます.

###### 2
`EI_ELFCLASS128` はわかりません.

###### 3
`p_phentsize`, `e_shentsize` は `e_ident[EI_CLASS]` により32-bitあるいは64-bitが判定できればProgram HeaderあるいはSection Headerのサイズは決まるのでなくても問題ないエントリと言えます.

###### 4
`riscv-probe/env/default/default.lds` によりプログラムセグメントが構成されます.

----

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

