# Step 2

## machine mode with `ecall` exception.
step2では RISC-V の CPU 例外を発行する命令である`ecall` の処理を実装しますが, プログラムカウンタ(=PC)がどのように遷移するのかを説明するために,  step1 の `make test` から振り返ります.
`make test` は `steps/1/Makefile` から includeされる `../../mk/m.mk`に書かれていて
```Makefile
test:
	qemu-system-riscv32 -nographic -machine sifive_u -kernel $(target)
```
となっていています. `make test` の実体は qemu の実行で `machine` (=機種)は `sifive_u`, `kernel` (=実行バイナリ)は `$(target)` が指定されていることがわかります. `$(target)` は `make` 時に作成される `m.elf` のことです. 
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
`reset_vec[8]` に命令が書かれていています. この命令を cpu_to_le32 でエンディアン変換[^1]して, `rom_add_blob_fixed_as` で, `memmap[SIFIVE_U_MROM].base` に `reset_vec` を サイズ `sizeof(reset_vec)` 分コピーするというコードになっています. `reset_vec[8]`がやろうとしていることは, 1. 現在のプログラムカウンタ(=PC)を得る, 2. PCの24-Byte先にあるDRAM_BASEの値を得る. 3. そこにジャンプする, です[^2]. これで `memmap[SIFIVE_U_DRAM].base`の値 `0x80000000` にジャンプすることがわかりました.

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
と表示され VirtAddr(=仮想アドレス)が `0x80000000` で始まっているため, この `m.elf` は `0x80000000` から動作するようにレイアウトされることがわかります. (qemuがPhysAddr(=物理アドレス)を見てレイアウトしているかコードを見るとわかると思いますがこの `m.elf` はどちらも同じであるためひとまず気にしないこととします). このメモリレイアウトを指定しているのは `riscv-probe/env/qemu-sifive_u/default.lds` というリンカスクリプトです. `riscv-mini/mk/m.mk` でこのリンカスクリプトを指定しています. では実行してみましょう.
```bash
$ make test
qemu-system-riscv32 -nographic -machine sifive_u -kernel m.elf
Hello RISC-V M-Mode.
ecall by machine mode at: 0x800000dc
```

`main.c` では riscv-probe の関数 `set_trap_fn` を使用し `trap_handler` で例外を捕捉できるようにし, `asm volatile ("ecall")` で `ecall` 命令を発行します. `ecall` 命令で発生した例外は `trap_handler` で捕捉され `mepc` を表示して終了します. `mepc` は上の実行結果では `0x800000dc` です. 

`riscv32-unknown-elf-objdump -d m.elf`

`riscv-probe/env/common/crtm.s` 



[^1]: RISC-Vはリトルエンディアンです. ホストがリトルエンディアンならば変換はありません.
[^2]: 以下のように
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

- readelf, objdump
- crt.Sの説明
- exitの実装
- m-mode, s-mode, u-modeそれぞれecall
  - m-mode, s-mode, u-modeの説明. 現在のmodeは知ることができない!