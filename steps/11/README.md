# Step 11

## task state: intruducing blocked.
このstepでは前のstepで導入したtaskの状態に新たにblocked状態を追加します.
blocking状態はイベントを待つ状態で, running状態から遷移します. 待っていたイベン
トが完了するとblocked状態が解除されready状態へと遷移します.
このstepではkeyboardの入力イベントを待ち, blocked状態へと遷移させます.
```
                +-------+
                |       |
      +---------|blocked|
      |         |       |
      |         +-------+
      |             ^
      |             |
      v             |
  +-------+     +-------+
  |       |---->|       |
  | ready |     |running|
  |       |<----|       |
  +-------+     +-------+
                    |
                    |
                    v
                +-------+
                | termi |
                | nated |
                |       |
                +-------+
```
今回以下の2点を新たに導入します.
- 外部割込み
- blocking syscall

### 外部割込みの導入
CSR MIE はこれまで MIE.MTIE のみが設定されいましたが, 今回外部割込みを有効にするため
MIE.MEIE を追加で設定します. timer interruptのstepで, 割り込みは CLINT(Core Level
Interruptor) あるいは PLIC (Platform Level Interrupt Controller) を経由して処理
されると書きましたが PLIC の割り込みを有効にするためには MIE.MEIE を有効にする必要が
あります. MIE.MTIE は timer割り込み, MIE.MEIE はそれ以外の割り込み, という認識で
問題ありません.
今回UART0の入力割り込みを有効にします. これまで riscv-probe でビルドした setup.o
をリンクしていましたが, setup.c をカスタマイズする必要が出てきたため独自の実装に
切り替えます. ですが追加する操作は UART0 の割り込み有効処理のみです.
割り込みを有効にするために必要なデバイス側の操作は, 1. UART デバイスの入力割り込
みを有効にする, 2. 割り込み優先度を設定する, 3. 割り込み優先度スレッショルドを設
定する, の3つです. 優先度は0から7の値を設定することができて, 0は割り込み
しない, 7は最優先で割り込みする, となりますが, 優先度の値が割り込み優先度スレッ
ショルド以下である場合, 割り込みがされません.
今回扱うデバイスは1つなので優先順位を気にする必要はなく0以外の値を設定します. ま
た割り込み優先度スレッショルドも0を設定します.

### blocking syscall


変更したい点
- これまでは常にready状態であったので, どのtaskを選んでもすぐにrunning状態に遷移できましたがblocking状態のtaskはrunning状態には遷移できません. blocking状態ならrunningに遷移させないというだけのschedulerを導入します. schedulerを導入するとあるtaskを優先してrunning状態に遷移させたりすこしkernelっぽくなります.
- blocking状態を作り出すためにuartの入力をとれるようにします. uartは1つしかないため1つのtaskだけがuartの入力を受け取れます. uartの入力待ちによるblocking状態と, ほかのtaskがuartの入力待ち状態であることによるblocking状態を発生させます.
- exitを入力するとtaskをexitさせterminatd状態に遷移させます. terminated状態のtaskはschedulingの対象から外されます.
- uartの入力待ちのtaskがいない場合, uartの入力はkernelが取得します. ここでもexitを受け付けるようにして system(=qemu) を終了させます.
- 
