#!/usr/bin/env bash

file_path1="/home/ngd/csd-worker-module/opencsd-storage-node12"
file_path2="/home/ngd/csd-worker-module/opencsd-storage-node12/keti"
password="1234"

num_of_csd=$1 # 1, 2, 3, 4, 5, 6, 7, 8
file_name=$2 # keti or file_name

if [ "$2" == "keti" ] || [ "$2" == "testsnippet" ]; then
    cd /root/csd-worker-module/opencsd-storage-node11
    for((i=1;i<$num_of_csd+1;i++)); do
        ip="10.1.$i.2"
        echo scp -rp $file_name ngd@$ip:$file_path1 copying...
        sshpass -p $password scp -rp $file_name ngd@$ip:$file_path1
    done
else
    for((i=1;i<$num_of_csd+1;i++)); do
        ip="10.1.$i.2"
        echo scp -p $file_name ngd@$ip:$file_path2 copying...
        sshpass -p $password scp -p $file_name ngd@$ip:$file_path2
    done
fi


