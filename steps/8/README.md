# Step 8

## multiple user mode.
このstepでは複数のuser modeプログラムを同時に実行する方法を説明します. 以前のstepで `タイマー割り込み` と `仮想アドレス上でuser modeプログラムを実行する方法` の説明をしていますので, 今回の実装は 1. 複数のuser modeプログラムを動作させ, 2. タイマー割り込み発生のタイミングでその2つのプログラムを切り替える, の2点です.






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