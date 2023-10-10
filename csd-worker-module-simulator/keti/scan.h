#pragma once
#include <sys/stat.h>
#include <algorithm>
#include <iomanip>
#include <cstdio>

#include "rocksdb/sst_file_reader.h"
#include "rocksdb/slice.h"
#include "rocksdb/iterator.h"
#include "rocksdb/cache.h"
#include "rocksdb/options.h"
#include "rocksdb/status.h"
#include "rocksdb/slice.h"

#include "filter.h"
#include "csd_table_manager.h"

using namespace ROCKSDB_NAMESPACE;

struct Snippet; 
struct PrimaryKey;

extern WorkQueue<Snippet> ScanQueue;
extern WorkQueue<Result> FilterQueue;
extern WorkQueue<MergeResult> ReturnQueue;

const int INDEX_NUM_SIZE = 4;

class Scan{
    public:
        Scan(TableManager table_m){
            CSDTableManager_ = table_m;
        }
        
        void Scanning();
        void BlockScan(SstFileReader* sstBlockReader_, Snippet *snippet_, Result *scan_result, string file_path);//file path 추가
        void IndexScan(SstBlockReader* sstBlockReader_, BlockInfo* blockInfo, 
                        Snippet *snippet_, Result *scan_result);
        void EnQueueData(Result scan_result, int scan_type);
        void Column_Filtering(Result *scan_result, Snippet snippet_);
        void GetColumnOff(char* data, Snippet snippet_, int colindex, int &varcharflag);
        void WalScan(Snippet *snippet_, Result *scan_result);

    private:
        TableManager CSDTableManager_;
        unordered_map<string,int> newoffmap;
        unordered_map<string,int> newlenmap;
        // int dev_fd;

};


struct PrimaryKey{
  string key_name;
  int key_type;
  int key_length;
};

struct Snippet{
    int work_id;//*워크ID
    int query_id;//*쿼리ID
    string csd_name;//*csd이름
    string table_name;//*스캔 테이블 이름
    vector<string> table_col;//*스캔 테이블 컬럼명
    vector<int> table_offset;//*스캔 테이블 컬럼오프셋
    vector<int> table_offlen;//*스캔 테이블 컬럼길이
    vector<int> table_datatype;//*스캔 테이블 컬럼타입
    list<BlockInfo> block_info_list;//*스캔 블록 리스트
    int total_block_count;//전체 블록 수
    unordered_map<string, int> colindexmap;//컬럼의 순서
    list<PrimaryKey> primary_key_list;//테이블 pk 정보 *primary_count 
    uint64_t kNumInternalBytes;//key에 붙는 디폴트값 길이
    int primary_length;//총 pk 길이
    string table_filter;//*where절 정보
    vector<Projection> column_projection;//*select문 정보
    vector<int> projection_datatype;//컬럼 프로젝션 후 컬럼의 데이터타입
    vector<int> projection_length;//컬럼 프로젝션 후 컬럼의 길이
    vector<string> groupby_col;
    int scan_type;//스캔 타입 결정
    vector<string> seek_pk_list;
    vector<string> deleted_key;//*delete key
    vector<string> inserted_key;//*insert key
    vector<string> inserted_value;//*insert value
    bool is_deleted;//delete 여부
    bool is_inserted;//insert 여부

    Snippet(const char* json_){
        bool index_scan = false;
        bool only_scan = true;

        total_block_count = 0;
        Document document;
        document.Parse(json_);
        
        query_id = document["queryID"].GetInt();
        work_id = document["workID"].GetInt();
        csd_name = document["csdName"].GetString();
        table_name = document["tableName"].GetString();
        
        int primary_count = document["primaryKey"].GetInt();
        if(primary_count == 0){
          kNumInternalBytes = 8;
        }else{
          kNumInternalBytes = 0;
        }

        // uint64_t block_offset_, block_length_;
        // int block_id_ = 0;//필요없을듯
        
        // //block list 정보 저장
        // block_info_list.clear();
        // Value &pba = document["pba"];
        // Value &block_list = pba["blockList"];
        // for(int i = 0; i<block_list.Size(); i++){
        //     block_offset_ = block_list[i]["offset"].GetInt64();
        //     for(int j = 0; j < block_list[i]["length"].Size(); j++){
        //         block_length_ = block_list[i]["length"][j].GetInt64();
        //         BlockInfo newBlock(block_id_, block_offset_, block_length_);
        //         block_info_list.push_back(newBlock);
        //         block_offset_ = block_offset_ + block_length_;
        //         block_id_++;
        //         total_block_count++;
        //         if(block_length_ > 4096){
        //           cout << "block#" << block_id_ <<  " length is over 4096 : " << block_length_ << endl;
        //         }
        //     }
        // }   

        //테이블 스키마 정보 저장
        table_col.clear();
        table_offset.clear();
        table_offlen.clear();
        table_datatype.clear();
        colindexmap.clear();
        primary_key_list.clear();
        Value &table_col_ = document["tableCol"];
        for(int j=0; j<table_col_.Size(); j++){
            string col = table_col_[j].GetString();
            int startoff = document["tableOffset"][j].GetInt();
            int offlen = document["tableOfflen"][j].GetInt();
            int datatype = document["tableDatatype"][j].GetInt();
            table_col.push_back(col);
            table_offset.push_back(startoff);
            table_offlen.push_back(offlen);
            table_datatype.push_back(datatype);
            colindexmap.insert({col,j});
            
            //pk 정보 저장
            primary_length = 0;
            if(j<primary_count){
              string key_name_ = document["tableCol"][j].GetString();
              int key_type_ = document["tableDatatype"][j].GetInt();
              int key_length_ = document["tableOfflen"][j].GetInt();
              primary_key_list.push_back(PrimaryKey{key_name_,key_type_,key_length_});
              primary_length += key_length_;
            }
        }

        //컬럼 프로젝션 정보 저장
        column_projection.clear();
        projection_datatype.clear();
        Value &column_projection_ = document["columnProjection"];
        for(int i = 0; i < column_projection_.Size(); i++){
          int ptype = column_projection_[i]["selectType"].GetInt();
          vector<string> vlist;vlist.clear();
          vector<int> vtypes;vtypes.clear();
          Value &v =  column_projection_[i]["value"];
          Value &vt =  column_projection_[i]["valueType"];
          for(int j=0; j < v.Size(); j++){
            vlist.push_back(v[j].GetString());
            vtypes.push_back(vt[j].GetInt());
          }
          column_projection.push_back(Projection{ptype,vlist,vtypes});
        }

        //filter 작업인 경우 확인
        Value &table_filter_ = document["tableFilter"];
        if(table_filter_.Size() != 0){
          Document small_document;
          small_document.SetObject();
          rapidjson::Document::AllocatorType& allocator = small_document.GetAllocator();
          small_document.AddMember("tableFilter",table_filter_,allocator);
          StringBuffer strbuf;
          rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
          small_document.Accept(writer);
          table_filter = strbuf.GetString();
          only_scan = false;
        }

        // Seek PK
        if(document.HasMember("seekPKList")){
          index_scan = true;
          seek_pk_list.clear();
          Value &seek_pk_list_ = document["seekPKList"];
        	for(int i = 0; i < seek_pk_list_.Size(); i++){
        		seek_pk_list.push_back(seek_pk_list_[i].GetString());
        	}
        }
        
        // deleted_key.clear();
        // Value &deleted_key_ = document["deletedKey"];
        // is_deleted = false;
        // for(int i = 0; i < deleted_key_.Size(); i++){
        //     is_deleted = true;
        //     deleted_key.push_back(deleted_key_[i].GetString());
        // }

        // inserted_key.clear();
        // inserted_value.clear();
        // is_inserted = false;
        // Value &inserted_key_ = document["unflushedRows"]["key"];
        // Value &inserted_value_ = document["unflushedRows"]["value"];
        // for(int i = 0; i < inserted_key_.Size(); i++){
        //     is_inserted = true;
        //     inserted_key.push_back(inserted_key_[i].GetString());
        //     inserted_value.push_back(inserted_value_[i].GetString());
        // }

        //스캔 타입 결정
        if(!index_scan && !only_scan){
          scan_type = Full_Scan_Filter;
        }else if(!index_scan && only_scan){
          scan_type = Full_Scan;
        }else if(index_scan && !only_scan){
          scan_type = Index_Scan_Filter;
        }else{
          scan_type = Index_Scan;
        }
       
    }
};