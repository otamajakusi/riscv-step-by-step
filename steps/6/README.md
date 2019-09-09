# USER MODE HELLO WORLD WITH SYSTEM CALL
このステップでは user mode プログラムから system call(=syscall) を使用し "helllo
world" を表示する方法について説明します. 前回のステップで実行された user mode
プログラムは直接デバイスにアクセスするため,
アクセスする領域を限定できていません. 一般的に user mode
上では任意のプログラムが実行されるため, user mode
プログラムが自身の領域以外(他の user mode プログラム, あるいは machine mode
プログラム)にアクセスできることはセキュリティ的に望ましくありません.

