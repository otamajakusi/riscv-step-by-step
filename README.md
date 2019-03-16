# riscv-mini

## setup
git submodule update --init ovpsim
git submodule update --init riscv-gnu-toolchain

TODO: binutils

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
