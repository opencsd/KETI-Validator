#pragma once

#include <vector>
#include <unordered_map>
#include <mutex>
#include <fcntl.h>
#include <unistd.h>
#include <iomanip>
#include <algorithm>
#include <condition_variable>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h" 

#include "storage_engine_instance.grpc.pb.h"

#include "keti_log.h"
#include "ip_config.h"
#include "internal_queue.h"
#include "TableManager.h"

using namespace std;
using StorageEngineInstance::Request;

class IndexManager { 

public:

	static string makeKey(int qid, int wid){
		string key = to_string(qid)+"|"+to_string(wid);
        return key;
    }

	static void InitIndexManager(){
		GetInstance().initIndexManager();
	}

    static void PushQueue(Request request){
        GetInstance().pushQueue(request);
    }

private:
	IndexManager() {};
    IndexManager(const IndexManager&);
    IndexManager& operator=(const IndexManager&){
        return *this;
    };

    static IndexManager& GetInstance() {
        static IndexManager _instance;
        return _instance;
    }

    static Request pushQueue(Request request){
        GetInstance().indexManagerQueue.push_work(request);
    }

    static Request popQueue(){
        Request request = GetInstance().indexManagerQueue.wait_and_pop();
        return request;
    }

	void initIndexManager();
    void requestIndexScan();

public:
    inline const static string LOGTAG = "Monitoring Container::Index Manager";

private:
    mutex Index_Mutex;
	kQueue<Request> indexManagerQueue;
    thread IndexManager_Thread;
};

#pragma once

#include <vector>
#include <unordered_map>
#include <mutex>
#include <fcntl.h>
#include <unistd.h>
#include <iomanip>
#include <algorithm>
#include <condition_variable>
#include <iostream>
#include <fstream>
#include <string>
#include <map>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include "rocksdb/sst_file_reader.h"
#include "rocksdb/sst_file_manager.h"
#include "rocksdb/slice.h"
#include "rocksdb/iterator.h"
#include "rocksdb/table_properties.h"
#include "rocksdb/db.h"

using ROCKSDB_NAMESPACE::SstFileReader;
using ROCKSDB_NAMESPACE::Slice;
using ROCKSDB_NAMESPACE::Status;
using ROCKSDB_NAMESPACE::Iterator;
using ROCKSDB_NAMESPACE::ReadOptions;
using ROCKSDB_NAMESPACE::Options;
using ROCKSDB_NAMESPACE::TableProperties;

#include "storage_engine_instance.grpc.pb.h"
#include "keti_log.h"
#include "ip_config.h"

using namespace std;
using StorageEngineInstance::Snippet;
using StorageEngineInstance::MetaDataResponse;

struct InternalRequest {
	int query_id;
	int work_id;
	string table_name;
};

struct Response {
	MetaDataResponse* metadataResponse;
	Snippet snippet;
	int block_count;
	bool is_done;
	condition_variable cond;
	mutex mu;
	bool lba2pba_done = false;
	bool index_scan_done = false;
	bool wal_done = false;
	Response(){
		this->metadataResponse = new StorageEngineInstance::MetaDataResponse;
	}
};

#include "IndexManager.h"

using namespace rapidjson;
using StorageEngineInstance::MetaDataResponse_PBAInfo;

void IndexManager::initIndexManager(){
	TableManager::Table table = TableManager::GetMutableTable("lineitem");
	for(int i=0; i<table.SSTList.size(); i++){
		TableManager::SSTFile sst = table.SSTList[i];
		
		char* ikey0 = &data0[0];
		string index0(ikey0 + 4, ikey0 + 8);
		string pk0(ikey0 + 4, ikey0 + 12);
		sst.IndexTable[index0].push_back(pk0);

		char* ikey1 = &data1[0];
		string index1(ikey1 + 4, ikey1 + 8);
		string pk1(ikey1 + 4, ikey1 + 12);
		sst.IndexTable[index1].push_back(pk1);

		char* ikey2 = &data2[0];
		string index2(ikey2 + 4, ikey2 + 8);
		string pk2(ikey2 + 4, ikey2 + 12);
		sst.IndexTable[index2].push_back(pk2);

		char* ikey3 = &data3[0];
		string index3(ikey3 + 4, ikey3 + 8);
		string pk3(ikey3 + 4, ikey3 + 12);
		sst.IndexTable[index3].push_back(pk3);

		char* ikey4 = &data4[0];
		string index4(ikey4 + 4, ikey4 + 8);
		string pk4(ikey4 + 4, ikey4 + 12);
		sst.IndexTable[index4].push_back(pk4);

		char* ikey5 = &data5[0];
		string index5(ikey5 + 4, ikey5 + 8);
		string pk5(ikey5 + 4, ikey5 + 12);
		sst.IndexTable[index5].push_back(pk5);
		
	}
    
	std::thread IndexManager_Thread(&IndexManager::requestIndexScan,this);
    IndexManager_Thread.detach();
}

void IndexManager::requestIndexScan(){
    while(1){
        Request request = popQueue();

        int qid = request.query_id();
        int wid = request.work_id();
        string tname = request.table_name();

		string key = TableManager::makeKey(qid,wid);
        Response* data = TableManager::GetReturnData(key);
        unique_lock<mutex> lock(data->mu);
		TableManager::Table table = TableManager::GetMutableTable(tname);

		for(int i=0; i<table.SSTList.size(); i++){
			MetaDataResponse_PBAInfo pba_info;

			int find_index = 10784;
			int table_index_num = 380; 

			char temp[4];
			memcpy(temp, &find_index, sizeof(int));

			char find_index_char[4];
			for(int i=0; i<4; i++){
				find_index_char[i] = temp[3-i];
			}
			find_index_char[0] ^= 0x80;

			string find_index_string(find_index_char,find_index_char + 4);

			vector<string> seek_pk_list;
			
			memcpy(temp, &table_index_num, sizeof(int));
			char table_index_num_char[4];
			for(int i=0; i<4; i++){
				table_index_num_char[i] = temp[3-i];
			}

			int PK_SIZE = 8;
			for(int i=0; i<table.SSTList[i].IndexTable[find_index_string].size(); i++){
				char seek_pk_char[4 + PK_SIZE];
				memcpy(seek_pk_char, table_index_num_char, 4);
				memcpy(seek_pk_char + 4, table.SSTList[i].IndexTable[find_index_string][i].c_str(), PK_SIZE);

				cout << "seek_pk_char: ";
				for(int i=0; i<12; i++){
					printf("%02X ",(u_char)seek_pk_char[i]);
				}
				cout << endl;

				string seek_pk_string(seek_pk_char, seek_pk_char + 4 + PK_SIZE);
				seek_pk_list.push_back(seek_pk_string);
			}

			data->metadataResponse->mutable_sst_csd_map()->insert({file_name,pba_info});
		}

	}
}

class TableManager { 

public:
	struct ColumnSchema {
		string column_name;
		int type;
		int length;
		int offset;
	};
	
	struct DataBlockHandle {
		string IndexBlockHandle;
		off64_t Offset;
		off64_t Length;
	};
	
	struct SSTFile{
		string filename;
		vector<struct DataBlockHandle> BlockList;
		map<string,vector<string>> IndexTable;
	};
	
	struct Table {
		string tablename;
		int tablesize;
		vector<struct ColumnSchema> Schema;
		vector<struct SSTFile> SSTList;
		vector<string> IndexList;
		vector<string> PK;

	
		vector<string> IndexColumnNames; 
		vector<int> IndexColumnBytes;
		int IndexCnt;
		
		vector<string> PkColumnNames; 
		vector<int> PkColumnBytes; 
		int PkCnt;

		bool PkExist;
		string IndexNum;
		bool isPK;
		bool isIndex;
	};

	static int GetTableSchema(string tablename,vector<struct ColumnSchema> &dst){
		return GetInstance().getTableSchema(tablename,dst);
	}

	static vector<string> GetOrderedTableBySize(vector<string> tablenames){
		return GetInstance().getOrderedTableBySize(tablenames);
	}

	static int GetIndexList(string tablename, vector<string> &dst){
		return GetInstance().getIndexList(tablename,dst);
	}

	static vector<string> GetSSTList(string tablename){
		return GetInstance().getSSTList(tablename);
	}

	static void PrintTableManager(){
		GetInstance().printTableManager();
	}

	static Table GetTable(string tablename){
		return GetInstance().m_TableManager[tablename];
	}

	static Table GetMutableTable(string tablename){
		Table& table_ref = GetInstance().m_TableManager[tablename];
		return table_ref;
	}

	static Table SetResponseSnippet(Snippet snippet, string key){
		GetInstance().return_data[key]->snippet->CopyFrom(snippet);
	}

	static string makeKey(int qid, int wid){
		string key = to_string(qid)+"|"+to_string(wid);
        return key;
    }

	static void SetReturnData(string key){
		Response* response = new Response;
		GetInstance().return_data[key] = response;
	}

	static Response* GetReturnData(string key){
		return GetInstance().return_data[key];
	}

	static void InitTableManager(){
		GetInstance().initTableManager();
	}

private:
	TableManager() {};
    TableManager(const TableManager&);
    TableManager& operator=(const TableManager&){
        return *this;
    };

    static TableManager& GetInstance() {
        static TableManager _instance;
        return _instance;
    }

	int initTableManager();
	int getTableSchema(string tablename,vector<struct ColumnSchema> &dst);
	vector<string> getOrderedTableBySize(vector<string> tablenames);
	int getIndexList(string tablename, vector<string> &dst);
	vector<string> getSSTList(string tablename);
	void printTableManager();
	
	bool areVectorsEqual(const vector<string>& vec1, const vector<string>& vec2);
	void getIndexScanMap(struct Table table); 

public:
    inline const static string LOGTAG = "Monitoring Container::Table Manager";

private:
    mutex Table_Mutex;
	unordered_map<string,struct Table> m_TableManager;
	unordered_map<string,Response*> return_data;
	unordered_map<string,struct Table> M_TableManager;
};

#include "TableManager.h"

using namespace rapidjson;

int TableManager::initTableManager(){
	KETILOG::DEBUGLOG(LOGTAG, "# Init TableManager");

	string json = "";
	std::ifstream openFile("");
	if(openFile.is_open() ){
		std::string line;
		while(getline(openFile, line)){
			json += line;
		}
		openFile.close();
	}
	
	Document document;
	document.Parse(json.c_str());

	Value &TableList = document["Table List"];
	
	for(int i=0;i<TableList.Size();i++){
		Value &TableObject = TableList[i];
		auto tbl = new Table();
		if (TableObject.HasMember("TableName")){
			Value &tableNameObject = TableObject["TableName"];
			Value &tableSizeObject = TableObject["TableSize"];
			std::cout << "tableNameObject : "<< tableNameObject.GetString() << std::endl;
			std::cout << "tableSizeObject : "<< tableSizeObject.GetInt() << std::endl;
			tbl->tablename = tableNameObject.GetString(); 
			tbl->tablesize = tableSizeObject.GetInt();
			std::cout << "tbl->tablename : "<< tbl->tablename << std::endl;
			std::cout << "tbl->tablesize : "<< tbl->tablesize << std::endl;
		}

		if (TableObject.HasMember("IndexColumnNames")){
			const Value& arrName = TableObject["IndexColumnNames"].GetArray();
			const Value& arrBytes = TableObject["IndexColumnBytes"].GetArray();
			int arrSize = static_cast<int>(arrName.Size());
			tbl->IndexCnt = arrSize;
			for (SizeType i = 0; i < arrName.Size(); ++i) { 
				tbl->IndexColumnNames.push_back(arrName[i].GetString());
				tbl->IndexColumnBytes.push_back(arrBytes[i].GetInt());
			}
		}
		if (TableObject.HasMember("PkColumnNames")){
			const Value& arrName= TableObject["PkColumnNames"].GetArray();
			const Value& arrBytes= TableObject["PkColumnBytes"].GetArray();
			int arrSize = static_cast<int>(arrName.Size());
			tbl->PkCnt = arrSize;
			for (SizeType i = 0; i < arrName.Size(); ++i) {
				tbl->PkColumnNames.push_back(arrName[i].GetString());
				tbl->PkColumnBytes.push_back(arrBytes[i].GetInt());
			}
		}
		
		Value &SchemaObject = TableObject["Schema"];
		for(int j=0;j<SchemaObject.Size();j++){
			Value &ColumnObject = SchemaObject[j];
			auto Column = new ColumnSchema;
			
			Column->column_name = ColumnObject["column_name"].GetString();
			Column->type = ColumnObject["type"].GetInt();
			Column->length = ColumnObject["length"].GetInt();
			Column->offset = ColumnObject["offset"].GetInt();
			tbl->Schema.push_back(*Column);
		}

		Value &SSTList = TableObject["SST List"];
		for(int j=0;j<SSTList.Size();j++){
			Value &SSTObject = SSTList[j];
			auto SstFile = new SSTFile;
			
			SstFile->filename = SSTObject["filename"].GetString();

			Value &BlockList = SSTObject["Block List"];
			for(int k=0;k<BlockList.Size();k++){
				Value &BlockHandleObject = BlockList[k];
				struct DataBlockHandle DataBlockHandle;
				DataBlockHandle.Offset = BlockHandleObject["Offset"].GetInt();
				DataBlockHandle.Length = BlockHandleObject["Length"].GetInt();
				DataBlockHandle.IndexBlockHandle = BlockHandleObject["IndexBlockHandle"].GetString();
				SstFile->BlockList.push_back(DataBlockHandle);
			}
			tbl->SSTList.push_back(*SstFile);
		 	m_TableManager.insert({tbl->tablename,*tbl});
		}
	}

	for (const auto& entry : m_TableManager){
		const std::string& tableName = entry.first; 
        const Table& table = entry.second;

		vector<string> idx_IndexColumnNames = table.IndexColumnNames; 
		vector<int> idx_IndexColumnBytes = table.IndexColumnBytes;
		int idx_IndexCnt = table.IndexCnt;
		vector<string> idx_PkColumnNames = table.PkColumnNames; 
		vector<int> idx_PkColumnBytes = table.PkColumnBytes;
		bool idx_PkExist = table.PkExist;  
		int idx_PkCnt = table.PkCnt;
		vector<struct SSTFile> idx_SSTList = table.SSTList;
		std::string idx_IndexNum = table.IndexNum;

		std::cout << "Table Name : " << tableName << std::endl;
		int cnt = 0;

		Options options; 
		
		for (const SSTFile& sst : idx_SSTList) {
			SSTFile idx_SstFile = sst;
			std::string idx_sstFileName = idx_SstFile.filename;
			vector<struct DataBlockHandle> idx_sstFileBlockList = idx_SstFile.BlockList;
			
			
			std::string file_path = "/root/workspace/HUN/tpch_small_index_sst/" + tableName + "/" + idx_sstFileName; 
			
			Options options; 
			ReadOptions readOptions;
			SstFileReader sstFileReader(options);
			Status s = sstFileReader.Open(file_path);
			if (!s.ok()) {
				std::cout << "Error opening SST file: " << s.ToString() << std::endl;
			}
			Iterator* it = sstFileReader.NewIterator(readOptions);

			
			map<string, vector<string>> indexScanMap; 
			string indexList = " "; 
			vector<std::string> noPkIndexList; 

			for (it->SeekToFirst(); it->Valid(); it->Next()){
			
				const Slice& key = it->key();
				const Slice& value = it->value();
				int BlockOffset = 8;	
				std::string keyData = key.ToString();
				std::string tableIndexNum = keyData.substr(0,8); 
				if (value.empty() && (tableIndexNum == idx_IndexNum)){
					for (int i=0; i<idx_IndexCnt; i++){
						
						int indexSize = idx_IndexColumnBytes.at(i)*2; 
						if (i != 0){
							indexList += "|" + keyData.substr(BlockOffset, indexSize); 
						}
						noPkIndexList.push_back(keyData.substr(BlockOffset, indexSize));
						BlockOffset += indexSize; 
					}
					if (idx_PkExist==true){
						std::cout << "Pk Exist Test" << std::endl;
						if (areVectorsEqual(idx_IndexColumnNames,idx_PkColumnNames)){ 
							
							int newBlockOffset = 8;
							for (int i=0; i<idx_PkCnt; i++){
								int pkSize = idx_PkColumnBytes.at(i)*2;
								indexScanMap[indexList].push_back(keyData.substr(newBlockOffset, pkSize));
								newBlockOffset += pkSize;
							}
						}
						else {
							
							for (int i=0; i<idx_PkCnt; i++){
								int pkSize = idx_PkColumnBytes.at(i)*2;
								indexScanMap[indexList].push_back(keyData.substr(BlockOffset, pkSize));
								BlockOffset += pkSize;
							}
						}
					}
					else {
						
						indexScanMap[indexList].push_back(keyData.substr(BlockOffset));
					}
				}
				for (const auto& entry : indexScanMap) {
					const std::string& key = entry.first;
					const std::vector<std::string>& values = entry.second;
					std::cout << "TEST" << std::endl;
					std::cout << "Key: " << key << ", Values: ";
					for (const auto& value : values) {
						std::cout << value << " ";
					}
					std::cout << std::endl;
				}
			}
			idx_SstFile.IndexTable = indexScanMap;
			for (const auto& entry : indexScanMap) {
				const std::string& key = entry.first;
				const std::vector<std::string>& values = entry.second;
				std::cout << "TEST" << std::endl;
				std::cout << "Key: " << key << ", Values: ";
				for (const auto& value : values) {
					std::cout << value << " ";
				}
				std::cout << std::endl;
			}
			cnt++;
			std::cout << "Cnt : " << cnt << std::endl;
		}
		M_TableManager.insert({tableName, table});
	}
	return 0;
}

vector<string> TableManager::getSSTList(string tablename){
	vector<string> sstlist;
	for(int i = 0; i < m_TableManager[tablename].SSTList.size(); i++){
		sstlist.push_back(m_TableManager[tablename].SSTList[i].filename);
	}
	return sstlist;
}

int TableManager::getTableSchema(std::string tablename,vector<struct ColumnSchema> &dst){	
    if (m_TableManager.find(tablename) == m_TableManager.end()){
		KETILOG::FATALLOG(LOGTAG,"Table [" + tablename + "] Not Present");
        return -1;
	}

	struct Table tbl = m_TableManager[tablename];
	dst = tbl.Schema;
	return 0;
}

vector<string> TableManager::getOrderedTableBySize(vector<string> tablenames){
	vector<pair<string,int>> tablesize;
	for(int i = 0; i < tablenames.size(); i++){
		struct Table tbl = m_TableManager[tablenames[i]];
		tablesize.push_back(make_pair(tbl.tablename,tbl.tablesize));
	}
	sort(tablesize.begin(),tablesize.end(),[](pair<string,int> a, pair<string,int> b){
		return a.second < b.second;
	});
	vector<string> ret;
	for(int i = 0; i < tablesize.size(); i++){
		ret.push_back(tablesize[i].first);
	}
	return ret;
}

int TableManager::getIndexList(string tablename, vector<string> &dst){
	if (m_TableManager.find(tablename) == m_TableManager.end()){
		KETILOG::FATALLOG(LOGTAG,"Table [" + tablename + "] Not Present");
        return -1;
	}
	struct Table tbl = m_TableManager[tablename];
	dst = tbl.IndexList;
	return 0;
}

void TableManager::printTableManager(){
	unordered_map <string,struct Table> :: iterator it;

	for(it = m_TableManager.begin(); it != m_TableManager.end() ; it++){		
		struct Table tmp = it->second;
		std::cout << "tablename : " << tmp.tablename << std::endl;
		std::cout << "Column name     Type  Length Offset" << std::endl;
		vector<struct ColumnSchema>::iterator itor = tmp.Schema.begin();

		for (; itor != tmp.Schema.end(); itor++) {
			std::cout << left << setw(15) << (*itor).column_name << " " << left<< setw(5) << (*itor).type << " " << left << setw(6) <<(*itor).length << " " << left << setw(5) << (*itor).offset << std::endl;
		}

		vector<struct SSTFile>::iterator itor2 = tmp.SSTList.begin();

		for (; itor2 != tmp.SSTList.end(); itor2++) {
			std::cout << "SST filename : " << (*itor2).filename << std::endl;

			vector<struct DataBlockHandle>::iterator itor3 = (*itor2).BlockList.begin();
			std::cout << " StartOffset-Length" << endl;
			for (; itor3 != (*itor2).BlockList.end(); itor3++) {
				std::cout << "Block Handle : " << (*itor3).Offset << "-" << (*itor3).Length << std::endl;				
			}
		}
	}
}

bool TableManager::areVectorsEqual(const vector<string>& vec1, const vector<string>& vec2) {
    if (vec1.size() != vec2.size()) {
        return false;
    }
    for (size_t i = 0; i < vec1.size(); ++i) {
        if (vec1[i] != vec2[i]) {
            return false;
        }
    }
    return true;
}

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
        Scan(){}
        
        void Scanning();
        void BlockScan(SstBlockReader* sstBlockReader_, BlockInfo* blockInfo, 
                        Snippet *snippet_, Result *scan_result);
        void IndexScan(SstBlockReader* sstBlockReader_, BlockInfo* blockInfo, 
                        Snippet *snippet_, Result *scan_result);
        void EnQueueData(Result scan_result, Snippet snippet_);
        void Column_Filtering(Result *scan_result, Snippet snippet_);
        void GetColumnOff(char* data, Snippet snippet_, int colindex, int &varcharflag);
        void WalScan(Snippet *snippet_, Result *scan_result);

        inline const static std::string LOGTAG = "CSD Scan";
        char msg[200];

    private:
        int getPrimaryKeyData(const char* ikey_data, char* dest, list<PrimaryKey> pk_list);
        char* hexstr_to_char(const char* hexstr, int* row_size);
        string char_to_hexstr(const char *data, int len);

        uint64_t kNumInternalBytes_;
        bool index_valid;
        int ipk;
        bool check;
        char origin_index_num[INDEX_NUM_SIZE];
        int total_block_row_count;
        int current_block_count;
};

struct PrimaryKey{
  string key_name;
  int key_type;
  int key_length;
};

struct Snippet{
    int work_id;
    int query_id;
    string csd_name;
    string table_name;
    vector<string> table_col;
    vector<int> table_offset;
    vector<int> table_offlen;
    vector<int> table_datatype;
    list<BlockInfo> block_info_list;
    int total_block_count;
    unordered_map<string, int> colindexmap;
    list<PrimaryKey> primary_key_list;
    uint64_t kNumInternalBytes;
    int primary_length;
    string table_filter;
    vector<Projection> column_projection;
    vector<int> projection_datatype;
    vector<int> projection_length;
    int scan_type;
    vector<string> seek_pk_list;
    vector<string> deleted_key;
    vector<string> inserted_key;
    vector<string> inserted_value;
    bool is_deleted;
    bool is_inserted;

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

        uint64_t block_offset_, block_length_;
        int block_id_ = 0;

        block_info_list.clear();
        Value &pba = document["pba"];
        Value &block_list = pba["blockList"];
        for(int i = 0; i<block_list.Size(); i++){
            block_offset_ = block_list[i]["offset"].GetInt64();
            for(int j = 0; j < block_list[i]["length"].Size(); j++){
                block_length_ = block_list[i]["length"][j].GetInt64();
                BlockInfo newBlock(block_id_, block_offset_, block_length_);
                block_info_list.push_back(newBlock);
                block_offset_ = block_offset_ + block_length_;
                block_id_++;
                total_block_count++;
                if(block_length_ > 4096){
                  cout << "block#" << block_id_ <<  " length is over 4096 : " << block_length_ << endl;
                }
            }
        }   

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
            
            primary_length = 0;
            if(j<primary_count){
              string key_name_ = document["tableCol"][j].GetString();
              int key_type_ = document["tableDatatype"][j].GetInt();
              int key_length_ = document["tableOfflen"][j].GetInt();
              primary_key_list.push_back(PrimaryKey{key_name_,key_type_,key_length_});
              primary_length += key_length_;
            }
        }

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

        if(document.HasMember("seekPKList")){
          index_scan = true;
          seek_pk_list.clear();
          Value &seek_pk_list_ = document["seekPKList"];
        	for(int i = 0; i < seek_pk_list_.Size(); i++){
        		seek_pk_list.push_back(seek_pk_list_[i].GetString());
        	}
        }
        
        deleted_key.clear();
        Value &deleted_key_ = document["deletedKey"];
        is_deleted = false;
        for(int i = 0; i < deleted_key_.Size(); i++){
            is_deleted = true;
            deleted_key.push_back(deleted_key_[i].GetString());
        }

        inserted_key.clear();
        inserted_value.clear();
        is_inserted = false;
        Value &inserted_key_ = document["unflushedRows"]["key"];
        Value &inserted_value_ = document["unflushedRows"]["value"];
        for(int i = 0; i < inserted_key_.Size(); i++){
            is_inserted = true;
            inserted_key.push_back(inserted_key_[i].GetString());
            inserted_value.push_back(inserted_value_[i].GetString());
        }

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

#include "scan.h"

using namespace ROCKSDB_NAMESPACE;

int temp = 0;
int current_row_count = 0; 

char sep = 0x03;
char gap = 0x20;
char fin = 0x02;

inline Slice ExtractUserKey(const Slice& internal_key, uint64_t kNumInternalBytes) {
  assert(internal_key.size() >= kNumInternalBytes);
  return Slice(internal_key.data(), internal_key.size() - kNumInternalBytes);
}

class InternalKey {
 private:
  string rep_;
  uint64_t kNumInternalBytes_;

 public:
  InternalKey() {}  
  void DecodeFrom(const Slice& s, uint64_t k) { 
    rep_.assign(s.data(), s.size()); 
    kNumInternalBytes_ = k;
  }
  Slice user_key() const { return ExtractUserKey(rep_, kNumInternalBytes_); }
};

void Scan::Scanning(){
    while (1){
        Snippet snippet = ScanQueue.wait_and_pop();

        TableRep table_rep = CSDTableManager::GetTableRep(snippet.table_name);
        kNumInternalBytes_ = snippet.kNumInternalBytes;
		                
        Options options;
        SstBlockReader sstBlockReader(
            options, table_rep.blocks_maybe_compressed, table_rep.blocks_definitely_zstd_compressed, 
            table_rep.immortal_table, table_rep.read_amp_bytes_per_bit, table_rep.dev_name);

        FilterInfo filterInfo(
          snippet.table_col, snippet.table_offset, snippet.table_offlen,
          snippet.table_datatype, snippet.colindexmap, snippet.table_filter,
          snippet.column_projection, snippet.projection_datatype, snippet.projection_length);

        Result scanResult(snippet.query_id, snippet.work_id, snippet.csd_name, snippet.total_block_count, filterInfo);
        
        current_block_count = 0;
        index_valid = true;
        ipk = 0;
        check = true;
        total_block_row_count = 0;
        temp = 0; 

        if(snippet.is_inserted){
          WalScan(&snippet, &scanResult);
          EnQueueData(scanResult, snippet);
          scanResult.InitResult();
        }

        list<BlockInfo>::iterator iter;
        for(iter = snippet.block_info_list.begin(); iter != snippet.block_info_list.end(); iter++){
            current_block_count++;
            current_row_count = 0;
            
            scanResult.result_block_count++;

            BlockInfo blockInfo = *iter;
            BlockScan(&sstBlockReader, &blockInfo, &snippet, &scanResult);

            if(!index_valid){
              scanResult.result_block_count += snippet.total_block_count - current_block_count;
              current_block_count = snippet.total_block_count;
            }

            if(current_block_count == snippet.total_block_count){
              float temp_size = float(scanResult.length) / float(1024);
              memset(msg, '\0', sizeof(msg));
              sprintf(msg,"Scanning Data ... (Block : %d/%d, Scanned Size : %.1fK, Total Rows: %d, BLock Rows: %d)\n",current_block_count,snippet.total_block_count,temp_size, total_block_row_count, temp);
              sprintf(msg,"Scanning Data ... (Block : %d/%d, Scanned Size : %.1fK, Total Rows: %d)\n",current_block_count,snippet.total_block_count,temp_size, total_block_row_count);
              KETILOG::INFOLOG(LOGTAG, msg);
              EnQueueData(scanResult, snippet);
              scanResult.InitResult();
              break;
            }else if(current_block_count % NUM_OF_BLOCKS == 0){
              float temp_size = float(scanResult.length) / float(1024);
              memset(msg, '\0', sizeof(msg));
              sprintf(msg,"Scanning Data ... (Block : %d/%d, Scanned Size : %.1fK, Blcok Rows: %d)\n",current_block_count,snippet.total_block_count,temp_size, temp);
              sprintf(msg,"Scanning Data ... (Block : %d/%d, Scanned Size : %.1fK)\n",current_block_count,snippet.total_block_count,temp_size);
              KETILOG::DEBUGLOG(LOGTAG, msg);
              
              temp = 0;
              EnQueueData(scanResult, snippet);
              scanResult.InitResult();
            }

            memset(msg, '\0', sizeof(msg));
            sprintf(msg,"current block count : %d | current row count : %d\n", current_block_count, current_row_count);
            KETILOG::TRACELOG(LOGTAG, msg);
        }
    }
}

void Scan::WalScan(Snippet *snippet_, Result *scan_result){
  for(int i=0; i<snippet_->inserted_key.size(); i++){
      scan_result->row_offset.push_back(scan_result->length);
      int row_size = 0;
      char* inserted_data;
      inserted_data = hexstr_to_char(snippet_->inserted_value[i].c_str(),&row_size);
      memcpy(scan_result->data+scan_result->length,inserted_data,row_size);
      scan_result->length += row_size;
      scan_result->row_count++;
  }
}

void Scan::BlockScan(SstBlockReader* sstBlockReader_, BlockInfo* blockInfo, Snippet *snippet_, Result *scan_result){
  Status s  = sstBlockReader_->Open(blockInfo);
  if(!s.ok()){
      KETILOG::ERRORLOG(LOGTAG, "Block Open Error \n");
  }

  const char* ikey_data;
  const char* row_data;
  size_t row_size;

  Iterator* datablock_iter = sstBlockReader_->NewIterator(ReadOptions());

  if(snippet_->scan_type == Full_Scan_Filter || snippet_->scan_type == Full_Scan){

    for (datablock_iter->SeekToFirst(); datablock_iter->Valid(); datablock_iter->Next()) {
      
      Status s = datablock_iter->status();
      if (!s.ok()) {
        KETILOG::ERRORLOG(LOGTAG, "Error reading the block - Skipped \n");
        break;
      }               

      const Slice& key = datablock_iter->key();
      const Slice& value = datablock_iter->value();

      InternalKey ikey;
      ikey.DecodeFrom(key, snippet_->kNumInternalBytes);

      ikey_data = ikey.user_key().data();
      row_data = value.data();
      row_size = value.size();
      
      if(check){
        memcpy(origin_index_num,ikey_data,INDEX_NUM_SIZE);
        check = false;
      }

      check row index number
      char index_num[INDEX_NUM_SIZE];
      memcpy(index_num,ikey_data,INDEX_NUM_SIZE);
      if(memcmp(origin_index_num, index_num, INDEX_NUM_SIZE) != 0){
        KETILOG::WARNLOG(LOGTAG, "index invalid \n");
        index_valid = false;
        return;
      }

      check is deleted key
      const char *key_data = key.data();
      string keystr = char_to_hexstr(key_data,kNumInternalBytes_+indexnum_size);
      if(!(find(snippet_->deleted_key.begin(), snippet_->deleted_key.end(), keystr) == snippet_->deleted_key.end())) { 
        cout << "deleted" << endl;
        continue;
      }
      
      std::cout << "[Row(HEX)] KEY: " << ikey.user_key().ToString(true) << " | VALUE: " << value.ToString(true) << endl;

      scan_result->row_offset.push_back(scan_result->length);
      total_block_row_count++;
      current_row_count++;
      temp++;

      if(snippet_->primary_length != 0){
        char total_row_data[snippet_->primary_length+row_size];
        int pk_length;

        pk_length = getPrimaryKeyData(ikey_data, total_row_data, snippet_->primary_key_list);
       
        memcpy(total_row_data + pk_length, row_data, row_size);
        memcpy(scan_result->data + scan_result->length, total_row_data, pk_length + row_size);
        
        scan_result->length += row_size + pk_length;
        scan_result->row_count++;
      }else{
        memcpy(scan_result->data+scan_result->length, row_data, row_size);
        scan_result->length += row_size;
        scan_result->row_count++; 
      }
    } 

  }else{
    string pk_str = snippet_->index_pk;
    Document document;
    document.Parse(pk_str.c_str());
    Value &index_pk = document["index_pk"];

    vector<char> target_pk;
    target_pk.assign(snippet_->index_num,snippet_->index_num+4);

    bool pk_valid = true;

    while(pk_valid){
      for(int i=0; i<index_pk.Size(); i++){
        int key_type = snippet_->table_datatype[i];

        if(key_type == MySQL_INT32 || key_type == MySQL_DATE){
          int key_length = snippet_->table_offlen[i];
          union{
            int value;
            char byte[4];
          }pk;
          pk.value = index_pk[i][ipk].GetInt();
          for(int j=0; j<key_length; j++){
            target_pk.push_back(pk.byte[j]);
          }

        }else if(key_type == MySQL_INT64 || key_type == 246){
          int key_length = snippet_->table_offlen[i];
          union{
            int64_t value;
            char byte[8];
          }pk;
          pk.value = index_pk[i][ipk].GetInt64();
          for(int j=0; j<key_length; j++){
            target_pk.push_back(pk.byte[j]);
          }

        }else if(key_type == 254 || key_type == 15){
          string pk = index_pk[i][ipk].GetString();
          int key_length = pk.length();
          for(int j=0; j<key_length; j++){
            target_pk.push_back(pk[j]);
          }
        }
      }

      char *p = &*target_pk.begin();
      Slice target_slice(p,target_pk.size());

      datablock_iter->Seek(target_slice);

      if(datablock_iter->Valid()){
        const Slice& key = datablock_iter->key();
        const Slice& value = datablock_iter->value();

        InternalKey ikey;
        ikey.DecodeFrom(key);
        ikey_data = ikey.user_key().data();

        테스트 출력
        std::cout << "[Row(HEX)] KEY: " << ikey.user_key().ToString(true) << " | VALUE: " << value.ToString(true) << endl;

        if(target_slice.compare(ikey.user_key()) == 0){ 
          row_data = value.data();
          row_size = value.size();

          char total_row_data[snippet_->primary_length+row_size];
          int pk_length;

          pk_length = getPrimaryKeyData(ikey_data, total_row_data, snippet_->primary_key_list);
        
          memcpy(total_row_data + pk_length, row_data, row_size);
          memcpy(scan_result->data + scan_result->length, total_row_data, pk_length + row_size);
          
          scan_result->length += row_size + pk_length;
          scan_result->row_count++;
          
        }else{
          primary key error(출력 지우지 말기)
          cout << "primary key error [no primary key!]" << endl;      

          cout << "target: ";
          for(int i=0; i<target_slice.size(); i++){
            printf("%02X ",(u_char)target_slice[i]);
          }
          cout << endl;

          cout << "ikey: ";
          for(int i=0; i<ikey.user_key().size(); i++){
            printf("%02X ",(u_char)ikey.user_key()[i]);
          }
          cout << endl;

          check row index number
          char index_num[indexnum_size];
          memcpy(index_num,ikey_data,indexnum_size);
          if(memcmp(snippet_->index_num, index_num, indexnum_size) != 0){
            cout << "different index number: ";
            for(int i=0; i<indexnum_size; i++){
              printf("(%02X %02X)",(u_char)snippet_->index_num[i],(u_char)index_num[i]);
            }
            cout << endl;
            index_valid = false;
            pk_valid = false;
          }
        }
        ipk++;

      }else{
        go to next block
        pk_valid = false;
      }

    }
  }
}

void Scan::EnQueueData(Result scan_result, Snippet snippet_){
    if(snippet_.scan_type == Full_Scan_Filter){
      if(scan_result.length != 0){
        FilterQueue.push_work(scan_result);        
      }else{
        MergeQueue.push_work(scan_result);
      }

    }else if(snippet_.scan_type == Full_Scan){
      MergeQueue.push_work(scan_result);

    }else if(snippet_.scan_type == Index_Scan_Filter){
      if(scan_result.length != 0){
        FilterQueue.push_work(scan_result);
      }else{
        MergeQueue.push_work(scan_result);
      }
      
    }else{
      MergeQueue.push_work(scan_result);
    }
}

int Scan::getPrimaryKeyData(const char* ikey_data, char* dest, list<PrimaryKey> pk_list){
  int offset = 4;
  int pk_length = 0;

  std::list<PrimaryKey>::iterator piter;
  for(piter = pk_list.begin(); piter != pk_list.end(); piter++){
      int key_length = (*piter).key_length;
      int key_type = (*piter).key_type;

      switch(key_type){
        case MySQL_INT32:
        case MySQL_INT64:{
          char pk[key_length];
          pk[0] = 0x00;
          for(int i = 0; i < key_length; i++){
            pk[i] = ikey_data[offset+key_length-i-1];
            ikey[00 00 00 00 00 00 00 01]->dest[01 00 00 00 00 00 00 00]
          }
          memcpy(dest+pk_length, pk, key_length);
          pk_length += key_length;
          offset += key_length;
          break;
        }case MySQL_DATE:{
          char pk[key_length];
          for(int i = 0; i < key_length; i++){
            pk[i] = ikey_data[offset+key_length-i-1];
            ikey[0F 98 8C]->dest[8C 98 0F]
          }
          memcpy(dest+pk_length, pk, key_length);
          pk_length += key_length;
          offset += key_length;
          break;
        }case MySQL_NEWDECIMAL:
         case MySQL_STRING:{
          ikey_data[39 33 37 2D 32 34 31 2D 33 31 39 38 20 20 20]
          dest[39 33 37 2D 32 34 31 2D 33 31 39 38 20 20 20]
          memcpy(dest+pk_length, ikey_data+offset, key_length);
          pk_length += key_length;
          offset += key_length;
          break;
        }case MySQL_VARSTRING:{
          char pk[key_length];
          int var_key_length = 0;
          bool end = false;
          while(!end){
            if(ikey_data[offset] == sep || ikey_data[offset] == gap){
              offset++;
            }else if(ikey_data[offset] == fin){
              offset++;   
              end = true;  
            }else{
              pk[var_key_length] = ikey_data[offset];
              offset++;
              var_key_length++;
            }
          }
          if(var_key_length < 256){
            char len = (char)var_key_length;
            dest[pk_length] = len;
            pk_length += 1;
          }else{
            char len[2];
            int l1 = var_key_length / 256;
            int l2 = var_key_length % 256;
            len[0] = (char)l1;
            len[1] = (char)l2;
            memcpy(dest+pk_length,len,2);
            pk_length += 2;
          }
          memcpy(dest+pk_length, pk, var_key_length);
          pk_length += var_key_length;
          break;
        }default:{
          KETILOG::FATALLOG(LOGTAG, "Data Type Not Defined! ("+to_string(key_type)+ ")\n");
        }
      }
  }

  return pk_length;
}

char* Scan::hexstr_to_char(const char* hexstr, int* row_size){
    size_t len = strlen(hexstr);
    if(len % 2 != 0){
        return NULL;
    }
    size_t final_len = len / 2;
    char* chrs = (char*)malloc((final_len+1) * sizeof(*chrs));
    for (size_t i=0, j=0; j<final_len; i+=2, j++)
        chrs[j] = (hexstr[i] % 32 + 9) % 25 * 16 + (hexstr[i+1] % 32 + 9) % 25;
    chrs[final_len] = '\0';
    *row_size = static_cast<int>(final_len);
    return chrs;
}

constexpr char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                           '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
string Scan::char_to_hexstr(const char *data, int len){
  std::string s(len * 2, ' ');
  for (int i = 0; i < len; ++i) {
    s[2 * i]     = hexmap[(data[i] & 0xF0) >> 4];
    s[2 * i + 1] = hexmap[data[i] & 0x0F];
  }
  return s;
}


