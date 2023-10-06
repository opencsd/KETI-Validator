#include "csd_table_manager.h"

void CSDTableManager::initCSDTableManager(){
    table_rep.insert({{"1","lineitem"},"/tpch_origin_sst/lineitem/A/001379.sst"});
    table_rep.insert({{"1","lineitem"},"/tpch_origin_sst/lineitem/A/001437.sst"});
    table_rep.insert({{"1","customer"},"/tpch_origin_sst/customer/000644.sst"});
    table_rep.insert({{"1","orders"},"/tpch_origin_sst/orders/000212.sst"});
    table_rep.insert({{"1","part"},"/tpch_origin_sst/part/000336.sst"});
    table_rep.insert({{"1","supplier"},"/tpch_origin_sst/supplier/000768.sst"});
    table_rep.insert({{"1","partsupp"},"/tpch_origin_sst/partsupp/000460.sst"});
    table_rep.insert({{"1","region"},"/tpch_origin_sst/region/000584.sst"});

    table_rep.insert({{"2","lineitem"},"/tpch_origin_sst/lineitem/B/001382.sst"});
    table_rep.insert({{"2","lineitem"},"/tpch_origin_sst/lineitem/B/001471.sst"});
    table_rep.insert({{"2","customer"},"/tpch_origin_sst/customer/000645.sst"});
    table_rep.insert({{"2","orders"},"/tpch_origin_sst/orders/000271.sst"});
    table_rep.insert({{"2","part"},"/tpch_origin_sst/part/000395.sst"});
    table_rep.insert({{"2","supplier"},"/tpch_origin_sst/supplier/000769.sst"});
    table_rep.insert({{"2","partsupp"},"/tpch_origin_sst/partsupp/000519.sst"});
    table_rep.insert({{"2","nation"},"/tpch_origin_sst/nation/000643.sst"});

    table_rep.insert({{"3","lineitem"},"/tpch_origin_sst/lineitem/A/001383.sst"});
    table_rep.insert({{"3","lineitem"},"/tpch_origin_sst/lineitem/A/001472.sst"});
    table_rep.insert({{"3","customer"},"/tpch_origin_sst/customer/000646.sst"});
    table_rep.insert({{"3","orders"},"/tpch_origin_sst/orders/000272.sst"});
    table_rep.insert({{"3","part"},"/tpch_origin_sst/part/000396.sst"});
    table_rep.insert({{"3","supplier"},"/tpch_origin_sst/supplier/000770.sst"});
    table_rep.insert({{"3","partsupp"},"/tpch_origin_sst/partsupp/000520.sst"});

    table_rep.insert({{"4","lineitem"},"/tpch_origin_sst/lineitem/B/001384.sst"});
    table_rep.insert({{"4","lineitem"},"/tpch_origin_sst/lineitem/B/001473.sst"});
    table_rep.insert({{"4","customer"},"/tpch_origin_sst/customer/000705.sst"});
    table_rep.insert({{"4","orders"},"/tpch_origin_sst/orders/000273.sst"});//
    table_rep.insert({{"4","part"},"/tpch_origin_sst/part/000397.sst"});
    table_rep.insert({{"4","supplier"},"/tpch_origin_sst/supplier/000829.sst"});
    table_rep.insert({{"4","partsupp"},"/tpch_origin_sst/partsupp/000521.sst"});

    table_rep.insert({{"5","lineitem"},"/tpch_origin_sst/lineitem/A/001400.sst"});
    table_rep.insert({{"5","lineitem"},"/tpch_origin_sst/lineitem/A/001474.sst"});
    table_rep.insert({{"5","customer"},"/tpch_origin_sst/customer/000706.sst"});
    table_rep.insert({{"5","orders"},"/tpch_origin_sst/orders/000274.sst"});//
    table_rep.insert({{"5","part"},"/tpch_origin_sst/part/000398.sst"});
    table_rep.insert({{"5","supplier"},"/tpch_origin_sst/supplier/000830.sst"});
    table_rep.insert({{"5","partsupp"},"/tpch_origin_sst/partsupp/000522.sst"});

    table_rep.insert({{"6","lineitem"},"/tpch_origin_sst/lineitem/B/001434.sst"});
    table_rep.insert({{"6","lineitem"},"/tpch_origin_sst/lineitem/A/001506.sst"});
    table_rep.insert({{"6","customer"},"/tpch_origin_sst/customer/000707.sst"});
    table_rep.insert({{"6","orders"},"/tpch_origin_sst/orders/000333.sst"});
    table_rep.insert({{"6","part"},"/tpch_origin_sst/part/000457.sst"});
    table_rep.insert({{"6","supplier"},"/tpch_origin_sst/supplier/000831.sst"});
    table_rep.insert({{"6","partsupp"},"/tpch_origin_sst/partsupp/000581.sst"});

    table_rep.insert({{"7","lineitem"},"/tpch_origin_sst/lineitem/A/001435.sst"});
    table_rep.insert({{"7","lineitem"},"/tpch_origin_sst/lineitem/B/001507.sst"});
    table_rep.insert({{"7","customer"},"/tpch_origin_sst/customer/000708.sst"});
    table_rep.insert({{"7","orders"},"/tpch_origin_sst/orders/000334.sst"});//
    table_rep.insert({{"7","part"},"/tpch_origin_sst/part/000458.sst"});
    table_rep.insert({{"7","supplier"},"/tpch_origin_sst/supplier/000832.sst"});
    table_rep.insert({{"7","partsupp"},"/tpch_origin_sst/partsupp/000582.sst"});

    table_rep.insert({{"8","lineitem"},"/tpch_origin_sst/lineitem/B/001436.sst"});
    table_rep.insert({{"8","lineitem"},"/tpch_origin_sst/lineitem/B/001508.sst"});
    table_rep.insert({{"8","customer"},"/tpch_origin_sst/customer/000767.sst"});
    table_rep.insert({{"8","orders"},"/tpch_origin_sst/orders/000335.sst"});//
    table_rep.insert({{"8","part"},"/tpch_origin_sst/part/000459.sst"});
    table_rep.insert({{"8","supplier"},"/tpch_origin_sst/supplier/000834.sst"});
    table_rep.insert({{"8","partsupp"},"/tpch_origin_sst/partsupp/000583.sst"});
}

TableRep CSDTableManager::getTableRep(string table_name){
    TableRep temp = table_rep_[table_name];
	return table_rep_[table_name];
}