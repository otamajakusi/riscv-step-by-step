# Step 13

## mutex, condition variable. futex!
このstepでは前のstepで導入したthreadで使われる mutex と condition
variableを実装します.
mutexは.... 詳細は下記mutexで説明をします.
condition variableは... 詳細は下記 condition variableで説明をします.

### mutex
#### mutex_example
### condition variable (=cond_var)
#### condvar_example

### 変更点/ポイント
futex syscallを導入します.
atomic operationについて説明します.
- 生で書く / gcc built-in 2つのオプションがる.
user空間へのread/writeを行う関数を導入します.
userプログラムにmutex_example, condvar_exampleを導入します.
