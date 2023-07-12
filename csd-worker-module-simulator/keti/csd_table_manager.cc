
#include "csd_table_manager.h"

void TableManager::InitCSDTableManager(){
    table_rep.insert({{"1","lineitem"},"/tpch_small_sst/lineitem/000088.sst"});
    table_rep.insert({{"1","customer"},"/tpch_small_sst/customer/000644.sst"});
    table_rep.insert({{"1","orders"},"/tpch_small_sst/orders/000212.sst"});
    table_rep.insert({{"1","part"},"/tpch_small_sst/part/000336.sst"});
    table_rep.insert({{"1","supplier"},"/tpch_small_sst/supplier/000768.sst"});
    table_rep.insert({{"1","partsupp"},"/tpch_small_sst/partsupp/000460.sst"});
    table_rep.insert({{"1","region"},"/tpch_small_sst/region/000584.sst"});

    table_rep.insert({{"2","lineitem"},"/tpch_small_sst/lineitem/000147.sst"});
    table_rep.insert({{"2","customer"},"/tpch_small_sst/customer/000645.sst"});
    table_rep.insert({{"2","orders"},"/tpch_small_sst/orders/000271.sst"});
    table_rep.insert({{"2","part"},"/tpch_small_sst/part/000395.sst"});
    table_rep.insert({{"2","supplier"},"/tpch_small_sst/supplier/000769.sst"});
    table_rep.insert({{"2","partsupp"},"/tpch_small_sst/partsupp/000519.sst"});
    table_rep.insert({{"2","nation"},"/tpch_small_sst/nation/000643.sst"});

    table_rep.insert({{"3","lineitem"},"/tpch_small_sst/lineitem/000148.sst"});
    table_rep.insert({{"3","customer"},"/tpch_small_sst/customer/000646.sst"});
    table_rep.insert({{"3","orders"},"/tpch_small_sst/orders/000272.sst"});
    table_rep.insert({{"3","part"},"/tpch_small_sst/part/000396.sst"});
    table_rep.insert({{"3","supplier"},"/tpch_small_sst/supplier/000770.sst"});
    table_rep.insert({{"3","partsupp"},"/tpch_small_sst/partsupp/000520.sst"});

    table_rep.insert({{"4","lineitem"},"/tpch_small_sst/lineitem/000149.sst"});
    table_rep.insert({{"4","customer"},"/tpch_small_sst/customer/000705.sst"});
    table_rep.insert({{"4","orders"},"/tpch_small_sst/orders/000273.sst"});//
    table_rep.insert({{"4","part"},"/tpch_small_sst/part/000397.sst"});
    table_rep.insert({{"4","supplier"},"/tpch_small_sst/supplier/000829.sst"});
    table_rep.insert({{"4","partsupp"},"/tpch_small_sst/partsupp/000521.sst"});

    table_rep.insert({{"5","lineitem"},"/tpch_small_sst/lineitem/000150.sst"});
    table_rep.insert({{"5","customer"},"/tpch_small_sst/customer/000706.sst"});
    table_rep.insert({{"5","orders"},"/tpch_small_sst/orders/000274.sst"});//
    table_rep.insert({{"5","part"},"/tpch_small_sst/part/000398.sst"});
    table_rep.insert({{"5","supplier"},"/tpch_small_sst/supplier/000830.sst"});
    table_rep.insert({{"5","partsupp"},"/tpch_small_sst/partsupp/000522.sst"});

    table_rep.insert({{"6","lineitem"},"/tpch_small_sst/lineitem/000209.sst"});
    table_rep.insert({{"6","customer"},"/tpch_small_sst/customer/000707.sst"});
    table_rep.insert({{"6","orders"},"/tpch_small_sst/orders/000333.sst"});
    table_rep.insert({{"6","part"},"/tpch_small_sst/part/000457.sst"});
    table_rep.insert({{"6","supplier"},"/tpch_small_sst/supplier/000831.sst"});
    table_rep.insert({{"6","partsupp"},"/tpch_small_sst/partsupp/000581.sst"});

    table_rep.insert({{"7","lineitem"},"/tpch_small_sst/lineitem/000210.sst"});
    table_rep.insert({{"7","customer"},"/tpch_small_sst/customer/000708.sst"});
    table_rep.insert({{"7","orders"},"/tpch_small_sst/orders/000334.sst"});//
    table_rep.insert({{"7","part"},"/tpch_small_sst/part/000458.sst"});
    table_rep.insert({{"7","supplier"},"/tpch_small_sst/supplier/000832.sst"});
    table_rep.insert({{"7","partsupp"},"/tpch_small_sst/partsupp/000582.sst"});

    table_rep.insert({{"8","lineitem"},"/tpch_small_sst/lineitem/000211.sst"});
    table_rep.insert({{"8","customer"},"/tpch_small_sst/customer/000767.sst"});
    table_rep.insert({{"8","orders"},"/tpch_small_sst/orders/000335.sst"});//
    table_rep.insert({{"8","part"},"/tpch_small_sst/part/000459.sst"});
    table_rep.insert({{"8","supplier"},"/tpch_small_sst/supplier/000834.sst"});
    table_rep.insert({{"8","partsupp"},"/tpch_small_sst/partsupp/000583.sst"});
}

string TableManager::GetTableRep(string csd_name, string table_name){
    string file_path = table_rep[make_pair(csd_name,table_name)];
	return file_path;
}