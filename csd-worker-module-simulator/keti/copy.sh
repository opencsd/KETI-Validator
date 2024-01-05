#!/usr/bin/env bash

file_path1="/home/ngd/workspace/csd-worker-module/csd_pba_compress_scan"
file_path2="/home/ngd/workspace/csd-worker-module/csd_pba_compress_scan/keti"
password="1234"

num_of_csd=$1 # 1, 2, 3, 4, 5, 6, 7, 8
file_name=$2 # keti or file_name

if [ "$2" == "keti" ] || [ "$2" == "testsnippet" ]; then
    cd /root/workspace/CSD-Worker-Module/csd_pba_compress_scan
    for((i=1;i<$num_of_csd+1;i++)); do
        ip="10.1.$i.2"
        echo scp -rp $file_name root@$ip:$file_path1 copying...
        sshpass -p $password scp -rp -o ConnectTimeout=60 $file_name root@$ip:$file_path1
    done
else
    for((i=1;i<$num_of_csd+1;i++)); do
        ip="10.1.$i.2"
        echo scp -p $file_name root@$ip:$file_path2 copying...
        sshpass -p $password scp -p -o ConnectTimeout=60 $file_name root@$ip:$file_path2
    done
fi


