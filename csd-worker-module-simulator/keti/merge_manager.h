#pragma once
#include <stack>
#include <bitset>
#include <math.h>

#include "return.h"

struct Result;
struct FilterInfo;

extern WorkQueue<Result> MergeQueue;
extern WorkQueue<MergeResult> ReturnQueue;

struct T{
  string varString;
  int varInt;
  int64_t varLong;
  float varFloat;
  double varDouble;
  int real_size = 0;//소수점 길이//여기추가
};

typedef std::pair<int, int> pair_key;
struct pair_hash{
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2> &pair) const{
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};

class MergeManager{
public:
    void Merging();
    void MergeBlock(Result &result);

    inline const static std::string LOGTAG = "CSD Merge Manager";
    char msg[200];

private:
    unordered_map<pair_key, MergeResult, pair_hash> m_MergeManager;// key=<qid,wid>

    pair<T,int> stack_charToValue(char* dest, int type, int len);
    int stack_valueToChar(char* dest, int dest_type, T value, int type);
    void getColOffset(const char *origin_row_data, FilterInfo filter_info, int* col_offset);
    int calculCase(FilterInfo filter_info, char* origin_row_data, int* col_offset, int l, char* dest);
    int calculSubstring(FilterInfo filter_info, char* origin_row_data, int* col_offset, int l, char* dest);
    int calculExtract(FilterInfo filter_info, char* origin_row_data, int* col_offset, int l, char* dest);
    int calculPostfix(vector<string> values, vector<int> types, FilterInfo filter_info, char* origin_row_data, int* col_offset, char* dest, int projection_datatype);
};

struct Projection{
    int projection; // KETI_SELECT_TYPE
    vector<string> values;
    vector<int> types; // KETI_ValueType
};

struct FilterInfo{
    vector<string> table_col;//스캔테이블
    vector<int> table_offset;
    vector<int> table_offlen;
    vector<int> table_datatype;
    unordered_map<string, int> colindexmap;//col index
    string table_filter;//where절 정보
    vector<Projection> column_projection;//select절 정보
    vector<int> projection_datatype;//*컬럼 프로젝션 후 컬럼의 데이터타입
    vector<int> projection_length;//*컬럼 프로젝션 후 컬럼의 길이
    
    FilterInfo(){}
    FilterInfo(
      vector<string> table_col_, vector<int> table_offset_, 
      vector<int> table_offlen_, vector<int> table_datatype_,
      unordered_map<string, int> colindexmap_, string table_filter_,
      vector<Projection> column_projection_, vector<int> projection_datatype_,
      vector<int> projection_length_)
      : table_col(table_col_),
        table_offset(table_offset_),
        table_offlen(table_offlen_),
        table_datatype(table_datatype_),
        colindexmap(colindexmap_),
        table_filter(table_filter_),
        column_projection(column_projection_),
        projection_datatype(projection_datatype_),
        projection_length(projection_length_){}
};

struct Result{
    int query_id;
    int work_id;
    string csd_name;
    int total_block_count;
    FilterInfo filter_info;
    int row_count;
    int length;
    char data[BUFF_SIZE];
    vector<int> row_offset;
    vector<vector<int>> row_column_offset;
    int result_block_count;

    //scan, filter의 최초 생성자
    Result(
      int query_id_, int work_id_, string csd_name_,
      int total_block_count_, FilterInfo filter_info_,
      int result_block_count_ = 0)
      : query_id(query_id_),
        work_id(work_id_), 
        csd_name(csd_name_),
        total_block_count(total_block_count_),
        filter_info(filter_info_),
        result_block_count(result_block_count_){
          row_count = 0;
          length = 0;
          memset(&data, 0, sizeof(BUFF_SIZE));
          row_offset.clear();
          row_column_offset.clear();
        } 
 
  void InitResult(){
    row_count = 0;
    length = 0;
    memset(&data, 0, sizeof(BUFF_SIZE));
    row_offset.clear();
    row_column_offset.clear();
    result_block_count = 0;
  }
};

inline std::string& rtrim_(std::string& s, const char* t = " \t\n\r\f\v\0"){
	s.erase(s.find_last_not_of(t) + 1);
	return s;
}

inline std::string& ltrim_(std::string& s, const char* t = " \t\n\r\f\v\0"){
	s.erase(0, s.find_first_not_of(t));
	return s;
}

inline std::string& trim_(std::string& s, const char* t = " \t\n\r\f\v\0"){
	return ltrim_(rtrim_(s, t), t);
}