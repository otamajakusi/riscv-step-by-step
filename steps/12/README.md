# Step 12

## thread
このstepではthreadの導入を行います. threadは同じメモリ保護空間で動作する実行の単位で, 
これまでのtaskは別のメモリ保護空間で動作しましたが,,,

### 変更点/ポイント
threadの作成, 待機を行うsyscallを作成します. cloneとwaitpidです.
kernelではthreadもtask構造体で管理します.
threadの作成はuser modeから行われるため,
kernel側ではuserが必要になる程度のtask構造体をあらかじめ用意します.
user側ではclone syscallを使用して thread_create を, waitpid syscallを使用して
thread_join を実装します. それぞれの動作は pthread_create, pthread_join
とできるだけ似た動作になるように実装していますが異なる部分も存在します.
(thread_joinの引数とか)
RISC-Vに限定した話題はこのstepにはありません.

### プログラムの説明
...
thread_create -> __clone -> clone_task -> thread_entry -> thread_exit -> exit ->
waitpid -> thread_join

pthread_createは entryからreturnするとthreadがexitする点.

前のstepでblockedを導入したので waitpidできる.

