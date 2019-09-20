# 技術書典7にて頒布した [RISC-V step-by-step](https://techbookfest.org/event/tbf07/circle/5765209499107328) のサポート情報です.

## 全般
### PMPについて
PMP(=Physical Memory Protection) の実装は完全でないことが分かりました.
まず PTE(=Page Table Entry) のページテーブルウォークは supervisor mode
でアクセスされますが PTE に PMP を設定していません.

### riscv-qemu-3.1 以外のサポート
現状 https://github.com/riscv/riscv-qemu/tree/riscv-qemu-3.1
以外のQEMUに対応できていません. これは上に書いた通り PMP
への対応が不完全なのもあるのですが QEMU 側のPMPへの対応も不完全で現在対中です.
https://github.com/otamajakusi/riscv-step-by-step/issues/6

### 正誤情報
1. 50ページ setup_pmp. size に 2 のべき乗を渡します.  
   誤)
   ```
   setup_pmp(pa, 0x2000);
   ```
   正)
   ```
   setup_pmp(pa,          0x1000);
   setup_pmp(pa + 0x1000, 0x1000);
   ```
