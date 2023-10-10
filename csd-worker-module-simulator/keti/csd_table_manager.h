#include <iostream>
#include <unordered_map>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <vector>

using namespace std;

#include "rocksdb/sst_file_reader.h"

typedef std::pair<string, string> string_pair_key;
struct pair_hash2{
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2> &pair) const{
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};

class TableManager{
public:
    TableManager() {}
    void InitCSDTableManager();//임시로 데이터 넣어놓음
    string GetTableRep(string csd_name, string table_name);

private:
    unordered_map<string_pair_key,string, pair_hash2> table_rep;
    //<csd_name, <table_name, file_path>> 
    //              key          value  
    //    key              value
};