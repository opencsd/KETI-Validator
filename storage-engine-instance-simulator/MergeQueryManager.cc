#include "MergeQueryManager.h"
#include <chrono>
using StorageEngineInstance::Snippet_Order_OrderDirection_ASC;
using StorageEngineInstance::Snippet_Order_OrderDirection_DESC;
using StorageEngineInstance::Snippet_ValueType_STRING;
using StorageEngineInstance::Snippet_Filter_OperType;

void MergeQueryManager::RunSnippetWork(){
    tableCnt = snippet.table_name_size();
    isGroupby = (snippet.group_by_size() == 0) ? false : true;
    isOrderby = (snippet.order_by().column_name_size() == 0) ? false : true;
    isLimit = (snippet.limit().length() != 0) ? true : false;
    
    //Save "base_table"
    for(int i=0; i<tableCnt; i++){
        TableData tableData = BufferManager::GetTableData(snippet.query_id(),snippet.table_name(i));
        if(!tableData.valid){
            KETILOG::FATALLOG(LOGTAG,"error>> query_id error or non init table");
        }

        // 테이블 데이터 로우 수 확인 - Debug Code  
        if(KETILOG::IsLogLevelUnder(TRACE)){
            //cout << "<driving/driven table row>" << endl;
            for(auto i : tableData.table_data){
                if(i.second.type == TYPE_STRING){
                    cout << i.first << "|" << i.second.strvec.size() << "|" << i.second.type << endl;
                }else if(i.second.type == TYPE_INT){
                    cout << i.first << "|" << i.second.intvec.size() << "|" << i.second.type << endl;
                }else if(i.second.type == TYPE_FLOAT){
                    cout << i.first << "|" << i.second.floatvec.size() << "|" << i.second.type << endl;
                }else if(i.second.type == TYPE_EMPTY){
                    cout << i.first << "|" << "empty row" << endl;
                }else{
                    cout << "target table row else ?" << endl;
                }
            }  
        }

        (i==0) ? left_table = tableData : right_table = tableData;
    }  
    
    //Do snippet work -> (Make "hash_table") -> Make "target_table"
    switch(snippetType){
        case StorageEngineInstance::SnippetRequest::AGGREGATION_SNIPPET:{
            //single table aggregation&projection
            target_table = left_table;
            break;
        }case StorageEngineInstance::SnippetRequest::STORAGE_FILTER_SNIPPET:{
            Filtering();
            break;
        }case StorageEngineInstance::SnippetRequest::INNER_JOIN_SNIPPET:{
            // InnerJoin_nestedloop();
            InnerJoin_hash();
            break;
        }case StorageEngineInstance::SnippetRequest::LEFT_OUTER_JOIN_SNIPPET:{
            // LeftOuterJoin_nestedloop();
            LeftOuterJoin_hash();
            break;
        }case StorageEngineInstance::SnippetRequest::RIGHT_OUTER_JOIN_SNIPPET:{
            RightOuterJoin_hash();
            break;
        }case StorageEngineInstance::SnippetRequest::CROSS_JOIN_SNIPPET:{
            CrossJoin();
            break;
        }case StorageEngineInstance::SnippetRequest::UNION_SNIPPET:{
            Union();          
            break;
        }case StorageEngineInstance::SnippetRequest::IN_SNIPPET:{
            In();
            break;
        }case StorageEngineInstance::SnippetRequest::DEPENDENCY_INNER_JOIN_SNIPPET:{
            DependencyInnerJoin();
            break;
        }case StorageEngineInstance::SnippetRequest::DEPENDENCY_EXIST_SNIPPET:{
            DependencyExist();
            break;
        }case StorageEngineInstance::SnippetRequest::DEPENDENCY_IN_SNIPPET:{
            DependencyIn();
            break;
        }
    }

    left_table.table_data.clear();
    right_table.table_data.clear();
    hash_table.clear();

    // 테이블 데이터 로우 수 확인 - Debug Code   
    if(KETILOG::IsLogLevelUnder(TRACE)){
        cout << "<target table row>" << endl;
        for(auto i : target_table.table_data){
            if(i.second.type == TYPE_STRING){
                cout << i.first << "|" << i.second.strvec.size() << "|" << i.second.type << endl;
                // cout << i.first << "|" << i.second.strvec[0] << endl;
            }else if(i.second.type == TYPE_INT){
                cout << i.first << "|" << i.second.intvec.size() << "|" << i.second.type << endl;
                // cout << i.first << "|" << i.second.intvec[0] << endl;
            }else if(i.second.type == TYPE_FLOAT){
                cout << i.first << "|" << i.second.floatvec.size() << "|" << i.second.type << endl;
                // cout << i.first << "|" << i.second.floatvec[0] << endl;
            }else if(i.second.type == TYPE_EMPTY){
                cout << i.first << "|" << "empty row" << endl;
            }else{
            cout << "target table row else ?" << endl;
            }
        }
    }

    //GroupBy -> Make "group_by_key" & "group_by_table"
    if(isGroupby){
        GroupBy();
    }else{
        group_by_table.push_back(target_table);
    } 
    target_table.table_data.clear();

    if(KETILOG::IsLogLevelUnder(TRACE)){
        // 테이블 데이터 로우 수 확인 - Debug Code 
        cout << "<group by table row>" << endl;
        for(int j = 0; j < group_by_table.size(); j++){
            for(auto i : group_by_table[j].table_data){
                if(i.second.type == TYPE_STRING){
                    cout << i.first << "|" << i.second.strvec.size() << "|" << i.second.type << endl;
                    // cout << i.first << "|" << i.second.strvec[0] << endl;
                }else if(i.second.type == TYPE_INT){
                    cout << i.first << "|" << i.second.intvec.size() << "|" << i.second.type << endl;
                    // cout << i.first << "|" << i.second.intvec[0] << endl;
                }else if(i.second.type == TYPE_FLOAT){
                    cout << i.first << "|" << i.second.floatvec.size() << "|" << i.second.type << endl;
                    // cout << i.first << "|" << i.second.floatvec[0] << endl;
                }else if(i.second.type == TYPE_EMPTY){
                    cout << i.first << "|" << "empty row" << "|" << i.second.type << endl;
                }else{
                cout << "target table row else ?" << endl;
                }
            }
            cout << "--" << endl;
        }
    }

    //Column Projection -> Make "result_table"
    Aggregation();
    group_by_table.clear();
    
    if(KETILOG::IsLogLevelUnder(TRACE)){
        // 테이블 데이터 로우 수 확인 - Debug Code 
        cout << "<result table row>" << endl;
        for(auto i : result_table.table_data){
            if(i.second.type == TYPE_STRING){
                cout << i.first << "|" << i.second.strvec.size() << "|" << i.second.type << endl;
                // cout << i.first << "|" << i.second.strvec[0] << endl;
            }else if(i.second.type == TYPE_INT){
                cout << i.first << "|" << i.second.intvec.size() << "|" << i.second.type << endl;
                // cout << i.first << "|" << i.second.intvec[0] << endl;
            }else if(i.second.type == TYPE_FLOAT){
                cout << i.first << "|" << i.second.floatvec.size() << "|" << i.second.type << endl;
                // cout << i.first << "|" << i.second.floatvec[0] << endl;
            }else if(i.second.type == TYPE_EMPTY){
                cout << i.first << "|" << "empty row" << "|" << i.second.type << endl;
            }else{
            cout << "target table row else ?" << endl;
            }
        }
    }

    if(isOrderby){
        //Order By -> Make "ordered_index" & "order_by_table"
        OrderBy();
        //Save "order_by_table"
        if(isLimit){
            BufferManager::SaveTableData(snippet.query_id(),snippet.table_alias(),order_by_table,snippet.limit().offset(),snippet.limit().length());
        }else{
            BufferManager::SaveTableData(snippet.query_id(),snippet.table_alias(),order_by_table,0,order_by_table.row_count);
        }
    }else{
        //Save "result_table"
        if(isLimit){
            BufferManager::SaveTableData(snippet.query_id(),snippet.table_alias(),result_table,snippet.limit().offset(),snippet.limit().length());
        }else{
            BufferManager::SaveTableData(snippet.query_id(),snippet.table_alias(),result_table,0,result_table.row_count);
        } 
    }

}

void MergeQueryManager::Aggregation(){
    //한 프로젝션씩 결과 생성
    int result_row_num = TYPE_EMPTY;

    for(int p = 0; p < snippet.column_projection_size(); p++){
        ColData colData;//프로젝션 결과 컬럼 벡터

        //한 그룹씩 프로젝션 수행
        for(size_t g = 0; g<group_by_table.size(); g++){
            int group_row_num = group_by_table[g].row_count;
            if(group_row_num == 0) continue;
        
            switch(snippet.column_projection(p).select_type()){
                case StorageEngineInstance::Snippet_Projection_SelectType_COLUMNNAME:{
                    T t;
                    if(isGroupby){//로우 하나
                        t = Projection(g,p,0);//p번째 프로젝션을 g번째 그룹의 0번째 로우에 적용
                        switch(t.type){
                            case TYPE_STRING:{
                                colData.strvec.push_back(t.varString);
                                colData.isnull.push_back(false);
                                colData.row_count++;
                                break;
                            }case TYPE_INT:{    
                                colData.intvec.push_back(t.varInt);
                                colData.isnull.push_back(false);
                                colData.row_count++;
                                break;
                            }case TYPE_FLOAT:{
                                colData.floatvec.push_back(t.varFloat);
                                colData.isnull.push_back(false);
                                colData.row_count++;
                                break;
                            }default:{
                                KETILOG::ERRORLOG(LOGTAG,"Snippet_Projection_SelectType_COLUMNNAME1 => check plz..");
                            }
                        }
                        colData.type = t.type;
                    }else{//로우 개수만큼
                        for (int r = 0; r < group_row_num; r++){
                            t = Projection(g,p,r);//p번째 프로젝션을 g번째 그룹의 r번째 로우에 적용
                            switch(t.type){
                                case TYPE_STRING:{
                                    colData.strvec.push_back(t.varString);
                                    colData.isnull.push_back(false);
                                    colData.row_count++;
                                    break;
                                }case TYPE_INT:{    
                                    colData.intvec.push_back(t.varInt);
                                    colData.isnull.push_back(false);
                                    colData.row_count++;
                                    break;
                                }case TYPE_FLOAT:{
                                    colData.floatvec.push_back(t.varFloat);
                                    colData.isnull.push_back(false);
                                    colData.row_count++;
                                    break;
                                }default:{
                                    KETILOG::ERRORLOG(LOGTAG,"Snippet_Projection_SelectType_COLUMNNAME2 => check plz..");
                                }
                            }
                            colData.type = t.type;
                        }
                    }
                    break;
                }case StorageEngineInstance::Snippet_Projection_SelectType_SUM:{
                    T t, t_;
                    for (int r = 0; r < group_row_num; r++){
                        t_ = Projection(g,p,r);
                        switch(t_.type){
                            case TYPE_INT:{    
                                t.varInt += t_.varInt;
                                break;
                            }case TYPE_FLOAT:{
                                t.varFloat += t_.varFloat;
                                break;
                            }default:{
                                KETILOG::ERRORLOG(LOGTAG,"MergeQueryManager::Snippet_Projection_SelectType_SUM1 => check plz..");
                            }
                        }
                    }
                    t.type = t_.type;
                    switch(t.type){
                        case TYPE_INT:{    
                            colData.intvec.push_back(t.varInt);
                            colData.isnull.push_back(false);
                            colData.row_count++;
                            colData.type = t.type;
                            break;
                        }case TYPE_FLOAT:{
                            colData.floatvec.push_back(t.varFloat);
                            colData.isnull.push_back(false);
                            colData.row_count++;
                            colData.type = t.type;
                            break;
                        }default:{
                            KETILOG::ERRORLOG(LOGTAG,"MergeQueryManager::Snippet_Projection_SelectType_SUM2 => check plz..");
                        }
                    }
                    break;
                }case StorageEngineInstance::Snippet_Projection_SelectType_AVG:{
                    T t, t_;
                    for (int r = 0; r < group_row_num; r++){
                        t_ = Projection(g,p,r);
                        switch(t_.type){
                            case TYPE_INT:{    
                                t.varInt += t_.varInt;
                                break;
                            }case TYPE_FLOAT:{
                                t.varFloat += t_.varFloat;
                                break;
                            }default:{
                                KETILOG::ERRORLOG(LOGTAG,"MergeQueryManager::Snippet_Projection_SelectType_AVG1 => check plz..");
                            }
                        }
                    }
                    t.type = t_.type;
                    float avg;
                    switch(t.type){
                        case TYPE_INT:{    
                            avg = t.varInt / group_row_num;
                            break;
                        }case TYPE_FLOAT:{
                            avg = t.varFloat / group_row_num;
                            break;
                        }default:{
                            KETILOG::ERRORLOG(LOGTAG,"MergeQueryManager::Snippet_Projection_SelectType_AVG2 => check plz..");
                        }
                    }
                    colData.floatvec.push_back(avg);
                    colData.isnull.push_back(false);
                    colData.row_count++;
                    colData.type = TYPE_FLOAT;
                    break;
                }case StorageEngineInstance::Snippet_Projection_SelectType_COUNT:{//해당 컬럼의 null 데이터 제외 로우 개수 카운트
                    vector<bool> is_column_null = group_by_table[g].table_data[snippet.column_projection(p).value(0)].isnull;
                    int count = std::count(is_column_null.begin(), is_column_null.end(), false);
                    colData.intvec.push_back(count);
                    colData.isnull.push_back(false);
                    colData.row_count++;
                    colData.type = TYPE_INT;
                    break;
                }case StorageEngineInstance::Snippet_Projection_SelectType_COUNTSTAR:{//로우 개수 카운트
                    colData.intvec.push_back(group_row_num);
                    colData.isnull.push_back(false);
                    colData.row_count++;
                    colData.type = TYPE_INT;
                    break;
                }case StorageEngineInstance::Snippet_Projection_SelectType_COUNTDISTINCT:{
                    int distinct_row_count = 0;
                    switch(group_by_table[g].table_data[snippet.column_projection(p).value(0)].type){
                        case TYPE_INT:{    
                            std::set<int> uniqueElements( group_by_table[g].table_data[snippet.column_projection(p).value(0)].intvec.begin(), group_by_table[g].table_data[snippet.column_projection(p).value(0)].intvec.end());
                            distinct_row_count = uniqueElements.size();
                            break;
                        }case TYPE_FLOAT:{
                            std::set<float> uniqueElements( group_by_table[g].table_data[snippet.column_projection(p).value(0)].floatvec.begin(), group_by_table[g].table_data[snippet.column_projection(p).value(0)].floatvec.end());
                            distinct_row_count = uniqueElements.size();
                            break;
                        }case TYPE_STRING:{
                            std::set<string> uniqueElements( group_by_table[g].table_data[snippet.column_projection(p).value(0)].strvec.begin(), group_by_table[g].table_data[snippet.column_projection(p).value(0)].strvec.end());
                            distinct_row_count = uniqueElements.size();
                            break;
                        }default:{
                            KETILOG::ERRORLOG(LOGTAG,"MergeQueryManager::Snippet_Projection_SelectType_COUNTDISTINCT => check plz..");
                        }
                    }
                    colData.intvec.push_back(distinct_row_count);
                    colData.isnull.push_back(false);
                    colData.row_count++;
                    colData.type = TYPE_INT;
                    break;
                }case StorageEngineInstance::Snippet_Projection_SelectType_TOP:{
                    KETILOG::ERRORLOG(LOGTAG,"MergeQueryManager::Snippet_Projection_SelectType_TOP => check plz.. ");
                    break;
                }case StorageEngineInstance::Snippet_Projection_SelectType_MIN:{
                    T t, t_;
                    t_ = Projection(g,p,0);//맨 처음 로우를 최소값으로 저장
                    if(t_.type == TYPE_INT){
                        t.varInt = t_.varInt;
                    }else if(t_.type == TYPE_FLOAT){
                        t.varFloat = t_.varFloat;
                    }
                    for (int r = 1; r < group_row_num; r++){
                        t_ = Projection(g,p,r);
                        switch(t.type){
                            case TYPE_INT:{    
                                t.varInt = (t.varInt < t_.varInt) ? t.varInt : t_.varInt;
                                break;
                            }case TYPE_FLOAT:{
                                t.varFloat = (t.varFloat < t_.varFloat) ? t.varFloat : t_.varFloat;
                                break;
                            }case TYPE_STRING:{
                                t.varString = (t.varString < t_.varString) ? t.varString : t_.varString;
                                break;
                            }default:{
                                KETILOG::ERRORLOG(LOGTAG,"MergeQueryManager::Snippet_Projection_SelectType_MIN1 => check plz..");
                            }
                        }
                    }
                    switch(t.type){
                        case TYPE_INT:{    
                            colData.intvec.push_back(t.varInt);
                            colData.isnull.push_back(false);
                            colData.row_count++;
                            break;
                        }case TYPE_FLOAT:{
                            colData.floatvec.push_back(t.varFloat);
                            colData.isnull.push_back(false);
                            colData.row_count++;
                            break;
                        }case TYPE_STRING:{
                            colData.strvec.push_back(t.varString);
                            colData.isnull.push_back(false);
                            colData.row_count++;
                            break;
                        }default:{
                            KETILOG::ERRORLOG(LOGTAG,"MergeQueryManager::Snippet_Projection_SelectType_MIN2 => check plz..");
                        }
                    }
                    colData.type = t_.type;
                    break;
                }case StorageEngineInstance::Snippet_Projection_SelectType_MAX:{
                    T t, t_;
                    t_ = Projection(g,p,0);//맨 처음 로우를 최대값으로 저장
                    if(t_.type == TYPE_INT){
                        t.varInt = t_.varInt;
                        t.type = TYPE_INT;
                    }else if(t_.type == TYPE_FLOAT){
                        t.varFloat = t_.varFloat;
                        t.type = TYPE_FLOAT;
                    }else{
                        KETILOG::ERRORLOG(LOGTAG,"MergeQueryManager::Snippet_Projection_SelectType_MAX0 => check plz..");
                    }
                    for (int r = 1; r < group_row_num; r++){
                        t_ = Projection(g,p,r);
                        switch(t.type){
                            case TYPE_INT:{    
                                t.varInt = (t.varInt > t_.varInt) ? t.varInt : t_.varInt;
                                break;
                            }case TYPE_FLOAT:{
                                t.varFloat = (t.varFloat > t_.varFloat) ? t.varFloat : t_.varFloat;
                                break;
                            }case TYPE_STRING:{
                                t.varString = (t.varString > t_.varString) ? t.varString : t_.varString;
                                break;
                            }default:{
                                KETILOG::ERRORLOG(LOGTAG,"MergeQueryManager::Snippet_Projection_SelectType_MAX1 => check plz..");
                            }
                        }
                    }
                    switch(t.type){
                        case TYPE_INT:{    
                            colData.intvec.push_back(t.varInt);
                            colData.isnull.push_back(false);
                            colData.row_count++;
                            break;
                        }case TYPE_FLOAT:{
                           colData.floatvec.push_back(t.varFloat);
                            colData.isnull.push_back(false);
                            colData.row_count++;
                            break;
                        }case TYPE_STRING:{
                            colData.strvec.push_back(t.varString);
                            colData.isnull.push_back(false);
                            colData.row_count++;
                            break;
                        }default:{
                            KETILOG::ERRORLOG(LOGTAG,"MergeQueryManager::Snippet_Projection_SelectType_MAX2 => check plz..");
                        }
                    }
                    colData.type = t_.type;
                    break;
                }
            }
        }
        result_table.table_data.insert({snippet.column_alias(p),colData});//한 프로젝션 컬럼 저장
        if(result_row_num == TYPE_EMPTY){//첫번째 컬럼을 처리하고 저장
            result_row_num = colData.row_count;
        }
    }
    result_table.row_count = result_row_num;
}

T MergeQueryManager::Projection(int groupIndex, int projectionIndex, int rowIndex){
    T postfixResult;
    StorageEngineInstance::Snippet_Projection projection = snippet.column_projection(projectionIndex);
    string first_operator = projection.value(0);

    if(first_operator == "CASE"){ //CASE WHEN THEN
        int case_len = projection.value_size();
        vector<pair<int,int>> when_then_offset;
        int else_offset, when_offset, then_offset = 0;
        bool passed = false;

        for(int i = 1; i<case_len; i++){
            if(projection.value(i) == "WHEN"){
                when_offset = i;
            }else if(projection.value(i) == "THEN"){
                then_offset = i;
                when_then_offset.push_back({when_offset, then_offset});
            }else if(projection.value(i) == "ELSE"){
                else_offset = i;
            }
        }

        T t;
        vector<pair<int,int>>::iterator iter;
        for (iter = when_then_offset.begin(); iter != when_then_offset.end(); ++iter){
            when_offset = (*iter).first;
            then_offset = (*iter).second;
            int offset = when_offset + 1;
            int start = offset;
            
            while(offset < then_offset + 1){
                if(projection.value(offset) == "AND"){
                    t = Postfix(groupIndex, projectionIndex, rowIndex, start, offset);
                    if(t.boolean){
                        offset++;
                        start = offset;
                    }else{
                        passed = false;
                        break;
                    }
                }else if(projection.value(offset) == "OR"){
                    t = Postfix(groupIndex, projectionIndex, rowIndex, start, offset);
                    if(t.boolean){
                        passed = true;
                        break;
                    }else{
                        offset++;
                        start = offset;
                    } 
                }else if(projection.value(offset) == "THEN"){
                    t = Postfix(groupIndex, projectionIndex, rowIndex, start, offset);
                    if(t.boolean){
                        passed = true;
                        break;
                    }else{
                        offset++;
                    }
                }else{
                    offset++;
                }
            }

            offset = then_offset + 1;

            if(passed){//THEN
                start = offset;
                while(true){
                    if(projection.value(offset) == "WHEN" || 
                        projection.value(offset) == "ELSE" || 
                        projection.value(offset) == "END"){
                        break;
                    }
                    offset++;
                }
                t = Postfix(groupIndex, projectionIndex, rowIndex, start, offset);

                switch (t.type){
                case TYPE_INT:
                    postfixResult.varInt = t.varInt;
                    break;
                case TYPE_STRING:
                    postfixResult.varString =t.varString;
                    break;
                case TYPE_FLOAT:
                    postfixResult.varFloat = t.varFloat;
                    break;
                default:
                    KETILOG::ERRORLOG(LOGTAG,"else type check plz.. "+to_string(t.type));
                    break;
                }
                postfixResult.isnull = false;
                postfixResult.type = t.type;

                break;
            }
        }

        if(!passed){//ELSE
            t = Postfix(groupIndex, projectionIndex, rowIndex, else_offset+1, projection.value_size()-1);

            switch (t.type){
            case TYPE_INT:
                postfixResult.varInt = t.varInt;
                break;
            case TYPE_STRING:
                postfixResult.varString =t.varString;
                break;
            case TYPE_FLOAT:
                postfixResult.varFloat = t.varFloat;
                break;
            default:
                KETILOG::ERRORLOG(LOGTAG,"else type check plz.. "+to_string(t.type));
                break;
            }
            postfixResult.isnull = false;
            postfixResult.type = t.type;
        }
    }else if(first_operator == "SUBSTRING"){//["SUBSTRING",col_name,start,length],
        string src = projection.value(1);
        int type = projection.value_type(1);
        int start = stoi(projection.value(2));
        int length = stoi(projection.value(3));
        string value;

        if(type == StorageEngineInstance::Snippet_ValueType_COLUMN){
            value = group_by_table[groupIndex].table_data[src].strvec[rowIndex];
        }else if(type == StorageEngineInstance::Snippet_ValueType_STRING){
            value = src;
        }else{
            KETILOG::ERRORLOG(LOGTAG,"SUBSTRING => check plz..");
        }

        postfixResult.varString = value.substr(start,length);;
        postfixResult.isnull = false;
        postfixResult.type = TYPE_STRING;
    }else if(first_operator == "EXTRACT"){//["EXTRACT","YEAR",col_name]
        string unit = projection.value(1);
        string col = projection.value(2);
        int type = projection.value_type(2);
        int value, result;

        if(type == StorageEngineInstance::Snippet_ValueType_COLUMN){
            value = group_by_table[groupIndex].table_data[col].intvec[rowIndex];
        }else{
            KETILOG::ERRORLOG(LOGTAG,"Extract1 => check plz..");
        }

        if(unit == "YEAR"){
            result = value / 512;
        }else if(unit == "MONTH"){
            value %= 512;
            result = value / 32;
        }else if(unit == "DAY"){
            value = value % 512;
            value %= 32;
            result = value;
        }else{
            KETILOG::ERRORLOG(LOGTAG,"Extract2 => check plz..");
        }

        postfixResult.varInt = result;
        postfixResult.isnull = false;
        postfixResult.type = TYPE_INT;
    }else{ //POSTFIX
        postfixResult = Postfix(groupIndex, projectionIndex, rowIndex, 0, projection.value_size());
    }
    return postfixResult;
}

T MergeQueryManager::Postfix(int groupIndex, int projectionIndex, int rowIndex, int start, int end){
    T postfixResult;
    StorageEngineInstance::Snippet_Projection projection = snippet.column_projection(projectionIndex);
    string first_operator = projection.value(0);

    stack<T> oper_stack;
    for (int i = start; i < end; i++){
        T t;
        switch(projection.value_type(i)){
            case StorageEngineInstance::Snippet_ValueType_COLUMN:{
                if(group_by_table[groupIndex].table_data[projection.value(i)].type == TYPE_STRING){
                    t.varString = group_by_table[groupIndex].table_data[projection.value(i)].strvec[rowIndex];
                    t.type = TYPE_STRING;
                    oper_stack.push(t);
                }else if(group_by_table[groupIndex].table_data[projection.value(i)].type == TYPE_INT){
                    t.varInt = group_by_table[groupIndex].table_data[projection.value(i)].intvec[rowIndex];
                    t.type = TYPE_INT;
                    oper_stack.push(t);
                }else if(group_by_table[groupIndex].table_data[projection.value(i)].type == TYPE_FLOAT){
                    t.varFloat = group_by_table[groupIndex].table_data[projection.value(i)].floatvec[rowIndex];
                    t.type = TYPE_FLOAT;
                    oper_stack.push(t);
                }else{
                    cout << groupIndex << " " << projection.value(i) << " " << group_by_table[groupIndex].table_data[projection.value(i)].row_count << endl;
                    KETILOG::ERRORLOG(LOGTAG,"Postfix type check plz... (a) " + to_string(group_by_table[groupIndex].table_data[projection.value(i)].type));
                }
                break;
            }case StorageEngineInstance::Snippet_ValueType_OPERATOR:{
                if(projection.value(i) == "+"){
                    T oper2 = oper_stack.top();
                    oper_stack.pop();
                    T oper1 = oper_stack.top();
                    oper_stack.pop();
                    if(oper1.type == TYPE_INT){
                        t.varInt = oper1.varInt + oper2.varInt;
                        t.type = TYPE_INT;
                        oper_stack.push(t);
                    }else if(oper1.type == TYPE_FLOAT){
                        t.varFloat = oper1.varFloat + oper2.varFloat;
                        t.type = TYPE_FLOAT;
                        oper_stack.push(t);
                    }else{
                        KETILOG::ERRORLOG(LOGTAG,"Postfix type check plz... (b)");
                    }
                }else if(projection.value(i) == "-"){
                    T oper2 = oper_stack.top();
                    oper_stack.pop();
                    T oper1 = oper_stack.top();
                    oper_stack.pop();
                    if(oper1.type == TYPE_INT){
                        if (oper2.type == TYPE_INT){
                            t.varInt = oper1.varInt - oper2.varInt;
                            t.type = TYPE_INT;
                            oper_stack.push(t);
                        }else if(oper2.type == TYPE_FLOAT){
                            t.varFloat = oper1.varInt - oper2.varFloat;
                            t.type = TYPE_FLOAT;
                            oper_stack.push(t);
                        }else{
                            KETILOG::ERRORLOG(LOGTAG,"Postfix type check plz... (c)");
                        }
                    }else if(oper1.type == TYPE_FLOAT){
                        if (oper2.type == TYPE_INT){
                            t.varInt = oper1.varFloat - oper2.varInt;
                            t.type = TYPE_FLOAT;
                            oper_stack.push(t);
                        }else if(oper2.type == TYPE_FLOAT){
                            t.varFloat = oper1.varFloat - oper2.varFloat;
                            t.type = TYPE_FLOAT;
                            oper_stack.push(t);
                        }else{
                            KETILOG::ERRORLOG(LOGTAG,"Postfix type check plz... (d)");
                        }
                    }
                }else if(projection.value(i) == "*"){
                    T oper2 = oper_stack.top();
                    oper_stack.pop();
                    T oper1 = oper_stack.top();
                    oper_stack.pop();
                    if(oper1.type == TYPE_INT){
                        if (oper2.type == TYPE_INT){
                            t.varInt = oper1.varInt * oper2.varInt;
                            t.type = TYPE_INT;
                            oper_stack.push(t);
                        }else if(oper2.type == TYPE_FLOAT){
                            t.varFloat = oper1.varInt * oper2.varFloat;
                            t.type = TYPE_FLOAT;
                            oper_stack.push(t);
                        }else{
                            KETILOG::ERRORLOG(LOGTAG,"Postfix type check plz... (e)");
                        }
                    }else if(oper1.type == TYPE_FLOAT){
                        if (oper2.type == TYPE_INT){
                            t.varInt = oper1.varFloat * oper2.varInt;
                            t.type = TYPE_FLOAT;
                            oper_stack.push(t);
                        }else if(oper2.type == TYPE_FLOAT){
                            t.varFloat = oper1.varFloat * oper2.varFloat;
                            t.type = TYPE_FLOAT;
                            oper_stack.push(t);
                        }else{
                            KETILOG::ERRORLOG(LOGTAG,"Postfix type check plz... (f)");
                        }
                    }
                }else if(projection.value(i) == "/"){
                    T oper2 = oper_stack.top();
                    oper_stack.pop();
                    T oper1 = oper_stack.top();
                    oper_stack.pop();
                    if(oper1.type == TYPE_INT){
                        if (oper2.type == TYPE_INT){
                            t.varInt = oper1.varInt / oper2.varInt;
                            t.type = TYPE_INT;
                            oper_stack.push(t);
                        }else if(oper2.type == TYPE_FLOAT){
                            t.varFloat = oper1.varInt / oper2.varFloat;
                            t.type = TYPE_FLOAT;
                            oper_stack.push(t);
                        }else{
                            KETILOG::ERRORLOG(LOGTAG,"Postfix type check plz... (g)");
                        }
                    }else if(oper1.type == TYPE_FLOAT){
                        if (oper2.type == TYPE_INT){
                            t.varInt = oper1.varFloat / oper2.varInt;
                            t.type = TYPE_FLOAT;
                            oper_stack.push(t);
                        }else if(oper2.type == TYPE_FLOAT){
                            t.varFloat = oper1.varFloat / oper2.varFloat;
                            t.type = TYPE_FLOAT;
                            oper_stack.push(t);
                        }else{
                            KETILOG::ERRORLOG(LOGTAG,"Postfix type check plz... (h)");
                        }
                    }else{
                        KETILOG::ERRORLOG(LOGTAG,"Snippet_ValueType_OPERATOR '<>' => check plz..");
                    }
                }else if(projection.value(i) == "="){
                    T oper2 = oper_stack.top();
                    oper_stack.pop();
                    T oper1 = oper_stack.top();
                    oper_stack.pop();
                    if(oper1.type == TYPE_STRING && oper2.type == TYPE_STRING){
                        t.boolean = (oper1.type == oper2.type);
                    }else{
                        KETILOG::ERRORLOG(LOGTAG,"Snippet_ValueType_OPERATOR '=' => check plz..");
                    }
                    oper_stack.push(t);
                }else if(projection.value(i) == "<>"){
                    T oper2 = oper_stack.top();
                    oper_stack.pop();
                    T oper1 = oper_stack.top();
                    oper_stack.pop();
                    if(oper1.type == TYPE_STRING && oper2.type == TYPE_STRING){
                        t.boolean = (oper1.type != oper2.type);
                    }else{
                        KETILOG::ERRORLOG(LOGTAG,"Snippet_ValueType_OPERATOR '<>' => check plz..");
                    }
                    oper_stack.push(t);
                }else if(projection.value(i) == "LIKE"){//%A%B%... 케이스 모두 체크 필요
                    T oper2 = oper_stack.top();
                    oper_stack.pop();
                    T oper1 = oper_stack.top();
                    oper_stack.pop();
                    if(oper1.type == TYPE_STRING && oper2.type == TYPE_STRING){
                        string col = oper1.varString;
                        string str = oper2.varString;
                        if(str.substr(0,1) == "%" && str.substr(str.length()-1) == "%"){//'%str%'
                            string find_str = str.substr(1,str.length()-2);
                            t.boolean = (col.find(find_str) != string::npos) ? true : false;
                        }else if(str.substr(0,1) == "%"){//'%str'
                            string find_str = str.substr(1);
                            string comp = col.substr(col.length()-find_str.length());
                            t.boolean = (find_str == comp) ? true : false;
                        }else if(str.substr(str.length()-1) == "%"){//'str%'
                            string find_str = str.substr(0,str.length()-1);
                            string comp = col.substr(0,find_str.length());
                            t.boolean = (find_str == comp) ? true : false;
                        }else{
                            t.boolean = (col == str) ? true : false;
                        }
                        oper_stack.push(t);
                    }else{
                        KETILOG::ERRORLOG(LOGTAG,"Snippet_ValueType_OPERATOR 'LIKE' => check plz..");
                    }
                }else{
                    KETILOG::ERRORLOG(LOGTAG,"Projection => check plz..");
                }
                break;
            }case StorageEngineInstance::Snippet_ValueType_FLOAT32:
             case StorageEngineInstance::Snippet_ValueType_FLOAT64:
             case StorageEngineInstance::Snippet_ValueType_NUMERIC:{
                t.varFloat = stod(projection.value(i));
                t.type = TYPE_FLOAT;
                oper_stack.push(t);
                break;
            }case StorageEngineInstance::Snippet::INT16:
             case StorageEngineInstance::Snippet::INT32:
             case StorageEngineInstance::Snippet::INT64:{
                t.varInt = stoi(projection.value(i));
                t.type = TYPE_INT;
                oper_stack.push(t);
                break;
            }case StorageEngineInstance::Snippet_ValueType_STRING:{
                t.varString = projection.value(i);
                t.type = TYPE_STRING;
                oper_stack.push(t);
                break;
            }default:{
                KETILOG::ERRORLOG(LOGTAG,"projection defualt -> check plz..");
            }
        }
    }
    postfixResult = oper_stack.top();
    oper_stack.pop();
    
    return postfixResult;
}

void MergeQueryManager::GroupBy(){
    int increase_index = 0;
    int target_row_num = target_table.row_count;

    if(target_row_num == 0){
        group_by_table.push_back(target_table);
    }else{
        for(int r=0; r<target_row_num; r++){
            string key = makeGroupbyKey(r);
            int group_by_index;

            if (group_by_key.find(key) == group_by_key.end()) {//group not exist
                group_by_index = increase_index;
                group_by_key.insert({key,group_by_index});
                increase_index++;
                TableData new_table;
                group_by_table.push_back(new_table);
            }else{//group exist
                group_by_index = group_by_key[key];
            }
            
            for(auto& col: target_table.table_data){
                string col_name = col.first;
                switch(col.second.type){
                    case TYPE_INT:
                        group_by_table[group_by_index].table_data[col_name].intvec.push_back(target_table.table_data[col_name].intvec[r]);
                        break;
                    case TYPE_STRING:
                        group_by_table[group_by_index].table_data[col_name].strvec.push_back(target_table.table_data[col_name].strvec[r]);
                        break;
                    case TYPE_FLOAT:
                        group_by_table[group_by_index].table_data[col_name].floatvec.push_back(target_table.table_data[col_name].floatvec[r]);
                        break;
                    default:
                    KETILOG::ERRORLOG(LOGTAG,"group by type check plz.. "+col_name);
                        break;
                }
                group_by_table[group_by_index].table_data[col_name].isnull.push_back(target_table.table_data[col_name].isnull[r]);
                group_by_table[group_by_index].table_data[col_name].row_count++;
            }
            group_by_table[group_by_index].row_count++;
        }

        for(auto& col: target_table.table_data){
            string col_name = col.first;
            for(int i=0; i<increase_index; i++){
                group_by_table[i].table_data[col_name].type = col.second.type;
            }
        }
    }
}

void MergeQueryManager::OrderBy(){
    if(result_table.row_count == 0){
        order_by_table = result_table;
        return;
    }

    for(int i=0; i<result_table.row_count; i++){
        ordered_index.push_back(i);
    }

    sort(ordered_index.begin(), ordered_index.end(), [&](int i, int j) {
        for(int c=0; c<snippet.order_by().column_name_size(); c++){
            if(snippet.order_by().ascending(c) == Snippet_Order_OrderDirection_ASC){
                switch(result_table.table_data[snippet.order_by().column_name(c)].type){
                    case TYPE_INT:
                        if(result_table.table_data[snippet.order_by().column_name(c)].intvec[i] != result_table.table_data[snippet.order_by().column_name(c)].intvec[j]){
                            return result_table.table_data[snippet.order_by().column_name(c)].intvec[i] < result_table.table_data[snippet.order_by().column_name(c)].intvec[j];
                        }
                        break;
                    case TYPE_FLOAT:
                        if(result_table.table_data[snippet.order_by().column_name(c)].floatvec[i] != result_table.table_data[snippet.order_by().column_name(c)].floatvec[j]){
                            return result_table.table_data[snippet.order_by().column_name(c)].floatvec[i] < result_table.table_data[snippet.order_by().column_name(c)].floatvec[j];
                        }
                        break;
                    case TYPE_STRING:
                        if(result_table.table_data[snippet.order_by().column_name(c)].strvec[i] != result_table.table_data[snippet.order_by().column_name(c)].strvec[j]){
                            return result_table.table_data[snippet.order_by().column_name(c)].strvec[i] < result_table.table_data[snippet.order_by().column_name(c)].strvec[j];
                        }
                        break;
                    default:
                        KETILOG::ERRORLOG(LOGTAG,"order by sort asc default");
                        return true;
                }
            }else{//Snippet_Order_OrderDirection_DESC
                switch(result_table.table_data[snippet.order_by().column_name(c)].type){
                    case TYPE_INT:
                        if(result_table.table_data[snippet.order_by().column_name(c)].intvec[i] != result_table.table_data[snippet.order_by().column_name(c)].intvec[j]){
                            return result_table.table_data[snippet.order_by().column_name(c)].intvec[i] > result_table.table_data[snippet.order_by().column_name(c)].intvec[j];
                        }
                        break;
                    case TYPE_FLOAT:
                        if(result_table.table_data[snippet.order_by().column_name(c)].floatvec[i] != result_table.table_data[snippet.order_by().column_name(c)].floatvec[j]){
                            return result_table.table_data[snippet.order_by().column_name(c)].floatvec[i] > result_table.table_data[snippet.order_by().column_name(c)].floatvec[j];
                        }
                        break;
                    case TYPE_STRING:
                        if(result_table.table_data[snippet.order_by().column_name(c)].strvec[i] != result_table.table_data[snippet.order_by().column_name(c)].strvec[j]){
                            return result_table.table_data[snippet.order_by().column_name(c)].strvec[i] > result_table.table_data[snippet.order_by().column_name(c)].strvec[j];
                        }
                        break;
                    default:
                        KETILOG::ERRORLOG(LOGTAG,"order by sort desc default");
                        return true;
                }
            }
        }
    });

    
    if(KETILOG::IsLogLevelUnder(TRACE)){
        // 정렬 인덱스 확인 - Debug Code   
        cout << "<check ordered index> ";
        for(int i=0; i<ordered_index.size(); i++){
            cout << ordered_index[i] << " ";
        }
        cout << endl;
    }

    for(auto table: result_table.table_data){
        switch(table.second.type){
            case TYPE_INT:
                for(int i=0; i<ordered_index.size(); i++){
                    order_by_table.table_data[table.first].intvec.push_back(table.second.intvec[ordered_index[i]]);
                    order_by_table.table_data[table.first].isnull.push_back(table.second.isnull[ordered_index[i]]);
                }
                order_by_table.table_data[table.first].type = TYPE_INT;
                break;
            case TYPE_FLOAT:
                for(int i=0; i<ordered_index.size(); i++){
                    order_by_table.table_data[table.first].floatvec.push_back(table.second.floatvec[ordered_index[i]]);
                    order_by_table.table_data[table.first].isnull.push_back(table.second.isnull[ordered_index[i]]);
                }
                order_by_table.table_data[table.first].type = TYPE_FLOAT;
                break;
            case TYPE_STRING:
                for(int i=0; i<ordered_index.size(); i++){
                    order_by_table.table_data[table.first].strvec.push_back(table.second.strvec[ordered_index[i]]);
                    order_by_table.table_data[table.first].isnull.push_back(table.second.isnull[ordered_index[i]]);
                }
                order_by_table.table_data[table.first].type = TYPE_STRING;
                break;
            default:
                KETILOG::ERRORLOG(LOGTAG,"order by type error check plz..");
        }
        order_by_table.table_data[table.first].row_count = table.second.row_count;
    }
    order_by_table.row_count = result_table.row_count;
}

void MergeQueryManager::Filtering(){
    for (auto it = left_table.table_data.begin(); it != left_table.table_data.end(); it++){
        target_table.table_data[(*it).first].type = (*it).second.type;
    }

    for(int r=0; r<left_table.row_count; r++){
        bool passed = true;

        for(int f=0; f<snippet.table_filter_size(); f++){
            if(f%2 == 1){
                if(snippet.table_filter(f).operator_() == Snippet_Filter_OperType::Snippet_Filter_OperType_KETI_AND){
                    if(passed == false) break;
                }else{
                    KETILOG::ERRORLOG(LOGTAG,"Filtering: " + snippet.table_filter(f).operator_());
                }
            }else{
                string left_col = snippet.table_filter(f).lv().value(0);

                switch(left_table.table_data[left_col].type){
                case TYPE_STRING:{
                    string lv = trim(left_table.table_data[left_col].strvec[r]);
                    string rv = snippet.table_filter(f).rv().value(0);
                    passed = compareByOperator(snippet.table_filter(f).operator_(), lv, rv);
                    break;
                }case TYPE_FLOAT:{
                    float lv = left_table.table_data[left_col].floatvec[r];
                    float rv = stof(snippet.table_filter(f).rv().value(0));
                    passed = compareByOperator(snippet.table_filter(f).operator_(), lv, rv);
                    break;
                }case TYPE_INT:{
                    int lv = left_table.table_data[left_col].intvec[r];
                    int rv = stoi(snippet.table_filter(f).rv().value(0));
                    passed = compareByOperator(snippet.table_filter(f).operator_(), lv, rv);
                    break;
                }default:
                    KETILOG::ERRORLOG(LOGTAG,"Filtering (d)");
                }
            }
        }

        if(passed){ 
            for (auto it = left_table.table_data.begin(); it != left_table.table_data.end(); it++){
                switch((*it).second.type){
                case TYPE_INT:
                    target_table.table_data[(*it).first].intvec.push_back((*it).second.intvec[r]);
                    break;
                case TYPE_STRING:
                    target_table.table_data[(*it).first].strvec.push_back((*it).second.strvec[r]);
                    break;
                case TYPE_FLOAT:
                    target_table.table_data[(*it).first].floatvec.push_back((*it).second.floatvec[r]);
                    break;
                }
                target_table.table_data[(*it).first].isnull.push_back((*it).second.isnull[r]);
                target_table.table_data[(*it).first].row_count++;
            }
            target_table.row_count++;
        }
    }
}

void MergeQueryManager::InnerJoin_hash(){
    for (auto it = left_table.table_data.begin(); it != left_table.table_data.end(); it++){
        target_table.table_data[(*it).first].type = (*it).second.type;
    }
    for (auto it = right_table.table_data.begin(); it != right_table.table_data.end(); it++){
        target_table.table_data[(*it).first].type = (*it).second.type;
    }

    vector<int> equal_join_index;//join조건 중 equal 조인의 인덱스
    for(int f = 0; f < snippet.table_filter_size(); f++){
        if(snippet.table_filter(f).operator_() == Snippet_Filter_OperType::Snippet_Filter_OperType_KETI_ET){
            equal_join_index.push_back(f);
        }
    }

    right_is_smaller = (left_table.row_count >= right_table.row_count)? true : false;
    equal_join_exist = (equal_join_index.size() != 0)? true : false;

    if(equal_join_exist){ //hash join::equal filter + (nested loop join::non equal filter)
        createHashTable(equal_join_index);
        
        if(right_is_smaller){//right table로 hash table 만듦

            for(int r=0; r<left_table.row_count; r++){
                string hash_key = "";

                for(int f = 0; f < equal_join_index.size(); f++){
                    int index = equal_join_index[f];
                    string hash_key_column = snippet.table_filter(index).lv().value(0);

                    switch(left_table.table_data[hash_key_column].type){
                    case TYPE_STRING:
                        hash_key += "|" + left_table.table_data[hash_key_column].strvec[r];
                        break;
                    case TYPE_INT:
                        hash_key += "|" + to_string(left_table.table_data[hash_key_column].intvec[r]);
                        break;
                    case TYPE_FLOAT:
                        hash_key += "|" + to_string(left_table.table_data[hash_key_column].floatvec[r]);
                        break;
                    }   
                }

                for(int i = 0; i < hash_table[hash_key].size(); i++){
                    bool passed = true;
                    for(int f = 0; f < snippet.table_filter_size(); f++){
                        if( snippet.table_filter(f).operator_() == Snippet_Filter_OperType::Snippet_Filter_OperType_KETI_GE ||
                            snippet.table_filter(f).operator_() == Snippet_Filter_OperType::Snippet_Filter_OperType_KETI_LE ||
                            snippet.table_filter(f).operator_() == Snippet_Filter_OperType::Snippet_Filter_OperType_KETI_GT ||
                            snippet.table_filter(f).operator_() == Snippet_Filter_OperType::Snippet_Filter_OperType_KETI_LT ||
                            snippet.table_filter(f).operator_() == Snippet_Filter_OperType::Snippet_Filter_OperType_KETI_NE ){

                            string left_column = snippet.table_filter(f).lv().value(0);
                            string right_column = snippet.table_filter(f).rv().value(0);
                            int oper = snippet.table_filter(f).operator_();

                            // passed = compareByOperator(oper, left_column, right_column, r, hash_table[hash_key][i]);
                            switch(left_table.table_data[left_column].type){
                            case TYPE_STRING:
                                passed = compareByOperator(oper, left_table.table_data[left_column].strvec[r],
                                                            right_table.table_data[right_column].strvec[hash_table[hash_key][i]]);
                                break;
                            case TYPE_INT:
                                passed = compareByOperator(oper, left_table.table_data[left_column].intvec[r],
                                                            right_table.table_data[right_column].intvec[hash_table[hash_key][i]]);
                                break;
                            case TYPE_FLOAT:
                                passed = compareByOperator(oper, left_table.table_data[left_column].floatvec[r],
                                                            right_table.table_data[right_column].floatvec[hash_table[hash_key][i]]);
                                break;
                            }

                            if(passed == false) break;
                        }else{
                            continue;
                        }
                    }

                    if(passed){
                        for (auto it = left_table.table_data.begin(); it != left_table.table_data.end(); it++){
                            switch((*it).second.type){
                            case TYPE_INT:
                                target_table.table_data[(*it).first].intvec.push_back((*it).second.intvec[r]);
                                break;
                            case TYPE_STRING:
                                target_table.table_data[(*it).first].strvec.push_back((*it).second.strvec[r]);
                                break;
                            case TYPE_FLOAT:
                                target_table.table_data[(*it).first].floatvec.push_back((*it).second.floatvec[r]);
                                break;
                            }
                            target_table.table_data[(*it).first].isnull.push_back((*it).second.isnull[r]);
                            target_table.table_data[(*it).first].row_count++;
                        }
                        for (auto it = right_table.table_data.begin(); it != right_table.table_data.end(); it++){
                            switch((*it).second.type){
                            case TYPE_INT:
                                target_table.table_data[(*it).first].intvec.push_back((*it).second.intvec[hash_table[hash_key][i]]);
                                break;
                            case TYPE_STRING:
                                target_table.table_data[(*it).first].strvec.push_back((*it).second.strvec[hash_table[hash_key][i]]);
                                break;
                            case TYPE_FLOAT:
                                target_table.table_data[(*it).first].floatvec.push_back((*it).second.floatvec[hash_table[hash_key][i]]);
                                break;
                            }
                            target_table.table_data[(*it).first].isnull.push_back((*it).second.isnull[i]);
                            target_table.table_data[(*it).first].row_count++;
                        }
                        target_table.row_count++;
                    }
                }                              
            }
            
        }else{//left table로 hash table 만듦
            for(int r = 0; r < right_table.row_count; r++){
                string hash_key = "";

                for(int f = 0; f < equal_join_index.size(); f++){
                    int index = equal_join_index[f];
                    string hash_key_column = snippet.table_filter(index).rv().value(0);

                    switch(right_table.table_data[hash_key_column].type){
                    case TYPE_STRING:
                        hash_key += "|" + right_table.table_data[hash_key_column].strvec[r];
                        break;
                    case TYPE_INT:
                        hash_key += "|" + to_string(right_table.table_data[hash_key_column].intvec[r]);
                        break;
                    case TYPE_FLOAT:
                        hash_key += "|" + to_string(right_table.table_data[hash_key_column].floatvec[r]);
                        break;
                    }   
                }

                for(int i = 0; i < hash_table[hash_key].size(); i++){
                    bool passed = true;
                    for(int f = 0; f < snippet.table_filter_size(); f++){
                        if(snippet.table_filter(f).operator_() == Snippet_Filter_OperType::Snippet_Filter_OperType_KETI_GE ||
                            snippet.table_filter(f).operator_() == Snippet_Filter_OperType::Snippet_Filter_OperType_KETI_LE ||
                            snippet.table_filter(f).operator_() == Snippet_Filter_OperType::Snippet_Filter_OperType_KETI_GT ||
                            snippet.table_filter(f).operator_() == Snippet_Filter_OperType::Snippet_Filter_OperType_KETI_LT ||
                            snippet.table_filter(f).operator_() == Snippet_Filter_OperType::Snippet_Filter_OperType_KETI_NE){

                            string left_column = snippet.table_filter(f).lv().value(0);
                            string right_column = snippet.table_filter(f).rv().value(0);
                            int oper = snippet.table_filter(f).operator_();

                            // passed = compareByOperator(oper, left_column, right_column, hash_table[hash_key][i], r);
                            switch(left_table.table_data[left_column].type){
                            case TYPE_STRING:
                                passed = compareByOperator(oper, left_table.table_data[left_column].strvec[hash_table[hash_key][i]],
                                                            right_table.table_data[right_column].strvec[r]);
                                break;
                            case TYPE_INT:
                                passed = compareByOperator(oper, left_table.table_data[left_column].intvec[hash_table[hash_key][i]],
                                                            right_table.table_data[right_column].intvec[r]);
                                break;
                            case TYPE_FLOAT:
                                passed = compareByOperator(oper, left_table.table_data[left_column].floatvec[hash_table[hash_key][i]],
                                                            right_table.table_data[right_column].floatvec[r]);
                                break;
                            }

                            if(passed == false) break;
                        }else{
                            continue;
                        }
                    }

                    if(passed){ 
                        for (auto it = right_table.table_data.begin(); it != right_table.table_data.end(); it++){
                            switch((*it).second.type){
                            case TYPE_INT:
                                target_table.table_data[(*it).first].intvec.push_back((*it).second.intvec[r]);
                                break;
                            case TYPE_STRING:
                                target_table.table_data[(*it).first].strvec.push_back((*it).second.strvec[r]);
                                break;
                            case TYPE_FLOAT:
                                target_table.table_data[(*it).first].floatvec.push_back((*it).second.floatvec[r]);
                                break;
                            }
                            target_table.table_data[(*it).first].isnull.push_back((*it).second.isnull[r]);
                            target_table.table_data[(*it).first].row_count++;
                        }
                        for (auto it = left_table.table_data.begin(); it != left_table.table_data.end(); it++){
                            switch((*it).second.type){
                            case TYPE_INT:
                                target_table.table_data[(*it).first].intvec.push_back((*it).second.intvec[hash_table[hash_key][i]]);
                                break;
                            case TYPE_STRING:
                                target_table.table_data[(*it).first].strvec.push_back((*it).second.strvec[hash_table[hash_key][i]]);
                                break;
                            case TYPE_FLOAT:
                                target_table.table_data[(*it).first].floatvec.push_back((*it).second.floatvec[hash_table[hash_key][i]]);
                                break;
                            }
                            target_table.table_data[(*it).first].isnull.push_back((*it).second.isnull[hash_table[hash_key][i]]);
                            target_table.table_data[(*it).first].row_count++;
                        }
                        target_table.row_count++;
                    }
                }
            }
        }
    }else{ // nested loop join::only non equal filter
        for(int r1=0; r1<left_table.row_count; r1++){
            for(int r2=0; r2<right_table.row_count; r2++){
                bool passed = true;

                for(int f=0; f<snippet.table_filter_size(); f++){
                    if(snippet.table_filter(f).operator_() == Snippet_Filter_OperType::Snippet_Filter_OperType_KETI_GE ||
                        snippet.table_filter(f).operator_() == Snippet_Filter_OperType::Snippet_Filter_OperType_KETI_LE ||
                        snippet.table_filter(f).operator_() == Snippet_Filter_OperType::Snippet_Filter_OperType_KETI_GT ||
                        snippet.table_filter(f).operator_() == Snippet_Filter_OperType::Snippet_Filter_OperType_KETI_LT ||
                        snippet.table_filter(f).operator_() == Snippet_Filter_OperType::Snippet_Filter_OperType_KETI_NE){

                        string left_column = snippet.table_filter(f).lv().value(0);
                        string right_column = snippet.table_filter(f).rv().value(0);
                        int oper = snippet.table_filter(f).operator_();

                        // passed = compareByOperator(snippet.table_filter(f).operator_(), left_column, right_column, r1,r2);
                        switch(left_table.table_data[left_column].type){
                        case TYPE_STRING:
                            passed = compareByOperator(oper, left_table.table_data[left_column].strvec[r1],
                                                        right_table.table_data[right_column].strvec[r2]);
                            break;
                        case TYPE_INT:
                            passed = compareByOperator(oper, left_table.table_data[left_column].intvec[r1],
                                                        right_table.table_data[right_column].intvec[r2]);
                            break;
                        case TYPE_FLOAT:
                            passed = compareByOperator(oper, left_table.table_data[left_column].floatvec[r1],
                                                        right_table.table_data[right_column].floatvec[r2]);
                            break;
                        }

                        if(passed == false) break;
                    }else{
                        continue;
                    }
                }

                if(passed){ 
                    for (auto it = left_table.table_data.begin(); it != left_table.table_data.end(); it++){
                        switch((*it).second.type){
                        case TYPE_INT:
                            target_table.table_data[(*it).first].intvec.push_back((*it).second.intvec[r1]);
                            break;
                        case TYPE_STRING:
                            target_table.table_data[(*it).first].strvec.push_back((*it).second.strvec[r1]);
                            break;
                        case TYPE_FLOAT:
                            target_table.table_data[(*it).first].floatvec.push_back((*it).second.floatvec[r1]);
                            break;
                        }
                        target_table.table_data[(*it).first].isnull.push_back((*it).second.isnull[r1]);
                        target_table.table_data[(*it).first].row_count++;
                    }
                    for (auto it = right_table.table_data.begin(); it != right_table.table_data.end(); it++){
                        switch((*it).second.type){
                        case TYPE_INT:
                            target_table.table_data[(*it).first].intvec.push_back((*it).second.intvec[r2]);
                            break;
                        case TYPE_STRING:
                            target_table.table_data[(*it).first].strvec.push_back((*it).second.strvec[r2]);
                            break;
                        case TYPE_FLOAT:
                            target_table.table_data[(*it).first].floatvec.push_back((*it).second.floatvec[r2]);
                            break;
                        }
                        target_table.table_data[(*it).first].isnull.push_back((*it).second.isnull[r2]);
                        target_table.table_data[(*it).first].row_count++;
                    }
                    target_table.row_count++;
                }
            }
        }
    }   
}

void MergeQueryManager::InnerJoin_nestedloop(){
    for (auto it = left_table.table_data.begin(); it != left_table.table_data.end(); it++){
        target_table.table_data[(*it).first].type = (*it).second.type;
    }
    for (auto it = right_table.table_data.begin(); it != right_table.table_data.end(); it++){
        target_table.table_data[(*it).first].type = (*it).second.type;
    }

    for(int r1=0; r1<left_table.row_count; r1++){

        for(int r2=0; r2<right_table.row_count; r2++){
            bool passed = false;

            for(int f=0; f<snippet.table_filter_size(); f++){//동등조인,비동등조인 여부 판단 필요
                if(f%2 == 1){
                    switch(snippet.table_filter(f).operator_())
                    case KETI_AND:{//and가 아닌경우??
                        continue;
                    }
                }else{
                    bool filter_passed = false;
                    string driving_c1 = snippet.table_filter(f).lv().value(0);
                    string driven_c2 = snippet.table_filter(f).rv().value(0);

                    switch(left_table.table_data[driving_c1].type){
                    case TYPE_INT:
                        if(left_table.table_data[driving_c1].intvec[r1] == right_table.table_data[driven_c2].intvec[r2]){
                            filter_passed = true;
                            break;
                        }
                        break;
                    case TYPE_FLOAT:
                        if(left_table.table_data[driving_c1].floatvec[r1] == right_table.table_data[driven_c2].floatvec[r2]){
                            filter_passed = true;
                            break;
                        }
                        break;
                    case TYPE_STRING:
                        if(left_table.table_data[driving_c1].strvec[r1] == right_table.table_data[driven_c2].strvec[r2]){
                            filter_passed = true;
                            break;
                        }
                        break;
                    default:
                        KETILOG::ERRORLOG(LOGTAG,"inner join type check plz... " + to_string(left_table.table_data[driving_c1].type));
                    }

                    if(filter_passed){
                        passed = true;
                        filter_passed = false;
                    }else{
                        passed = false;
                        break;
                    }
                }
            }

            if(passed){ 
                for (auto it = left_table.table_data.begin(); it != left_table.table_data.end(); it++){
                    switch((*it).second.type){
                    case TYPE_INT:
                        target_table.table_data[(*it).first].intvec.push_back((*it).second.intvec[r1]);
                        break;
                    case TYPE_STRING:
                        target_table.table_data[(*it).first].strvec.push_back((*it).second.strvec[r1]);
                        break;
                    case TYPE_FLOAT:
                        target_table.table_data[(*it).first].floatvec.push_back((*it).second.floatvec[r1]);
                        break;
                    }
                    target_table.table_data[(*it).first].isnull.push_back((*it).second.isnull[r1]);
                    target_table.table_data[(*it).first].row_count++;
                }
                for (auto it = right_table.table_data.begin(); it != right_table.table_data.end(); it++){
                    switch((*it).second.type){
                    case TYPE_INT:
                        target_table.table_data[(*it).first].intvec.push_back((*it).second.intvec[r2]);
                        break;
                    case TYPE_STRING:
                        target_table.table_data[(*it).first].strvec.push_back((*it).second.strvec[r2]);
                        break;
                    case TYPE_FLOAT:
                        target_table.table_data[(*it).first].floatvec.push_back((*it).second.floatvec[r2]);
                        break;
                    }
                    target_table.table_data[(*it).first].isnull.push_back((*it).second.isnull[r2]);
                    target_table.table_data[(*it).first].row_count++;
                }
                target_table.row_count++;
            }
        }
    }
}

void MergeQueryManager::LeftOuterJoin_hash(){
    for (auto it = left_table.table_data.begin(); it != left_table.table_data.end(); it++){
        target_table.table_data[(*it).first].type = (*it).second.type;
    }
    for (auto it = right_table.table_data.begin(); it != right_table.table_data.end(); it++){
        target_table.table_data[(*it).first].type = (*it).second.type;
    }

    vector<int> equal_join_index;//join조건 중 equal 조인의 인덱스, left outer join은 = 조건만 존재
    for(int f = 0; f < snippet.table_filter_size(); f++){
        if(snippet.table_filter(f).operator_() == Snippet_Filter_OperType::Snippet_Filter_OperType_KETI_ET){
            equal_join_index.push_back(f);
        }
    }

    right_is_smaller = true;//left outer join은 무조건 right table로 해시테이블 생성, left테이블의 모든 로우 포함
    equal_join_exist = true;//left outer join은 = 조건만 존재
 
    createHashTable(equal_join_index);
        
    for(int r=0; r<left_table.row_count; r++){
        string hash_key = "";
        bool passed = false;

        for(int f = 0; f < equal_join_index.size(); f++){
            string hash_key_column = snippet.table_filter(equal_join_index[f]).lv().value(0);

            switch(left_table.table_data[hash_key_column].type){
            case TYPE_STRING:
                hash_key += "|" + left_table.table_data[hash_key_column].strvec[r];
                break;
            case TYPE_INT:
                hash_key += "|" + to_string(left_table.table_data[hash_key_column].intvec[r]);
                break;
            case TYPE_FLOAT:
                hash_key += "|" + to_string(left_table.table_data[hash_key_column].floatvec[r]);
                break;
            }   
        }

        passed = (hash_table.find(hash_key) != hash_table.end())? true : false;

        if(passed){ 
            for(int i = 0; i < hash_table[hash_key].size(); i++){
                for (auto it = left_table.table_data.begin(); it != left_table.table_data.end(); it++){
                    switch((*it).second.type){
                    case TYPE_INT:
                        target_table.table_data[(*it).first].intvec.push_back((*it).second.intvec[r]);
                        break;
                    case TYPE_STRING:
                        target_table.table_data[(*it).first].strvec.push_back((*it).second.strvec[r]);
                        break;
                    case TYPE_FLOAT:
                        target_table.table_data[(*it).first].floatvec.push_back((*it).second.floatvec[r]);
                        break;
                    }
                    target_table.table_data[(*it).first].isnull.push_back((*it).second.isnull[r]);
                    target_table.table_data[(*it).first].row_count++;
                }
                for (auto it = right_table.table_data.begin(); it != right_table.table_data.end(); it++){
                    switch((*it).second.type){
                    case TYPE_INT:
                        target_table.table_data[(*it).first].intvec.push_back((*it).second.intvec[hash_table[hash_key][i]]);
                        break;
                    case TYPE_STRING:
                        target_table.table_data[(*it).first].strvec.push_back((*it).second.strvec[hash_table[hash_key][i]]);
                        break;
                    case TYPE_FLOAT:
                        target_table.table_data[(*it).first].floatvec.push_back((*it).second.floatvec[hash_table[hash_key][i]]);
                        break;
                    }
                    target_table.table_data[(*it).first].isnull.push_back((*it).second.isnull[i]);
                    target_table.table_data[(*it).first].row_count++;
                }
                target_table.row_count++;
            }
        }else{
            for (auto it = left_table.table_data.begin(); it != left_table.table_data.end(); it++){
                switch((*it).second.type){
                case TYPE_INT:
                    target_table.table_data[(*it).first].intvec.push_back((*it).second.intvec[r]);
                    break;
                case TYPE_STRING:
                    target_table.table_data[(*it).first].strvec.push_back((*it).second.strvec[r]);
                    break;
                case TYPE_FLOAT:
                    target_table.table_data[(*it).first].floatvec.push_back((*it).second.floatvec[r]);
                    break;
                }
                target_table.table_data[(*it).first].isnull.push_back((*it).second.isnull[r]);
                target_table.table_data[(*it).first].row_count++;
            }
            for (auto it = right_table.table_data.begin(); it != right_table.table_data.end(); it++){
                switch((*it).second.type){
                case TYPE_INT:
                    target_table.table_data[(*it).first].intvec.push_back(0);
                    break;
                case TYPE_STRING:
                    target_table.table_data[(*it).first].strvec.push_back("");
                    break;
                case TYPE_FLOAT:
                    target_table.table_data[(*it).first].floatvec.push_back(0);
                    break;
                }
                target_table.table_data[(*it).first].isnull.push_back(true);//null check
                target_table.table_data[(*it).first].row_count++;
            }
            target_table.row_count++;
        }
    }
      
}

void MergeQueryManager::LeftOuterJoin_nestedloop(){
    for (auto it = left_table.table_data.begin(); it != left_table.table_data.end(); it++){
        target_table.table_data[(*it).first].type = (*it).second.type;
    }
    for (auto it = right_table.table_data.begin(); it != right_table.table_data.end(); it++){
        target_table.table_data[(*it).first].type = (*it).second.type;
    }

    for(int r1=0; r1<left_table.row_count; r1++){
        bool exist = false;

        for(int r2=0; r2<right_table.row_count; r2++){
            bool passed = false;

            for(int f=0; f<snippet.table_filter_size(); f++){//동등조인,비동등조인 여부 판단 필요
                if(f%2 == 1){
                    switch(snippet.table_filter(f).operator_())
                    case KETI_AND:{//and가 아닌경우??
                        continue;
                    }
                }else{
                    bool filter_passed = false;
                    string driving_c1 = snippet.table_filter(f).lv().value(0);
                    string driven_c2 = snippet.table_filter(f).rv().value(0);

                    switch(left_table.table_data[driving_c1].type){
                    case TYPE_INT:
                        if(left_table.table_data[driving_c1].intvec[r1] == right_table.table_data[driven_c2].intvec[r2]){
                            filter_passed = true;
                            break;
                        }
                        break;
                    case TYPE_FLOAT:
                        if(left_table.table_data[driving_c1].floatvec[r1] == right_table.table_data[driven_c2].floatvec[r2]){
                            filter_passed = true;
                            break;
                        }
                        break;
                    case TYPE_STRING:
                        if(left_table.table_data[driving_c1].strvec[r1] == right_table.table_data[driven_c2].strvec[r2]){
                            filter_passed = true;
                            break;
                        }
                        break;
                    default:
                        KETILOG::ERRORLOG(LOGTAG,"inner join type check plz... " + to_string(left_table.table_data[driving_c1].type));
                    }

                    if(filter_passed){
                        passed = true;
                        filter_passed = false;
                    }else{
                        passed = false;
                        break;
                    }
                }
            }

            if(passed){ 
                exist = true;
                for (auto it = left_table.table_data.begin(); it != left_table.table_data.end(); it++){
                    switch((*it).second.type){
                    case TYPE_INT:
                        target_table.table_data[(*it).first].intvec.push_back((*it).second.intvec[r1]);
                        break;
                    case TYPE_STRING:
                        target_table.table_data[(*it).first].strvec.push_back((*it).second.strvec[r1]);
                        break;
                    case TYPE_FLOAT:
                        target_table.table_data[(*it).first].floatvec.push_back((*it).second.floatvec[r1]);
                        break;
                    }
                    target_table.table_data[(*it).first].isnull.push_back((*it).second.isnull[r1]);
                    target_table.table_data[(*it).first].row_count++;
                }
                for (auto it = right_table.table_data.begin(); it != right_table.table_data.end(); it++){
                    switch((*it).second.type){
                    case TYPE_INT:
                        target_table.table_data[(*it).first].intvec.push_back((*it).second.intvec[r2]);
                        break;
                    case TYPE_STRING:
                        target_table.table_data[(*it).first].strvec.push_back((*it).second.strvec[r2]);
                        break;
                    case TYPE_FLOAT:
                        target_table.table_data[(*it).first].floatvec.push_back((*it).second.floatvec[r2]);
                        break;
                    }
                    target_table.table_data[(*it).first].isnull.push_back((*it).second.isnull[r2]);
                    target_table.table_data[(*it).first].row_count++;
                }
                target_table.row_count++;
            }
        }

        if(!exist){
            for (auto it = left_table.table_data.begin(); it != left_table.table_data.end(); it++){
                switch((*it).second.type){
                case TYPE_INT:
                    target_table.table_data[(*it).first].intvec.push_back((*it).second.intvec[r1]);
                    break;
                case TYPE_STRING:
                    target_table.table_data[(*it).first].strvec.push_back((*it).second.strvec[r1]);
                    break;
                case TYPE_FLOAT:
                    target_table.table_data[(*it).first].floatvec.push_back((*it).second.floatvec[r1]);
                    break;
                }
                target_table.table_data[(*it).first].isnull.push_back((*it).second.isnull[r1]);
                target_table.table_data[(*it).first].row_count++;
            }
            for (auto it = right_table.table_data.begin(); it != right_table.table_data.end(); it++){
                switch((*it).second.type){
                case TYPE_INT:
                    target_table.table_data[(*it).first].intvec.push_back(0);
                    break;
                case TYPE_STRING:
                    target_table.table_data[(*it).first].strvec.push_back("");
                    break;
                case TYPE_FLOAT:
                    target_table.table_data[(*it).first].floatvec.push_back(0);
                    break;
                }
                target_table.table_data[(*it).first].isnull.push_back(true);//null check
                target_table.table_data[(*it).first].row_count++;
            }
            target_table.row_count++;
        }
    }
}

void MergeQueryManager::RightOuterJoin_hash(){
    for (auto it = left_table.table_data.begin(); it != left_table.table_data.end(); it++){
        target_table.table_data[(*it).first].type = (*it).second.type;
    }
    for (auto it = right_table.table_data.begin(); it != right_table.table_data.end(); it++){
        target_table.table_data[(*it).first].type = (*it).second.type;
    }

    vector<int> equal_join_index;//join조건 중 equal 조인의 인덱스, left outer join은 = 조건만 존재
    for(int f = 0; f < snippet.table_filter_size(); f++){
        if(snippet.table_filter(f).operator_() == Snippet_Filter_OperType::Snippet_Filter_OperType_KETI_ET){
            equal_join_index.push_back(f);
        }
    }

    right_is_smaller = true;//left outer join은 무조건 right table로 해시테이블 생성, left테이블의 모든 로우 포함
    equal_join_exist = true;//left outer join은 = 조건만 존재
 
    createHashTable(equal_join_index);
        
    for(int r=0; r<left_table.row_count; r++){
        string hash_key = "";
        bool passed = false;

        for(int f = 0; f < equal_join_index.size(); f++){
            string hash_key_column = snippet.table_filter(equal_join_index[f]).lv().value(0);

            switch(left_table.table_data[hash_key_column].type){
            case TYPE_STRING:
                hash_key += "|" + left_table.table_data[hash_key_column].strvec[r];
                break;
            case TYPE_INT:
                hash_key += "|" + to_string(left_table.table_data[hash_key_column].intvec[r]);
                break;
            case TYPE_FLOAT:
                hash_key += "|" + to_string(left_table.table_data[hash_key_column].floatvec[r]);
                break;
            }   
        }

        passed = (hash_table.find(hash_key) != hash_table.end())? true : false;

        if(passed){ 
            for(int i = 0; i < hash_table[hash_key].size(); i++){
                for (auto it = left_table.table_data.begin(); it != left_table.table_data.end(); it++){
                    switch((*it).second.type){
                    case TYPE_INT:
                        target_table.table_data[(*it).first].intvec.push_back((*it).second.intvec[r]);
                        break;
                    case TYPE_STRING:
                        target_table.table_data[(*it).first].strvec.push_back((*it).second.strvec[r]);
                        break;
                    case TYPE_FLOAT:
                        target_table.table_data[(*it).first].floatvec.push_back((*it).second.floatvec[r]);
                        break;
                    }
                    target_table.table_data[(*it).first].isnull.push_back((*it).second.isnull[r]);
                    target_table.table_data[(*it).first].row_count++;
                }
                for (auto it = right_table.table_data.begin(); it != right_table.table_data.end(); it++){
                    switch((*it).second.type){
                    case TYPE_INT:
                        target_table.table_data[(*it).first].intvec.push_back((*it).second.intvec[hash_table[hash_key][i]]);
                        break;
                    case TYPE_STRING:
                        target_table.table_data[(*it).first].strvec.push_back((*it).second.strvec[hash_table[hash_key][i]]);
                        break;
                    case TYPE_FLOAT:
                        target_table.table_data[(*it).first].floatvec.push_back((*it).second.floatvec[hash_table[hash_key][i]]);
                        break;
                    }
                    target_table.table_data[(*it).first].isnull.push_back((*it).second.isnull[i]);
                    target_table.table_data[(*it).first].row_count++;
                }
                target_table.row_count++;
            }
        }else{
            for (auto it = left_table.table_data.begin(); it != left_table.table_data.end(); it++){
                switch((*it).second.type){
                case TYPE_INT:
                    target_table.table_data[(*it).first].intvec.push_back((*it).second.intvec[r]);
                    break;
                case TYPE_STRING:
                    target_table.table_data[(*it).first].strvec.push_back((*it).second.strvec[r]);
                    break;
                case TYPE_FLOAT:
                    target_table.table_data[(*it).first].floatvec.push_back((*it).second.floatvec[r]);
                    break;
                }
                target_table.table_data[(*it).first].isnull.push_back((*it).second.isnull[r]);
                target_table.table_data[(*it).first].row_count++;
            }
            for (auto it = right_table.table_data.begin(); it != right_table.table_data.end(); it++){
                switch((*it).second.type){
                case TYPE_INT:
                    target_table.table_data[(*it).first].intvec.push_back(0);
                    break;
                case TYPE_STRING:
                    target_table.table_data[(*it).first].strvec.push_back("");
                    break;
                case TYPE_FLOAT:
                    target_table.table_data[(*it).first].floatvec.push_back(0);
                    break;
                }
                target_table.table_data[(*it).first].isnull.push_back(true);//null check
                target_table.table_data[(*it).first].row_count++;
            }
            target_table.row_count++;
        }
    }
      
}

void MergeQueryManager::Union(){
    if(snippet.table_filter_size() == 0){// Union
        // 중복 로우 포함 X
    }else{ // Union All
        target_table.table_data = left_table.table_data;
        cout << "check " << left_table.row_count << " " << right_table.row_count << endl;

        for (auto it = target_table.table_data.begin(); it != target_table.table_data.end(); it++){
            switch((*it).second.type){
            case TYPE_INT:
                target_table.table_data[(*it).first].intvec.insert(target_table.table_data[(*it).first].intvec.end(),right_table.table_data[(*it).first].intvec.begin(),right_table.table_data[(*it).first].intvec.end());
                break;
            case TYPE_STRING:
                target_table.table_data[(*it).first].strvec.insert(target_table.table_data[(*it).first].strvec.end(),right_table.table_data[(*it).first].strvec.begin(),right_table.table_data[(*it).first].strvec.end());
                break;
            case TYPE_FLOAT:
                target_table.table_data[(*it).first].floatvec.insert(target_table.table_data[(*it).first].floatvec.end(),right_table.table_data[(*it).first].floatvec.begin(),right_table.table_data[(*it).first].floatvec.end());
                break;
            }
            target_table.table_data[(*it).first].row_count += right_table.table_data[(*it).first].row_count;
            target_table.table_data[(*it).first].isnull.insert(target_table.table_data[(*it).first].isnull.end(),right_table.table_data[(*it).first].isnull.begin(),right_table.table_data[(*it).first].isnull.end());
        }

        target_table.row_count = left_table.row_count + right_table.row_count;
    }
}

void MergeQueryManager::In(){
    for (auto it = left_table.table_data.begin(); it != left_table.table_data.end(); it++){
        target_table.table_data[(*it).first].type = (*it).second.type;
    }
    for (auto it = right_table.table_data.begin(); it != right_table.table_data.end(); it++){
        target_table.table_data[(*it).first].type = (*it).second.type;
    }

    right_is_smaller = true;//in은 무조건 right table로 해시테이블 생성

    vector<int> equal_join_index;
    bool is_not_in = false;
    if(snippet.table_filter(0).operator_() == Snippet_Filter_OperType::Snippet_Filter_OperType_KETI_NOT){
        is_not_in = true;
        createHashTable({1});
    }else{
        createHashTable({0});
    }

    for(int r=0; r<left_table.row_count; r++){
        string hash_key = "";
        bool passed = true;

        if(is_not_in){
            string hash_key_column = snippet.table_filter(1).lv().value(0);

            switch(left_table.table_data[hash_key_column].type){
            case TYPE_STRING:
                hash_key += "|" + left_table.table_data[hash_key_column].strvec[r];
                break;
            case TYPE_INT:
                hash_key += "|" + to_string(left_table.table_data[hash_key_column].intvec[r]);
                break;
            case TYPE_FLOAT:
                hash_key += "|" + to_string(left_table.table_data[hash_key_column].floatvec[r]);
                break;
            } 

            passed = (hash_table.find(hash_key) != hash_table.end())? false : true;
            
        }else{
            string hash_key_column = snippet.table_filter(0).lv().value(0);

            switch(left_table.table_data[hash_key_column].type){
            case TYPE_STRING:
                hash_key += "|" + left_table.table_data[hash_key_column].strvec[r];
                break;
            case TYPE_INT:
                hash_key += "|" + to_string(left_table.table_data[hash_key_column].intvec[r]);
                break;
            case TYPE_FLOAT:
                hash_key += "|" + to_string(left_table.table_data[hash_key_column].floatvec[r]);
                break;
            } 

            passed = (hash_table.find(hash_key) != hash_table.end())? true : false;
        }

        if(passed){
            for (auto it = left_table.table_data.begin(); it != left_table.table_data.end(); it++){
                switch((*it).second.type){
                case TYPE_INT:
                    target_table.table_data[(*it).first].intvec.push_back((*it).second.intvec[r]);
                    break;
                case TYPE_STRING:
                    target_table.table_data[(*it).first].strvec.push_back((*it).second.strvec[r]);
                    break;
                case TYPE_FLOAT:
                    target_table.table_data[(*it).first].floatvec.push_back((*it).second.floatvec[r]);
                    break;
                }
                target_table.table_data[(*it).first].isnull.push_back((*it).second.isnull[r]);
                target_table.table_data[(*it).first].row_count++;
            }
            target_table.row_count++;
        } 
    }
}

void MergeQueryManager::createHashTable(vector<int> equal_join_index){
    if(right_is_smaller){//right table로 hash table 만듦
        for (int i = 0; i < right_table.row_count; i++){
            string hash_key = "";

            for(int j = 0; j < equal_join_index.size(); j++){
                int index = equal_join_index[j];

                string hash_key_column = snippet.table_filter(index).rv().value(0);

                switch(right_table.table_data[hash_key_column].type){
                case TYPE_STRING:
                    hash_key += "|" + right_table.table_data[hash_key_column].strvec[i];
                    break;
                case TYPE_INT:
                    hash_key += "|" + to_string(right_table.table_data[hash_key_column].intvec[i]);
                    break;
                case TYPE_FLOAT:
                    hash_key += "|" + to_string(right_table.table_data[hash_key_column].floatvec[i]);
                    break;
                }
            }
            hash_table[hash_key].push_back(i);
        }
        
    }else{//left table로 hash table 만듦
        for (int i = 0; i < left_table.row_count; i++){
            string hash_key = "";
            
            for(int j = 0; j < equal_join_index.size(); j++){
                int index = equal_join_index[j];
                
                string hash_key_column = snippet.table_filter(index).lv().value(0);

                switch(left_table.table_data[hash_key_column].type){
                case TYPE_STRING:
                    hash_key += "|" + left_table.table_data[hash_key_column].strvec[i];
                    break;
                case TYPE_INT:
                    hash_key += "|" + to_string(left_table.table_data[hash_key_column].intvec[i]);
                    break;
                case TYPE_FLOAT:
                    hash_key += "|" + to_string(left_table.table_data[hash_key_column].floatvec[i]);
                    break;
                }

            }
            hash_table[hash_key].push_back(i);
        }
    }

}

template <typename T>
bool MergeQueryManager::compareByOperator(int oper, const T& arg1, const T& arg2) {
    bool passed;
    if(oper == Snippet_Filter_OperType::Snippet_Filter_OperType_KETI_ET){
        passed = (arg1 == arg2) ? true : false;
    }else if(oper == Snippet_Filter_OperType::Snippet_Filter_OperType_KETI_GE){
        passed = (arg1 >= arg2) ? true : false;
    }else if(oper == Snippet_Filter_OperType::Snippet_Filter_OperType_KETI_LE){
        passed = (arg1 <= arg2) ? true : false;
    }else if(oper == Snippet_Filter_OperType::Snippet_Filter_OperType_KETI_GT){
        passed = (arg1 > arg2) ? true : false;
    }else if(oper == Snippet_Filter_OperType::Snippet_Filter_OperType_KETI_LT){
        passed = (arg1 < arg2) ? true : false;
    }else if(oper == Snippet_Filter_OperType::Snippet_Filter_OperType_KETI_NE){
        passed = (arg1 != arg2) ? true : false;
    }
    return passed;
}

// bool MergeQueryManager::compareByOperator(int oper, string left_column, string right_column, int left_index, int right_index){
//     bool passed = true;
//     if(oper == Snippet_Filter_OperType::Snippet_Filter_OperType_KETI_GE){
//         switch(left_table.table_data[left_column].type){
//         case TYPE_STRING:
//             if(!(left_table.table_data[left_column].strvec[left_index] >= 
//                 right_table.table_data[right_column].strvec[right_index])){
//                     passed = false;
//                 }
//             break;
//         case TYPE_INT:
//             if(!(left_table.table_data[left_column].intvec[left_index] >= 
//                 right_table.table_data[right_column].intvec[right_index])){
//                     passed = false;
//                 }
//             break;
//         case TYPE_FLOAT:
//             if(!(left_table.table_data[left_column].floatvec[left_index] >= 
//                 right_table.table_data[right_column].floatvec[right_index])){
//                     passed = false;
//                 }
//             break;
//         }
//     }else if(oper == Snippet_Filter_OperType::Snippet_Filter_OperType_KETI_LE){
//         switch(left_table.table_data[left_column].type){
//         case TYPE_STRING:
//             if(!(left_table.table_data[left_column].strvec[left_index] <= 
//                 right_table.table_data[right_column].strvec[right_index])){
//                     passed = false;
//                 }
//             break;
//         case TYPE_INT:
//             if(!(left_table.table_data[left_column].intvec[left_index] <= 
//                 right_table.table_data[right_column].intvec[right_index])){
//                     passed = false;
//                 }
//             break;
//         case TYPE_FLOAT:
//             if(!(left_table.table_data[left_column].floatvec[left_index] <= 
//                 right_table.table_data[right_column].floatvec[right_index])){
//                     passed = false;
//                 }
//             break;
//         }
//     }else if(oper == Snippet_Filter_OperType::Snippet_Filter_OperType_KETI_GT){
//         switch(left_table.table_data[left_column].type){
//         case TYPE_STRING:
//             if(!(left_table.table_data[left_column].strvec[left_index] > 
//                 right_table.table_data[right_column].strvec[right_index])){
//                     passed = false;
//                 }
//             break;
//         case TYPE_INT:
//             if(!(left_table.table_data[left_column].intvec[left_index] > 
//                 right_table.table_data[right_column].intvec[right_index])){
//                     passed = false;
//                 }
//             break;
//         case TYPE_FLOAT:
//             if(!(left_table.table_data[left_column].floatvec[left_index] > 
//                 right_table.table_data[right_column].floatvec[right_index])){
//                     passed = false;
//                 }
//             break;
//         }
//     }else if(oper == Snippet_Filter_OperType::Snippet_Filter_OperType_KETI_LT){
//         switch(left_table.table_data[left_column].type){
//         case TYPE_STRING:
//             if(!(left_table.table_data[left_column].strvec[left_index] < 
//                 right_table.table_data[right_column].strvec[right_index])){
//                     passed = false;
//                 }
//             break;
//         case TYPE_INT:
//             if(!(left_table.table_data[left_column].intvec[left_index] < 
//                 right_table.table_data[right_column].intvec[right_index])){
//                     passed = false;
//                 }
//             break;
//         case TYPE_FLOAT:
//             if(!(left_table.table_data[left_column].floatvec[left_index] < 
//                 right_table.table_data[right_column].floatvec[right_index])){
//                     passed = false;
//                 }
//             break;
//         }
//     }else if(oper == Snippet_Filter_OperType::Snippet_Filter_OperType_KETI_NE){
//         switch(left_table.table_data[left_column].type){
//         case TYPE_STRING:
//             if(!(left_table.table_data[left_column].strvec[left_index] != 
//                 right_table.table_data[right_column].strvec[right_index])){
//                     passed = false;
//                 }
//             break;
//         case TYPE_INT:
//             if(!(left_table.table_data[left_column].intvec[left_index] != 
//                 right_table.table_data[right_column].intvec[right_index])){
//                     passed = false;
//                 }
//             break;
//         case TYPE_FLOAT:
//             if(!(left_table.table_data[left_column].floatvec[left_index] != 
//                 right_table.table_data[right_column].floatvec[right_index])){
//                     passed = false;
//                 }
//             break;
//         }
//     }
//     return passed;
// }


void MergeQueryManager::CrossJoin(){}
void MergeQueryManager::DependencyInnerJoin(){}
void MergeQueryManager::DependencyExist(){}    
void MergeQueryManager::DependencyIn(){}
