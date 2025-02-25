# KETI Validator

![Project Logo](image.png)

# Emulate CSD on Raspberry Pi 3b using QEMU in 64 bit 

## Install QEMU 6.2 from source:
1. Make sure you do not have QEMU preinstalled. Remove the existing installation if needed by executing the following - 
```
$ sudo apt-get autoremove qemu*
```

2. Download QEMU 6.2 from [here](https://download.qemu.org/qemu-6.2.0.tar.xz) - 
```
$ wget https://download.qemu.org/qemu-6.2.0.tar.xz
```

3. Decompress the source code for QEMU 6.2
```
$ tar xvJf qemu-6.2.0.tar.xz
```

4. Install the prerequisites for QEMU 6.2 - 
```
$ sudo apt-get install build-essential zlib1g-dev pkg-config libglib2.0-dev \
binutils-dev libboost-all-dev autoconf libtool libssl-dev libpixman-1-dev \
python-capstone virtualenv ninja-build
```

5. Change to qemu-6.2.0 directory 
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
1. Kernel: [kernel8.img](kernel8.img)
2. DTB: [bcm2710-rpi-3-b-plus.dtb](bcm2710-rpi-3-b-plus.dtb)
3. Disk Image (disk.img): You can use download either the .xz format or the .img format directly -
    
    a. [disk.xz](https://drive.google.com/file/d/19rMQRcKmV8g-wfR1IOHp4uLPXYmExoCx/view?usp=sharing) Archive .xz format (~833MB) need to decompress using (``` $ tar xvJf disk.xz ```)  

    b. [disk.img](https://drive.google.com/file/d/19cPWTYIuFTxdRxrnRhw2qMevb4umpK_H/view?usp=sharing) Decompressed .img format (~4G) 

## Execute Launch Script:
Once you are done downloading, put all the files in the same directory and you should be able to execute the ```launch.sh``` without any trouble. When prompted, the id and the password for default account -  

**Username:** pi <br>
**Password:** raspberry

If you did this successfully you should be able to see this - 
![Success!](success.png "Success")

# Execute QEMU CSD Validator
## Modify Port
modify launch.sh script to set port you want
```
-netdev user,id=net0,hostfwd=tcp::"$FORWARD_PORT"-:22,hostfwd=tcp::"$NETWORK_PORT"-:"$NETWORK_PORT"
```
## Execute DB-Connector Instance 
launch db-connector-instance using docker
copy snippet file to db-connector-instance docker
```
tar -czf- snippets/ | docker exec -i simulator_db_connector_container tar -C / -xzf-
```
## Run Query Using Query-Script
put option that what you want to execute
```
./runquery 1 11
```

## References:
These are the references used for executing the script. However, you might need to modify the files directly obtained from these sources. 
- Kernel: https://github.com/dhruvvyas90/qemu-rpi-kernel/blob/master/native-emulation/5.4.51%20kernels/kernel8.img
- DTB: https://github.com/dhruvvyas90/qemu-rpi-kernel/blob/master/native-emulation/dtbs/bcm2710-rpi-3-b-plus.dtb
- Disk Image: https://downloads.raspberrypi.org/raspbian/images/raspbian-2020-02-14/2020-02-13-raspbian-buster.zip
- QEMU 6.2.0: https://download.qemu.org/qemu-6.2.0.tar.xz 
