# Step 7

## virtual address / page protection.
このstepでは仮想アドレス(=virtual address)上でuser modeプログラムを実行する方法を説明します. RISC-Vでは supervisor mode と user mode が virtual address で動作することができます. 一方 machine mode は常に物理アドレスでのアクセスとなります. virtual addressはアドレス変換機構によりアクセス権が確認されたのち物理アドレス(=physical address)に変換されアクセスされます.

### Address Translation and Protection
アドレス変換機構は主に2つのことを行います. 1つはプログラムがアクセスするvirtual addressのphysical address変換, もう一つはアクセスの種類(read, write, execute)と権限modeに応じたアクセス制御です[^1](1).

#### Address Translation
アドレスはページと呼ばれる単位でvirtual addressからphysical addressに変換されます. RV32のページサイズは4KiBと2MiBがあり同時に使用することができます. 2MiB ページのことを `superpage` と呼んでいます. 今回の実装では4KiBページを使用します. 小さいページサイズのほうがメモリを効率的に使用できるというメリットがありますが反面, ページごとに必要な変換用のエントリ(Page Table Entry)数が多くなるというデメリットもあります[^2](2).
以下に4KiBページでの virtual address(va) - physical address(pa) 変換の図を示します[^3](3).
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
上のアドレス変換の図中の flags に設定する値により, user modeでのページアクセス, read, write, executeでのページアクセスなどの制御が可能です[^4](4). 詳細は [Privileged Architecture Version 1.10](https://github.com/riscv/riscv-isa-manual/blob/master/release/riscv-privileged-v1.10.pdf) `4.3.1 Addressing and Memory Protection` を参照してください.

### 変更点
今回は以下の前提があるものとして実装がされています. 
`user modeプログラムがvirtual address 0x0からサイズ4KiBが.text, .rodata, virtual address 0x1000からサイズ4KiBが .data, .bssで使用される.`

#### m/vm.c
Page Table Entryを設定します. m/main.cから呼び出されます.

#### m/elfldr.c
p_vaddrはvirtual addressなので, machine modeからELFをロードする際にはphysical addressで扱う必要があります.

#### m/syscall.c
userから渡されるアドレスはvirtual addressなのでそれをphysical addressに変換しています. 変換方法はアドホックです.

#### u
linker scriptとしてu-va.ldを指定しています. プログラムセグメントの数を2つにして, 常に4KiBでアラインされるようにしています. crt.S も riscv-probe libfemto_start_main の呼び出しをやめて, crt.S で bss をクリアするように変更しました.

###### 1
Page Table EntryのAccessed, Dirty bitについては以降のstepで説明する予定です.

###### 2
一般にTLBと呼ばれるアドレス変換用のキャッシュがあり, ページサイズが小さいほうがキャッシュにヒットしにくくなるというデメリットもあります.

###### 3
2MiBページでのアドレス変換は2nd level page tableがありません.

###### 4
RISC-Vはx86などと異なりreadとexecuteを独立に設定できます. executeをセットし, readをクリアすることで, instruction fetchのみ許可, data fetchは不許可となります. これはReturn Oriented Programという攻撃に対して効果があります.
