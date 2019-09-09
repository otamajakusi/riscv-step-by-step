# MACHINE MODE WITH TIMER INTERRUPT
このステップではRISC-Vのタイマー割り込みを説明します. 前回のステップでは machine
mode での ecall 命令の実行による例外を扱い, CSR mcause から例外/割り込み要因,
CSR mepc から例外/割り込みアドレスを取得しました.
例外/割り込みはどちらも発生直後に PC 
を特定のアドレス(=trap_vector)に遷移させる仕組みです.
例外/割り込みの2つの用語の使い分けですが, 例外はCPU内部で発生,
割り込みはCPU外部のデバイスなどにより発生する, 程度の区分けでよいと思います.
