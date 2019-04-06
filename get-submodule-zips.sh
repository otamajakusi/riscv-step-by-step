#!/bin/bash

IFS='
'
submodules=$(git submodule status)

urls=$(mktemp)

if false; then

for submodule in ${submodules}; do
    hash=$(echo ${submodule} | awk '{sub("-","",$1); print $1}')
    path=$(echo ${submodule} | awk '{print $2}')
    url=$(cat .gitmodules | awk '/^\[submodule "'${path}'"\]/ {getline; getline; sub("../","",$3); gsub(".git","",$3); printf $3}')
    echo "https://github.com/riscv/${url}/archive/${hash}.zip" >> ${urls}
done

cat ${urls} | xargs -n 1 -P 0 wget

fi

for submodule in ${submodules}; do
    hash=$(echo ${submodule} | awk '{sub("-","",$1); print $1}')
    path=$(echo ${submodule} | awk '{print $2}')
    url=$(cat .gitmodules | awk '/^\[submodule "'${path}'"\]/ {getline; getline; sub("../","",$3); gsub(".git","",$3); printf $3}')
    if [ ! -d ${url}-${hash} ]; then
        echo "unzip ${hash}.zip to ${url}-${hash}"
        unzip -q ${hash}.zip
    fi
    rm -rf ${path}
    ln -s ${url}-${hash} ${path}
done


