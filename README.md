# KETI-VALIDATOR
# Emulate CSD validator using QEMU
CSD 환경을 에뮬레이팅 하기 위해 같은 ARM cortexa53을 CPU로 쓰는 Raspberry Pi 3b를 기반으로 한 디스크 이미지 기반 동작 

## Install QEMU 6.2 from source:
1. 미리 설치된 QEMU가 없도록 확인
```
$ sudo apt-get autoremove qemu*
```

2. qemu 6.2 다운로드 [here](https://download.qemu.org/qemu-6.2.0.tar.xz) - 
```
$ wget https://download.qemu.org/qemu-6.2.0.tar.xz
```

3. QEMU 6.2의 소스 코드 압축해제
```
$ tar xvJf qemu-6.2.0.tar.xz
```

4. QEMU 6.2 의 요구 사항 설치- 
```
$ sudo apt-get install build-essential zlib1g-dev pkg-config libglib2.0-dev \
binutils-dev libboost-all-dev autoconf libtool libssl-dev libpixman-1-dev \
python-capstone virtualenv ninja-build
```

5. qemu-6.2.0 로 이동
```
$ cd qemu-6.2.0
```

6. Inside the qemu-6.2.0 directory, configure the build (default settings would work, but you can fix it as needed)
```
$ ./configure
```
7. Make the build with any number of physical threads you have. 
```
$ make -j8
```
8. Add the build directory to the path in ```.bashrc``` file. Add the following line to the ```.bashrc``` file
```
export PATH=$PATH:<path-to-qemu-6.2.0/build>
```
Change ```<path-to-qemu-6.2.0/build>``` to appropriate value

9. Restart the terminal or reload the environment by executing  
```
$ source ~/.bashrc
```

## Download Required Files:
For the next step you need to download the kernel, dtb and disk image and save them to any folder. You would also need to download the launch script. 
1. Launch Script: [launch.sh](launch.sh) 
2. Kernel: [kernel8.img](kernel8.img)
3. DTB: [bcm2710-rpi-3-b-plus.dtb](bcm2710-rpi-3-b-plus.dtb)
4. Disk Image (disk.img): You can use download either the .xz format or the .img format directly -
    
    a. [disk.xz](https://drive.google.com/file/d/19rMQRcKmV8g-wfR1IOHp4uLPXYmExoCx/view?usp=sharing) Archive .xz format (~833MB) need to decompress using (``` $ tar xvJf disk.xz ```)  

    b. [disk.img](https://drive.google.com/file/d/19cPWTYIuFTxdRxrnRhw2qMevb4umpK_H/view?usp=sharing) Decompressed .img format (~4G) 

## Execute Launch Script:
Once you are done downloading, put all the files in the same directory and you should be able to execute the ```launch.sh``` without any trouble. When prompted, the id and the password for default account -  

**Username:** pi <br>
**Password:** raspberry

If you did this successfully you should be able to see this - 
![Success!](success.png "Success")
## References:
These are the references used for executing the script. However, you might need to modify the files directly obtained from these sources. 
- Kernel: https://github.com/dhruvvyas90/qemu-rpi-kernel/blob/master/native-emulation/5.4.51%20kernels/kernel8.img
- DTB: https://github.com/dhruvvyas90/qemu-rpi-kernel/blob/master/native-emulation/dtbs/bcm2710-rpi-3-b-plus.dtb
- Disk Image: https://downloads.raspberrypi.org/raspbian/images/raspbian-2020-02-14/2020-02-13-raspbian-buster.zip
- QEMU 6.2.0: https://download.qemu.org/qemu-6.2.0.tar.xz 
