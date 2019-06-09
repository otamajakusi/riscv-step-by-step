# Step 9

## lazy loading.
このstepではlazy loadingについて説明します. lazy loadingはプログラムをメモリにロードする手法の一つで, user modeプログラムの実行前にプログラムをメモリにロードするのではなく, user modeプログラムがpageにアクセスした段階でそのpageをメモリにロードします.

### シーケンス
今回は以下のシーケンスで実装を行いました.
1. user modeプログラムがアクセスする予定のページテーブルエントリを用意する. ただしV-bitは設定しない.
2. user mode プログラムの実行を開始する.
3. page faultが発生する.
4. page faultはuser modeプログラムによるものか確認する.
5. page faultの要因とpage faultが発生したエラーアドレスが適切であることを確認する. page faultが発生したエラーアドレスはmtvalで示される.
	a. page faultの要因が `Instruction page fault` の場合, エラーアドレスがexecute flagを持つプログラムセグメントであることを確認する.
	b. page faultの要因が `Load page fault` の場合e, エラーアドレスがread flagを持つプログラムセグメントであることを確認する.
	c. page faultの要因が `Store/AMO page fault` の場合, エラーアドレスがwrite flagを持つプログラムセグメントであることを確認する.
6. 5で得られたプログラムセグメントからpage faultが発生したアドレスのpageをメモリにロードする.
7. 6でロードしたページのページテーブルエントリのV-bitを設定しメモリ同期命令を発行する.
8. page faultが発生したアドレスからuser modeプログラムを再開させる.

### メリット/デメリット
メリット:
- 実行に必要なプログラムのpageのみロードされるため少ないメモリでプログラムを実行できる
- プログラムの開始時にプログラムをロードしないため開始時間を短くできる

デメリット:
- pageへの初回アクセス時間が長くなる

### 変更点(TODO)