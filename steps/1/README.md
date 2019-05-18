# Step 1

## machine mode hello world.

このステップでは開発環境のセットアップを行いmachine modeでhello worldを出力することを目標とします. 開発環境としてコンパイラはgcc (ricsv-gcc), 実行環境はqemu (riscv-qemu), ライブラリとして riscv-probe を使用します. なおRISC-Vは32-bit RV32を使用します. 64-bit RV64と大きく変わりませんが, 仮想アドレスがほんの少しだけRV32のほうがシンプルです.

このレポジトリは `where/to/repos/riscv-mini` にcloneされているとします.

### setup gcc, binutils, newlib, etc

```bash
cd where/to/repos
git clone https://github.com/riscv/riscv-gnu-toolchain.git
```

その後のセットアップは [README.md](https://github.com/riscv/riscv-gnu-toolchain/blob/master/README.md) に従うのが公式な方法ですがsubmoduleのcloneに時間がかかるため, すぐにこのレポジトリを試したい, riscv-gnu-toolchainのgit操作が不要という人に限りスクリプトを使用してsubmoduleのダウンロードを行います.

```bash
cd where/to/repos/riscv-gnu-toolchain
sh where/to/repos/riscv-mini/get-submodule-zips.sh
```

その後のビルド方法は公式のREADME.mdに従いますが, RV32のみを使用する場合は以下を実行します.

```bash
cd where/to/repos/riscv-gnu-toolchain
mkdir build
cd build
../configure --prefix=/opt/rv32gc --with-arch=rv32gc --with-abi=ilp32d
make
sudo make install
```
パスを通します.
```bash
export PATH=/opt/rv32gc/bin:$PATH
```

### setup riscv-qemu
qemuもRV32のみを使用するため, 32-bitのビルドのみとします.

```bash
cd where/to/repos
git clone -n https://github.com/riscv/riscv-qemu.git
cd riscv-qemu
git checkout 13c24edaa742181af8d9c6b027ee366b04de1ea1
# see https://github.com/riscv/riscv-qemu/wiki
mkdir build
cd build
../configure --target-list=riscv32-softmmu --prefix=/opt/qemu
make
sudo make install
```
パスを通します.
```bash
export PATH=/opt/qemu/bin:$PATH
```

### setup riscv-probe
```bash
cd where/to/repos/riscv-mini
git submodle update --init
cd riscv-probe
make
```

### hello world!
```bash
cd where/to/repos/riscv-mini
cd steps/1
make
make run

```
`Hello RISC-V M-Mode.` が表示されればこのステップは目標達成となります. このステップで使用するソースコードは main.c のみとなります. crt.S (=c runtime) などはriscv-probeでビルドしたものをそのまま使用します.

なお `make run` で開始したプログラムは別のterminalから `make stop` もしくは `killall qemu-system-riscv32` として終了させます.