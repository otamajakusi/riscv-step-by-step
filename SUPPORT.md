# 技術書典7にて頒布した [RISC-V step-by-step](https://techbookfest.org/event/tbf07/circle/5765209499107328) のサポート情報です.

## 全般
### PMPについて
PMP(=Physical Memory Protection) の実装は完全でないことが分かりました.
まず PTE(=Page Table Entry) のページテーブルウォークは supervisor mode
でアクセスされますが PTE に PMP を設定していません.

### riscv-qemu-3.1 以外のサポート
現状 https://github.com/riscv/riscv-qemu/tree/riscv-qemu-3.1
以外のQEMUに対応できていません. これは上に書いた通り PMP
への対応が不完全なのもあるのですが QEMU 側のPMPへの対応も不完全で現在対中です.
https://github.com/otamajakusi/riscv-step-by-step/issues/6

### 正誤情報
1. 50ページ setup_pmp. size に 2 のべき乗を渡します.  
   誤)
   ```
   setup_pmp(pa, 0x2000);
   ```
   正)
   ```
   setup_pmp(pa,          0x1000);
   setup_pmp(pa + 0x1000, 0x1000);
   ```

2. 4ページ セットアップ  
   追加)
   ```
   Macで実行する場合, docker 環境上での実行を推奨します.

   $ docker pull ubuntu:18.04
   $ docker run -it ubuntu:18.04
   事前準備
   apt-get が使用できる環境では事前に以下のコマンドで必要なツールをインストールします. その他環境では以下に相当するツールを事前にインストールします.

   $ sudo apt-get update
   $ sudo apt-get install -y wget unzip git
   $ sudo apt-get install -y autoconf automake autotools-dev curl libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex  texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev
   $ sudo apt-get intall -y python pkg-config libglib2.0-dev libpixman-1-dev
   ```
   steps/1/README.md に事前準備とMacで実行する場合を記載しています.

3. 5ページ riscv-probe make.  
   誤)
   ```
   $ cd /path/to/dir/riscv-step-by-step
   $ git submodle update --init
   $ cd riscv-probe
   $ make
   ```
   正)
   ```
   $ cd /path/to/dir/riscv-step-by-step
   $ git submodle update --init
   $ cd riscv-probe
   $ make CROSS_COMPILE=riscv32-unknown-elf- 
   ```
