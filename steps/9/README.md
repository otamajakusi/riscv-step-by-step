# Step 9

## lazy loading.
このstepではlazy loadingについて説明します. lazy loadingはプログラムをメモリにロードする手法の一つで, user modeプログラム(=task)の実行前にプログラムをメモリにロードするのではなく, taskがpageにアクセスしたタイミングでそのpageをメモリにロードします.

### シーケンス
今回は以下のシーケンスでlazy loadingを実装しました.
1. taskがアクセスする予定のページテーブルエントリを用意する. ただしV-bitは設定しない.
2. taskの実行を開始する.
3. page faultが発生する.
4. page faultは適切なtaskによるものか確認する.
5. page faultの要因とpage faultが発生したエラーアドレスが適切であることを確認する. page faultが発生したエラーアドレスはmtvalで示される.
	a. page faultの要因が `Instruction page fault` の場合, エラーアドレスがexecute flagを持つプログラムセグメントであることを確認する.
	b. page faultの要因が `Load page fault` の場合, エラーアドレスがread flagを持つプログラムセグメントであることを確認する.
	c. page faultの要因が `Store/AMO page fault` の場合, エラーアドレスがwrite flagを持つプログラムセグメントであることを確認する.
6. 5で得られたプログラムセグメントからpage faultが発生したアドレスのpageをメモリにロードする.
7. 6でロードしたページのページテーブルエントリのV-bitを設定しメモリ同期命令を発行する.
8. page faultが発生したアドレスからtaskを再開させる.

### メリット/デメリット
メリット:
- 実行に必要なプログラムのpageのみロードされるため少ないメモリでプログラムを実行できる
- プログラムの開始時にプログラムをロードしないため開始時間を短くできる

デメリット:
- pageへの初回アクセス時間が長くなる

### 変更点
今回から少しずつセキュリティに気を配るようにしていきたいと思います. ここでのセキュリティとは 1. task が 別の task にアクセスできないこと, 2. task が kernel にアクセスできないことの2点です. 例えば steps/8/m/syscall.c で
```c
static void handle_write(uintptr_t* regs, const task_t* curr)
{
    // FIXME: make sure, the buffer address is in the appropriate range.
    char *c = (char*)(regs[REG_CTX_A2] + curr->pa[0]);
```
REG_CTX_A2の値は \_\_write syscallの第2引数の値で task から操作可能です. `__write(0, -0x100000, 0)` を呼び出すことでkernel領域あるいは別のtaskの物理メモリにアクセス(読み出し)できてしまいます. lazy loadingを導入するにあたってpage faultが適切かどうかを判定する必要があるためこれまでより厳格にtaskのふるまいについてチェックしていきます.

#### m/elfldr.c
`check_elf`: program headerが適切なレンジであることの確認を行います. taskのELFファイルはkernel埋め込みですが外部から読み込んだものとして扱うことにしています.
`get_phdr_from_va`: virtual address(va)とアクセス種別(read, write, exec)から該当するprogram headerを取り出します. page fault時にこの関数を使用してprogram headerを取り出します.
`load_program_segment`: virtual address(va)を含むプログラムセグメントのデータを1ページ(最終ページでサイズが1ページに満たない場合はそのサイズを)ロードします.

#### m/main.c
`handle_page_fault`: mcauseからアクセス種別(read, write, exec)を判別し, mtvalからエラーアドレスを得て `get_phdr_from_va` と `load_segment_segment` でページをロードします.
`allocate_pa`: 物理アドレスをallocateします. 実装を見ていただくと分かりますが解放はできません.
`setup_va`: ELFのプログラムセグメントに従ってPage Table Entry(PTE)を作成します. ただしpage faultを発生させるためにV-bitは設定しません.

#### m/vm.c
`va_to_pa`: taskのPTEをたどってvirtual address(va)をphysical address(pa)に変換します. 変換できないvaは不正として扱われます. これによりtask.hでpaを管理する必要がなくなりました. handle_writeで参照する `regs[REG_CTX_A2]`が安全になりました.

