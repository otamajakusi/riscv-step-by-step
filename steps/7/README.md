# Step 7

## virtual address / page protection.
このstepでは仮想アドレス(=virtual address)上でuser modeプログラムを実行する方法を説明します. RISC-Vでは supervisor mode と user mode が virtual address で動作することができます. 一方 machine mode は常に物理アドレスでのアクセスとなります. virtual addressはアドレス変換機構によりアクセス権が確認されたのち物理アドレス(=physical address)に変換されアクセスされます.

### Address Translation and Protection
アドレス変換機構は主に2つのことを行います. 1つはプログラムがアクセスするvirtual addressのphysical address変換, もう一つはアクセスの種類(read, write, execute)と権限modeに応じたアクセス制御です.

#### Address Translation
アドレスはページと呼ばれる単位でvirtual addressからphysical addressに変換されます. RV32のページサイズは4KiBと4MiBがあり同時に使用することができます. 4MiB ページのことを `superpage` と呼んでいます. 今回の実装では4KiBページを使用します. 小さいページサイズのほうがメモリを効率的に使用できるというメリットがありますが反面, ページごとに必要な変換用のエントリ(Page Table Entry)数が多くなるというデメリットもあります[^1](1).
以下に4KiBページでの virtual address(va) - physical address(pa) 変換の図を示します[^2](2).
```text
     +--------+--------+--------+
  va | vpn[1] | vpn[0] | offset |
     +----|---+----|---+--------+
          |        |
      +------------+
      |   |                                        (1)
      |   |   +-----------------+--------+ <- satp.ppn * 4KiB
      |   |   |       ppn       | flags  |
      |   |   +--------------------------+
      |   +-->|       ppn ---------------------+
      |  (2)  +-----------------+--------+     |
      |       |       ...       |        |     |
      |       +-----------------+--------+     | (3)
      |             (1st level) pte            |
      |                                        |
      |                                        |
      |       +-----------------+--------+ <---+ (1st level) pte.ppn * 4KiB
      |       |       ppn       | flags  |
      |       +--------------------------+
      +------>|       ppn ---------------------+
        (4)   +-----------------+--------+     |
              |       ...       |        |     |
              +-----------------+--------+     | (5)
                    (2nd level) pte            |
                                               |
                                               |
              +--------------------------------+
              |
     +--------v--------+--------+
  pa |       ppn       | offset |
     +-----------------+--------+

satp: satp register
va: virtual address
pa: physical address
pte: page table entry
vpn: virtual page number
ppn: physical page number
```
1. satp registerのppnから1st level page table entryの先頭を得る
2. va.ppn[1]で1st level page table entryのindexを得る
3. 2で得られたpte.ppnから2nd level page table entryの先頭を得る
4. va.ppn[0]で2nd level page table entryのindexを得る
5. 4で得られたpte.ppnとva.offsetからpaを得る

#### Protection
上のアドレス変換の図中の flags に設定する値により, user modeでのページアクセス, read, write, executeでのページアクセスなどの制御が可能です[^3](3)[^4](4).
詳細は [Privileged Architecture Version 1.10](https://github.com/riscv/riscv-isa-manual/blob/master/release/riscv-privileged-v1.10.pdf) `4.3.1 Addressing and Memory Protection` を参照してください.

#### satp, Page Table Entry
satpレジスタはアドレス変換の有効・無効とアドレス変換が有効の場合の先頭Page Table Entryを示すレジスタです.
Page Table Entry(=PTE)はアドレス変換用のppn(physical page number)とアクセス制御用のflagsを保持しする1エントリが4Byteのメモリ上のデータです. PTEは1024の連続したエントリで構成されその先頭アドレスは4KiB境界で配置される必要があります.
以前CPU cacheの説明をしましたがこのPTEもCPU cacheへ配置するだけでは不十分で必ずメモリに配置する必要があります. satpにPTEアドレスを設定する前に, `SFENCE.VMA` 命令を発行しPTEがメモリに配置されることを保証します.

### 変更点
今回は以下の前提があるものとして実装がされています. 
`user modeプログラムがvirtual address 0x0からサイズ4KiBが.text, .rodata, virtual address 0x1000からサイズ4KiBが .data, .bssで使用される.`

#### m/vm.c
Page Table Entryを設定します. m/main.cから呼び出されます.

#### m/elfldr.c
p_vaddrはvirtual addressなので, machine modeからELFをロードする際にはphysical addressで扱う必要があります.

#### m/syscall.c
userから渡されるアドレスはvirtual addressなのでそれをphysical addressに変換しています. 変換方法はアドホックです.

#### u/*
linker scriptとしてu-va.ldを指定しています. プログラムセグメントの数を2つにして, 常に4KiBでアラインされるようにしています. crt.S も riscv-probe libfemto_start_main の呼び出しをやめて, crt.S で bss をクリアするように変更しました.

###### 1
一般にTLBと呼ばれるアドレス変換用のキャッシュがあり, ページサイズが小さいほうがキャッシュにヒットしにくくなるというデメリットもあります.

###### 2
2MiBページでのアドレス変換は2nd level page tableがありません.

###### 3
RISC-Vはx86などと異なりreadとexecuteを独立に設定できます. executeをセットし, readをクリアすることで, instruction fetchのみ許可, data fetchは不許可となります. これはReturn Oriented Programという攻撃に対して効果があります.

###### 4
Accessed-bit, Dirty-bitはソフトウェアから設定するのではなく, read, write, executeがあった場合Accessed-bit, writeがあった場合Dirty-bitが自動的に設定されます. swap-outの実装にこれらが使用されます. なおAccessed-bit, Dirty-bitはoptionalでRISC-Vの必須機能ではありません.
