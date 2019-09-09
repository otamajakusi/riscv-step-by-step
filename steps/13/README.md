# MUTEX WITH ATOMIC OPERATION AND FUTEX
いよいよ最後のステップです. 最後のステップは前回のステップで実装したスレッドで使用されるミューテックスを実装します.
ミューテックスはスレッド間の排他制御を行います. 例えば以下の increment() を複数のスレッドが同時に呼び出した場合, count の値は不定になるでしょう.  
```c
volatile int count = 0;
void increment()
{
    count ++;
}
// increment() w/o mutex lock
```
なぜなら count ++ という操作が「読み出し」た値に「1を加算」して「書き込む」, という一息(=atomic)ではない動作を行うため, 複数スレッドで同時に実行すると, スレッドＡが読み出した値に1を加算して, スレッドＢが読み出した値に1を加算して書き込みを行い, スレッドＡが書き込みを行う, という順番に実行されると値は増えないことになります.
スレッドＡとスレッドＢのように複数のスレッドが共有する変数にアクセスする前に排他制御を行うとこの問題は解決します. count を操作できるのは1つのスレッドのみとなります.
```c
volatile int count = 0;
void increment()
{
    mutex_lock(mutex);
    count ++;
    mutex_unlock(mutex);
}
// increment() w/ mutex lock
```
考え方はとても簡単ですが, 実装はなかなか複雑です!
