# Step 10

## task state: ready, running and blocked.
このstepではuser modeプログラムのtaskの状態の管理をします. このstepまでのtaskはblockedという状態がありませんでした. 通常taskは何らかの待ち状態(e.g. keyboardからの入力待ち, ネットワークからのパケット待ち, 別taskからのレスポンス待ち)であることがほとんどで, この状態をblocked状態として管理します. blocked状態のtaskにはCPUを割り当てません. すべてのtaskがblocked状態となった場合はidle taskと呼ばれる特別なtaskを実行しtaskのblocked状態が解除されるのを待ちます.

### scheduler


変更したい点
- これまでは常にready状態であったので, どのtaskを選んでもすぐにrunning状態に遷移できましたがblocking状態のtaskはrunning状態には遷移できません. blocking状態ならrunningに遷移させないというだけのschedulerを導入します. schedulerを導入するとあるtaskを優先してrunning状態に遷移させたりすこしkernelっぽくなります.
- blocking状態を作り出すためにuartの入力をとれるようにします. uartは1つしかないため1つのtaskだけがuartの入力を受け取れます. uartの入力待ちによるblocking状態と, ほかのtaskがuartの入力待ち状態であることによるblocking状態を発生させます.
- exitを入力するとtaskをexitさせterminatd状態に遷移させます. terminated状態のtaskはschedulingの対象から外されます.
- idle taskを導入します. ready状態のtaskが存在しないときidle taskがrunning状態になります.
- uartの入力待ちのtaskがいない場合, uartの入力はkernelが取得します. ここでもexitを受け付けるようにして system(=qemu) を終了させます.
- 