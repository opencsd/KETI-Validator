#pragma once
#include <string>
#include <vector>
const int lineitemRow =  6000000;
const double lineitemSize =62700;
const double lineitemCol = 16;
const int ordersRow =1500000;
const double ordersSize = 20580;
const double orderCol = 9;
const int partRow= 200000;
const double partSize= 3534;
const double partCol = 9;
const int customerRow= 15000;
const double customerSize= 2789;
const double customerCol = 8;
const int partsuppRow= 800000;
const double partsuppSize = 12105;
const double partsuppCol = 5;
const int nationRow =25;
const double nationSize =0.02;
const double nationCol = 4;
const int regionRow = 5;
const double regionSize =0.02;
const double regionCol = 3;
const int supplierRow =10000;
const double supplierSize= 187;
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
    float filterRatio = 1;
    int topTableRow = 0;
    std::string snippetOriginal = "";
};

struct StorageCalcInfo{
    int workID = 0;
    float curRatio = 1;
    int joinRow = 0;
    int joinTableRow = 0;
    int aggRow = 0;
    int havingRow = 0;
    int existRow = 0;
    int depjoinRow = 0;
    int unionRow = 0;
    int storFilterRow = 0;
    int groupByRow = 0;
    int topTableRow  = 0;
    int leftouterJoinRow = 0;
    int inRow = 0;
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
    double aggregateRow;
    double executionTime;
};

struct csdValidtion{
    int validationID;
    int storageID;
    double cpuUsage;
    double powerUsage;
};

const double ssdAllWeight = 1.0;

const double ssdscanTime =1208000.0;
const double ssdaggTime =559700.0;
const double ssdRjoinTime =247.0;
const double ssdBjoinTime = 445.0;
const double ssdGjoinTime = 556.0;
const double ssdYjoinTime = 703.0;
const double ssdOjoinTime = 1098.0;

const double ssdCPUWeight =50;
const double ssdPowerWeight =71.5;


const long query1join =0;
const long query2join =12000000;
const long query3join= 7000000;
const long query4join =21000000;
const long query5join =160000000;
const long query6join =0;
const long query7join =170000000;
const long query8join =79000000000;
const long query9join =410000000000;
const long query10join =1700000;
const long query11join =65000;
const long query12join =4700000;
const long query13join =20000000;
const long query14join =1500000;
const long query15join =750;
const long query16join =2300000;
const long query17join =1200000;
const long query18join =14000;
const long query19join =10000;
const long query20join =1200000000;
const long query21join =12000000000;
const long query22join =3500000;

const double csdpowerweight= 50.025;
const double csdRedCPUWeight = 99.65;
const double csdOrangeCPUWeight = 149.23;
const double csdGreenCPUWeight = 136.6;
const double csdYellowCPUWeight = 183.71;
const double csd13CPUWeight = 121.99;

const double qemuRedWeight = 74.727;
const double qemuOrangeWeight = 20.372;
const double qemuYellowWeight = 28.443;
const double qemuBrownWeight = 44.932;
const double qemuGreenWeight = 9.87;

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
const double storageq8CPU= 1015.69;
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
const double storageq14time =2.56;
const double storageq15time =1.72;
const double storageq16time =8.69;
const double storageq17time =2415;
const double storageq18time =55.87;
const double storageq19time =0.16;
const double storageq20time =5231;
const double storageq21time =26000;
const double storageq22time =7325;