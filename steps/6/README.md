# Step 6

## user mode hello world with system call.
このstepではuser modeプログラムからsystem call(=syscall)を使用しhelllo worldを表示する方法について説明します. 前回のstepで実行されたuser modeプログラムは直接デバイスにアクセスするため, アクセスする領域を限定できていません. 一般的にuser mode上では任意のプログラムが実行されるため, machine modeを含むuser mode以外のmodeが使用する領域への任意アクセスと別のuser modeプログラムが実行される領域への任意アクセスは制限されなくてはなりません.

### kernel, system call
上で説明した通り, user modeプログラムが別のプログラムにアクセスすることは制限されなくてはなりませんがその制限はkernelにより実施されます. kernelは複数のプログラムの実行の切り替えやsyscallの提供なども行います. syscallとは, user modeプログラムが直接アクセスできないリソースへkernelが代わりにアクセスする仕組みです. 例えば実行中のuser modeプログラムのリストはkernelのリソースといえるので, syscallにより提供されるかもしれません.

このレポジトリではkernelはmachine modeで実装されます.
今回UARTに1文字出力するwrite syscallとプログラムを終了するexit syscallを実装し, user modeが動作する領域を制限します.

### 変更点
今回の変更点を説明をします.

#### m/crt.S
これまでは `riscv-probe/env/common/crtm.s` を使用していましたが今回から独自のcrt.Sを実装します. `riscv-probe/env/common/crtm.s`でkernelを実装しようとした際に, 1. trapが発生した時点のStack Pointer(=SP)にレジスタを退避する, 2. 32個あるレジスタのうち16個のみ退避する, という2つの動作が問題になります. 1つ目の動作は, SPの値がuser modeで操作可能である為, 例えばSPにkernel領域の値を設定しtrapを発生させることで簡単にkernelを破壊できてしまいます. 2つ目の動作は退避しないuser modeのレジスタをkernel内で使用してしまった際にuser modeを復元できない問題があります[^1](1). 今回実装するcrt.Sはこの2つの問題を解決します. レジスタはkernel内の専用のレジスタ退避領域に退避されます.

#### m/main.c#setup_pmp
user modeプログラムが使用する領域を見てみます.
```bash
$ readelf -l steps/6/u/u.elf
...
Program Headers:
  Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
  LOAD           0x000094 0x80100000 0x80100000 0x00194 0x00194 R E 0x4
  LOAD           0x000228 0x80100194 0x80100194 0x00ea4 0x00ea4 RW  0x4
  LOAD           0x0010cc 0x80101038 0x80101038 0x00000 0x00400 RW  0x4
...
```
アドレス 0x80100000 から 0x80101038+0x4000 をアクセスするようです. 領域サイズは約20KiB程度ですので, PMPを使用してアドレス 0x80100000から領域サイズ32KiBを許可します.

#### m/syscall.c
m/main.c#handlerでcause_user_ecallをハンドルできるように修正します.
```c
static void handler(uintptr_t* regs, uintptr_t mcause, uintptr_t mepc)
{
...
    } else if (mcause == cause_user_ecall) {
        handle_syscall(regs, mepc);
...
```
m/syscall.c ではsyscall番号 `SYSCALL_WRITE` と `SYSCALL_EXIT` を処理します. u/syscall.h にsyscallの定義がありますが, このkernelでは関数呼び出しの第一引数をsyscall番号として扱います[^2](2).
u/syscall.h に以下の定義がありますが, 
```c
static inline int __write(int fd, const void *buf, size_t count) ...
```
今回 `SYSCALL_WRITE` で取り扱うデータは1B `buf[0]` のみです. countとfdは使われません. また `SYSCALL_EXIT` は本来syscallを呼び出したプログラムを終了させますが, 今回はシステム全体(=qemu)を終了させます.

#### u/setup.c
user modeプログラムではこれまで `riscv-probe/env/qemu-sifive_u/setup.c` を使用していましたが, putcharとexitをsyscall経由とするるため独自のsetup.cを作成します. 上で説明しましたが, \_\_writeは1Bしか処理されない点に注意します.

#### u/syscall.{h,S}
u/syscall.Sには関数のエントリ(\_\_syscall0, \_\_syscall1, \_\_syscall2, ...)とその実体 `ecall` しかありませんが u/syscall.h に定義があるので(引数をセットアップする実装がなくても)コンパイラは引数のセットアップを行ってくれます.	
`ecall` 命令は自分の現在のmodeに依存せず呼び出すことができる, いわゆるsyscall命令で, `ecall`呼び出し時のmodeに応じて異なる例外が発生します. RISC-Vではどのmodeであっても(できるだけ?)同じ実装をとれるような設計がされていています. ちなみにRISC-Vではプログラムからそのプログラムが動作しているmodeを取得することができません.

#### 領域のチェック
最後に領域のチェックについて簡単に説明します.
user modeプログラムのアクセスは制限されなくてはなりませんがuser modeプログラムが指定するアドレスのチェックがいくつか抜けています. 例えば m/syscall.c の `REG_CTX_A2` です.
```c
static void handle_write(uintptr_t* regs, uintptr_t mepc)
{
    // FIXME: make sure, the buffer address is in the appropriate range.
    char *c = (char*)regs[REG_CTX_A2];
    putchar(*c);
    ...
```
`REG_XTX_A2`はsyscallの第3引数で, user modeプログラムから任意の値を指定可能ですのでこの実装は任意の領域を読み出してputcharに渡すことができます. またELFのELF Header, Program Headerの値も本来は制限する必要があります.

###### 1
退避しないレジスタをkernel内で使用しないことを確認できるなら問題にはなりません.

###### 2
関数の引数とレジスタ割り当ては [RISC-V ELF psABI specification](https://github.com/riscv/riscv-elf-psabi-doc/blob/master/riscv-elf.md#-register-convention) `Register Convention`を参照