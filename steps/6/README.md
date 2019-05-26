# Step 6

## user mode hello world with system call.
このstepではuser modeプログラムからsystem call(=syscall)を使用しhelllo worldを表示する方法について説明します. 前回のstepで実行されたuser modeプログラムは直接デバイスにアクセスするため, アクセスする領域を限定できていません. 一般的にuser mode上では任意のプログラムが実行されるため, machine modeを含むuser mode以外のmodeが使用する領域への任意アクセスと別のuser modeプログラムが実行される領域への任意アクセスは制限されなくてはなりません.

### kernel, system call
上で説明した通り, user modeプログラムが別のプログラムにアクセスすることは制限されますがその制限はkernelにより実施されます. kernelは複数のプログラムの実行の切り替えやsyscallの提供なども行います. syscallとは, user modeプログラムが直接アクセスできないリソースへkernelが代わりにアクセスする仕組みです. 例えば実行中のuser modeプログラムのリストはkernelのリソースといえるので, syscallにより提供されるかもしれません.

このレポジトリではkernelはmachine modeで実装されます.
今回UARTに1文字出力するsyscallとexit syscallを実装し, user modeが動作する領域を制限します.

