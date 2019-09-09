CREATED, READY, RUNNING AND TERMINATED - TASK STATE
このステップではタスクの状態の管理をします.
これまでのステップのタスクはずっと実行していたため状態を管理する必要がありませんでした. 状態の管理していなかったものの, タスクは ready 状態と running 状態を交互に繰り返していました.
```
  +-------+     +-------+
  |       |---->|       |
  | ready |     |running|
  |       |<----|       |
  +-------+     +-------+
task state: read and running
```
running 状態はCPUを使用してプログラムを実行している状態, ready 状態はいつでもrunning状態に遷移できる状態です.
今回新たに terminated 状態を導入します.
terminated 状態はプログラムが終了した状態で ready や running 状態には戻れません.
このステップでは exit syscall を発行したタスクの状態を terminated 状態に遷移させます.
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
                |termina|
                |  ted  |
                +-------+
task state: ready, running and terminated
```
terminated 状態を導入すると, "terminated 状態のタスクをを ready/running状態にしない" という処理が必要になります.
ready 状態のタスクを連結リスト(=linked list)でつなぎ, そのリストから単に切り離すことで terminated 状態を表現できそうですが, 今回は差分を小さく, 分かりやすくするために, 単にタスク構造体に状態を追加する実装にします.
```c
typedef enum {
    task_state_created = 0,
    task_state_ready,
    task_state_running,
    task_state_blocked,
    task_state_terminated,
} task_state_t;

typedef struct {
    uintptr_t       entry;
...
    uintptr_t       mepc;
    task_state_t    state;
} task_t;
```
さて terminated 状態を導入するとアイドルタスク(=アイドルスレッドとも呼ばれる)と呼ばれるkernel内で実行される特別なタスクを導入する必要が出てきます. アイドルタスクは他のすべてのタスクが running 状態に遷移できない場合 - 特にこのステップの場合すべてのタスクが terminated 状態となる場合 - に実行される特別なタスクで単に無限ループで実装される場合, あるいは(消費電力を小さくする目的などで)割り込みが起こるまで待つ命令を発行する場合があります. RISC-Vにも wfi(=wait for interrupt) という命令があります.
なおアイドルタスクは "タスク" と呼んでいますが単なる wfi 命令なのでタスクの退避/復帰処理は不要です.
