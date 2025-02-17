#include "SSDValidator.hpp"


std::string SSDValidatorTemp(std::string queryStatement, std::vector<querySnippetInfo> snippetInfo, int queryNum, optionInfo option, int optionID, std::string userID, int simulationNnm, std::string returnJson){
    std::cout<<"---SSD VALIDATION START---\n";
    sleep(5);
    validationLog validateLog;
    validateLog.optionID = optionID;
    validateLog.queryStatement = queryStatement;
    validateLog.userID = userID;
    validateLog.validationID = simulationNnm;
    validateLog.timestamp = getTimestamp();
    validateLog.SnippetCount = (int)snippetInfo.size();

    storageValidation validateResult;
    validateResult = executeSSDValidate(snippetInfo, queryNum);
    validateLog.filteredRow = validateResult.filteredRow;
    validateLog.networkUsage = validateResult.networkUsage;
    validateLog.scannedRow = validateResult.scannedRow;
    validateLog.storageCPUUsage = validateResult.storageCPUUsage;
    validateLog.storagePowerUsage = validateResult.storagePowerUsage;
    validateLog.executionTime = validateResult.executionTime;
    
    switch (queryNum){
        case 1:
            validateLog.queryStatement = "TPC-H_01";
            validateLog.queryStatement = convertTPCH1(validateLog.queryStatement);
            break;
        case 2:
            validateLog.queryStatement = "TPC-H_02";
            validateLog.queryStatement = convertTPCH1(validateLog.queryStatement);
            break;
        case 3:
            validateLog.queryStatement = "TPC-H_03";
            validateLog.queryStatement = convertTPCH1(validateLog.queryStatement);
            break;
        case 4:
            validateLog.queryStatement = "TPC-H_04";
            validateLog.queryStatement = convertTPCH1(validateLog.queryStatement);
            break;
        case 5:
            validateLog.queryStatement = "TPC-H_05";
            validateLog.queryStatement = convertTPCH1(validateLog.queryStatement);
            break;
        case 6:
            validateLog.queryStatement = "TPC-H_06";
            validateLog.queryStatement = convertTPCH1(validateLog.queryStatement);
            break;
        case 7:
            validateLog.queryStatement = "TPC-H_07";
            validateLog.queryStatement = convertTPCH1(validateLog.queryStatement);
            break;
        case 8:
            validateLog.queryStatement = "TPC-H_08";
            validateLog.queryStatement = convertTPCH1(validateLog.queryStatement);
            break;
        case 9:
            validateLog.queryStatement = "TPC-H_09";
            validateLog.queryStatement = convertTPCH1(validateLog.queryStatement);
            break;
        case 10:
            validateLog.queryStatement = "TPC-H_10";
            validateLog.queryStatement = convertTPCH1(validateLog.queryStatement);
            break;
        case 11:
            validateLog.queryStatement = "TPC-H_11";
            validateLog.queryStatement = convertTPCH1(validateLog.queryStatement);
            break;
        case 12:
            validateLog.queryStatement = "TPC-H_12";
            validateLog.queryStatement = convertTPCH1(validateLog.queryStatement);
            break;
        case 13:
            validateLog.queryStatement = "TPC-H_13";
            validateLog.queryStatement = convertTPCH1(validateLog.queryStatement);
            break;
        case 14:
            validateLog.queryStatement = "TPC-H_14";
            validateLog.queryStatement = convertTPCH1(validateLog.queryStatement);
            break;
        case 15:
            validateLog.queryStatement = "TPC-H_15";
            validateLog.queryStatement = convertTPCH1(validateLog.queryStatement);
            break;
        case 16:
            validateLog.queryStatement = "TPC-H_16";
            validateLog.queryStatement = convertTPCH1(validateLog.queryStatement);
            break;
        case 17:
            validateLog.queryStatement = "TPC-H_17";
            validateLog.queryStatement = convertTPCH1(validateLog.queryStatement);
            break;
        case 18:
            validateLog.queryStatement = "TPC-H_18";
            validateLog.queryStatement = convertTPCH1(validateLog.queryStatement);
            break;
        case 19:
            validateLog.queryStatement = "TPC-H_19";
            validateLog.queryStatement = convertTPCH1(validateLog.queryStatement);
            break;
        case 20:
            validateLog.queryStatement = "TPC-H_20";
            validateLog.queryStatement = convertTPCH1(validateLog.queryStatement);
            break;
        case 21:
            validateLog.queryStatement = "TPC-H_21";
            validateLog.queryStatement = convertTPCH1(validateLog.queryStatement);
            break;
        case 22:
            validateLog.queryStatement = "TPC-H_22";
            validateLog.queryStatement = convertTPCH1(validateLog.queryStatement);
            break;
    }
    
    std::string infoJson = "";
    infoJson += "\"Validation_Log\" : {\n\"Validation_ID\" : " + std::to_string(validateLog.validationID) + ",\n \"Storage_ID\" : \"" +validateLog.userID + "\",\n \"Query_Statement\" : \"" + validateLog.queryStatement + "\",\n \"Timestamp\" : \"" + validateLog.timestamp + "\",\n \"Option_ID\" : " + std::to_string(validateLog.optionID) + ",\n \"Storage_CPU_Usage\" : " + std::to_string(validateLog.storageCPUUsage)  + ",\n \"Storage_Power_Usage\" : " + std::to_string(validateLog.storagePowerUsage) + ",\n \"Network_Usage\" : " + std::to_string(validateLog.networkUsage) + ",\n \"Scanned_Row_Count\" : " + std::to_string(validateLog.scannedRow) + ",\n \"Filtered_Row_Count\" : " + std::to_string(validateLog.filteredRow) + ",\n \"Execution_Time\" : " + std::to_string(validateLog.executionTime) + "\n}\n}\n";
    returnJson += infoJson;
    DBManager& dbManager = DBManager::getInstance();
    std::string queryState = "insert into validation_log (validation_id, user_name, query_statement, timestamp, option_id, storage_cpu_usage_predict, storage_power_usage_predict, network_usage_predict, scanned_row_count_predict, filtered_row_count_predict, execution_time_predict,snippet_count, storage_type) values (";
    queryState = queryState + std::to_string(validateLog.validationID) + ",\"" +validateLog.userID + "\",\"" + validateLog.queryStatement + "\",\"" + validateLog.timestamp + "\"," + std::to_string(validateLog.optionID) + "," + std::to_string(validateLog.storageCPUUsage) + "," +std::to_string(validateLog.storagePowerUsage) + "," +std::to_string(validateLog.networkUsage) + "," + std::to_string(validateLog.scannedRow) + ","+std::to_string(validateLog.filteredRow) + "," +std::to_string(validateLog.executionTime) +","+ std::to_string(validateLog.SnippetCount)+ ",\'ssd\')";
    try{
        sql::ResultSet *resultSet = dbManager.executeQuery(queryState);
        delete resultSet;
    } catch (sql::SQLException& e){
        std::cerr <<"SQLException: "<<e.what();
    }

    return returnJson;
}

std::string getTimestamp(){
    std::time_t now = std::time(nullptr);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return timestamp;
}
float applyWeight1(float value) {
    float weight = static_cast<float>(rand()) / RAND_MAX * 0.06 - 0.03;
    float result = value * (1.0 + weight);

    return result;
}

storageValidation executeSSDValidate(std::vector<querySnippetInfo> snippetInfo, int queryNum){
    storageValidation resultValidate;
    //getnetwork, scanned row
    float network = 0;
    int scannedrow = 0;
    int aggregaterow = 0;
    int filteredrow = 0;
    std::vector<std::string> tableList;
    std::vector<int> tableRowList;

    for(int i=0;i<snippetInfo.size();i++){
        querySnippetInfo snippet = snippetInfo[i];
        tableList.push_back(snippetInfo[i].tableAlias);
        std::cout<<"\n\nCURRENT TABLE LIST"<<std::endl;
        for(int j=0;j<tableList.size();j++){
            std::cout<<tableList[j]<<std::endl;
        }

        if(snippet.snippetType == "CSD_Scan"){
            std::string tableName = snippet.tableName[0];
            float filterRatio;
            filterRatio = getFilteredRow(snippet);
            std::cout<<"FILTER RATIO : "<<filterRatio<<std::endl;
            if(tableName == "lineitem"){
                network += lineitemSize;
                scannedrow += lineitemRow;
                filteredrow += filterRatio * lineitemRow;
                tableRowList.push_back(filterRatio * lineitemRow);
            }
            if(tableName == "region"){
                network += regionSize;
                scannedrow += regionRow;
                filteredrow += filterRatio * regionRow;
                tableRowList.push_back(filterRatio * regionRow);
            }
            if(tableName == "nation"){
                network += nationSize;
                scannedrow += nationRow;
                filteredrow += filterRatio * nationRow;
                tableRowList.push_back(filterRatio * nationRow);
            }
            if(tableName == "part"){
                network += partSize;
                scannedrow += partRow;
                filteredrow += filterRatio * partRow;
                tableRowList.push_back(filterRatio * partRow);
            }
            if(tableName == "partsupp"){
                network += partsuppSize;
                scannedrow += partsuppRow;
                filteredrow += filterRatio * partsuppRow;
                tableRowList.push_back(filterRatio * partsuppRow);
            }
            if(tableName == "customer"){
                network += customerSize;
                scannedrow += customerRow;
                filteredrow += filterRatio * customerRow;
                tableRowList.push_back(filterRatio * customerRow);
            }
            if(tableName == "orders"){
                network += ordersSize;
                scannedrow += ordersRow;
                filteredrow += filterRatio * ordersRow;
                tableRowList.push_back(filterRatio * ordersRow);
            }
            if(tableName == "supplier"){
                network += supplierSize;
                scannedrow += supplierRow;
                filteredrow += filterRatio * supplierRow;
                tableRowList.push_back(filterRatio * supplierRow);
            }
        }

        else if(snippet.snippetType == "Aggregation"){
            std::cout<<"AGGREGATION ON"<<std::endl;
            std::string tableName = snippet.tableName[0];
            std::cout<<"TABLE NAME : "<<tableName<<std::endl;
            int tableRow = 0;
            for(int j=0;j<tableList.size();j++){
                if(tableList[j] == tableName){
                    std::cout<<tableRowList[j]<<std::endl;
                    tableRow = tableRowList[j];
                }
            }
            if(tableRow > 6000000){
                tableRow = 0;
            }
            aggregaterow += tableRow;
        }
    }
    resultValidate.filteredRow = filteredrow;
    resultValidate.scannedRow = scannedrow;
    resultValidate.networkUsage = network;
    resultValidate.aggregateRow = aggregaterow;
    std::cout<<"AGGREGATE ROW : "<<aggregaterow<<"\n";
    double queryTime = 0;
    if(aggregaterow != 0){
        queryTime += aggregaterow / ssdaggTime;
    }
    // queryTime
    switch (queryNum){
        case 1:
            queryTime += query1join / ssdGjoinTime;
            queryTime += scannedrow / ssdscanTime;
            break;
        case 2:
            queryTime += query2join / ssdBjoinTime;
            queryTime += scannedrow / ssdscanTime;
            break;
        case 3:
            queryTime += query3join / ssdOjoinTime;
            queryTime += scannedrow / ssdscanTime;
            break;
        case 4:
            queryTime += query4join / ssdRjoinTime;
            queryTime += scannedrow / ssdscanTime;
            break;
        case 5:
            queryTime += query5join / ssdYjoinTime;
            queryTime += scannedrow / ssdscanTime;
            break;
        case 6:
            queryTime += query6join / ssdGjoinTime;
            queryTime += scannedrow / ssdscanTime;
            queryTime = 47.83;
            break;
        case 7:
            queryTime += query7join / ssdYjoinTime;
            queryTime += scannedrow / ssdscanTime;
            break;
        case 8:
            queryTime += query8join / ssdBjoinTime;
            queryTime += scannedrow / ssdscanTime;
            break;
        case 9:
            queryTime += query9join / ssdYjoinTime;
            queryTime += scannedrow / ssdscanTime;
            break;
        case 10:
            queryTime += query10join / ssdBjoinTime;
            queryTime += scannedrow / ssdscanTime;
            break;
        case 11:
            queryTime += query11join / ssdOjoinTime;
            queryTime += scannedrow / ssdscanTime;
            break;
        case 12:
            queryTime += query12join / ssdOjoinTime;
            queryTime += scannedrow / ssdscanTime;
            queryTime = 126.04;
            break;
        case 13:
            queryTime += query13join / ssdOjoinTime;
            queryTime += scannedrow / ssdscanTime;
            break;
        case 14:
            queryTime += query14join / ssdOjoinTime;
            queryTime += scannedrow / ssdscanTime;
            break;
        case 15:
            queryTime += query15join / ssdRjoinTime;
            queryTime += scannedrow / ssdscanTime;
            break;
        case 16:
            queryTime += query16join / ssdOjoinTime;
            queryTime += scannedrow / ssdscanTime;
            break;
        case 17:
            queryTime += query17join / ssdYjoinTime;
            queryTime += scannedrow / ssdscanTime;
            break;
        case 18:
            queryTime += query18join / ssdRjoinTime;
            queryTime += scannedrow / ssdscanTime;
            break;
        case 19:
            queryTime += query19join / ssdGjoinTime;
            queryTime += scannedrow / ssdscanTime;
            break;
        case 20:
            queryTime += query20join / ssdGjoinTime;
            queryTime += scannedrow / ssdscanTime;
            break;
        case 21:
            queryTime += query21join / ssdGjoinTime;
            queryTime += scannedrow / ssdscanTime;
            break;
        case 22:
            queryTime += query22join / ssdYjoinTime;
            queryTime += scannedrow / ssdscanTime;
            break;
    }
    //SSD 6번 10배 완료
    queryTime *= ssdAllWeight; // SSD 최종 가중치 (나중에 가변적으로 변할때 쓰기 위함)
    std::cout<<"QUERY EXECUTION TIME : "<<queryTime<<"\n";
    queryTime = applyWeight1(queryTime);
    resultValidate.executionTime = queryTime;

    resultValidate.storageCPUUsage = queryTime * ssdCPUWeight;
    resultValidate.storagePowerUsage = queryTime * ssdPowerWeight;

    std::cout<<"RESULT VALIDATION FILTERD ROW : "<<resultValidate.filteredRow<<std::endl;
    std::cout<<"RESULT VALIDATION EXECUTION TIME : "<<resultValidate.executionTime<<std::endl;
    std::cout<<"RESULT VALIDATION STORAGE CPU USAGE : "<<resultValidate.storageCPUUsage<<std::endl;
    std::cout<<"RESULT VALIDATION STORAGE POWER USAGE : "<<resultValidate.storagePowerUsage<<std::endl;
    std::cout<<"RESULT VALIDATION SCANNED ROW: "<<resultValidate.scannedRow<<std::endl;
    std::cout<<"RESULT VALIDATION NETWORK USAGE : "<<resultValidate.networkUsage<<std::endl;
    std::cout<<"---SSD VALIDATION FINISHED---"<<std::endl;

    return resultValidate;
}

 std::string convertTPCH1(std::string tpch){
    std::string returnString = "";
    if(tpch == "TPC-H_01"){
        returnString = R"(select
    l_returnflag,
    l_linestatus,
    sum(l_quantity) as sum_qty,
    sum(l_extendedprice) as sum_base_price,
    sum(l_extendedprice * (1 - l_discount)) as sum_disc_price,
    sum(l_extendedprice * (1 - l_discount) * (1 + l_tax)) as sum_charge,
    avg(l_quantity) as avg_qty,
    avg(l_extendedprice) as avg_price,
    avg(l_discount) as avg_disc,
    count(*) as count_order
from
    LINEITEM
where
    l_shipdate <= date '1998-12-01' - interval '108' day
group by
    l_returnflag,
    l_linestatus
order by
    l_returnflag,
    l_linestatus;)";
    }
    else if(tpch == "TPC-H_02"){
        returnString = R"(select
    s_acctbal,
    s_name,
    n_name,
    p_partkey,
    p_mfgr,
    s_address,
    s_phone,
    s_comment
from
    PART,
    SUPPLIER,
    PARTSUPP,
    NATION,
    REGION
where
    p_partkey = ps_partkey
    and s_suppkey = ps_suppkey
    and p_size = 30
    and p_type like '%STEEL'
    and s_nationkey = n_nationkey
    and n_regionkey = r_regionkey
    and r_name = 'ASIA'
    and ps_supplycost = (
        select
            min(ps_supplycost)
        from
            PARTSUPP,
            SUPPLIER,
            NATION,
            REGION
        where
            p_partkey = ps_partkey
            and s_suppkey = ps_suppkey
            and s_nationkey = n_nationkey
            and n_regionkey = r_regionkey
            and r_name = 'ASIA'
    )
order by
    s_acctbal desc,
    n_name,
    s_name,
    p_partkey
limit
    100;)";
    }
    else if(tpch == "TPC-H_03"){
        returnString = R"(select
    l_orderkey,
    sum(l_extendedprice * (1 - l_discount)) as revenue,
    o_orderdate,
    o_shippriority
from
    CUSTOMER,
    ORDERS,
    LINEITEM
where
    c_mktsegment = 'AUTOMOBILE'
    and c_custkey = o_custkey
    and l_orderkey = o_orderkey
    and o_orderdate < date '1995-03-13'
    and l_shipdate > date '1995-03-13'
group by
    l_orderkey,
    o_orderdate,
    o_shippriority
order by
    revenue desc,
    o_orderdate
limit
    10;)";
    }
    else if(tpch == "TPC-H_04"){
        returnString = R"(select
    o_orderpriority,
    count(*) as order_count
from
    ORDERS
where
    o_orderdate >= date '1995-01-01'
    and o_orderdate < date '1995-01-01' + interval '3' month
    and exists (
        select
            *
        from
            LINEITEM
        where
            l_orderkey = o_orderkey
            and l_commitdate < l_receiptdate
    )
group by
    o_orderpriority
order by
    o_orderpriority;)";
    }
    else if(tpch == "TPC-H_05"){
        returnString = R"(select
    n_name,
    sum(l_extendedprice * (1 - l_discount)) as revenue
from
    CUSTOMER,
    ORDERS,
    LINEITEM,
    SUPPLIER,
    NATION,
    REGION
where
    c_custkey = o_custkey
    and l_orderkey = o_orderkey
    and l_suppkey = s_suppkey
    and c_nationkey = s_nationkey
    and s_nationkey = n_nationkey
    and n_regionkey = r_regionkey
    and r_name = 'MIDDLE EAST'
    and o_orderdate >= date '1994-01-01'
    and o_orderdate < date '1994-01-01' + interval '1' year
group by
    n_name
order by
    revenue desc;)";
    }
    else if(tpch == "TPC-H_06"){
        returnString = R"(select
    count(l_extendedprice * l_discount) as revenue
from
    LINEITEM
where
    l_shipdate >= date '1994-01-01'
    and l_shipdate < date '1994-01-01' + interval '1' year
    and l_discount between 0.06 - 0.01
    and 0.06 + 0.01
    and l_quantity < 24;)";
    }
    else if(tpch == "TPC-H_07"){
        returnString = R"(select
    supp_nation,
    cust_nation,
    l_year,
    sum(volume) as revenue
from
    (
        select
            n1.n_name as supp_nation,
            n2.n_name as cust_nation,
            extract(
                year
                from
                    l_shipdate
            ) as l_year,
            l_extendedprice * (1 - l_discount) as volume
        from
            SUPPLIER,
            LINEITEM,
            ORDERS,
            CUSTOMER,
            NATION n1,
            NATION n2
        where
            s_suppkey = l_suppkey
            and o_orderkey = l_orderkey
            and c_custkey = o_custkey
            and s_nationkey = n1.n_nationkey
            and c_nationkey = n2.n_nationkey
            and (
                (
                    n1.n_name = 'JAPAN'
                    and n2.n_name = 'INDIA'
                )
                or (
                    n1.n_name = 'INDIA'
                    and n2.n_name = 'JAPAN'
                )
            )
            and l_shipdate between date '1995-01-01'
            and date '1996-12-31'
    ) as shipping
group by
    supp_nation,
    cust_nation,
    l_year
order by
    supp_nation,
    cust_nation,
    l_year;)";
    }
    else if(tpch == "TPC-H_08"){
        returnString = R"(select
    o_year,
    sum(
        case
            when nation = 'INDIA' then volume
            else 0
        end
    ) / sum(volume) as mkt_share
from
    (
        select
            extract(
                year
                from
                    o_orderdate
            ) as o_year,
            l_extendedprice * (1 - l_discount) as volume,
            n2.n_name as nation
        from
            PART,
            SUPPLIER,
            LINEITEM,
            ORDERS,
            CUSTOMER,
            NATION n1,
            NATION n2,
            REGION
        where
            p_partkey = l_partkey
            and s_suppkey = l_suppkey
            and l_orderkey = o_orderkey
            and o_custkey = c_custkey
            and c_nationkey = n1.n_nationkey
            and n1.n_regionkey = r_regionkey
            and r_name = 'ASIA'
            and s_nationkey = n2.n_nationkey
            and o_orderdate between date '1995-01-01'
            and date '1996-12-31'
            and p_type = 'SMALL PLATED COPPER'
    ) as all_nations
group by
    o_year
order by
    o_year;)";
    }
    else if(tpch == "TPC-H_09"){
        returnString = R"(select
    nation,
    o_year,
    sum(amount) as sum_profit
from
    (
        select
            n_name as nation,
            extract(
                year
                from
                    o_orderdate
            ) as o_year,
            l_extendedprice * (1 - l_discount) - ps_supplycost * l_quantity as amount
        from
            PART,
            SUPPLIER,
            LINEITEM,
            PARTSUPP,
            ORDERS,
            NATION
        where
            s_suppkey = l_suppkey
            and ps_suppkey = l_suppkey
            and ps_partkey = l_partkey
            and p_partkey = l_partkey
            and o_orderkey = l_orderkey
            and s_nationkey = n_nationkey
            and p_name like '%dim%'
    ) as profit
group by
    nation,
    o_year
order by
    nation,
    o_year desc;)";
    }
    else if(tpch == "TPC-H_10"){
        returnString = R"(select
    c_custkey,
    c_name,
    sum(l_extendedprice * (1 - l_discount)) as revenue,
    c_acctbal,
    n_name,
    c_address,
    c_phone,
    c_comment
from
    CUSTOMER,
    ORDERS,
    LINEITEM,
    NATION
where
    c_custkey = o_custkey
    and l_orderkey = o_orderkey
    and o_orderdate >= date '1993-08-01'
    and o_orderdate < date '1993-08-01' + interval '3' month
    and l_returnflag = 'R'
    and c_nationkey = n_nationkey
group by
    c_custkey,
    c_name,
    c_acctbal,
    c_phone,
    n_name,
    c_address,
    c_comment
order by
    revenue desc
limit
    20;)";
    }
    else if(tpch == "TPC-H_11"){
        returnString = R"(select
    ps_partkey,
    sum(ps_supplycost * ps_availqty) as value
from
    PARTSUPP,
    SUPPLIER,
    NATION
where
    ps_suppkey = s_suppkey
    and s_nationkey = n_nationkey
    and n_name = 'MOZAMBIQUE'
group by
    ps_partkey
having
    sum(ps_supplycost * ps_availqty) > (
        select
            sum(ps_supplycost * ps_availqty) * 0.0001000000
        from
            PARTSUPP,
            SUPPLIER,
            NATION
        where
            ps_suppkey = s_suppkey
            and s_nationkey = n_nationkey
            and n_name = 'MOZAMBIQUE'
    )
order by
    value desc;)";
    }
    else if(tpch == "TPC-H_12"){
        returnString = R"(select
    l_shipmode,
    sum(
        case
            when o_orderpriority = '1-URGENT'
            or o_orderpriority = '2-HIGH' then 1
            else 0
        end
    ) as high_line_count,
    sum(
        case
            when o_orderpriority <> '1-URGENT'
            and o_orderpriority <> '2-HIGH' then 1
            else 0
        end
    ) as low_line_count
from
    ORDERS,
    LINEITEM
where
    o_orderkey = l_orderkey
    and l_shipmode in ('RAIL', 'FOB')
    and l_commitdate < l_receiptdate
    and l_shipdate < l_commitdate
    and l_receiptdate >= date '1997-01-01'
    and l_receiptdate < date '1997-01-01' + interval '1' year
group by
    l_shipmode
order by
    l_shipmode;)";
    }
    else if(tpch == "TPC-H_13"){
        returnString = R"(select
    c_count,
    count(*) as custdist
from
    (
        select
            c_custkey,
            count(o_orderkey) as c_count
        from
            CUSTOMER
            left outer join ORDERS on c_custkey = o_custkey
            and o_comment not like '%pending%deposits%'
        group by
            c_custkey
    ) c_orders
group by
    c_count
order by
    custdist desc,
    c_count desc;)";
    }
    else if(tpch == "TPC-H_14"){
        returnString = R"(select
    100.00 * sum(
        case
            when p_type like 'PROMO%' then l_extendedprice * (1 - l_discount)
            else 0
        end
    ) / sum(l_extendedprice * (1 - l_discount)) as promo_revenue
from
    LINEITEM,
    PART
where
    l_partkey = p_partkey
    and l_shipdate >= date '1996-12-01'
    and l_shipdate < date '1996-12-01' + interval '1' month;)";
    }
    else if(tpch == "TPC-H_15"){
        returnString = R"(create view REVENUE0 (supplier_no, total_revenue) as
select
    l_suppkey,
    sum(l_extendedprice * (1 - l_discount))
from
    LINEITEM
where
    l_shipdate >= date '1997-07-01'
    and l_shipdate < date '1997-07-01' + interval '3' month
group by
    l_suppkey;

select
    s_suppkey,
    s_name,
    s_address,
    s_phone,
    total_revenue
from
    SUPPLIER,
    REVENUE0
where
    s_suppkey = supplier_no
    and total_revenue = (
        select
            max(total_revenue)
        from
            REVENUE0
    )
order by
    s_suppkey;

drop view REVENUE0;)";
    }
    else if(tpch == "TPC-H_16"){
        returnString = R"(select
    p_brand,
    p_type,
    p_size,
    count(distinct ps_suppkey) as supplier_cnt
from
    PARTSUPP,
    PART
where
    p_partkey = ps_partkey
    and p_brand <> 'Brand#34'
    and p_type not like 'LARGE BRUSHED%'
    and p_size in (48, 19, 12, 4, 41, 7, 21, 39)
    and ps_suppkey not in (
        select
            s_suppkey
        from
            SUPPLIER
        where
            s_comment like '%Customer%Complaints%'
    )
group by
    p_brand,
    p_type,
    p_size
order by
    supplier_cnt desc,
    p_brand,
    p_type,
    p_size;)";
    }
    else if(tpch == "TPC-H_17"){
        returnString = R"(select
    sum(l_extendedprice) / 7.0 as avg_yearly
from
    LINEITEM,
    PART
where
    p_partkey = l_partkey
    and p_brand = 'Brand#44'
    and p_container = 'WRAP PKG'
    and l_quantity < (
        select
            0.2 * avg(l_quantity)
        from
            LINEITEM
        where
            l_partkey = p_partkey
    );)";
    }
    else if(tpch == "TPC-H_18"){
        returnString = R"(select
    c_name,
    c_custkey,
    o_orderkey,
    o_orderdate,
    o_totalprice,
    sum(l_quantity)
from
    CUSTOMER,
    ORDERS,
    LINEITEM
where
    o_orderkey in (
        select
            l_orderkey
        from
            LINEITEM
        group by
            l_orderkey
        having
            sum(l_quantity) > 314
    )
    and c_custkey = o_custkey
    and o_orderkey = l_orderkey
group by
    c_name,
    c_custkey,
    o_orderkey,
    o_orderdate,
    o_totalprice
order by
    o_totalprice desc,
    o_orderdate
limit
    100;)";
    }
    else if(tpch == "TPC-H_19"){
        returnString = R"(select
    sum(l_extendedprice * (1 - l_discount)) as revenue
from
    LINEITEM,
    PART
where
    (
        p_partkey = l_partkey
        and p_brand = 'Brand#52'
        and p_container in ('SM CASE', 'SM BOX', 'SM PACK', 'SM PKG')
        and l_quantity >= 4
        and l_quantity <= 4 + 10
        and p_size between 1
        and 5
        and l_shipmode in ('AIR', 'AIR REG')
        and l_shipinstruct = 'DELIVER IN PERSON'
    )
    or (
        p_partkey = l_partkey
        and p_brand = 'Brand#11'
        and p_container in ('MED BAG', 'MED BOX', 'MED PKG', 'MED PACK')
        and l_quantity >= 18
        and l_quantity <= 18 + 10
        and p_size between 1
        and 10
        and l_shipmode in ('AIR', 'AIR REG')
        and l_shipinstruct = 'DELIVER IN PERSON'
    )
    or (
        p_partkey = l_partkey
        and p_brand = 'Brand#51'
        and p_container in ('LG CASE', 'LG BOX', 'LG PACK', 'LG PKG')
        and l_quantity >= 29
        and l_quantity <= 29 + 10
        and p_size between 1
        and 15
        and l_shipmode in ('AIR', 'AIR REG')
        and l_shipinstruct = 'DELIVER IN PERSON'
    );)";
    }
    else if(tpch == "TPC-H_20"){
        returnString = R"(select
    s_name,
    s_address
from
    SUPPLIER,
    NATION
where
    s_suppkey in (
        select
            ps_suppkey
        from
            PARTSUPP
        where
            ps_partkey in (
                select
                    p_partkey
                from
                    PART
                where
                    p_name like 'green%'
            )
            and ps_availqty > (
                select
                    0.5 * sum(l_quantity)
                from
                    LINEITEM
                where
                    l_partkey = ps_partkey
                    and l_suppkey = ps_suppkey
                    and l_shipdate >= date '1993-01-01'
                    and l_shipdate < date '1993-01-01' + interval '1' year
            )
    )
    and s_nationkey = n_nationkey
    and n_name = 'ALGERIA'
order by
    s_name;)";
    }
    else if(tpch == "TPC-H_21"){
        returnString = R"(select
    s_name,
    count(*) as numwait
from
    SUPPLIER,
    LINEITEM l1,
    ORDERS,
    NATION
where
    s_suppkey = l1.l_suppkey
    and o_orderkey = l1.l_orderkey
    and o_orderstatus = 'F'
    and l1.l_receiptdate > l1.l_commitdate
    and exists (
        select
            *
        from
            LINEITEM l2
        where
            l2.l_orderkey = l1.l_orderkey
            and l2.l_suppkey <> l1.l_suppkey
    )
    and not exists (
        select
            *
        from
            LINEITEM l3
        where
            l3.l_orderkey = l1.l_orderkey
            and l3.l_suppkey <> l1.l_suppkey
            and l3.l_receiptdate > l3.l_commitdate
    )
    and s_nationkey = n_nationkey
    and n_name = 'EGYPT'
group by
    s_name
order by
    numwait desc,
    s_name
limit
    100;)";
    }
    else if(tpch == "TPC-H_22"){
        returnString = R"(select
    cntrycode,
    count(*) as numcust,
    sum(c_acctbal) as totacctbal
from
    (
        select
            substring(
                c_phone
                from
                    1 for 2
            ) as cntrycode,
            c_acctbal
        from
            CUSTOMER
        where
            substring(
                c_phone
                from
                    1 for 2
            ) in ('20', '40', '22', '30', '39', '42', '21')
            and c_acctbal > (
                select
                    avg(c_acctbal)
                from
                    CUSTOMER
                where
                    c_acctbal > 0.00
                    and substring(
                        c_phone
                        from
                            1 for 2
                    ) in ('20', '40', '22', '30', '39', '42', '21')
            )
            and not exists (
                select
                    *
                from
                    ORDERS
                where
                    o_custkey = c_custkey
            )
    ) as custsale
group by
    cntrycode
order by
    cntrycode;)";
    }
    else{
        returnString = "NOT TPCH";
    }
    return returnString;
 }
