# Step 5

## user mode hello world.
このstepではRISC-Vでuser modeプログラムを実行する方法について説明します.

### Physical Memory Protection
RISC-VにはPhysical Memory Protection(=PMP)という機能があります. PMPはhart単位でmodeに対して物理アドレス領域へのアクセス(read, write, execute)を制御することができます[^1](1).
machine modeはdefaultで物理領域にアクセスできますが, supervisor mode, user modeはdefaultで物理領域にアクセスできません. machine modeにとってPMPはアクセスを制限する目的で使用され, supervisor mode, user modeにとってPMPはアクセスを取得する目的で使用されます.

### user mode program
このstepで使用するuser modeプログラムは, 表示するメッセージを除いて前のstepで使用した`u` ディレクトリと同じものになります. 実はこのプログラムは直接デバイスにアクセスするためアクセスする領域を限定できていません. 今回はアドレス0から3GiBの物理領域[^2](2)へのアクセス許可しuser modeプログラムを実行します. user modeプログラムのアクセス領域の限定は以降のstepで行います. 

### pmpaddr, pmpcfg CSR
PMPに関するCSRは pmpaddr0 - pmpaddr15 と pmp0cfg - pmp15cfg あり16の領域をそれぞれ設定可能です. レジスタの詳細は [Privileged Architecture Version 1.10](https://github.com/riscv/riscv-isa-manual/blob/master/release/riscv-privileged-v1.10.pdf) `3.6.1 Physical Memory Protection CSRs`にありますが, 今回の実装では 0- 3GiB の領域を `naturally aligned power-of-2 regions (NAPOT)` を使用して0-1GiB, 1GiB-2GiB, 2GiB-3Gibの3領域をuser modeにRWXで許可します[^2](2). 
pmpaddrに関してだけコードの説明をします. アドレスと領域の長さはともに2のべき乗で, アドレスは4Bアドレス, つまりバイトアドレスを4で割った(=2bit右シフトした)値(=a), 領域の長さは8で割った(=3bit右シフトした)値から1引いた値(=l)のaとlをそれぞれbit ORした値をpmpaddrに設定します.

最後に動作を確認します.

```bash
$ make
$ make run
```
PMPについての感想をまとめます[^3](3).

###### 1
PMPはoptionalです. RISC-Vの必須機能ではありません.

###### 2
なぜか 0-4GiB の1領域を設定する方法がうまく動作しませんでした. qemu RISC-Vの実装を確認する必要があります.

###### 3
個人的な感想です.
RISC-V PMPと似た機能としてARM Core-MのMPU(Memory Protection Unit)があります. ARM Core-MのMPUはMMUの置き換えという位置づけで, MPUとMMUを同時に実装することはできません. しかしRISC-VはPMPとaddress translation(RISC-VにMMUという表現がないのでこう呼びます)を同時に実装することができます. 仮想化について考えたとき, ARMのメモリアクセスの仮想化は2-stage address translationで行われますが, RISC-Vにはそれがありません. しかしPMPを使用してメモリアクセスを限定できるため完全な仮想マシンを実現することはできませんが2-stage address translationで発生するオーバヘッド無しに、かつ安価に仮想化を実現できる戦略をとったものと考えます.