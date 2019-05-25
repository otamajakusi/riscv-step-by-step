# Step 5

## user mode hello world.
このstepではRISC-Vでuser modeプログラムを実行する方法について説明します.

### Physical Memory Protection
RISC-VにはPhysical Memory Protection(=PMP)という機能があります. PMPはhart単位でmodeに応じた物理アドレス領域のアクセス(read, write, execute)を制御することができます[^1](1).
machine modeはdefaultで物理領域にアクセスできますが, supervisor mode, user modeはdefaultで物理領域にアクセスできません. machine modeにとってPMPはアクセスを制限する目的で使用され, supervisor mode, user modeにとってPMPはアクセスを取得する目的で使用されます.

### user mode program
このstepで使用するuser modeプログラムは, 表示するメッセージを除いて前のstepで使用した`u` ディレクトリと同じものになります. 実はこのプログラムは直接デバイスにアクセスするためアクセスする領域を限定できていません. 今回はすべての物理領域をアクセス許可しuser modeプログラムを実行します. user modeプログラムのアクセス領域の限定は以降のstepで行います.

###### 1
PMPはoptionalです. RISC-Vの必須機能ではありません.

PMPの説明: machine modeではdefault許可, supervisor, userではdefault不許可
PMPとvirtual address translationが存在するRISC-Vの良さ(感想)
riscv-probeのuser modeサンプル

----

Physical Memory Protectionは指定されるPhysical Address領域へのアクセスをhartごとに制限する仕組みで, machine modeではPhysical Addressへのアクセスは

Virtual Address Translaionで行われる

- riscv-probe/examples/user/user.c について
	- 同じコードをuser modeで実行してもいいけど, べつのプログラムをロードしたいよね.
- PMP+MMUを同時に使える risc-v の優位性. nested page tableが不要になるのが有利な点. ARMではMPU+MMUシステムは存在しないか? 要確認.
- PMPの説明を普通にする

2-stage address translation

<https://www.embedded.com/design/mcus-processors-and-socs/4461129/Understanding-virtualization-facilities-in-the-ARMv8-processor-architecture>

<https://www.csd.uoc.gr/~hy428/reading/vm-support-ARM-may18-2016.pdf>

<https://news.mynavi.jp/article/20100928-arm_cortex-a15/>

