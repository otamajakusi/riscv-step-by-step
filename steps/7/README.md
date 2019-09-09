# VIRTUAL ADDRESS / PAGE PROTECTION
このステップでは仮想アドレス(=virtual address)上で user mode
プログラムを実行する方法を説明します. RISC-V では supervisor mode と user mode
が virtual address で動作することができます. 一方 machine mode
は常に物理アドレス(=physical address)でのアクセスとなります. virtual address
はアドレス変換機構によりアクセス権が確認されたのち物理アドレス(=physical
address)に変換されアクセスされます. 仮想アドレスを採用することで,
同じ仮想アドレスであっても異なる物理アドレスにマップできるため同じ仮想アドレスにアクセスするプログラムを複数同時に動かすことが可能になります.
また本ドキュメントでは行いませんが読み込み専用の領域(.text,
.rodataなど)を複数プログラムで共有することも簡単にできるようになります.
