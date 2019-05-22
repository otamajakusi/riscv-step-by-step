# Step 4

## Executable loading.
このstepでは実行ファイルのロード方法について説明します. ELF形式のファイル(=ELFファイル)の扱いとCPU cacheの操作について説明します. このstepで実行ファイルのロード方法について理解し, 以降のstepでuser modeプログラムを実行する準備を行います.

### ELF
ELFは実行ファイル, 共有ライブラリ, オブジェクトファイルなどを格納するファイル形式の1つで, ELF Headerと必要に応じてProgram Header, Section Headerとそれらヘッダーから参照されるエントリで構成されます. 今回は実行ファイルに焦点を絞ってELFの説明をおこないます.
ELFの実行ファイルはELF Header, Program Header, Section Headerから構成されます[^1](1). `riscv-probe/libfemto/include/elf.h` にある定義を参照します.

#### ELF Header
```c
enum {
    EI_MAG0 = 0,
    EI_MAG1 = 1,
    EI_MAG2 = 2,
    EI_MAG3 = 3,
    EI_CLASS = 4,
    ...
};
enum {
    ELFCLASSNONE = 0,
    ELFCLASS32 = 1,
    ELFCLASS64 = 2,
    ELFCLASS128 = 3
};
typedef struct {
    Elf32_Byte  e_ident[EI_NIDENT];
    Elf32_Half  e_type;
    Elf32_Half  e_machine;
    Elf32_Word  e_version;
    Elf32_Addr  e_entry;
    Elf32_Off   e_phoff;
    Elf32_Off   e_shoff;
    Elf32_Word  e_flags;
    Elf32_Half  e_ehsize;
    Elf32_Half  e_phentsize;
    Elf32_Half  e_phnum;
    Elf32_Half  e_shentsize;
    Elf32_Half  e_shnum;
    Elf32_Half  e_shstrndx;
} Elf32_Ehdr;
```
重要なエントリのみ説明します.
`e_ident[EI_CLASS]` には `ELFCLASS32` あるいは `ELFCLASS64`が格納されます[^2](2). この値をもとにELFの32-bitあるいは64-bitを判定します. 今回は32-bitのRV32を扱うため`Elf32_Ehdr` を使用します.
`e_entry` にはプログラムがメモリにレイアウトされた後の実行開始アドレスが格納されます.
`e_phoff`, `e_phentsize` そして `e_phnum` にはProgram Headerのそれぞれファイルオフセット, サイズそして数が格納されます. 同様に, `e_shoff`, `e_shentsize` そして `e_shnum` にはSection Headerのそれぞれファイルオフセット, サイズそして数が格納されます[^3](3).

#### Program Header
```c
typedef struct {
    Elf32_Word  p_type;
    Elf32_Off   p_offset;
    Elf32_Addr  p_vaddr;
    Elf32_Addr  p_paddr;
    Elf32_Word  p_filesz;
    Elf32_Word  p_memsz;
    Elf32_Word  p_flags;
    Elf32_Word  p_align;
} Elf32_Phdr;
```
Program HeaderはELF Header e_phoffで示されるファイルオフセットに配置されます[^4](4). Program HeaderはELF Header e_phnumで示される数だけ連続で存在し, それぞれがプログラムセグメントと呼ばれるエントリを参照します.
`p_type` にはプログラムセグメントのタイプ情報が格納されます. 実行プログラムのロード対象となるタイプは `PT_LOAD` です.
`p_offset` にはプログラムセグメントのファイルオフセットが格納されます.
`p_vaddr` にはプログラムセグメントをロードするアドレス, `p_filesz` にはそのサイズがそれぞれ格納されます. `p_memsz` にはプログラムセグメントに必要とされるサイズが格納されます. 以下に `p_offset`, `p_vaddr`, `p_filesz` そして `p_memsz` の関係を示します.

```text
                             +--------+ ^
                             |        | |
                             |  zero  | | p_memsz
                             |        | |
            +--------+       +--------+ | ^
            |        | copy  |        | | |
            |  data  | ====> |  data  | | | p_filesz
            |        |       |        | | |
 p_offset-> +--------+       +--------+ v v <- p_vaddr
             ELF file          memory 
```

ELFファイルのロードについてまとめると, 1. ELF Header`e_ident[EI_CLASS]` など確認, 2. ELF Header `e_phoff`, `e_phnum` からProgram Headerを取り出し, 3. Program Header `p_type` が `PT_LOAD` であることを確認, 4. メモリ領域 `max(p_filesz, p_memsz)` を `p_vaddr` アドレスから確保, 5. ELFファイル `p_offset` オフセットからアドレス `p_vaddr` にサイズ `p_filesz` コピー, 6. `3, 4, 5`をすべての Program Headerに対して実行する.
Section Headerはプログラムのロードに不要なため説明を省略します.
`readelf` を使用して前のstep `steps/3/m.elf` のProgram Headerを見てみます.

```bash
$ riscv32-unknown-elf-readelf -l steps/3/m.elf

Elf file type is EXEC (Executable file)
Entry point 0x80000000
There are 3 program headers, starting at offset 52

Program Headers:
  Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
  LOAD           0x000094 0x80000000 0x80000000 0x00764 0x00764 R E 0x4
  LOAD           0x0007f8 0x80000764 0x80000764 0x008fc 0x008fc RW  0x4
  LOAD           0x001100 0x80001060 0x80001060 0x00000 0x01010 RW  0x10

 Section to Segment mapping:
  Segment Sections...
   00     .text .rodata
   01     .data
   02     .bss
```

`steps/3/m.elf` は3つのProgram Headerから構成されていることがわかります[^5](5). Offset, VirtAddr, FileSiz, MemSizはそれぞれ上で説明した, p_offset, p_vaddr, p_filesz, p_memszに対応します. Flgは1番目のプログラムセグメントが `RE`(Read and Execute), 2番目と3番目のプログラムセグメントが `RW`(Read and Write) であることがわかります. 下の`Section to Segment mapping:` と書かれた部分にはプログラムセグメントに格納されるデータの種別が書かれています. `.text` は命令, `.rodata`は読み込み専用のデータ, `.data`は読み書きを行うデータ, `.bss` は初期値0のデータです. `.text`, `.rodata` は1番目のプログラムセグメントなので `RE`, `.data` は2番目のプログラムセグメントなので `RW`, 同様に `.bss` も`RW`で配置します.

### ディレクトリ構成
今回から2つのディレクトリを作成します. 1つは実行ファイルをロードする machine mode プログラムが作成される `m` ディレクトリ, もう一つは `m` ディレクトリで作成されたプログラムからロードされる実行ファイルが作成される `u` ディレクトリです. このstepでは`u` ディレクトリで作成される実行ファイルは user mode で実行されませんが, 以降のstepでは user mode で実行するためこのような名前にしています.

```bash
$ ls steps/4/
steps/4:
README.md  m  u

steps/4/m:
Makefile  main.c

steps/4/u:
Makefile  crt.S  main.c
```
`m` ディレクトリと異なり `u/crt.S` があることがわかります. user modeで例外/割り込み処理をしないため `crt.S` は `m` ディレクトリがリンクするのものと別にしています[^6](6).

### u.elfの埋め込み
さて, ELFファイルをロードする方法を説明してu.elfを作成することはできました. 残りはu.elfの保存場所です. このシステムは現時点で外部からデータを取得できません. 今回はプログラム内部にプログラムを保持し, そこからELFファイルをロードします.
`m/Makefile` に以下の記述があります.

```Makefile
../u/u.elf:
        $(MAKE) -C ../u

u.elf.o: ../u/u.elf
        $(OBJCOPY) -I binary -O elf32-littleriscv -B riscv $< $@ \
                --redefine-sym _binary____u_u_elf_start=u_start \
                --redefine-sym _binary____u_u_elf_end=u_end \
                --redefine-sym _binary____u_u_elf_size=u_size
```
これは, `u/u.elf` をbinaryとして扱い, RISC-Vのオブジェクトファイルに変換しています. `--redefine-sym` は変換時にsymbol名を変更するオプションです. 変換された `u.elf.o` は `m.elf` にリンクされます. 

## CPU Cache Operation[^7](7)
RISC-Vのcacheアーキテクチャは実装依存という扱いなのか, [Privileged Architecture](<https://github.com/otamajakusi/riscv-mini#privileged-architecture-version-110>) に記述はなく [The RISC-V Instruction Set Manual](https://github.com/otamajakusi/riscv-mini#the-risc-v-instruction-set-manual-22) には `FENCE` と `FENCE.I` 命令の説明があるのみです. 多くのCPUは命令キャッシュ(I-cache, I$)とデータキャッシュ(D-cache, D$)が実装されていますが, ここのRISC-Vでも同様の実装がされているものとして説明を行います. 以下はI-cache, D-cache, CPU coreそしてmemoryの関係を示した図です.

```
  +------------------+
  |     CPU core     |
  +------------------+
      ^         ^
      |         |
      |         v
  +-------+ +-------+
  |I-cache| |D-cache|
  +-------+ +-------+
      ^         ^
      |         |
      |         v
  +-----------------+
  |      memory     |
  +-----------------+
```

CPU cacheは 1. CPUがfetchするアドレスのデータがcacheに存在すればそのデータをCPUに返す, 2. 無ければmemoryからデータをコピーしそのデータをCPUに返す, という動作をします. D-cacheのデータは図の上下方向にデータが転送されますが, I-cacheのデータは常にmemoryからI-cache方向, I-cacheからCPU core方法に転送されます.
今回ELFファイルによる実行ファイルのロード方法を説明しましたがmemory -> D-cache -> CPU core -> D-cache -> memoryという経路でデータがコピーされただけでデータを命令として実行するためにはI-cacheにデータを転送する必要があります. `FENCE.I` 命令はその命令発行時に I-cache に存在するデータを無効化させ, 結果としてmemoryからI-cacheを経由してCPU coreにデータを転送します.

###### 1
Section HeaderはProgram Headerで参照されるセグメントをロードするためだけの目的では不要ですがobjdumpなどのtoolでは参照されます.

###### 2
`EI_ELFCLASS128` はわかりません.

###### 3
`p_phentsize`, `e_shentsize` は `e_ident[EI_CLASS]` により32-bitあるいは64-bitが判定できればProgram HeaderあるいはSection Headerのサイズは決まるのでなくても問題ないエントリと言えます.

###### 4
通常e_phoffははELF Headerの終わりを示します.

###### 5
`riscv-probe/env/default/default.lds` によりプログラムセグメントが構成されます.

###### 6
`file.s` と `file.S`. 前者はgccによりpreprocessされない, 後者はpreprocessされるという違いがあります. https://gcc.gnu.org/onlinedocs/gcc-9.1.0/gcc/Overall-Options.html#Overall-Options

###### 7
RISC-Vのcache操作はhartsごと行う必要がありますが, 今回のシステムはhartsが1つなので説明を省略します.
