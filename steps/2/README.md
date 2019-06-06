# Step 2

## machine mode with ecall exception.
step2では RISC-V の CPU 例外を発行する命令である`ecall` の処理を実装しますが, プログラムカウンタ(=PC)がどのように遷移するのかを説明するために,  step1 の `make run` から振り返ります.
`make run` は `steps/1/Makefile` から includeされる `../../mk/m.mk`に書かれていて

```Makefile
test:
	qemu-system-riscv32 -nographic -machine sifive_u -kernel $(target)
```
となっていています. `make run` の実体は qemu の実行で `machine` (=機種)は `sifive_u`, `kernel` (=実行バイナリ)は `$(target)` が指定されていることがわかります. `$(target)` は `make` 時に作成される `m.elf` のことです. 
ちなみに `m.mk` の `m` は machine mode の `m` です. RISC-Vの mode は権限の高い順に machine mode, supervisor mode, user mode が存在します.
qemu のソースコードを見てみます. riscv-qemu/hw/riscv/sifive_u.c(55) に以下の記述があります.

```c
static const struct MemmapEntry {
    hwaddr base;
    hwaddr size;
} sifive_u_memmap[] = {
    [SIFIVE_U_DEBUG] =    {        0x0,      0x100 },
    [SIFIVE_U_MROM] =     {     0x1000,    0x11000 },
    [SIFIVE_U_CLINT] =    {  0x2000000,    0x10000 },
    [SIFIVE_U_PLIC] =     {  0xc000000,  0x4000000 },
    [SIFIVE_U_UART0] =    { 0x10013000,     0x1000 },
    [SIFIVE_U_UART1] =    { 0x10023000,     0x1000 },
    [SIFIVE_U_DRAM] =     { 0x80000000,        0x0 },
    [SIFIVE_U_GEM] =      { 0x100900FC,     0x2000 },
};
```
`SIFIVE_U_MROM` と `SIFIVE_U_DRAM` に注目します. 
`MROM` Mask ROMのことですが [SiFive FE310-G000 Manual](https://sifive.cdn.prismic.io/sifive%2F4d063bf8-3ae6-4db6-9843-ee9076ebadf7_fe310-g000.pdf) `6.2 Reset and Trap Vectors` によれば reset 時 0x1000 から最初の命令を取り出すとあるので 0x1000 (=SIFIVE_U_MROM) から実行が開始されると考えてよさそうです. その後 0x20000 の OTP 領域にジャンプするとありますが, qemu の実装 hw/riscv/sifive_u.c(431) は以下のようになっています.

```c
    /* reset vector */
    uint32_t reset_vec[8] = {
        0x00000297,                    /* 1:  auipc  t0, %pcrel_hi(dtb) */
        0x02028593,                    /*     addi   a1, t0, %pcrel_lo(1b) */
        0xf1402573,                    /*     csrr   a0, mhartid  */
#if defined(TARGET_RISCV32)
        0x0182a283,                    /*     lw     t0, 24(t0) */
#elif defined(TARGET_RISCV64)
        0x0182b283,                    /*     ld     t0, 24(t0) */
#endif
        0x00028067,                    /*     jr     t0 */
        0x00000000,
        memmap[SIFIVE_U_DRAM].base, /* start: .dword DRAM_BASE */
        0x00000000,
                                       /* dtb: */
    };

    /* copy in the reset vector in little_endian byte order */
    for (i = 0; i < sizeof(reset_vec) >> 2; i++) {
        reset_vec[i] = cpu_to_le32(reset_vec[i]);
    }
    rom_add_blob_fixed_as("mrom.reset", reset_vec, sizeof(reset_vec),
                          memmap[SIFIVE_U_MROM].base, &address_space_memory);
```
`reset_vec[8]` に命令が書かれていています. この命令を cpu_to_le32 でエンディアン変換[^1](#1)して, `rom_add_blob_fixed_as` で, `memmap[SIFIVE_U_MROM].base` に `reset_vec` を サイズ `sizeof(reset_vec)` 分コピーするというコードになっています. `reset_vec[8]`がやろうとしていることは, 1. 現在のプログラムカウンタ(=PC)を得る, 2. PCの24-Byte先にあるDRAM_BASEの値を得る. 3. そこにジャンプする, です[^2](#2). これで `memmap[SIFIVE_U_DRAM].base`の値 `0x80000000` にジャンプすることがわかりました.

ようやくここからstep2です. `m.elf` のメモリレイアウトを見てみましょう.

```bash
$ cd riscv-mini/steps/2
$ make
$ riscv32-unknown-elf-readelf -l m.elf
...
Program Headers:
  Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
  LOAD           0x000094 0x80000000 0x80000000 0x005c4 0x005c4 R E 0x4
  LOAD           0x000658 0x800005c4 0x800005c4 0x00a9c 0x00a9c RW  0x4
  LOAD           0x001100 0x80001060 0x80001060 0x00000 0x01010 RW  0x10
...
```
と表示されます. この `m.elf` は VirtAddr (=仮想アドレス)が`0x80000000` からレイアウトされることがわかります[^3](#3). このメモリレイアウトを指定しているのは `riscv-probe/env/qemu-sifive_u/default.lds` というリンカスクリプトです. `riscv-mini/mk/m.mk` でこのリンカスクリプトを指定しています. 

```bash
$ riscv32-unknown-elf-objdump -d m.elf
```

として`m.elf` の disassemble を先頭から見てみます.

```asm
80000000 <_start>:
80000000:       00000297                auipc   t0,0x0
80000004:       02c28293                addi    t0,t0,44 # 8000002c <trap_vector>
80000008:       30529073                csrw    mtvec,t0
```
まず最初の3行で, `trap_vector`を設定します. `trap_vector` は例外/割り込みが発生した際にCPUが実行を開始するアドレスです.

```asm
8000000c:       f14022f3                csrr    t0,mhartid
80000010:       02aa                    slli    t0,t0,0xa
80000012:       00001117                auipc   sp,0x1
80000016:       45e10113                addi    sp,sp,1118 # 80001470 <stacks+0x400>
8000001a:       9116                    add     sp,sp,t0
8000001c:       f1402573                csrr    a0,mhartid
80000020:       e119                    bnez    a0,80000026 <park>
80000022:       0fe0006f                j       80000120 <libfemto_start_main>
```
この `libfemto_start_main` から `main` が呼び出されます. 

```asm
80000026 <park>:
80000026:       10500073                wfi
8000002a:       bff5                    j       80000026 <park>

8000002c <trap_vector>:
8000002c:       7139                    addi    sp,sp,-64
8000002e:       c006                    sw      ra,0(sp)
80000030:       c22a                    sw      a0,4(sp)
80000032:       c42e                    sw      a1,8(sp)
80000034:       c632                    sw      a2,12(sp)
80000036:       c836                    sw      a3,16(sp)
80000038:       ca3a                    sw      a4,20(sp)
8000003a:       cc3e                    sw      a5,24(sp)
8000003c:       ce42                    sw      a6,28(sp)
8000003e:       d046                    sw      a7,32(sp)
80000040:       d216                    sw      t0,36(sp)
80000042:       d41a                    sw      t1,40(sp)
80000044:       d61e                    sw      t2,44(sp)
80000046:       d872                    sw      t3,48(sp)
80000048:       da76                    sw      t4,52(sp)
8000004a:       dc7a                    sw      t5,56(sp)
8000004c:       de7e                    sw      t6,60(sp)
8000004e:       850a                    mv      a0,sp
80000050:       342025f3                csrr    a1,mcause
80000054:       34102673                csrr    a2,mepc
80000058:       114000ef                jal     ra,8000016c <trap_handler>
...
```
`trap_vector` はSP(=Stack Pointer)に例外/割り込みが発生した際のregisterを保存し, sp(Stack Pointer), mcause(例外/割り込み要因), mepc(例外/割り込みアドレス) を引数として `trap_handler`を呼び出します. mcause, mepcはそれぞれRISC-VのCSRレジスタ`mcause`, `mepc`から`csrr`命令で取り出します. ソースコードは `riscv-probe/env/common/crtm.s` です.
では `m.elf` を実行してみましょう.

```bash
$ make run
qemu-system-riscv32 -nographic -machine sifive_u -kernel m.elf
Hello RISC-V M-Mode.
ecall by machine mode at: 0x800000dc
```

`main.c` を見てみます. `main.c` では riscv-probe の関数 `set_trap_fn` を使用し `handler` で例外を捕捉できるようにしたあと `asm volatile ("ecall")` で `ecall` 命令を発行します. `ecall` 命令で発生した例外は `handler` で捕捉され `mepc` を表示して終了します. `mepc` は上の実行結果では `0x800000dc` です. `0x800000dc` 周辺を disassemble してみます.

```bash
$ riscv32-unknown-elf-objdump -d m.elf | grep 800000dc -B 10

800000c4 <main>:
800000c4:       1141                    addi    sp,sp,-16
800000c6:       00000517                auipc   a0,0x0
800000ca:       49250513                addi    a0,a0,1170 # 80000558 <_text_end+0x48>
800000ce:       c606                    sw      ra,12(sp)
800000d0:       2ad9                    jal     800002a6 <puts>
800000d2:       00000517                auipc   a0,0x0
800000d6:       fcc50513                addi    a0,a0,-52 # 8000009e <handler>
800000da:       2061                    jal     80000162 <set_trap_fn>
800000dc:       00000073                ecall
```

`0x800000dc` のアドレスに `ecall` 命令があることがわかります.

ではまとめます.

1. 0x1000 (=reset vector)から実行を開始し, 0x80000000にジャンプ.
2. 0x80000000 の先頭で `trap_vector` を設定し, mainにジャンプ.
3. mainで `trap_vector` から呼び出される関数 `handler` を設定し ecall 命令を発行
4. CPUは trap_vector に遷移し, `handler` を呼び出す.
5. exitを呼び出し終了[4](#4)

###### 1
RISC-Vはリトルエンディアンです. ホストがリトルエンディアンならば変換はありません.

###### 2
以下のように
```c
        0x00000297,                    /* 1:  auipc  t0, %pcrel_hi(start) */` 
        0x01828293,                    /*     addi   t0, t0, %pcrel_lo(1b) */
        0xf1402573,                    /*     csrr   a0, mhartid  */
#if defined(TARGET_RISCV32)
        0x0002a283,                    /*     lw     t0, 0(t0) */
#elif defined(TARGET_RISCV64)
        0x0002b283,                    /*     ld     t0, 0(t0) */
#endif
        0x00028067,                    /*     jr     t0 */
        0x00000000,
        memmap[SIFIVE_U_DRAM].base, /* start: .dword DRAM_BASE */
        0x00000000,
```
として `start:` のアドレスをとれば24 byteのオフセットも不要になりすっきりします. `addi   a1, t0, %pcrel_lo(1b)` も何をやりたいのかわかりません. あと `%pcrel_lo` は [RISC-V Assembly Programmer's Manual](https://github.com/riscv/riscv-asm-manual/blob/master/riscv-asm.md)の説明によれば `PC-relative (LO12)` だけですが, あまり見たことのない動きをします. 別の機会に説明します.

###### 3
qemuがVirtAddrもしくはPhysAddr(=物理アドレス)のどちらを見てレイアウトしているかはコードを見るとわかると思いますがこの `m.elf` はどちらも同じであるためひとまず気にしないこととします.

###### 4
exitは別の機会に説明します.