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

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h" 

#include "storage_engine_instance.grpc.pb.h"

#include "keti_log.h"
#include "ip_config.h"

using namespace std;

struct Response {
	StorageEngineInstance::MetaDataResponse* metadataResponse;
	int row_count;
	bool is_done;
	condition_variable cond;
	mutex mu;
		Response(){
		this->metadataResponse = new StorageEngineInstance::MetaDataResponse;
	}
};

class TableManager { /* modify as singleton class */

/* Methods */
public:
	struct ColumnSchema {
		string column_name;
		int type;
		int length;
		int offset;
	};
	
	struct DataBlockHandle {
		off64_t Offset;
		off64_t Length;
	};
	
	struct SSTFile{
		string filename;
		string csdName;
		//vector<struct DataBlockHandle> BlockList;
	};
	
	struct Table {
		string tablename;
		int tablesize;
		int tablerow;
		vector<struct ColumnSchema> Schema;
		vector<struct SSTFile> SSTList;
		vector<vector<string>> IndexList;
		vector<string> PK;
	};

	static int GetTableSchema(string tablename,vector<struct ColumnSchema> &dst){
		return GetInstance().getTableSchema(tablename,dst);
	}

	static vector<string> GetOrderedTableBySize(vector<string> tablenames){
		return GetInstance().getOrderedTableBySize(tablenames);
	}

	static int GetIndexList(string tablename, vector<vector<string>> &dst){
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
	int getIndexList(string tablename, vector<vector<string>> &dst);
	vector<string> getSSTList(string tablename);
	void printTableManager();

/* Variables */
public:
    inline const static string LOGTAG = "Monitoring Container::Table Manager";

private:
    mutex Table_Mutex;
	unordered_map<string,struct Table> m_TableManager;
	unordered_map<string,Response*> return_data;
	
};