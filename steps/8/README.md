# Step 8

## multiple user mode.
このstepでは複数のuser modeプログラムを同時に実行する方法を説明します. 以前のstepで `タイマー割り込み` と `仮想アドレス上でuser modeプログラムを実行する方法` の説明をしていますので, 今回の実装は 1. 複数のuser modeプログラムを動作させ, 2. タイマー割り込み発生のタイミングでその2つのプログラムを切り替える, の2点です.

### memory layout
今回の実装からuser modeプログラムを `task` と呼ぶようにしています. `task.h` にその構造体を定義しています. 今回 `task` は2つ存在しそれぞれ `task[0]`, `task[1]` と呼ぶことにします. どちらのプログラムも同じELFファイルで, `task[0]`は物理アドレス 0x80100000, `task[1]` は物理アドレス 0x80200000 にロードします. 仮想アドレスはどちらも0x0から開始します. 仮想アドレスを使用することで全く同じプログラムを複数同時に実行することが可能になります.
```text
 +--------+
 |        |
 |task[1] |
 |        |
 +--------+ 0x80200000 (va=0x0)
 |        |
 |task[0] |
 |        |
 +--------+ 0x80100000 (va=0x0)
 |        |
 | kernel |
 |        |
 +--------+ 0x80000000
```

### 変更点
今回も前回と同様, 以下の前提があるものとして実装がされています. 
`user modeプログラムがvirtual address 0x0からサイズ4KiBが.text, .rodata, virtual address 0x1000からサイズ4KiBが .data, .bssで使用される.`

#### m/vm.c
`m/vm.c` にあったPage Table Entry `ptes1st`, `ptes2nd`は `task` ごと必要になるため `m/main.c` に移動させました. また context switch のsatpの更新を別の関数(set_satp)としました.

#### m/syscall.c
virtual addressのphysical address変換を現在実行している `task` に応じて処理できるように しました.

#### m/main.c
2つのtaskをtimer割り込みごとに入れ替える処理(switch_task)を加えました. また2つのtaskが並列に動作しているのを分かりやすくするためにtimer割り込みの割り込み間隔を10msecに変更しました.
PMP(Physical Memory Protection)の設定を複数設定可能にしました.

#### u/main.c
並列に動作しているのを分かりやすくするため, `Hello...` の表示を100回繰り返して終了するするようにしました.