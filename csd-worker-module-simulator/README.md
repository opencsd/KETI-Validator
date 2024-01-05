## Introduction of KETI-OpenCSD CSD-Container
-------------
![Alt text](/%EC%82%AC%EC%A7%84/pushdown.jpg)

CSD-Container for KETI-OpenCSD Platform

Developed by KETI

## Contents
-------------
[1. Requirement](#requirement)

[2. How To Install](#How-To-Install)

[3. Modules](#modules)

[4. Governance](#governance)

## Requirement
-------------
>   NGD CSD

>   Ubuntu 20.04.2 LTS (GNU/Linux 4.14.1_newport_4.1_5.2+ aarch64)

>   RapidJSON

## How To Install
-------------
# How To Build
```bash
git submodule add https://github.com/facebook/rocksdb
cd keti
./1.build.sh
```

## Modules
-------------
### CSD Input Layer
-------------
A module that receives the snippet delivered by the Storage Engine Node.

Parsing the received snippet and storing it in a structure that can be used inside the CSD

Check the snippet information, divide it into scan, scan&filter operations, and deliver it to the corresponding parallel execution queue

### CSD Scanning
-------------
Module that reads table data inside CSD

Read data by accessing the block device as a PBA (Physical Block Address) rather than through the file system

### CSD Filtering
-------------
Module that performs Where conditional filtering on blocks scanned by CSD Scanning

CSD Filtering module operates based on Where clause

<pre>
<code>
//loop row
for (int i = 0; i < rowcount; i++)
    {
        //loop whereclause
        for (int j = 0; j < whereclause.Size(); j++)
        {
            switch (whereclause operator)
                {
</code>
</pre>

### CSD Merge Manager
-------------
Module that merges scanned and filtered data

Considering that the data will be reduced after the CSD Filtering module operates, the number of communication is reduced by combining blocks.

### CSD Return Interface
-------------
A module that delivers the data merged by the CSD Merge Manager to the Buffer manager of the Storage Engine Node.

Merge information is delivered in JSON format, and merge data is delivered in raw data format.

## Governance
-------------
This work was supported by Institute of Information & communications Technology Planning & Evaluation (IITP) grant funded by the Korea government(MSIT) (No.2021-0-00862, Development of DBMS storage engine technology to minimize massive data movement)

## Others
-------------
Due to the structure change, in the first half of the year, we worked on local repositories and private github.
> https://github.com/KETI-OpenCSD/CSD-Container
