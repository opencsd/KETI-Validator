#include "csd_table_manager.h"

void CSDTableManager::initCSDTableManager(){
    // /dev/sda /dev/ngd-blk
    //"NoCompression", "ZSTD"
    TableRep tr1 = {"/dev/ngd-blk3",false,false,false,0,"NoCompression"};
	table_rep_.insert({"lineitem",tr1});
    TableRep tr2 = {"/dev/ngd-blk3",false,false,false,0,"NoCompression"};
	table_rep_.insert({"customer",tr2});
    TableRep tr3 = {"/dev/ngd-blk3",false,false,false,0,"NoCompression"};
	table_rep_.insert({"nation",tr3});
    TableRep tr4 = {"/dev/ngd-blk3",false,false,false,0,"NoCompression"};
	table_rep_.insert({"orders",tr4});
    TableRep tr5 = {"/dev/ngd-blk3",false,false,false,0,"NoCompression"};
	table_rep_.insert({"part",tr5});
    TableRep tr6 = {"/dev/ngd-blk3",false,false,false,0,"NoCompression"};
	table_rep_.insert({"partsupp",tr6});
    TableRep tr7 = {"/dev/ngd-blk3",false,false,false,0,"NoCompression"};
	table_rep_.insert({"region",tr7});
    TableRep tr8 = {"/dev/ngd-blk3",false,false,false,0,"NoCompression"};
	table_rep_.insert({"supplier",tr8});

    TableRep tr9 = {"/dev/ngd-blk3",false,false,false,0,"NoCompression"};
	table_rep_.insert({"lineitemNum1",tr9});
    TableRep tr10 = {"/dev/ngd-blk3",false,false,false,0,"NoCompression"};
	table_rep_.insert({"lineitemNum2",tr10});

    TableRep tr20 = {"./001928.sst",true,true,false,0,"ZSTD"};
    table_rep_.insert({"compress.lineitem",tr20});
    TableRep tr21 = {"./001699.sst",false,false,false,0,"NoCompression"};
    table_rep_.insert({"uncompress.lineitem",tr21});
    // EX)
    // TableRep tr22 = {"$file_path",false,false,false,0,"NoCompression"};
	// table_rep_.insert({"$table_name",tr22});
}

TableRep CSDTableManager::getTableRep(string table_name){
    TableRep temp = table_rep_[table_name];
	return table_rep_[table_name];
}