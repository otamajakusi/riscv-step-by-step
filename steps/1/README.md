# MACHINE MODE HELLO WORLD

最初のステップでは開発環境のセットアップを行い RISC-V の権限モードであるmachine
mode で "hello world" を出力することを目標とします. 開発環境としてコンパイラは
gcc (ricsv-gcc), 実行環境は QEMU (riscv-qemu), ライブラリとして riscv-probe
を使用します. なお RISC-V は32ビットの RV32 を使用します. 64ビットの RV64
と大きくは変わりませんが, 仮想アドレス変換がほんの少しだけ RV32
のほうがシンプルです.

## セットアップ
riscv-step-by-step, riscv-gnu-toolchain, binutils,
riscv-qemuそしてriscv-probeの順にセットアップします.
なおセットアップ先のディレクトリは /path/to/dir とします.

Macで実行する場合, docker 環境上での実行を推奨します.
```bash
$ docker pull ubuntu:18.04
$ docker run -it ubuntu:18.04
```

### 事前準備
apt-get が使用できる環境では事前に以下のコマンドで必要なツールをインストールします. その他環境では以下に相当するツールを事前にインストールします.

```bash
$ sudo apt-get update
$ sudo apt-get install -y wget unzip git
$ sudo apt-get install -y autoconf automake autotools-dev curl libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev
```

### riscv-step-by-stepセットアップ
```bash
$ cd /path/to/dir
$ git clone -b v1.0 https://github.com/otamajakusi/riscv-step-by-step.git
```
clone と同時に v1.0 を checkout します.

### riscv-gnu-toolchainセットアップ
```bash
$ cd /path/to/dir
$ git clone https://github.com/riscv/riscv-gnu-toolchain.git
```
その後のセットアップは riscv-gnu-toolchain の README.md
(https://github.com/riscv/riscv-gnu-toolchain/blob/master/README.md) に従うのが公式な方法ですが
submodule の clone に時間がかかるため, すぐにこのレポジトリを試したい,
riscv-gnu-toolchain の git 操作が不要という場合は riscv-step-by-step
内のスクリプトを使用して submodule のダウンロードを行います.
```bash
$ cd /path/to/dir/riscv-gnu-toolchain
$ sh /path/to/dir/riscv-step-by-step/get-submodule-zips.sh
```
その後のビルド方法も riscv-gnu-toolchain の README.md
(https://github.com/riscv/riscv-gnu-toolchain/blob/master/README.md) に従うのが公式な方法ですが,
RV32 のみを使用する場合は以下を実行します.
```bash
$ cd /path/to/dir/riscv-gnu-toolchain
$ mkdir build
$ cd build
$ ../configure --prefix=/opt/rv32gc --with-arch=rv32gc --with-abi=ilp32d
$ make
$ sudo make install
```
/opt/rv32gc/bin にパスを通します.
```bash
$ export PATH=/opt/rv32gc/bin:$PATH
```

### riscv-qemuセットアップ
QEMU も RV32 のみを使用するため32ビットのビルドのみを行います.
```bash
$ cd /path/to/dir
$ git clone -n https://github.com/riscv/riscv-qemu.git
$ cd riscv-qemu
$ git checkout riscv-qemu-3.1
$ mkdir build
$ cd build
$ ../configure --target-list=riscv32-softmmu --prefix=/opt/qemu
$ make
$ sudo make install
```
/opt/qemu/bin にパスを通します.
```bash
$ export PATH=/opt/qemu/bin:$PATH
```

### riscv-probeセットアップ
```bash
$ cd /path/to/dir/riscv-step-by-step
$ git submodle update --init
$ cd riscv-probe
$ make
```

## Hello World!
```bash
$ cd /path/to/dir/riscv-step-by-step
$ cd steps/1
$ make
$ make run
Hello RISC-V M-Mode.
```
"Hello RISC-V M-Mode." が表示されればこのステップは目標達成となります.
なお make run で開始したプログラムは別の terminal から make stop もしくは
killall qemu-system-riscv32 として終了させます.
