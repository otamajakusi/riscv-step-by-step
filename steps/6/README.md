# USER MODE HELLO WORLD WITH SYSTEM CALL
このステップでは user mode プログラムから system call(=syscall) を使用し "helllo
world" を表示する方法について説明します. 前回のステップで実行された user mode
プログラムは直接デバイスにアクセスするため,
アクセスする領域を限定できていません. 一般的に user mode
上では任意のプログラムが実行されるため, user mode
プログラムが自身の領域以外(他の user mode プログラム, あるいは machine mode
プログラム)にアクセスできることはセキュリティ的に望ましくありません.

## kernel, system call
上で説明した通り, user mode
プログラムが別のプログラムにアクセスすることは制限されなくてはなりませんがその制限は
kernel により実施されます. kernel は複数のプログラムの実行の切り替えや system
call(=syscall) の提供などを行います. syscall とは user mode
プログラムが直接アクセスできないリソースへ kernel
がアクセスを肩代わりする仕組みです. 例えば実行中の user mode
プログラムのリストは kernel のリソースといえるので syscall により user mode
に提供されるかもしれません.
本ドキュメントでは kernel は machine mode で実装されます. m.elf が kernel です.
今回 UART に1文字出力する write syscall と, プログラムを終了する exit syscall
の2つを実装し, user mode が動作する為に必要な領域を制限します.

