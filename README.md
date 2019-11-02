[技術書典7 RISC-V step-by-step サポート情報はこちら](../v1.0/SUPPORT.md)
# はじめに
小さなオリジナルの kernel を実装しながら RISC-V
のアーキテクチャを理解することを目的として書かれています.
ステップと呼ばれる章を順番に進めていくことで, 次第に実装が kernel
に近づいていく様子が分かります.
ステップ間の差分はできるだけ小さくなるように実装し, いつの間にか kernel
ができた, を目指します. CPU アーキテクチャを理解するためにはその CPU
アーキテクチャを使って自ら実装してみるのが一番ですが, その実装の対象として
kernel は最も適した題材だと考えます.

まず最初のステップで環境を構築し, 以降のステップで, 割り込み/例外のハンドリング,
タイマー割り込み, 実行ファイルのロード, ユーザーモード, システムコール,
仮想アドレス変換, 複数ユーザーモード, 遅延ロード, タスク状態管理, スレッド,
最後にミューテックスについて説明します.

## [MACHINE MODE HELLO WORLD](steps/1/README.md)
## [MACHINE MODE WITH ECALL EXCEPTION](steps/2/README.md)
## [MACHINE MODE WITH TIMER INTERRUPT](steps/3/README.md)
## [EXECUTABLE LOADING](steps/4/README.md)
## [USER MODE HELLO WORLD](steps/5/README.md)
## [USER MODE HELLO WORLD WITH SYSTEM CALL](steps/6/README.md)
## [VIRTUAL ADDRESS / PAGE PROTECTION](steps/7/README.md)
## [MULTIPLE USER MODE](steps/8/README.md)
## [LAZY LOADING](steps/9/README.md)
## [CREATED, READY, RUNNING AND TERMINATED - TASK STATE](steps/10/README.md)
## [BLOCKED - TASK STATE](steps/11/README.md)
## [THREAD](steps/12/README.md)
## [MUTEX WITH ATOMIC OPERATION AND FUTEX](steps/13/README.md)
## [SECURITY: EXEC-ONLY PAGE](steps/9-xonly/README.md)

## References

### The RISC-V Instruction Set Manual Volume II: Privileged Architecture 
https://github.com/riscv/riscv-isa-manual/releases/download/archive/riscv-privileged-v1.10.pdf

### The RISC-V Instruction Set Manual Volume I: User-Level ISA Document 
https://github.com/riscv/riscv-isa-manual/releases/download/archive/riscv-spec-v2.2.pdf

### SiFive FE310-G000 Manual v2p3
https://sifive.cdn.prismic.io/sifive%2F4d063bf8-3ae6-4db6-9843-ee9076ebadf7_fe310-g000.pdf

### RISC-V Assembly Programmer's Manual
https://github.com/riscv/riscv-asm-manual/blob/master/riscv-asm.md

### RISC-V ELF psABI specification
https://github.com/riscv/riscv-elf-psabi-doc/blob/master/riscv-elf.md

### riscv-qemu wiki
https://github.com/riscv/riscv-qemu/wiki

### riscv-qemu change log
https://wiki.qemu.org/ChangeLog
