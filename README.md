# RISC-V kernel sample

このレポジトリではRISC-Vの上で動作する簡単なkernelを実装します. このkernelの実装を通じて

- RISC-Vアーキテクチャ
- kernelの基本的動作
- gnu binary utilities (=binutils) の簡単な使い方
- セキュリティ

についての理解が深まると考えます. 実装は理解をしやすいように小さなステップに分けて段階的に進めていきます.

開発環境のセットアップから初めて, 例外処理, 割り込み処理, ユーザーモード, システムコール, アドレス変換, などを実装し, 最後にセキュリティについて実装をします.

なおコンパイラはgcc, 実行環境はqemu, ライブラリとしてriscv-probeを使用します.

### [Step 1](steps/1/README.md)
machine mode hello world.

### [Step 2](steps/2/README.md)
machine mode with ecall exception.

### [Step 3](steps/3/README.md)
machine mode with timer interrupt.

### [Step 4](steps/4/README.md)
Executable loading.

### [Step 5](steps/5/README.md)
user mode hello world.

### [Step 6](steps/6/README.md)
user mode hello world with system call.

### [Step 7](steps/7/README.md)
virtual address / page protection.

### [Step 8](steps/8/README.md)
multiple user mode.

### [Step 9](steps/9/README.md)
lazy loading.

### [Step 10](steps/10/README.md)
task state: created, ready, running and terminated.

### [Step 11](steps/11/README.md)
task state: blocking.

### [Step 12](steps/12/README.md)
thread, mutex, condition variable. futex!

### [Step 13](steps/13/README.md)
security (NX, SSP, ASLR, execute-only, SMEP/SMAP)

## References
### Privileged Architecture Version 1.10
https://github.com/riscv/riscv-isa-manual/blob/master/release/riscv-privileged-v1.10.pdf
(csr, virtual address, etc)

### The RISC-V Instruction Set Manual 2.2
https://github.com/riscv/riscv-isa-manual/blob/master/release/riscv-spec-v2.2.pdf

### RISC-V Assembly Programmer's Manual
https://github.com/riscv/riscv-asm-manual/blob/master/riscv-asm.md

### RISC-V ELF psABI specification
https://github.com/riscv/riscv-elf-psabi-doc/blob/master/riscv-elf.md
(calling convention, etc)

### SiFive FE310-G000 Manual
https://sifive.cdn.prismic.io/sifive%2F4d063bf8-3ae6-4db6-9843-ee9076ebadf7_fe310-g000.pdf
(PLIC, UART register, etc)

### riscv-qemu wiki
https://github.com/riscv/riscv-qemu/wiki

