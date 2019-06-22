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
さてterminated状態を導入するとidle task(=idle thread)というkernel内で実行される特別なtaskを導入する必要が出てきます. idle taskは他のすべてのtaskがrunning状態に遷移できない場合, とくにこのstepの場合すべてのtaskがterminated状態となる場合, に実行される特別なtaskで単に無限ループで実装される場合, あるいは(CPUの消費電力を落とす目的で)割り込みが起こるまで待つ命令を発行する場合があります. RISC-Vにも`wfi(=wait for interrupt)` という命令があります. なおidle taskはtaskと呼んでいますが単なる`wfi`命令なのでcontextのsave/restoreは不要です.

### scheduler
今回のstepからschedulerを導入します. schedulerはポリシーに従ってtaskを管理しrunning状態へ遷移させるtaskを決定します. schedulerのポリシーには, taskに優先度をつけ優先度が高いものからrunning状態にしたり, running状態のtaskは割り込まれないようにしたり, 一定の時間running状態が続いたら別のtaskをrunning状態にするものなど様々あります. 今回導入するschedulerは 1. timer割り込みが発生したらready状態のtaskをrunning状態にする. 2. もしready状態のtaskがなければrunning状態のtaskをrunning状態のままにする. 3. ready, running状態のtaskがなければ idle taskを実行する. となります. 優先度の管理やrunning状態の時間の管理もありません.


FIXME: switch_taskの名前
running適切にrunning状態するをrunning状態に遷移させるか


変更したい点
- これまでは常にready状態であったので, どのtaskを選んでもすぐにrunning状態に遷移できましたがblocking状態のtaskはrunning状態には遷移できません. blocking状態ならrunningに遷移させないというだけのschedulerを導入します. schedulerを導入するとあるtaskを優先してrunning状態に遷移させたりすこしkernelっぽくなります.
- blocking状態を作り出すためにuartの入力をとれるようにします. uartは1つしかないため1つのtaskだけがuartの入力を受け取れます. uartの入力待ちによるblocking状態と, ほかのtaskがuartの入力待ち状態であることによるblocking状態を発生させます.
- exitを入力するとtaskをexitさせterminatd状態に遷移させます. terminated状態のtaskはschedulingの対象から外されます.
- idle taskを導入します. ready状態のtaskが存在しないときidle taskがrunning状態になります.
- uartの入力待ちのtaskがいない場合, uartの入力はkernelが取得します. ここでもexitを受け付けるようにして system(=qemu) を終了させます.
- riscv-proveのsetup.o から卒業します. uart interruptを取得するためにカスタマイズする必要があるからです.
