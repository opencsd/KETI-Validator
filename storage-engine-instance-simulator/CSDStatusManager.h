// #pragma once
// #include <iostream>
// #include <unordered_map>
// #include <vector>

// #include "storage_engine_instance.grpc.pb.h"

// using namespace std;
// using StorageEngineInstance::CSDMetric;

// // struct CSDInfo{
// //     string CSDID;//M
// //     string CSDIP;//M
// //     string CSDReplica;//X
// //     int CSDWorkingBlock;//A
// //     vector<string> SSTList;
// //     vector<string> CSDList;
// //     double Metric;
// //     unordered_map<string,string> CSD_IP_map; //csd의 ip정보가 담긴 맵 <csdname, csdip>
// //     unordered_map<string, int> CSDWorkingBlock_map; //csd의 block work 수 가 담긴 맵 <csdname, csdworknum>
// //     unordered_map<string,double> CSD_Metric_map;
// //     unordered_map<string,string> SSTCSD_map; //sst파일의 위치 <sstname, csdname>
// //     int blockcount_;
// // };

// struct CSDInformation{
//     StorageEngineInstance::CSDMetric csd_data;
// };

// class CSDManager{
//     public:
//     static void CSDInfoinit();
//     vector<string> getCSDID();
//     CSDInformation getCSDInfo(string CSDID);

//     private:
//         unordered_map<string,CSDInformation> csd_csdinfo_map; //<CSDID, CSDInfo>
        
// };