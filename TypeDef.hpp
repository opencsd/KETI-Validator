#pragma once
#include <string>
#include <vector>
const int lineitemRow =  6000000;
const double lineitemSize =850;
const double lineitemCol = 16;
const int ordersRow =1500000;
const double ordersSize =196.72;
const double orderCol = 9;
const int partRow= 200000;
const double partSize= 32.56;
const double partCol = 9;
const int customerRow= 150000;
const double customerSize= 28.56;
const double customerCol = 8;
const int partsuppRow= 800000;
const double partsuppSize =135.66;
const double partsuppCol = 5;
const int nationRow =25;
const double nationSize =0.02;
const double nationCol = 4;
const int regionRow = 5;
const double regionSize =0.02;
const double regionCol = 3;
const int supplierRow =10000;
const double supplierSize= 2.52;
const double supplierCol = 7;

enum DBMSType{
    mysql = 0,
    tibero_db = 1,
    column_db = 2,
    approximate_db = 3,
};

enum CSDType{
    NGD = 0,
    Scaleflux = 1,
    SmartSSD = 2,
};

enum SchedulingAlgorithm{
    csd_metric_score =0 ,
    block_count = 1,
    sst_spread = 2,
};

enum QueryType{
    selectQuery = 0,
    updateQuery = 1,
    insertQuery = 2,
    deleteQuery = 3,
    dcl = 4,
    ddl = 5,
    other = 6,
};

enum StorageType{
    csd = 0,
    ssd = 1,
};

struct querySnippetInfo{
    int validationID =0 ;
    int workID =0 ;
    std::string snippetType = "";
    int projectionCount =0 ;
    int filterCount =0 ;
    int groupByCount =0 ;
    int orderByCount = 0;
    int havingCount = 0;
    int limitExist = 0;
    std::string tableAlias = "";
    std::string tableName[2];
    std::vector<std::string> filterInfo;
};

struct optionInfo{
    int optionID;
    std::string userID;
    std::string optionName;
    std::string DBType;
    std::string storageType;
    std::string csdType;
    int csdCount;
    int blockCount;
    std::string schedulingAlgorithm;
    int usingIndex = 0;
};

struct validationLog{
    int validationID;
    std::string userID;
    std::string queryStatement;
    std::string timestamp;
    int optionID;
    double storageCPUUsage;
    double storagePowerUsage;
    double networkUsage;
    int scannedRow;
    int filteredRow;
    double executionTime;
    int SnippetCount;
};

struct storageValidation {
    double storageCPUUsage;
    double storagePowerUsage;
    double networkUsage;
    double scannedRow;
    double filteredRow;
    double executionTime;
};

struct csdValidtion{
    int validationID;
    int storageID;
    double cpuUsage;
    double powerUsage;
};

const double ssdscanTime =1299508.0;
const double ssdaggTime =537297.0;
const double ssdjoinTime =11.95;

const double ssdCPUWeight =99.99;
const double ssdPowerWeight =56.1244;


const int query1join =0;
const int query2join =1120000;
const int query3join=491000;
const int query4join =206000;
const int query5join =2560000;
const int query6join =0;
const int query7join =1570000;
const long query8join =81100000000;
const int query9join =486400;
const int query10join =2930;
const int query11join =650;
const int query12join =45100;
const int query13join =225000;
const int query14join =15600;
const int query15join =0.1;
const int query16join =23100;
const int query17join =2646;
const int query18join =225000;
const int query19join =2170;
const int query20join =1890;
const long query21join =1435000000;
const int query22join =17500;

const double csdpowerweight= 50.025;
const double csdBadCPUweight =189.49;
const double CSDNormalCPUweight= 157.65;
const double CSDGoodCPUWeight= 119.38;
const double csdBadScanWeight= 15368.4;
const double csdNormalScanWeight= 20392.65;
const double csdGoodScanWeight= 28232.84 ;

const double storageLowPower= 38.76;
const double storageMidPower= 44.615;
const double storageHighPower= 50.875;
const double storageMaxPower =56.616;

const double StorageStaticCPU =1.826;
const double storageVLowCPU=126.66;
const double storageLowCPU= 153.546;
const double storageq3CPU =457.264;
const double storageq5CPU=273.197;
const double storageq6CPU =6.745;
const double storageq8CPU= 838.267;
const double storageq11CPU =199.269;
const double storageq12CPU =256.654;
const double storageq19CPU= 0.869; // CSDtime * cpu

const double storageq1time =150.14;
const double storageq2time =119.95;
const double staorgeq3time =4.22;
const double storageq4time =73500;
const double storageq5time =18.03;
const double storageq6time =0.41;
const double storageq7time =81.16;
const double storageq8time =3.86;
const double storageq9time =279.1;
const double storageq10time= 13.49;
const double storageq11time= 10.49;
const double storageq12time =3.29;
const double storageq13time =15.84;
const double storageq14time =5.56;
const double storageq15time =1.72;
const double storageq16time =8.69;
const double storageq17time =2415;
const double storageq18time =55.87;
const double storageq19time =0.16;
const double storageq20time =5231;
const double storageq21time =26000;
const double storageq22time =7325;