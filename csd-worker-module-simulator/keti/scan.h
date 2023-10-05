#pragma once
#include <sys/stat.h>
#include <algorithm>
#include <bitset>
#include <iomanip>

#include "rocksdb/sst_file_reader.h"
#include "rocksdb/slice.h"
#include "rocksdb/iterator.h"
#include "rocksdb/cache.h"
#include "rocksdb/options.h"
#include "rocksdb/status.h"
#include "rocksdb/slice.h"

#include "filter.h"
#include "csd_table_manager.h"




extern WorkQueue<Snippet> ScanQueue;
extern WorkQueue<Result> FilterQueue;
extern WorkQueue<MergeResult> ReturnQueue;

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

