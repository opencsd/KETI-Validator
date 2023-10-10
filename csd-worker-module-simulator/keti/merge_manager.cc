#include "merge_manager.h"

void MergeManager::Merging(){
    while (1){
        Result result = MergeQueue.wait_and_pop();  

        MergeBlock(result);
    }
}

void MergeManager::MergeBlock(Result &result){
    int origin_row_len = 0;
    int projection_type;
    pair_key key = make_pair(result.query_id,result.work_id);

    //Key에 해당하는 블록버퍼가 없다면 생성
    if(m_MergeManager.find(key)==m_MergeManager.end()){
        MergeResult mergeResult(result.query_id, result.work_id, result.csd_name, 
        result.filter_info.projection_datatype, result.filter_info.projection_length, result.total_block_count);
        m_MergeManager[key] = mergeResult;
    }

    //data가 있는 경우만 수행
    if(result.row_count != 0){
        vector<int> new_row_offset;
        new_row_offset.assign( result.row_offset.begin(), result.row_offset.end() );
        new_row_offset.push_back(result.length);

        //새 블록 한 row씩 넣기
        for(int i=0; i<result.row_count; i++){
            origin_row_len = new_row_offset[i+1] - new_row_offset[i];
            char origin_row_data[origin_row_len];
            memcpy(origin_row_data,result.data+result.row_offset[i],origin_row_len);

            char new_row_data[origin_row_len];
            int new_row_len = 0;
            int col_count = result.filter_info.table_col.size();
            int col_offset[col_count + 1];//각 row별 컬럼 오프셋 획득
            getColOffset(origin_row_data,result.filter_info, col_offset);
            col_offset[col_count] = origin_row_len;

            //한 row에 column projection 적용
            for(int l = 0; l<result.filter_info.column_projection.size(); l++){
                projection_type = result.filter_info.column_projection[l].projection;
                char col_data[origin_row_len];//col의 크기를 미리 알 수 있나?
                int idx, offset, col_len, type = 0;
                string value;

                if(projection_type != COL_NAME){
                    KETILOG::FATALLOG(LOGTAG,"<error> what projection type: " + to_string(projection_type));

                //단순 컬럼 획득(len==1인데 단순 컬럼이 아닌 경우가 있나?)
                }else if(result.filter_info.column_projection[l].values.size() == 1){
                    type = result.filter_info.column_projection[l].types[0];
                    value = result.filter_info.column_projection[l].values[0];
                    if(type == COLUMN){
                        idx = result.filter_info.colindexmap[value];
                        offset = col_offset[idx];
                        col_len = col_offset[idx+1] - col_offset[idx];
                        memcpy(col_data, origin_row_data+offset, col_len);
                    }else{//colname이 아닌 경우가 존재?
                        KETILOG::FATALLOG(LOGTAG,"merge_manager> col_projection> type not string");
                    }

                //연산수행
                }else{
                    string first_operator = result.filter_info.column_projection[l].values[0];
                    
                    if(first_operator == "CASE"){
                        col_len = calculCase(result.filter_info, origin_row_data, col_offset, l, col_data); 
                    }else if(first_operator == "SUBSTRING"){
                        col_len = calculSubstring(result.filter_info, origin_row_data, col_offset, l, col_data); 
                    }else if(first_operator == "EXTRACT"){
                        col_len = calculExtract(result.filter_info, origin_row_data, col_offset, l, col_data); 
                    }else{
                        col_len = calculPostfix(result.filter_info.column_projection[l].values, result.filter_info.column_projection[l].types, 
                                            result.filter_info, origin_row_data, col_offset, col_data, result.filter_info.projection_datatype[l]); 
                    }
                    
                }

                //row에 column 저장
                if(projection_type == COL_NAME){
                    memcpy(new_row_data+new_row_len, col_data, col_len);
                    new_row_len += col_len;
                }else{
                    KETILOG::FATALLOG(LOGTAG,"projection type error ");
                }
            }
            
            //row 추가 시 버퍼 크기 넘는지 확인 
            if(m_MergeManager[key].length + new_row_len > BUFF_SIZE){
                ReturnQueue.push_work(m_MergeManager[key]);
                m_MergeManager[key].InitMergeResult();
            }
        
            //버퍼에 column_projection 한 row 넣기
            m_MergeManager[key].row_offset.push_back(m_MergeManager[key].length);//row 시작 오프셋은 현재 데이터 길이
            m_MergeManager[key].row_count += 1;//row 수 +1
            int current_offset = m_MergeManager[key].length;
            memcpy(m_MergeManager[key].data+current_offset, new_row_data, new_row_len);
            m_MergeManager[key].length += new_row_len;// 데이터 길이 = row 전체 길이
        }

    }

    m_MergeManager[key].current_block_count += result.result_block_count;
    m_MergeManager[key].result_block_count += result.result_block_count;
    m_MergeManager[key].raw_row_count += result.raw_row_count;

    float temp_size = float(m_MergeManager[key].length) / float(1024);

    memset(msg, '\0', sizeof(msg));
    sprintf(msg,"Merging Block ... (Current Buffer Size : %.1fK)\n",temp_size);
    //KETILOG::DEBUGLOG(LOGTAG, msg);

    //전체 블록 병합이 끝났는지 확인
    if(m_MergeManager[key].total_block_count == m_MergeManager[key].current_block_count){
        ReturnQueue.push_work(m_MergeManager[key]);
        m_MergeManager[key].InitMergeResult();

        memset(msg, '\0', sizeof(msg));
        sprintf(msg,"Merging Block {ID : %d-%d} Done (Block : %d/%d)\n",result.query_id,result.work_id,result.total_block_count,result.total_block_count);
        KETILOG::DEBUGLOG(LOGTAG, msg);

        m_MergeManager.erase(key);
    }
    
}

int MergeManager::calculPostfix(vector<string> values, vector<int> types, FilterInfo filter_info, char* origin_row_data, int* col_offset, char* dest, int projection_datatype){
    stack<pair<T,int>> oper_stack;
    int type, idx, offset, dest_type, col_len, return_len;
    string value;
    for(int k=0; k<values.size(); k++){
        type = types[k];
        value = values[k];
        if(type != OPERATOR){//피연산자
            switch(type){
                case INT8:
                case INT16:
                case INT32:{
                    T t;
                    t.varInt = stoi(value);
                    oper_stack.push(make_pair(t,INT_));
                    break;
                }case INT64:{
                    T t;
                    t.varLong = stoll(value);
                    oper_stack.push(make_pair(t,Long_));
                    break;
                }case FLOAT32:{
                    T t;
                    t.varDouble = stof(value);//여기바꿈 더블로
                    oper_stack.push(make_pair(t,DOUBLE_));
                    break;
                }case DOUBLE:{
                    T t;
                    t.varDouble = stod(value);
                    t.real_size = 1;//임시작성!!! tpch20
                    oper_stack.push(make_pair(t,DOUBLE_));
                    break;
                }case STRING:{
                    T t;
                    t.varString = value;
                    oper_stack.push(make_pair(t,STRING_));
                    break;
                }case COLUMN:{
                    idx = filter_info.colindexmap[value];
                    offset = col_offset[idx];
                    dest_type = filter_info.table_datatype[idx];
                    col_len = col_offset[idx+1] - col_offset[idx];
                    char dest[col_len];
                    memcpy(dest, origin_row_data+offset, col_len);
                    pair<T,int> pair = stack_charToValue(dest,dest_type,col_len);
                    oper_stack.push(pair);
                    break;
                }case NUMERIC:{
                }case DATE:{
                } 
            }
        }else{//연산자
            pair<T,int> op2 = oper_stack.top();
            oper_stack.pop();
            pair<T,int> op1 = oper_stack.top();
            oper_stack.pop();

            	
            if(value == "+"){
                if(op1.second == INT_ && op2.second == DOUBLE_){
                    double result = op1.first.varInt + op2.first.varDouble;
                    T t;
                    t.varDouble = result;
                    t.real_size = 1;
                    oper_stack.push(make_pair(t,DOUBLE_));
                }else if(op1.second == DOUBLE_ && op2.second == INT_){
                    double result = op1.first.varDouble + op2.first.varInt;
                    T t;
                    t.varDouble = result;
                    t.real_size = 1;
                    oper_stack.push(make_pair(t,DOUBLE_));
                }else{
                    KETILOG::FATALLOG(LOGTAG,"merge_m>operator>plus>else");
                }
            }else if(value == "-"){
                if(op1.second == INT_ && op2.second == DOUBLE_){
                    double result = op1.first.varInt - op2.first.varDouble;
                    T t;
                    t.varDouble = result;
                    t.real_size = 1;
                    oper_stack.push(make_pair(t,DOUBLE_));
                }else if(op1.second == DOUBLE_ && op2.second == INT_){
                    double result = op1.first.varDouble - op2.first.varInt;
                    T t;
                    t.varDouble = result;
                    t.real_size = 1;
                    oper_stack.push(make_pair(t,DOUBLE_));
                }else if(op1.second == DOUBLE_ && op2.second == DOUBLE_){
                    double result = op1.first.varDouble - op2.first.varDouble;
                    T t;
                    t.varDouble = result;
                    t.real_size = max(op1.first.real_size, op2.first.real_size);
                    oper_stack.push(make_pair(t,DOUBLE_));
                }else{
                    KETILOG::FATALLOG(LOGTAG,"merge_m>operator>minus>else");
                }
            }else if(value == "*"){
                if(op1.second == DOUBLE_ && op2.second == DOUBLE_){
                    double result = op1.first.varDouble * op2.first.varDouble;
                    T t;
                    t.varDouble = result;
                    t.real_size = op1.first.real_size + op2.first.real_size;
                    oper_stack.push(make_pair(t,DOUBLE_));
                }else if(op1.second == DOUBLE_ && op2.second == INT_){
                    double result = op1.first.varDouble * op2.first.varInt;
                    T t;
                    t.varDouble = result;
                    t.real_size = op1.first.real_size;
                    oper_stack.push(make_pair(t,DOUBLE_));
                }else if(op1.second == INT_ && op2.second == DOUBLE_){
                    double result = op1.first.varInt * op2.first.varDouble;
                    T t;
                    t.varDouble = result;
                    t.real_size = op1.first.real_size;
                   oper_stack.push(make_pair(t,DOUBLE_));
                }
                else{
                    KETILOG::FATALLOG(LOGTAG,"merge_m>operator>multiple>else");
                }
            }else if(value == "/"){
                if(op1.second == INT_ && op2.second == DOUBLE_){
                    double result = op1.first.varInt / op2.first.varDouble;
                    T t;
                    t.varDouble = result;
                    t.real_size = 1;
                    oper_stack.push(make_pair(t,DOUBLE_));
                }else if(op1.second == DOUBLE_ && op2.second == INT_){
                    double result = op1.first.varDouble / op2.first.varInt;
                    T t;
                    t.varDouble = result;
                    t.real_size = 1;
                    oper_stack.push(make_pair(t,DOUBLE_));
                }else{
                    KETILOG::FATALLOG(LOGTAG,"merge_m>operator>divide>else");
                }
            }else if(value == "="){
                if(op1.second == STRING_ && op2.second == STRING_){
                    if(op1.first.varString == op2.first.varString){
                        dest[0] = 1;
                        return 1;
                    }else{
                        dest[0] = 0;
                        return 1;
                    }
                }else{
                    KETILOG::FATALLOG(LOGTAG,"merge_m>operator>equal>else");
                }
            }else if(value == "<>"){
                if(op1.second == STRING_ && op2.second == STRING_){
                    if(op1.first.varString != op2.first.varString){
                        dest[0] = 1;
                        return 1;
                    }else{
                        dest[0] = 0;
                        return 1;
                    }
                }else{
                    KETILOG::FATALLOG(LOGTAG,"merge_m>operator>not equal>else");
                }
            }else if(value == "LIKE"){
                if(op1.second == STRING_ && op2.second == STRING_){
                    string str = op2.first.varString;
                    string target = op1.first.varString;
                    if(str.substr(0,1) == "%" && str.substr(str.length()-1) == "%"){//'%str%'
                        string find = str.substr(1,str.length()-2);
                        if(target.find(find) != string::npos){
                            dest[0] = 1;
                            return 1;
                        }else{
                            dest[0] = 0;
                            return 1;
                        }
                    }else if(str.substr(0,1) == "%"){//'%str'
                        string find = str.substr(1);
                        string comp = target.substr(target.length()-find.length());
                        if(find == comp){
                            dest[0] = 1;
                            return 1;
                        }else{
                            dest[0] = 0;
                            return 1;
                        }
                    }else if(str.substr(str.length()-1) == "%"){//'str%'
                        string find = str.substr(0,str.length()-1);
                        string comp = target.substr(0,find.length());
                        if(find == comp){
                            dest[0] = 1;
                            return 1;
                        }else{
                            dest[0] = 0;
                            return 1;
                        }
                    }
                    
                }else{
                    KETILOG::FATALLOG(LOGTAG,"merge_m>operator>like>else");
                }
            }else{
                KETILOG::FATALLOG(LOGTAG,"else: " + value);
            }
        }
    }

    //convert result -> char array 
    T postfix_result = oper_stack.top().first;
    int postfix_type = oper_stack.top().second;
    oper_stack.pop();
    int len = stack_valueToChar(dest, projection_datatype, postfix_result, postfix_type);
    col_len = len;

    return len;
}

int MergeManager::calculCase(FilterInfo filter_info, char* origin_row_data, int* col_offset, int l, char* dest){
    int case_len = filter_info.column_projection[l].values.size();
    vector<pair<int,int>> when_then_offset;
    int else_offset, when, then = 0;
    int dest_type = filter_info.projection_datatype[l];

    for(int i = 1; i<case_len; i++){
        if(filter_info.column_projection[l].values[i] == "WHEN"){
            when = i;
        }else if(filter_info.column_projection[l].values[i] == "THEN"){
            then = i;
            when_then_offset.push_back({when,then});
        }else if(filter_info.column_projection[l].values[i] == "ELSE"){
            else_offset = i;
        }
    }

    vector<pair<int,int>>::iterator iter;
    for (iter = when_then_offset.begin(); iter != when_then_offset.end(); ++iter){
        when = (*iter).first;
        then = (*iter).second;
        int i = when + 1;
        bool passed = true;
        char passed_[1];

        vector<string> values;
        vector<int> types;
        while(i < then){
            if(filter_info.column_projection[l].values[i] == "AND"){
                calculPostfix(values,types,filter_info,origin_row_data,col_offset,passed_,0);
                if(passed_[0] == 1){
                    values.clear();
                    types.clear(); 
                    i++;
                    continue;
                }else{
                    passed = false;
                    break;
                }
            }else if(filter_info.column_projection[l].values[i] == "OR"){
                calculPostfix(values,types,filter_info,origin_row_data,col_offset,passed_,0);
                if(passed_[0] == 1){
                    break;
                }else{
                    values.clear();
                    types.clear(); 
                    i++;
                    continue;
                } 
            }else{
                values.push_back(filter_info.column_projection[l].values[i]);
                types.push_back(filter_info.column_projection[l].types[i]);
            }

            i++;

            if(i == then){
                calculPostfix(values,types,filter_info,origin_row_data,col_offset,passed_,0);
                if(passed_[0]){
                    passed = true;
                }else{
                    passed = false;
                } 
                values.clear();
                types.clear(); 
            }
            
        }

        i = then + 1;

        if(passed){
            values.clear();
            types.clear(); 
            while(true){
                if(filter_info.column_projection[l].values[i] == "WHEN" || 
                    filter_info.column_projection[l].values[i] == "ELSE" || 
                    filter_info.column_projection[l].values[i] == "END"){
                    break;
                }else{
                    values.push_back(filter_info.column_projection[l].values[i]);
                    types.push_back(filter_info.column_projection[l].types[i]);
                }
                i++;
            }

            if(values.size() == 1){
                int value = stoi(values[0]);
                memcpy(dest, &value, sizeof(int));
                return 4;
            }else{
                int l = calculPostfix(values,types,filter_info,origin_row_data,col_offset,dest,dest_type);
                return l;
            }
        }
    }

    if(else_offset != 0){
        // int else_len = case_len - else_offset - 1;
        int result = stoi(filter_info.column_projection[l].values[else_offset+1]);

        switch(dest_type){
            case MySQL_NEWDECIMAL:{
                //decimal(15,2)만 고려한 상황 -> col_len = 7 (integer:6/real:1)
                for(int i=0; i<7; i++){
                    dest[i] = 0x00;
                }
                dest[0] = 0x80;
                return 7;                
            }case MySQL_INT32:{
                memcpy(dest, &result, sizeof(int));
                return 4;
            }default:{
                KETILOG::FATALLOG(LOGTAG,"else clause dest_type error");
            }
        }
    }
    
    //NULL값 반환                               
    return 0;
}

int MergeManager::calculSubstring(FilterInfo filter_info, char* origin_row_data, int* col_offset, int l, char* dest){
    string str = filter_info.column_projection[l].values[1];
    int type = filter_info.column_projection[l].types[1];
    int start_offset = stoi(filter_info.column_projection[l].values[2]) - 1;
    int read_length;
    
    if(type == COLUMN){
        int idx = filter_info.colindexmap[str];
        int offset = col_offset[idx];
        int col_len = col_offset[idx+1] - col_offset[idx];
        char col_data[col_len];

        if(filter_info.column_projection[l].values.size() == 4){
            read_length = stoi(filter_info.column_projection[l].values[3]);
        }else{
            read_length = col_len - start_offset;
        }

        memcpy(col_data, origin_row_data+offset, col_len);
        memcpy(dest, col_data+start_offset, read_length);
    }else if(type == STRING){
        int col_len = str.length();
        char col_data[col_len];

        if(filter_info.column_projection[l].values.size() == 4){
            read_length = stoi(filter_info.column_projection[l].values[3]);
        }else{
            read_length = col_len - start_offset;
        }

        strcpy(col_data, str.c_str());
        memcpy(dest, col_data+start_offset, read_length);
    }else{
        KETILOG::FATALLOG(LOGTAG,"calcul Substring > not defined : " + to_string(type));
    }

    return read_length;
}

int MergeManager::calculExtract(FilterInfo filter_info, char* origin_row_data, int* col_offset, int l, char* dest){
    //["EXTRACT","YEAR",col_name]
    string unit = filter_info.column_projection[l].values[1];
    string col = filter_info.column_projection[l].values[2];
    int type = filter_info.column_projection[l].types[2];
    int value, result;

    if(type == COLUMN){
        int idx = filter_info.colindexmap[col];
        int offset = col_offset[idx];
        int col_len = col_offset[idx+1] - col_offset[idx];
        char dest[col_len+1];
        memcpy(dest, origin_row_data+offset, col_len);
        dest[3] = 0x00;
        value = *((int *)dest);
    }else{
        KETILOG::FATALLOG(LOGTAG,"calcul Extract > not defined : " + to_string(type));
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
        KETILOG::FATALLOG(LOGTAG,"calcul Extract > not defined : " + unit);
    }

    memcpy(dest, &result, sizeof(int));
    return 4;
}


pair<T,int> MergeManager::stack_charToValue(char* src, int type, int len){
    pair<T,int> result;
    switch (type){
        case MySQL_BYTE:{
            int value = *((int8_t *)src);
            T t;
            t.varInt = value;
            result = make_pair(t,INT_);
            break;
        }case MySQL_INT16:{
            int value = *((int16_t *)src);
            T t;
            t.varInt = value;
            result = make_pair(t,INT_);
            break;
        }case MySQL_INT32:{
            int value = *((int32_t *)src);
            T t;
            t.varInt = value;
            result = make_pair(t,INT_);
            break;
        }case MySQL_INT64:{
            int64_t value = *((int64_t *)src);
            T t;
            t.varInt = value;
            result = make_pair(t,Long_);
            break;
        }case MySQL_FLOAT32:{
            float value = *((float *)src);
            T t;
            t.varFloat = value;
            result = make_pair(t,FLOAT_);
            break;
        }case MySQL_DOUBLE:{
            double value = *((double *)src);
            T t;
            t.varFloat = value;
            result = make_pair(t,DOUBLE_);
            break;
        }case MySQL_NEWDECIMAL:{
            //decimal(15,2)만 고려한 상황 -> col_len = 7 (integer:6/real:1)
            bool is_negative = false;
            if(std::bitset<8>(src[0])[7] == 0){//음수일때 not +1
                is_negative = true;
                for(int i = 0; i<7; i++){
                    src[i] = ~src[i];//not연산
                }
                // src[6] = src[6] +1;//+1
            }   
            char integer[8];
            char real[1];
            memset(&integer, 0, 8);
            for(int k=0; k<5; k++){
                integer[k] = src[5-k];
            }
            real[0] = src[6];
            int64_t ivalue = *((int64_t *)integer); 
            double rvalue = *((int8_t *)real);
            rvalue *= 0.01;
            double value = ivalue + rvalue;
            T t;
            if(is_negative){
                value *= -1;
            }
            t.varDouble = value;
            t.real_size = 1;
            result = make_pair(t,DOUBLE_);
            break;
        }case MySQL_DATE:{
            char tempbuf[len+1];//col_len = 3
            memcpy(tempbuf,src,len);
            tempbuf[3] = 0x00;
            int value = *((int *)tempbuf);
            T t;
            t.varInt = value;
            result = make_pair(t,INT_);
            break;
        }case MySQL_TIMESTAMP:{
            char tempbuf[len];//col_len = 4
            memcpy(tempbuf,src,len);
            int value = *((int *)tempbuf);
            T t;
            t.varInt = value;
            result = make_pair(t,INT_);
            break;
        }case MySQL_STRING:
         case MySQL_VARSTRING:{
            char tempbuf[len+1];
            memcpy(tempbuf,src,len);
            tempbuf[len] = '\0';
            string value(tempbuf);
            T t;
            t.varString = trim_(value);//이렇게 하면 안되는 경우는?
            result = make_pair(t,STRING_);
            break;
        }default:{
            KETILOG::FATALLOG(LOGTAG,"charToValue>default>Type > not defined: " + to_string(type));
        }
    }
    return result;
}

int MergeManager::stack_valueToChar(char* dest, int dest_type, T value, int type){
    	switch(type){
         case INT_:{
            int src = value.varInt;
            char out[4];
            memcpy(dest, &src, sizeof(int));
            return 4;
        }case Long_:{
            int64_t src = value.varLong;
            char out[8];
            memcpy(dest, &src, sizeof(int64_t));
            return 8;
        }case FLOAT_:
         case DOUBLE_:{
            if(dest_type == MySQL_NEWDECIMAL){
                bool is_negative = false;
                if(value.varDouble<0){
                    is_negative = true;
                    value.varDouble *= -1;
                }
                int64_t integer, real;
                double integer_temp, real_temp;
                char integer_data[8], real_data[8];
                real_temp = modf( value.varDouble, &integer_temp);
                integer = integer_temp;

                memcpy(integer_data, &integer, sizeof(int64_t));
                for(int i=0; i<6; i++){
                    dest[i] = integer_data[5-i];
                }
                if(value.real_size == 1){
                    real = round(real_temp * 100);//두자리면 100
                    memcpy(real_data, &real, sizeof(int64_t));
                    dest[6] = real_data[0];
                }else if(value.real_size == 2){
                    real = round(real_temp * 10000);//네자리면 10000
                    memcpy(real_data, &real, sizeof(int64_t));
                    dest[6] = real_data[1];
                    dest[7] = real_data[0];
                }else if(value.real_size == 3){
                    real = round(real_temp * 1000000);//여섯자리면 1000000
                    memcpy(real_data, &real, sizeof(int64_t));
                    dest[6] = real_data[2];
                    dest[7] = real_data[1];
                    dest[8] = real_data[0];
                }else{
                    KETILOG::FATALLOG(LOGTAG,"value.real_size ???: " + to_string(value.real_size));
                }
                dest[0] = 0x80;
                if(is_negative){
                    for(int i=0; i<7; i++){
                        dest[i] = ~dest[i];
                    }
                     //dest[6] += 1;//이거테스트
                }
                return 6+value.real_size;
            }else{
                KETILOG::FATALLOG(LOGTAG,"DOUBLE_>dest type: " + to_string(dest_type));
                return 0;
            }
        }case STRING_:{
            string src = value.varString;
            int l = src.length();
            char char_array[l];
            strcpy(char_array, src.c_str());
            memcpy(dest, char_array, l);
            return l;
        }default:{
            KETILOG::FATALLOG(LOGTAG,"valueToChar>default>type > not defined" + to_string(type));
            return 0;
        }
    }
}

void MergeManager::getColOffset(const char* origin_row_data, FilterInfo filter_info, int* col_offset_list){
    int col_type, col_len, col_offset, new_col_offset = 0;
    int col_count = filter_info.table_col.size();
    int tune = 0;

    for(int i=0; i<col_count; i++){
        col_type = filter_info.table_datatype[i];
        col_len = filter_info.table_offlen[i];
        col_offset = filter_info.table_offset[i];
           
        new_col_offset = col_offset + tune;

        if(col_type == MySQL_VARSTRING){
            if(col_len < 256){//0~255
                char var_len[1];
                var_len[0] = origin_row_data[new_col_offset];
                uint8_t var_len_ = *((uint8_t *)var_len);
                tune += var_len_ + 1 - col_len;
            }else{//0~65535
                char var_len[2];
                var_len[0] = origin_row_data[new_col_offset];
                int new_col_offset_ = new_col_offset + 1;
                var_len[1] = origin_row_data[new_col_offset_];
                uint16_t var_len_ = *((uint16_t *)var_len);
                tune += var_len_ + 2 - col_len;
            }
        }

        col_offset_list[i] = new_col_offset;
    }
}
