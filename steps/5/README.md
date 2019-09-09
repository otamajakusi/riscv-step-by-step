# USER MODE HELLO WORLD
このステップではRISC-Vでuser modeプログラムを実行する方法について説明します.
前回のステップでは ELF をロードし machine mode でそのプログラムを実行しました.
その際 CSR mstatus.mpp に machime mode を設定し mret 命令を実行することで
machine mode から同じ machine mode へ遷移をさせましたが, CSR mstatus.mpp に user
mode を設定し mret 命令を実行することで machine mode から user mode
へ遷移をさせることができます. ですが, RISC-V の Physical Memory Protection(=PMP)
の初期設定では user mode, supervisor mode
では物理メモリに直接アクセスすることができません.
