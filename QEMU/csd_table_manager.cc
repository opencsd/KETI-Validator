#include "csd_table_manager.h"

void TableManager::InitCSDTableManager(){
    //small

    table_rep.insert({{"1","lineitem"},"/home/pi/CSD/csd-worker-module-simulator/tpch_small_sst/lineitem/000088.sst"});
    table_rep.insert({{"1","customer"},"/home/pi/CSD/csd-worker-module-simulator/tpch_small_sst/customer/000644.sst"});
    table_rep.insert({{"1","orders"},"/home/pi/CSD/csd-worker-module-simulator/tpch_small_sst/orders/000212.sst"});
    table_rep.insert({{"1","part"},"/home/pi/CSD/csd-worker-module-simulator/tpch_small_sst/part/000336.sst"});
    table_rep.insert({{"1","supplier"},"/home/pi/CSD/csd-worker-module-simulator/tpch_small_sst/supplier/000768.sst"});
    table_rep.insert({{"1","partsupp"},"/home/pi/CSD/csd-worker-module-simulator/tpch_small_sst/partsupp/000460.sst"});
    table_rep.insert({{"1","region"},"/home/pi/CSD/csd-worker-module-simulator/tpch_small_sst/region/000584.sst"});

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


    // 1m
    // table_rep.insert({{"1","lineitem"},"/home/pi/CSD/csd-worker-module/tpch_1m_no_index_sst/lineitem/002531.sst"});
    // table_rep.insert({{"1","customer"},"/home/pi/CSD/csd-worker-module/tpch_1m_no_index_sst/customer/003289.sst"});
    // table_rep.insert({{"1","orders"},"/home/pi/CSD/csd-worker-module/tpch_1m_no_index_sst/orders/002655.sst"});
    // table_rep.insert({{"1","part"},"/home/pi/CSD/csd-worker-module/tpch_1m_no_index_sst/part/002779.sst"});
    // table_rep.insert({{"1","supplier"},"/home/pi/CSD/csd-worker-module/tpch_1m_no_index_sst/supplier/003153.sst"});
    // table_rep.insert({{"1","partsupp"},"/home/pi/CSD/csd-worker-module/tpch_1m_no_index_sst/partsupp/002903.sst"});
    // table_rep.insert({{"1","region"},"/home/pi/CSD/csd-worker-module/tpch_1m_no_index_sst/region/003027.sst"});
 
    // table_rep.insert({{"2","lineitem"},"/tpch_1m_no_index_sst/lineitem/002532.sst"});
    // table_rep.insert({{"2","customer"},"/tpch_1m_no_index_sst/customer/003290.sst"});
    // table_rep.insert({{"2","orders"},"/tpch_1m_no_index_sst/orders/002656.sst"});
    // table_rep.insert({{"2","part"},"/tpch_1m_no_index_sst/part/002780.sst"});
    // table_rep.insert({{"2","supplier"},"/tpch_1m_no_index_sst/supplier/003154.sst"});
    // table_rep.insert({{"2","partsupp"},"/tpch_1m_no_index_sst/partsupp/002904.sst"});
    // table_rep.insert({{"2","nation"},"/tpch_1m_no_index_sst/nation/003028.sst"});

    // table_rep.insert({{"3","lineitem"},"/tpch_1m_no_index_sst/lineitem/002533.sst"});
    // table_rep.insert({{"3","customer"},"/tpch_1m_no_index_sst/customer/003349.sst"});
    // table_rep.insert({{"3","orders"},"/tpch_1m_no_index_sst/orders/002657.sst"});
    // table_rep.insert({{"3","part"},"/tpch_1m_no_index_sst/part/002781.sst"});
    // table_rep.insert({{"3","supplier"},"/tpch_1m_no_index_sst/supplier/003213.sst"});
    // table_rep.insert({{"3","partsupp"},"/tpch_1m_no_index_sst/partsupp/002905.sst"});

    // table_rep.insert({{"4","lineitem"},"/tpch_1m_no_index_sst/lineitem/002534.sst"});
    // table_rep.insert({{"4","customer"},"/tpch_1m_no_index_sst/customer/003350.sst"});
    // table_rep.insert({{"4","orders"},"/tpch_1m_no_index_sst/orders/002658.sst"});
    // table_rep.insert({{"4","part"},"/tpch_1m_no_index_sst/part/002782.sst"});
    // table_rep.insert({{"4","supplier"},"/tpch_1m_no_index_sst/supplier/003214.sst"});
    // table_rep.insert({{"4","partsupp"},"/tpch_1m_no_index_sst/partsupp/002906.sst"});

    // table_rep.insert({{"5","lineitem"},"/tpch_1m_no_index_sst/lineitem/002593.sst"});
    // table_rep.insert({{"5","customer"},"/tpch_1m_no_index_sst/customer/003351.sst"});
    // table_rep.insert({{"5","orders"},"/tpch_1m_no_index_sst/orders/002717.sst"});
    // table_rep.insert({{"5","part"},"/tpch_1m_no_index_sst/part/002841.sst"});
    // table_rep.insert({{"5","supplier"},"/tpch_1m_no_index_sst/supplier/003215.sst"});
    // table_rep.insert({{"5","partsupp"},"/tpch_1m_no_index_sst/partsupp/002966.sst"});

    // table_rep.insert({{"6","lineitem"},"/tpch_1m_no_index_sst/lineitem/002594.sst"});
    // table_rep.insert({{"6","customer"},"/tpch_1m_no_index_sst/customer/003352.sst"});
    // table_rep.insert({{"6","orders"},"/tpch_1m_no_index_sst/orders/002718.sst"});
    // table_rep.insert({{"6","part"},"/tpch_1m_no_index_sst/part/002842.sst"});
    // table_rep.insert({{"6","supplier"},"/tpch_1m_no_index_sst/supplier/003216.sst"});
    // table_rep.insert({{"6","partsupp"},"/tpch_1m_no_index_sst/partsupp/002966.sst"});

    // table_rep.insert({{"7","lineitem"},"/tpch_1m_no_index_sst/lineitem/002595.sst"});
    // table_rep.insert({{"7","customer"},"/tpch_1m_no_index_sst/customer/003354.sst"});
    // table_rep.insert({{"7","orders"},"/tpch_1m_no_index_sst/orders/002719.sst"});
    // table_rep.insert({{"7","part"},"/tpch_1m_no_index_sst/part/002843.sst"});
    // table_rep.insert({{"7","supplier"},"/tpch_1m_no_index_sst/supplier/003287.sst"});
    // table_rep.insert({{"7","partsupp"},"/tpch_1m_no_index_sst/partsupp/002967.sst"});

    // table_rep.insert({{"8","lineitem"},"/tpch_1m_no_index_sst/lineitem/002596.sst"});
    // table_rep.insert({{"8","customer"},"/tpch_1m_no_index_sst/customer/003394.sst"});
    // table_rep.insert({{"8","orders"},"/tpch_1m_no_index_sst/orders/002720.sst"});
    // table_rep.insert({{"8","part"},"/tpch_1m_no_index_sst/part/002844.sst"});
    // table_rep.insert({{"8","supplier"},"/tpch_1m_no_index_sst/supplier/003288.sst"});
    // table_rep.insert({{"8","partsupp"},"/tpch_1m_no_index_sst/partsupp/002968.sst"});

    //origin
    // table_rep.insert({{"1","lineitem"},"/home/pi/CSD/csd-worker-module/tpch_origin_sst/lineitem/A/001379.sst"});
    // table_rep.insert({{"1","lineitem"},"/home/pi/CSD/csd-worker-module/tpch_origin_sst/lineitem/B/001382.sst"});
    // table_rep.insert({{"1","customer"},"/home/pi/CSD/csd-worker-module/tpch_origin_sst/customer/000644.sst"});
    // table_rep.insert({{"1","orders"},"/home/pi/CSD/csd-worker-module/tpch_origin_sst/orders/000290.sst"});
    // table_rep.insert({{"1","part"},"/home/pi/CSD/csd-worker-module/tpch_origin_sst/part/003805.sst"});
    // table_rep.insert({{"1","supplier"},"/home/pi/CSD/csd-worker-module/tpch_origin_sst/supplier/000788.sst"});
    // table_rep.insert({{"1","partsupp"},"/home/pi/CSD/csd-worker-module/tpch_origin_sst/partsupp/000538.sst"});
    // table_rep.insert({{"1","region"},"/home/pi/CSD/csd-worker-module/tpch_origin_sst/region/000662.sst"});

    // table_rep.insert({{"2","lineitem"},"/tpch_origin_sst/lineitem/A/001383.sst"});
    // table_rep.insert({{"2","lineitem"},"/tpch_origin_sst/lineitem/B/001384.sst"});    
    // table_rep.insert({{"2","customer"},"/tpch_origin_sst/customer/000723.sst"});
    // table_rep.insert({{"2","orders"},"/tpch_origin_sst/orders/000291.sst"});
    // table_rep.insert({{"2","part"},"/tpch_origin_sst/part/003806.sst"});
    // table_rep.insert({{"2","supplier"},"/tpch_origin_sst/supplier/000847.sst"});
    // table_rep.insert({{"2","partsupp"},"/tpch_origin_sst/partsupp/000539.sst"});
    // table_rep.insert({{"2","nation"},"/tpch_origin_sst/nation/000663.sst"});

    // table_rep.insert({{"3","lineitem"},"/tpch_origin_sst/lineitem/A/001400.sst"});
    // table_rep.insert({{"3","lineitem"},"/tpch_origin_sst/lineitem/B/001434.sst"});    
    // table_rep.insert({{"3","customer"},"/tpch_origin_sst/customer/000724.sst"});
    // table_rep.insert({{"3","orders"},"/tpch_origin_sst/orders/000292.sst"});
    // table_rep.insert({{"3","part"},"/tpch_origin_sst/part/003807.sst"});
    // table_rep.insert({{"3","supplier"},"/tpch_origin_sst/supplier/000848.sst"});
    // table_rep.insert({{"3","partsupp"},"/tpch_origin_sst/partsupp/000540.sst"});

    // table_rep.insert({{"4","lineitem"},"/tpch_origin_sst/lineitem/A/001435.sst"});
    // table_rep.insert({{"4","lineitem"},"/tpch_origin_sst/lineitem/B/001436.sst"});    
    // table_rep.insert({{"4","customer"},"/tpch_origin_sst/customer/000725.sst"});
    // table_rep.insert({{"4","orders"},"/tpch_origin_sst/orders/000351.sst"});
    // table_rep.insert({{"4","part"},"/tpch_origin_sst/part/003808.sst"});
    // table_rep.insert({{"4","supplier"},"/tpch_origin_sst/supplier/000849.sst"});
    // table_rep.insert({{"4","partsupp"},"/tpch_origin_sst/partsupp/000599.sst"});

    // table_rep.insert({{"5","lineitem"},"/tpch_origin_sst/lineitem/A/001437.sst"});
    // table_rep.insert({{"5","lineitem"},"/tpch_origin_sst/lineitem/B/001471.sst"});    
    // table_rep.insert({{"5","customer"},"/tpch_origin_sst/customer/000726.sst"});
    // table_rep.insert({{"5","orders"},"/tpch_origin_sst/orders/000352.sst"});
    // table_rep.insert({{"5","part"},"/tpch_origin_sst/part/003809.sst"});
    // table_rep.insert({{"5","supplier"},"/tpch_origin_sst/supplier/000850.sst"});
    // table_rep.insert({{"5","partsupp"},"/tpch_origin_sst/partsupp/000600.sst"});

    // table_rep.insert({{"6","lineitem"},"/tpch_origin_sst/lineitem/A/001472.sst"});
    // table_rep.insert({{"6","lineitem"},"/tpch_origin_sst/lineitem/B/001473.sst"});    
    // table_rep.insert({{"6","customer"},"/tpch_origin_sst/customer/000785.sst"});
    // table_rep.insert({{"6","orders"},"/tpch_origin_sst/orders/000353.sst"});
    // table_rep.insert({{"6","part"},"/tpch_origin_sst/part/003810.sst"});
    // table_rep.insert({{"6","supplier"},"/tpch_origin_sst/supplier/000923.sst"});
    // table_rep.insert({{"6","partsupp"},"/tpch_origin_sst/partsupp/000601.sst"});

    // table_rep.insert({{"7","lineitem"},"/tpch_origin_sst/lineitem/A/001474.sst"});
    // table_rep.insert({{"7","lineitem"},"/tpch_origin_sst/lineitem/B/001507.sst"});    
    // table_rep.insert({{"7","customer"},"/tpch_origin_sst/customer/000786.sst"});
    // table_rep.insert({{"7","orders"},"/tpch_origin_sst/orders/000354.sst"});
    // table_rep.insert({{"7","part"},"/tpch_origin_sst/part/003811.sst"});
    // table_rep.insert({{"7","supplier"},"/tpch_origin_sst/supplier/000924.sst"});
    // table_rep.insert({{"7","partsupp"},"/tpch_origin_sst/partsupp/000602.sst"});

    // table_rep.insert({{"8","lineitem"},"/tpch_origin_sst/lineitem/A/001506.sst"});
    // table_rep.insert({{"8","lineitem"},"/tpch_origin_sst/lineitem/B/001508.sst"});    
    // table_rep.insert({{"8","customer"},"/tpch_origin_sst/customer/000787.sst"});
    // table_rep.insert({{"8","orders"},"/tpch_origin_sst/orders/000413.sst"});
    // table_rep.insert({{"8","part"},"/tpch_origin_sst/part/003812.sst"});
    // table_rep.insert({{"8","supplier"},"/tpch_origin_sst/supplier/000925.sst"});
    // table_rep.insert({{"8","partsupp"},"/tpch_origin_sst/partsupp/000661.sst"});

}

string TableManager::GetTableRep(string csd_name, string table_name){
    string file_path = table_rep[make_pair(csd_name,table_name)];
	return file_path;
}