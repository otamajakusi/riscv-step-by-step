# Step 7

## virtual address / page protection.
このstepでは仮想アドレス(=virtual address)上でuser modeプログラムを実行する方法を説明します. RISC-Vでは supervisor mode と user mode が virtual address で動作することができます. 一方 machine mode は常に物理アドレスでのアクセスとなります. virtual addressはアドレス変換機構によりアクセス権が確認されたのち物理アドレス(=physical address)に変換されアクセスされます.





-mcode=medany, position independent
strip, objcopy, diff

tick, quantum
scheduling, round robin
毎回同じところで割り込みが発生する理由がわからない



<http://www.coins.tsukuba.ac.jp/~yas/coins/os2-2013/2014-01-23/>

<https://www.valinux.co.jp/technologylibrary/document/linuxkernel/timer0002/>

<https://www.oreilly.com/library/view/linux-device-drivers/9781785280009/4041820a-bbe4-4502-8ef9-d1913e133332.xhtml>