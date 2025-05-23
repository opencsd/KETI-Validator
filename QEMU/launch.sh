#!/bin/bash
# OPTION=""

# while [[ "$1" != "" ]]; do
#     case $1 in
#         -csd[1-9])
#             OPTION="$1"
#             disk_number="${OPTION#-csd}"
#             FORWARD_PORT=$((7776+disk_number))
#             NETWORK_PORT=$((4444+disk_number))
#             ;;
#         *)
#             echo "Unknown Option: $1"
#             exit 1
#             ;;
#     esac
#     shift
# done
#disk_image="csd_disk${disk_number}.img"
disk_image="disk.img"


export PATH=$PATH:/root/workspace/yjk/QEMU/qemu-6.2.0/build/
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH 

source ~/.bashrc

qemu-system-aarch64 \
    -M raspi3b \
    -cpu cortex-a53 \
    -append "rw earlyprintk loglevel=8 console=ttyAMA0,115200 dwc_otg.lpm_enable=0 root=/dev/mmcblk0p2 rootdelay=1" \
    -dtb bcm2710-rpi-3-b-plus.dtb \
    -sd "$disk_image" \
    -kernel kernel8_qemu.img \
    -m 1G -smp 4 \
    -serial stdio \
    -usb -device usb-mouse -device usb-kbd \
	-device usb-net,netdev=br0 \
	-netdev user,id=br0,hostfwd=tcp::5555-:22,hostfwd=tcp::4445-:4445 \
