#include "csd_table_manager.h"

void TableManager::InitCSDTableManager(){
    table_rep.insert({{"1","lineitem"},"/tpch_origin_sst/lineitem/A/001379.sst"});
    table_rep.insert({{"1","lineitem"},"/tpch_origin_sst/lineitem/A/001437.sst"});
    table_rep.insert({{"1","customer"},"/tpch_origin_sst/customer/000664.sst"});
    table_rep.insert({{"1","orders"},"/tpch_origin_sst/orders/000290.sst"});
    table_rep.insert({{"1","part"},"/tpch_origin_sst/part/003805.sst"});
    table_rep.insert({{"1","supplier"},"/tpch_origin_sst/supplier/000788.sst"});
    table_rep.insert({{"1","partsupp"},"/tpch_origin_sst/partsupp/000538.sst"});
    table_rep.insert({{"1","region"},"/tpch_origin_sst/region/000662.sst"});

    table_rep.insert({{"2","lineitem"},"/tpch_origin_sst/lineitem/B/001382.sst"});
    table_rep.insert({{"2","lineitem"},"/tpch_origin_sst/lineitem/B/001471.sst"});
    table_rep.insert({{"2","customer"},"/tpch_origin_sst/customer/000723.sst"});
    table_rep.insert({{"2","orders"},"/tpch_origin_sst/orders/000291.sst"});
    table_rep.insert({{"2","part"},"/tpch_origin_sst/part/003806.sst"});
    table_rep.insert({{"2","supplier"},"/tpch_origin_sst/supplier/000847.sst"});
    table_rep.insert({{"2","partsupp"},"/tpch_origin_sst/partsupp/000539.sst"});
    table_rep.insert({{"2","nation"},"/tpch_origin_sst/nation/000663.sst"});

    table_rep.insert({{"3","lineitem"},"/tpch_origin_sst/lineitem/A/001383.sst"});
    table_rep.insert({{"3","lineitem"},"/tpch_origin_sst/lineitem/A/001472.sst"});
    table_rep.insert({{"3","customer"},"/tpch_origin_sst/customer/000724.sst"});
    table_rep.insert({{"3","orders"},"/tpch_origin_sst/orders/000292.sst"});
    table_rep.insert({{"3","part"},"/tpch_origin_sst/part/003807.sst"});
    table_rep.insert({{"3","supplier"},"/tpch_origin_sst/supplier/000848.sst"});
    table_rep.insert({{"3","partsupp"},"/tpch_origin_sst/partsupp/000540.sst"});

    table_rep.insert({{"4","lineitem"},"/tpch_origin_sst/lineitem/B/001384.sst"});
    table_rep.insert({{"4","lineitem"},"/tpch_origin_sst/lineitem/B/001473.sst"});
    table_rep.insert({{"4","customer"},"/tpch_origin_sst/customer/000725.sst"});
    table_rep.insert({{"4","orders"},"/tpch_origin_sst/orders/000351.sst"});//
    table_rep.insert({{"4","part"},"/tpch_origin_sst/part/003808.sst"});
    table_rep.insert({{"4","supplier"},"/tpch_origin_sst/supplier/000849.sst"});
    table_rep.insert({{"4","partsupp"},"/tpch_origin_sst/partsupp/000599.sst"});

    table_rep.insert({{"5","lineitem"},"/tpch_origin_sst/lineitem/A/001400.sst"});
    table_rep.insert({{"5","lineitem"},"/tpch_origin_sst/lineitem/A/001474.sst"});
    table_rep.insert({{"5","customer"},"/tpch_origin_sst/customer/000726.sst"});
    table_rep.insert({{"5","orders"},"/tpch_origin_sst/orders/000352.sst"});//
    table_rep.insert({{"5","part"},"/tpch_origin_sst/part/003809.sst"});
    table_rep.insert({{"5","supplier"},"/tpch_origin_sst/supplier/000850.sst"});
    table_rep.insert({{"5","partsupp"},"/tpch_origin_sst/partsupp/000600.sst"});

    table_rep.insert({{"6","lineitem"},"/tpch_origin_sst/lineitem/B/001434.sst"});
    table_rep.insert({{"6","lineitem"},"/tpch_origin_sst/lineitem/A/001506.sst"});
    table_rep.insert({{"6","customer"},"/tpch_origin_sst/customer/000785.sst"});
    table_rep.insert({{"6","orders"},"/tpch_origin_sst/orders/000353.sst"});
    table_rep.insert({{"6","part"},"/tpch_origin_sst/part/003810.sst"});
    table_rep.insert({{"6","supplier"},"/tpch_origin_sst/supplier/000923.sst"});
    table_rep.insert({{"6","partsupp"},"/tpch_origin_sst/partsupp/000601.sst"});

    table_rep.insert({{"7","lineitem"},"/tpch_origin_sst/lineitem/A/001435.sst"});
    table_rep.insert({{"7","lineitem"},"/tpch_origin_sst/lineitem/B/001507.sst"});
    table_rep.insert({{"7","customer"},"/tpch_origin_sst/customer/000786.sst"});
    table_rep.insert({{"7","orders"},"/tpch_origin_sst/orders/000354.sst"});//
    table_rep.insert({{"7","part"},"/tpch_origin_sst/part/003811.sst"});
    table_rep.insert({{"7","supplier"},"/tpch_origin_sst/supplier/000924.sst"});
    table_rep.insert({{"7","partsupp"},"/tpch_origin_sst/partsupp/000602.sst"});

    table_rep.insert({{"8","lineitem"},"/tpch_origin_sst/lineitem/B/001436.sst"});
    table_rep.insert({{"8","lineitem"},"/tpch_origin_sst/lineitem/B/001508.sst"});
    table_rep.insert({{"8","customer"},"/tpch_origin_sst/customer/000787.sst"});
    table_rep.insert({{"8","orders"},"/tpch_origin_sst/orders/000413.sst"});//
    table_rep.insert({{"8","part"},"/tpch_origin_sst/part/003812.sst"});
    table_rep.insert({{"8","supplier"},"/tpch_origin_sst/supplier/000925.sst"});
    table_rep.insert({{"8","partsupp"},"/tpch_origin_sst/partsupp/000661.sst"});
}

string TableManager::GetTableRep(string csd_name, string table_name){
    string file_path = table_rep[make_pair(csd_name,table_name)];
	return file_path;
}