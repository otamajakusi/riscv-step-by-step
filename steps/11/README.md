BLOCKED - TASK STATE
このステップでは前回のステップで導入したタスクの状態に新たに blocked 状態を追加します. blocked 状態はイベントを待つ状態で, running状態から遷移します. 待っていたイベン
トが完了すると blocked 状態が解除され ready 状態へと遷移します.
このステップではキーボードの入力イベントを待ち, blocked 状態へと遷移させます.
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
task state: ready, running, terminated and blocked
```
今回は以下の2点を新たに導入します.
- キーボードイベントのハンドリング
- キーボード入力を取得する read syscall と blocked 状態の管理
