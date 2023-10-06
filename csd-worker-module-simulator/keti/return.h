#pragma once
#include <iostream>
#include <unordered_map>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <unistd.h>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"

#include "internal_queue.h"
#include "config.h"
#include "keti_type.h"
#include "keti_log.h"

using namespace rapidjson;
using namespace std;

struct MergeResult;

extern WorkQueue<MergeResult> ReturnQueue;

class Return{
    public:
        Return(){}
        void ReturnResult();
        void SendDataToBufferManager(MergeResult &mergeResult);

        inline const static std::string LOGTAG = "CSD Return";
        char msg[200];
};

struct MergeResult{
    int query_id;
    int work_id;
    string csd_name;
    vector<int> row_offset; 
    int row_count; 
    char data[BUFF_SIZE];
    int length;
    vector<int> projection_datatype;
    vector<int> projection_length;
    int total_block_count;
    int result_block_count;
    int current_block_count;

    MergeResult(){}
    //merge.cc의 최초 생성자
    MergeResult(int query_id_, int work_id_, string csd_name_, 
    vector<int> projection_datatype_, vector<int> projection_length_,
    int total_block_count_)
    : query_id(query_id_),
      work_id(work_id_),
      csd_name(csd_name_),
      projection_datatype(projection_datatype_),
      projection_length(projection_length_),
      total_block_count(total_block_count_){
        row_offset.clear();
        row_count = 0;
        length = 0;
        memset(&data, 0, sizeof(BUFF_SIZE));
        result_block_count = 0;
        current_block_count = 0;
    }

    void InitMergeResult(){
        row_offset.clear();
        memset(&data, 0, sizeof(BUFF_SIZE));
        row_count = 0;
        length = 0;
        result_block_count = 0;
    }
};  
