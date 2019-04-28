# Step 3

## machine mode with timer interrupt.
step3ではRISC-Vのタイマー割り込みの説明をします. step2では `ecall` exception を扱い, CSRレジスタ `mcause` と `mepc` からそれぞれ 例外/割り込み要因と例外/割り込みアドレスを取得しました. 例外/割り込みどちらも発生直後にPCを特定のアドレス(step2の `trap_vector` )に遷移させる仕組みですが, その違いは例外はCPU内部で発生, 割り込みはCPU外部のデバイスなどにより発生する, 程度の区分けでよいと思います[^1](#1).
RISC-Vのタイマーは `mtime` と `mtimecmp` の2つのレジスタから構成されていています. 一般にCPUの周波数/電圧は可変のため, 実時間のタイマーを用意するためにはタイマーをCPU内部ではなくCPU外部のメモリマップドレジスタとして実装しています[^2](#2).  

## mstatus, mip, mie,  mtime, mtimecmp
machime modeでタイマー割り込みを操作するためのレジスタがいくつかあります.
- `mstatus MIE`: `mstatus` レジスタ  `MIE` フィールド. 0を設定すると, すべての割り込みを不許可にします. 1を設定すると `mie` で許可された割り込みを許可します.
- `mip MTIP`: `mip`レジスタ `MTIP` フィールド. `mip` レジスタは割り込みが発生していることを示します. (TODO 1を設定してクリア？いる). 
-  `mie MTIE`: `mie` レジスタ `MTIE` フィールド. `MTIE` (=machime mode timer enable) に1を設定すると, machine modeでタイマー割り込みが許可となります.
- `mtime`: 64bitのフリーランニングカウンタです. カウンタと書きましたが `mtime must run at constant frequency` と説明があるため, 実時間と同じタイミングでカウントアップをします.
- `mtimecmp`: `mtime` >= `mtimecmp` となった場合, 割り込み発生条件が満たされます. RV32システムが `mtimecmp` を更新する場合, 書き込みが最大32bitのため以下のように実装するように説明があります[^2](#2).

```asm
# New comparand is in a1:a0.
li t0, -1
sw t0, mtimecmp # No smaller than old value.
sw a1, mtimecmp+4 # No smaller than new value.
sw a0, mtimecmp # New value.
```

## CLINT, PLIC
RISC-Vの割り込みは CLINT(Core Level Interruptor) あるいは PLIC (Platform Level Interrupt Controller) を経由して処理されます. タイマー割り込みはCLINTで処理されます.
[SiFive FE310-G000 Manual](https://sifive.cdn.prismic.io/sifive%2F4d063bf8-3ae6-4db6-9843-ee9076ebadf7_fe310-g000.pdf) `11.1 E31 CLINT Address Map`によれば `mtimecmp for hart0` はアドレス `0x02004000`, `mtime` はアドレス `0x0200BFF8` です. `hart` は RICS-V の hardware thread のことです. SiFive FE310-G000はhartが1つなのでhart0のみが定義されています[^3](3).

## implementation
riscv-qemu/include/hw/riscv/sifive_clint.h(51) に以下の値があります. 名前から想像してこれがtimebaseであると予測します[^4](4).
```c
    SIFIVE_CLINT_TIMEBASE_FREQ = 10000000
```
main.cは上記の説明をそのまま実装したものですので説明は省略しますが, `riscv-probe/env/common/crtm.s` にある `mret` だけ説明します. `mret`命令は, 1. `mstatus.MPP` を mode に設定し, 2. `mstatus.MPIE` を `mstatus.MIE` にコピーし, 3. pc を mepc に設定します[^5](5). この動作により割り込みが発生したPCから動作が再開されます.

###### 1
`ソフトウェア割り込み` とは特定の命令によって発生する割り込みのことですが, `ecall` 命令の説明には `environment-call-from-M-mode exception` と説明されているように`exception` という言葉を使っています.

###### 2
[Privileged Architecture Version 1.10](https://github.com/riscv/riscv-isa-manual/blob/master/release/riscv-privileged-v1.10.pdf)) `3.1.15 Machine Timer Registers (mtime and mtimecmp)` 参照

###### 3
Hart ID Registerの定義によればマルチプロセッサシステムにおいてIDは連続の数値でなくてもかまいませんが少なくとも0がhart IDとして割り当てられている必要があります.

###### 4
timebaseはtimerの分解能ですがこの値が大きすぎると64-bit timerの桁あふれ(wrap around)するまでにかかる時間が小さくなるので適切な値でなければなりません. 例: pico秒単位の分解能であれば64-bitが桁あふれするのに584年程度かかります.

###### 5
[SiFive FE310-G000 Manual](https://sifive.cdn.prismic.io/sifive%2F4d063bf8-3ae6-4db6-9843-ee9076ebadf7_fe310-g000.pdf) `9.2 Interrupt Entry and Exit`

