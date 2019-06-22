# Step 10

## task state: ready, running and terminated.
このstepではuser modeプログラムのtaskの状態の管理をします. これまでのstepのtaskはずっと実行していたため特別に状態の管理を行う必要がありませんでした. 状態の管理は意識してしていませんでしたが, ready 状態と running 状態を交互に繰り返していました.
```
  +-------+     +-------+
  |       |---->|       |
  | ready |     |running|
  |       |<----|       |
  +-------+     +-------+
```
running状態はCPUを使用してプログラムを実行している状態, ready状態はいつでもrunning状態に遷移できる状態です. 今回新たに terminated状態を導入します. terminated状態はプログラムが終了した状態でreadyやrunning状態には戻れません. exit syscallを発行したtaskの状態をterminated状態に遷移させます.
```
  +-------+     +-------+
  |       |---->|       |
  | ready |     |running|
  |       |<----|       |
  +-------+     +-------+
                    |
                    |
                    v
                +-------+
                |       |
                |running|
                |       |
                +-------+
```
terminated状態を導入すると, `terminated状態のtaskをready/running状態にしない` という仕組みが必要になります. ready状態のtaskがlinked listでつながっている場合はそのリストから単に切り離せばterminated状態を表現できそうですが, 現状taskはlinked listではなく配列で管理されているため単にtaskに状態を追加する実装にしようと思います.  
さてterminated状態を導入するとidle task(=idle thread)というkernel内で実行される特別なtaskを導入する必要が出てきます. idle taskは他のすべてのtaskがrunning状態に遷移できない場合, とくにこのstepの場合すべてのtaskがterminated状態となる場合, に実行される特別なtaskで単に無限ループで実装される場合, あるいは(消費電力を小さくする目的などで)割り込みが起こるまで待つ命令を発行する場合があります. RISC-Vにも`wfi(=wait for interrupt)` という命令があります. なおidle taskはtaskと呼んでいますが単なる`wfi`命令なのでcontextのsave/restoreは不要です.

### scheduler
今回のstepからschedulerを導入します. schedulerはポリシーに従ってtaskを管理しrunning状態へ遷移させるtaskを決定します. schedulerのポリシーには, taskに優先度をつけ優先度が高いものからrunning状態にしたり, running状態のtaskは割り込まれないようにしたり, 一定の時間running状態が続いたら別のtaskをrunning状態にするものなど様々あります. 今回導入するschedulerは 1. timer割り込みが発生したらready状態のtaskをrunning状態にする, 2. もしready状態のtaskがなければrunning状態のtaskをrunning状態のままにする, 3. ready, running状態のtaskがなければ idle taskを実行する, となります. 優先度の管理やrunning状態の時間の管理もありません.

### 変更点
#### m/main.c
taskのインスタンス `task[USER_NUM]` はsched.cに移動させ, taskはschedulerの管理とします.

### m/sched.c
start_schedule: schedulerを開始します.
schedule: これまでswitch_taskとしてm/main.cで実装されていたものをm/sched.cに移動させ, 名前を変更しました. taskをswitchするかどうかはschedulerの責任です. 今回からidle taskを導入し, idle taskのcontextはsave/restoreの対象ではないためget_current_taskの戻り値がidle taskである場合, pickup_next_taskの戻り値がidle taskである場合の処理を加えています. どちらもidle taskの場合NULLが返されます.
create_task: これもこれまでm/main.cで行われていたものをm/sched.cに移動させたものです.
idle: idle taskが実行する関数で`wfi`のループで実装されています.

### wfiについて
`The WFI instruction is just a hint, and a legal implementation is to implement WFI as a NOPP.` とある[^1](#1)ので, `wfi`は割り込みがあるまでstallするわけではないことに注意します. またmachine modeとsupervisor modeでwfiは有効ですが, MSTATUS.TW=1の場合, supervisor modeでのwfiが一定時間内に完了しない場合, wfiはillegal instruction例外を発生させるとあります. これはsupervisor modeが提供するguest OSが無駄な時間を消費しないようにするためです.
