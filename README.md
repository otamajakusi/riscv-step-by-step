# riscv-mini

TODO: summalize march and mabi

## setup
git submodule update --init riscv-gnu-toolchain

TODO: binutils

git submodule update --init riscv-qemu

(https://github.com/riscv/riscv-qemu/wiki)

https://github.com/michaeljclark/riscv-probe.git

(cd riscv-probe; make; /qemu-system-riscv32 -nographic -machine sifive_u -kernel build/bin/rv32imac/qemu-sifive_u/hello)

wget https://ftp.gnu.org/gnu/gcc/gcc-8.3.0/gcc-8.3.0.tar.xz 

tar xf gcc-8.3.0.tar.xz -C riscv-gnu-toolchain

rm -rf riscv-gnu-toolchain/riscv-gcc

ln -s riscv-gnu-toolchain/{gcc-8.3.0,riscv-gcc}

wget ftp://sourceware.org/pub/newlib/newlib-3.0.0.tar.gz

tar zxvf newlib-3.0.0.tar.gz -C riscv-gnu-toolchain

rm -rf riscv-gnu-toolchain/riscv-newlib

ln -s riscv-gnu-toolchain/{newlib-3.0.0,riscv-newlib}

mkdir -p riscv-gnu-toolchain/build

cd riscv-gnu-toolchain/build

../configure --prefix=/opt/rv32gc --with-arch=rv32gc --with-abi=ilp32d --disable-gdb --disable-linux

# References
## Privileged Architecture Version 1.10
https://github.com/riscv/riscv-isa-manual/blob/master/release/riscv-privileged-v1.10.pdf
(csr, virtual address, etc)

## RISC-V ELF psABI specification
https://github.com/riscv/riscv-elf-psabi-doc/blob/master/riscv-elf.md
(calling convention, etc)

## SiFive FE310-G000 Manual
https://sifive.cdn.prismic.io/sifive%2F4d063bf8-3ae6-4db6-9843-ee9076ebadf7_fe310-g000.pdf
(PLIC, UART register, etc)

## riscv-qemu wiki
https://github.com/riscv/riscv-qemu/wiki 
