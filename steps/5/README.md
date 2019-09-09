# USER MODE HELLO WORLD
このステップではRISC-Vでuser modeプログラムを実行する方法について説明します.
前回のステップでは ELF をロードし machine mode でそのプログラムを実行しました.
その際 CSR mstatus.mpp に machime mode を設定し mret 命令を実行することで
machine mode から同じ machine mode へ遷移をさせましたが, CSR mstatus.mpp に user
mode を設定し mret 命令を実行することで machine mode から user mode
へ遷移をさせることができます. ですが, RISC-V の Physical Memory Protection(=PMP)
の初期設定では user mode, supervisor mode
では物理メモリに直接アクセスすることができません.

## Physical Memory Protection
RISC-V には Physical Memory Protection(=PMP) という物理メモリへのアクセスを制御する機能があります. PMP は hart 単位で権限モードに応じて物理メモリへのアクセス (read, write, execute) を制御することができます.
machine mode はデフォルトで物理メモリにアクセスできますが, supervisor mode, user modeはできません. machine mode にとって PMP はアクセスを制限する目的で使用され, supervisor mode, user mode にとって PMP はアクセスを取得する目的で使用されます.

### pmpaddr, pmpcfg CSR
PMP に関する CSR は pmpaddr0 - pmpaddr15 と pmp0cfg - pmp15cfg あり16の領域をそれぞれ設定可能です. レジスタの詳細は Privileged Architecture Version 1.10 (https://github.com/riscv/riscv-isa-manual/releases/download/archive/riscv-privileged-v1.10.pdf) - 3.6.1 Physical Memory Protection CSRs にありますが, 今回の実装では 0- 3GiB の領域を naturally aligned power-of-2 regions (NAPOT) を使用して 0-1GiB, 1GiB-2GiB, 2GiB-3GiB の3領域を user mode に RWX で許可します.
