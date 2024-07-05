#pragma once
#include <vector>
#include <unordered_map>
#include <fcntl.h>
#include <unistd.h>
#include <thread>
#include <fstream>
#include <string>
#include <algorithm>
#include <queue>
#include <tuple>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <mutex>
#include <condition_variable>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <list>
#include <unordered_set>
#include <bitset>
#include <stack>
#include <iostream>
#include "internal_queue.hpp"
#include "keti_type.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h" 


#define NCONNECTION 8
#define NUM_OF_BLOCKS 15
#define BUFF_SIZE (NUM_OF_BLOCKS * 5000)

class WorkQueue;

struct ColData{
  vector<string> strvec;
  vector<int64_t> intvec;
  vector<double> floatvec;
  vector<bool> isnull;//해당 인덱스 null 여부 (조인시)
  int type;//0 string, 1 int, 2 float (KETI_VECTOR_TYPE)
  int row_count;
  ColData(){
    strvec.clear();
    intvec.clear();
    floatvec.clear();
    isnull.clear();
    type = TYPE_EMPTY; 
    row_count = 0;
  }
};

struct WorkBuffer {
  string table_alias;//*결과 테이블의 별칭
  vector<string> table_column;//*결과 컬럼명
  unordered_map<string,ColData> table_data;//결과의 컬럼 별 데이터(컬럼명,타입,데이터)
  int left_low_count;//*남은 블록 수(csd 결과 병합 체크용) -> Monitoring Container 통신
  int status;//작업 상태
  int row_count;//행 개수
  condition_variable cond;
  mutex mu;

  WorkBuffer(){
    table_alias.clear();
    table_column.clear();
    status = NotFinished;
    table_data.clear();
    left_low_count = 0;
    row_count = 0;
  }
};

struct QueryBuffer{
  int query_id;//*쿼리ID
  unordered_map<int,WorkBuffer*> work_buffer_list;//워크버퍼 <key:*workID, value:Work_Buffer>
  unordered_map<string,int> tablename_workid_map;//<key:table_name, value: work_id>

  QueryBuffer(int qid)
  :query_id(qid){
    work_buffer_list.clear();
    tablename_workid_map.clear();
  }
};

struct BlockResult{
    int query_id;
    int work_id;
    double executionTime;
    int row_count;

    BlockResult(){}
    BlockResult(const char* json_){
        rapidjson::Document document;
        document.Parse(json_);

        query_id = document["queryID"].GetInt();
        work_id = document["workID"].GetInt();
        row_count = document["raw_row_count"].GetInt();
        executionTime = document["executionTime"].GetDouble();

    }
};

// struct BlockResult{//csd 결과 데이터 파싱 구조체
//     int query_id;
//     int work_id;  
//     char data[BUFF_SIZE];
//     int length;
//     int raw_row_count;
//     vector<int> row_offset; 
//     int row_count;
//     string csd_name;
//     int result_block_count;
//     vector<int> return_datatype;//결과의 컬럼 데이터 타입(csd 결과 파싱용) -> CSD가 리턴
//     vector<int> return_offlen;//결과의 컬럼 길이 (csd 결과 파싱용) -> CSD가 리턴

//     BlockResult(){}
//     BlockResult(const char* json_, char* data_){
//         rapidjson::Document document;
//         document.Parse(json_); 

//         query_id = document["queryID"].GetInt();
//         work_id = document["workID"].GetInt();
//         row_count = document["rowCount"].GetInt();

//         rapidjson::Value &row_offset_ = document["rowOffset"];
//         int row_offset_size = row_offset_.Size();
//         for(int i = 0; i<row_offset_size; i++){
//             row_offset.push_back(row_offset_[i].GetInt());
//         }

//         rapidjson::Value &return_datatype_ = document["returnDatatype"];
//         rapidjson::Value &return_offlen_ = document["returnOfflen"];
//         int return_datatype_size = return_datatype_.Size();
//         for(int i = 0; i<return_datatype_size; i++){
//             return_datatype.push_back(return_datatype_[i].GetInt());
//             return_offlen.push_back(return_offlen_[i].GetInt());
//         }

//         raw_row_count = document["raw_row_count"].GetInt();

//         length = document["length"].GetInt();

//         memcpy(data, data_, length);

//         csd_name = document["csdName"].GetString();
//         result_block_count = document["resultBlockCount"].GetInt();
//     }
// };

class QEMUHandler {
    public:
        unordered_map<int,BlockResult> resultMap;

        static void InitQEMUHandler(){
            getInstance().initQEMUHandler();
        }
        static void InputQEMUHander(){
            getInstance().inputQEMUHandler();
        }
        static double CalculateQEMU(int queryid, int workID){
            getInstance().calculateQEMU(queryid, workID);
        }
    private:
        QEMUHandler(){}
        ~QEMUHandler(){}
        QEMUHandler& operator=(const QEMUHandler&){
            return *this;
        }
        static QEMUHandler& getInstance(){
            static QEMUHandler qemuHandler;
            return qemuHandler;
        }

        void initQEMUHandler();
        void inputQEMUHandler();
        double calculateQEMU(int queryID, int workID); // queryID = workID + queryID*100
        
        kQueue<BlockResult> BlockResultQueue;
        thread QEMUHandler_Input_Thread;
};