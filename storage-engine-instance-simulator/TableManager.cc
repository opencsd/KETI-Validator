#include "TableManager.h"

using namespace rapidjson;

//테이블 스키마 정보 임의로 저장 >> 이후 DB File Monitoring을 통해 데이터 저장 -> 업데이트시 DB Connector Instance로 반영
int TableManager::initTableManager(){
	//KETILOG::DEBUGLOG(LOGTAG, "# Init TableManager");

	//read TableManager.json
	string json = "";
	std::ifstream openFile("/NewTableManager.json");
	//std::ifstream openFile("/root/KETI-Simulator/Simulator/storage-engine-instance-simulator/NewTableManager.json");
	if(openFile.is_open() ){
		std::string line;
		while(getline(openFile, line)){
			json += line;
		}
		openFile.close();
	}
	//parse json	
	Document document;
	document.Parse(json.c_str());

	Value &TableList = document["Table List"];
	for(int i=0;i<TableList.Size();i++){
		Value &TableObject = TableList[i];
		auto tbl = new Table();

		Value &tablenameObject = TableObject["tablename"];
		tbl->tablename = tablenameObject.GetString();

		// tbl.tablesize = TableObject["Size"].GetInt();

		// if(TableObject.HasMember("PK")){
		// 	for(int i = 0; i < TableObject["PK"].Size(); i++){
		// 		tbl.PK.push_back(TableObject["PK"][i].GetString());
		// 	}
		// }
		// if(TableObject.HasMember("Index")){
		// 	for(int i = 0; i < TableObject["Index"].Size(); i++){
		// 		vector<string> index;
		// 		for(int j = 0; j < TableObject["Index"][i].Size(); j++){
		// 			index.push_back(TableObject["Index"][i][j].GetString());
		// 		}
		// 		tbl.IndexList.push_back(index);
		// 	}
		// }
		// debug code check filename
		std::cout << "tablename : " << tbl->tablename << std::endl;
		Value &SchemaObject = TableObject["Schema"];
		for(int j=0;j<SchemaObject.Size();j++){
			Value &ColumnObject = SchemaObject[j];
			auto Column = new ColumnSchema;

			Column->column_name = ColumnObject["column_name"].GetString();
			Column->type = ColumnObject["type"].GetInt();
			Column->length = ColumnObject["length"].GetInt();
			Column->offset = ColumnObject["offset"].GetInt();

			tbl->Schema.push_back(*Column);
			//debug code check Schema
			std::cout << "Schema : " << tbl->Schema[j].column_name << std::endl;
		}
		Value &SSTList = TableObject["SST List"];
		for(int j=0;j<SSTList.Size();j++){
			Value &SSTObject = SSTList[j];
			auto SstFile = new SSTFile;
			SstFile->filename = SSTObject["filename"].GetString();
			SstFile->csdName = SSTObject["csd_name"].GetString();
			//Value &BlockList = SSTObject["Block List"];
			
			// for(int k=0;k<BlockList.Size();k++){
			// 	Value &BlockHandleObject = BlockList[k];
			// 	struct DataBlockHandle DataBlockHandle;

			// 	DataBlockHandle.Offset = BlockHandleObject["Offset"].GetInt();
			// 	DataBlockHandle.Length = BlockHandleObject["Length"].GetInt();

			// 	SstFile->BlockList.push_back(DataBlockHandle);
			// }

			tbl->SSTList.push_back(*SstFile);
			//debug code check sst filename
			std::cout << "SST : " << SSTObject["filename"].GetString() << std::endl;
		}
		Value &tablerowObject = TableObject["table_rows"];
		tbl -> tablerow = tablerowObject.GetInt();
		m_TableManager.insert({tbl->tablename,*tbl});
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
		std::cout << "Not Present" << std::endl;
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

int TableManager::getIndexList(string tablename, vector<vector<string>> &dst){
	if (m_TableManager.find(tablename) == m_TableManager.end()){
		std::cout << "Not Present" << std::endl;
        return -1;
	}
	struct Table tbl = m_TableManager[tablename];
	dst = tbl.IndexList;
	return 0;
}

// void TableManager::printTableManager(){
// 	unordered_map <string,struct Table> :: iterator it;

// 	for(it = m_TableManager.begin(); it != m_TableManager.end() ; it++){		
// 		struct Table tmp = it->second;
// 		std::cout << "tablename : " << tmp.tablename << std::endl;
// 		std::cout << "Column name     Type  Length Offset" << std::endl;
// 		vector<struct ColumnSchema>::iterator itor = tmp.Schema.begin();

// 		for (; itor != tmp.Schema.end(); itor++) {
// 			std::cout << left << setw(15) << (*itor).column_name << " " << left<< setw(5) << (*itor).type << " " << left << setw(6) <<(*itor).length << " " << left << setw(5) << (*itor).offset << std::endl;
// 		}

// 		vector<struct SSTFile>::iterator itor2 = tmp.SSTList.begin();

// 		for (; itor2 != tmp.SSTList.end(); itor2++) {
// 			std::cout << "SST filename : " << (*itor2).filename << std::endl;

// 			vector<struct DataBlockHandle>::iterator itor3 = (*itor2).BlockList.begin();
// 			std::cout << " StartOffset-Length" << endl;
// 			for (; itor3 != (*itor2).BlockList.end(); itor3++) {
// 				std::cout << "Block Handle : " << (*itor3).Offset << "-" << (*itor3).Length << std::endl;				
// 			}
// 		}
// 	}
// }