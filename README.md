# RISC-V kernel sample

このレポジトリではRISC-Vの上で動作する簡単なkernelを実装します。このkernelの実装を通じて

- RISC-Vアーキテクチャ
- kernelの基本的動作
- gnu binary utilities (=binutils) の簡単な使い方
- セキュリティ

についての理解が深まると考えます。実装は理解をしやすいように小さなステップに分けて段階的に進めていきます。

開発環境のセットアップから初めて、例外処理、割り込み処理、ユーザーモード、システムコール、アドレス変換、などを実装し、最後にセキュリティについて実装をします。

なおコンパイラはgcc, 実行環境はqemu, ライブラリとしてriscv-probeを使用します。

### step1
machine mode hello world.

### step2
machine mode with `ecall` exception.

### step3
machine mode with timer interrupt.

### step4
user mode hello world.

### step5
user mode hello world with syscall.

### step6
user mode loading with elf.

### step7
multiple user mode.

### step8
virtual address / page protection.

### step9
task state: ready, running and blocked.

### step10
thread, mutex, condition variable.

### step11
security (NX, SSP, ASLR, execute-only, SMEP/SMAP)

## References
### Privileged Architecture Version 1.10
https://github.com/riscv/riscv-isa-manual/blob/master/release/riscv-privileged-v1.10.pdf
(csr, virtual address, etc)

### RISC-V ELF psABI specification
https://github.com/riscv/riscv-elf-psabi-doc/blob/master/riscv-elf.md
(calling convention, etc)

### SiFive FE310-G000 Manual
https://sifive.cdn.prismic.io/sifive%2F4d063bf8-3ae6-4db6-9843-ee9076ebadf7_fe310-g000.pdf
(PLIC, UART register, etc)

## riscv-qemu wiki
https://github.com/riscv/riscv-qemu/wiki

