#include "BufferManager.h"

void getColOffset(const char* row_data, int* col_offset_list, vector<int> return_datatype, vector<int> table_offlen);

void BufferManager::initBufferManager(){
    std::thread BufferManager_Input_Thread(&BufferManager::inputBufferManager,this);
    std::thread BufferManager_Save_Thread(&BufferManager::runBufferManager,this);
    BufferManager_Input_Thread.detach();
    BufferManager_Save_Thread.detach();
}

void BufferManager::inputBufferManager(){
    int server_fd, client_fd;
	int opt = 1;
	struct sockaddr_in serv_addr, client_addr;
	socklen_t addrlen = sizeof(client_addr);
    static char cMsg[] = "ok";

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(40003);//(SE_MERGING_CONTAINER_BM_TCP_PORT); 
 
	if (bind(server_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
		perror("bind");
		exit(EXIT_FAILURE);
	} 

	if (listen(server_fd, NCONNECTION) < 0){
		perror("listen");
		exit(EXIT_FAILURE);
	}

    KETILOG(LOGTAG,"CSD Return Server Listening on 10.0.4.82:40003");

	while(1){
		if ((client_fd = accept(server_fd, (struct sockaddr*)&client_addr, (socklen_t*)&addrlen)) < 0){
			perror("accept");
        	exit(EXIT_FAILURE);
		}
		std::string json = "";\
        int njson;
		size_t ljson;

		recv(client_fd , &ljson, sizeof(ljson), 0);
        char buffer[ljson];
        memset(buffer, 0, ljson);
		while(1) {
			if ((njson = recv(client_fd, buffer, BUFF_SIZE-1, 0)) == -1) {
				perror("read");
				exit(1);
			}
			ljson -= njson;
		    buffer[njson] = '\0';
			json += buffer;

		    if (ljson == 0)
				break;
		}
        send(client_fd, cMsg, strlen(cMsg), 0);

		char data[BUFF_SIZE];
        char* dataiter = data;
		memset(data, 0, BUFF_SIZE);
        int ndata = 0;
        size_t ldata = 0;
        recv(client_fd , &ldata, sizeof(ldata),0);
		while(1) {
			if ((ndata = recv( client_fd , dataiter, ldata,0)) == -1) {
				perror("read");
				exit(1);
			}
            dataiter += ndata;
			ldata -= ndata;

		    if (ldata == 0)
				break;
		}
        send(client_fd, cMsg, strlen(cMsg), 0);
        PushQueue(BlockResult(json.c_str(), data));
        close(client_fd);		
	}   
	close(server_fd);
}

void BufferManager::runBufferManager(){
    while (1){
        BlockResult blockResult = PopQueue();
        string msg = "# Receive Data from CSD Return Interface {" + to_string(blockResult.query_id) + "|" + to_string(blockResult.work_id) + "}";
        //KETILOG(LOGTAG, msg);
        if((DataBuff.find(blockResult.query_id) == DataBuff.end()) || 
           (DataBuff[blockResult.query_id]->work_buffer_list.find(blockResult.work_id) 
                    == DataBuff[blockResult.query_id]->work_buffer_list.end())){
            cout << "<error> Work(" << blockResult.query_id << "-" << blockResult.work_id << ") Initialize First!" << endl;
        }   
        mergeBlock(blockResult);
    }
}

void BufferManager::mergeBlock(BlockResult result){
    int qid = result.query_id;
    int wid = result.work_id;
    string col_name;

    WorkBuffer* myWorkBuffer = DataBuff[qid]->work_buffer_list[wid];
    unique_lock<mutex> lock(myWorkBuffer->mu);
    if(myWorkBuffer->status == WorkDone){
        string msg = " error>> Work {" + to_string(qid) + "|" + to_string(wid) + "} is already done!";
        KETILOG(LOGTAG, msg);
        return;
    }
    
    if(result.length != 0){
        int col_type, col_offset, col_len, origin_row_len, col_count = 0;
        vector<int> new_row_offset;
        new_row_offset.assign(result.row_offset.begin(), result.row_offset.end());
        new_row_offset.push_back(result.length);

        
        // {
        // // 리턴 데이터 형식 확인 - Debug Code   
        // for(int i = 0; i<result.return_datatype.size(); i++){
        //     cout << result.return_datatype[i] << " ";
        // }
        // cout << endl;
        // for(int i = 0; i<result.return_datatype.size(); i++){
        //     cout << result.return_offlen[i] << " ";
        // }
        // cout << endl;  
        // }

        for(int i=0; i<result.row_count; i++){
            origin_row_len = new_row_offset[i+1] - new_row_offset[i];
            char row_data[origin_row_len];
            memcpy(row_data,result.data+result.row_offset[i],origin_row_len);
            // for(int i=0; i<origin_row_len; i++){
            //     printf("%02X ",(u_char)row_data[i]);
            // }
            // cout << endl;

            col_count = myWorkBuffer->table_column.size();
            int col_offset_list[col_count + 1];
            
            getColOffset(row_data, col_offset_list, result.return_datatype, result.return_offlen);
            col_offset_list[col_count] = origin_row_len;

            for(size_t j=0; j<myWorkBuffer->table_column.size(); j++){
                col_name = myWorkBuffer->table_column[j];
                col_offset = col_offset_list[j];
                col_len = col_offset_list[j+1] - col_offset_list[j];
                col_type = result.return_datatype[j];

                switch (col_type){
                    case MySQL_BYTE:{
                        char tempbuf[col_len];
                        memcpy(tempbuf,row_data+col_offset,col_len);
                        int64_t my_value = *((int8_t *)tempbuf);
                        myWorkBuffer->table_data[col_name].intvec.push_back(my_value);
                        break;
                    }case MySQL_INT16:{
                        char tempbuf[col_len];
                        memcpy(tempbuf,row_data+col_offset,col_len);
                        int64_t my_value = *((int16_t *)tempbuf);
                        myWorkBuffer->table_data[col_name].intvec.push_back(my_value);     
                        break;
                    }case MySQL_INT32:{
                        char tempbuf[col_len];
                        memcpy(tempbuf,row_data+col_offset,col_len);
                        int64_t my_value = *((int32_t *)tempbuf);
                        myWorkBuffer->table_data[col_name].intvec.push_back(my_value);
                        break;
                    }case MySQL_INT64:{
                        char tempbuf[col_len];
                        memcpy(tempbuf,row_data+col_offset,col_len);
                        int64_t my_value = *((int64_t *)tempbuf);
                        myWorkBuffer->table_data[col_name].intvec.push_back(my_value);
                        break;
                    }case MySQL_FLOAT32:{
                        //아직 처리X
                        char tempbuf[col_len];//col_len = 4
                        memcpy(tempbuf,row_data+col_offset,col_len);
                        double my_value = *((float *)tempbuf);
                        myWorkBuffer->table_data[col_name].floatvec.push_back(my_value);
                        break;
                    }case MySQL_DOUBLE:{
                        //아직 처리X
                        char tempbuf[col_len];//col_len = 8
                        memcpy(tempbuf,row_data+col_offset,col_len);
                        double my_value = *((double *)tempbuf);
                        myWorkBuffer->table_data[col_name].floatvec.push_back(my_value);
                        break;
                    }case MySQL_NEWDECIMAL:{
                        //decimal(15,2)만 고려한 상황 -> col_len = 7 or 8 (integer:6/real:1 or 2 or 3)
                        char tempbuf[col_len];//col_len = 7 or 8 or 9
                        memcpy(tempbuf,row_data+col_offset,col_len);
                        bool is_negative = false;
                        if(std::bitset<8>(tempbuf[0])[7] == 0){//음수일때 not +1
                            is_negative = true;
                            for(int i = 0; i<7; i++){
                                tempbuf[i] = ~tempbuf[i];//not연산
                            }
                            // tempbuf[6] = tempbuf[6] +1;//+1
                        }   
                        char integer[8];
                        memset(&integer, 0, 8);
                        for(int k=0; k<5; k++){
                            integer[k] = tempbuf[5-k];
                        }
                        int64_t ivalue = *((int64_t *)integer); 
                        double rvalue;
                        if(col_len == 7){
                            char real[1];
                            real[0] = tempbuf[6];
                            rvalue = *((int8_t *)real); 
                            rvalue *= 0.01;
                        }else if(col_len == 8){
                            char real[2];
                            real[0] = tempbuf[7];
                            real[1] = tempbuf[6];
                            rvalue = *((int16_t *)real); 
                            rvalue *= 0.0001;
                        }else if(col_len == 9){
                            char real[4];
                            real[0] = tempbuf[8];
                            real[1] = tempbuf[7];
                            real[2] = tempbuf[6];
                            real[3] = 0x00;
                            rvalue = *((int32_t *)real); 
                            rvalue *= 0.000001;
                        }else{
                            cout << "Mysql_newdecimal>else : " << col_len << endl;
                        }
                        double my_value = ivalue + rvalue;
                        if(is_negative){
                            my_value *= -1;
                        }
                        myWorkBuffer->table_data[col_name].floatvec.push_back(my_value);
                        // cout << col_name;
                        // printf("%f\n", my_value);
                        break;
                    }case MySQL_DATE:{
                        char tempbuf[col_len+1];
                        memcpy(tempbuf,row_data+col_offset,col_len);
                        tempbuf[3] = 0x00;
                        int64_t my_value = *((int *)tempbuf);
                        myWorkBuffer->table_data[col_name].intvec.push_back(my_value);
                        break;
                    }case MySQL_TIMESTAMP:{
                        //아직 처리X
                        char tempbuf[col_len];
                        memcpy(tempbuf,row_data+col_offset,col_len);
                        int my_value = *((int *)tempbuf);
                        myWorkBuffer->table_data[col_name].intvec.push_back(my_value);
                        break;
                    }case MySQL_STRING:{
                        char tempbuf[col_len+1];
                        memcpy(tempbuf,row_data+col_offset,col_len);
                        tempbuf[col_len] = '\0';
                        string my_value(tempbuf);
                        myWorkBuffer->table_data[col_name].strvec.push_back(my_value);
                        break;
                    }case MySQL_VARSTRING:{
                        char tempbuf[col_len];
                        if(col_len < 258){//0~257 (_1_,___256____)
                            memcpy(tempbuf,row_data+col_offset+1,col_len-1);
                            tempbuf[col_len-1] = '\0';
                        }else{//259~65535 (__2__,_____65535______)
                            memcpy(tempbuf,row_data+col_offset+2,col_len-2);
                            tempbuf[col_len-2] = '\0';
                        }
                        string my_value(tempbuf);
                        myWorkBuffer->table_data[col_name].strvec.push_back(my_value);
                        break;
                    }default:{
                        string msg = " error>> Type: " + to_string(col_type) + " is not defined!";
                        KETILOG(LOGTAG, msg);
                    }
                }
                myWorkBuffer->table_data[col_name].isnull.push_back(false);
                myWorkBuffer->table_data[col_name].row_count++;
            }
        }
    }else{
        for(size_t j=0; j<myWorkBuffer->table_column.size(); j++){
            col_name = myWorkBuffer->table_column[j];
            ColData coldata;
            myWorkBuffer->table_data[col_name] = coldata;
        }
        
    }
    // scheduler.csdworkdec(result.csd_name, result.result_block_count);
    //myWorkBuffer->left_block_count -= result.result_block_count;
    myWorkBuffer->row_count += result.row_count;
    myWorkBuffer->left_low_count -= result.raw_row_count;
    
    //KETILOG(LOGTAG,"Merging Data ... (Left Block : " + std::to_string(myWorkBuffer->left_low_count) + ")" + std::to_string(result.raw_row_count));

    if(myWorkBuffer->left_low_count <= 0){
        string msg = "Merging Data {" + to_string(qid) + "|" + to_string(wid) + "|" + myWorkBuffer->table_alias + "} Done";
        KETILOG(LOGTAG,msg);

        for(auto it = myWorkBuffer->table_data.begin(); it != myWorkBuffer->table_data.end(); it++){
            if((*it).second.floatvec.size() != 0){
                (*it).second.type = TYPE_FLOAT;
            }else if((*it).second.intvec.size() != 0){
                (*it).second.type = TYPE_INT;
            }else if((*it).second.strvec.size()!= 0){
                (*it).second.type = TYPE_STRING;
            }
        }
        
        myWorkBuffer->status = WorkDone;
        myWorkBuffer->cond.notify_all();
    }
}

int BufferManager::initWork(StorageEngineInstance::Snippet masked_snippet, int row_cnt){
    if(DataBuff.find(masked_snippet.query_id()) == DataBuff.end()){
        InitQuery(masked_snippet.query_id());
    }else if(DataBuff[masked_snippet.query_id()]->work_buffer_list.find(masked_snippet.work_id()) 
                != DataBuff[masked_snippet.query_id()]->work_buffer_list.end()){
        string msg = " error>> ID Duplicate Error {" + to_string(masked_snippet.query_id()) + "|" + to_string(masked_snippet.work_id()) + "}";
        KETILOG(LOGTAG, msg);
        return -1;            
    }   

    WorkBuffer* workBuffer = new WorkBuffer();

    string table_alias = masked_snippet.table_alias();
    workBuffer->table_alias = table_alias;
    for(int i = 0; i < masked_snippet.column_alias_size(); i++){
        workBuffer->table_column.push_back(masked_snippet.column_alias(i));
    }
    if(row_cnt != 0){
        workBuffer -> left_low_count = row_cnt;
    }
    // if(block_cnt != 0){
    //     workBuffer->left_block_count = block_cnt;
    // }

    DataBuff[masked_snippet.query_id()]->work_buffer_list[masked_snippet.work_id()] = workBuffer;
    DataBuff[masked_snippet.query_id()]->tablename_workid_map[table_alias] = masked_snippet.work_id();

    return 1;
}

void BufferManager::initQuery(int qid){
    QueryBuffer* queryBuffer = new QueryBuffer(qid);
    DataBuff.insert(pair<int,QueryBuffer*>(qid,queryBuffer));
}

int BufferManager::checkTableStatus(int qid, string tname){
    if(DataBuff.find(qid) == DataBuff.end()){
        cout << qid << endl;
        return QueryIDError;
    }else if(DataBuff[qid]->tablename_workid_map.find(tname) == DataBuff[qid]->tablename_workid_map.end()){
        cout << qid << tname << endl;
        return NonInitTable;
    }else{
        return DataBuff[qid]->work_buffer_list[DataBuff[qid]->tablename_workid_map[tname]]->status;
    }
}

int BufferManager::endQuery(int qid){
    if(DataBuff.find(qid) == DataBuff.end()){
        string msg = " error>> There's No Query ID {" + to_string(qid) + "}";
        KETILOG(LOGTAG, msg);
        return 0;
    }
    DataBuff.erase(qid);
    return 1;
}

TableData BufferManager::getTableData(int qid, string tname){ //flag : 스니펫 첫번째 테이블 여부 (중간 테이블일수도 있음)
    int MAX_TIME = 20;//wait 10sec
    TableData tableData;

    while(MAX_TIME > 0){
        int status = CheckTableStatus(qid,tname);

        if(status == NotFinished){
            cout << "<test> Not Finished " << qid << ":" << tname << endl;
            int wid = DataBuff[qid]->tablename_workid_map[tname];
            WorkBuffer* workBuffer = DataBuff[qid]->work_buffer_list[wid];
            unique_lock<mutex> lock(workBuffer->mu);
            workBuffer->cond.wait(lock);
            tableData.table_data = workBuffer->table_data;
            tableData.valid = true;
            tableData.row_count = workBuffer->row_count;
            //디버깅 출력
            for(auto i : workBuffer->table_data){
                cout << i.first << "|" << i.second.row_count << endl;
            }
            cout << "<test> Finished " << qid << ":" << tname << endl;
            break;
        }else if(status == WorkDone){
            int wid = DataBuff[qid]->tablename_workid_map[tname];
            WorkBuffer* workBuffer = DataBuff[qid]->work_buffer_list[wid];
            unique_lock<mutex> lock(workBuffer->mu);
            tableData.table_data = workBuffer->table_data;
            tableData.valid = true;
            tableData.row_count = workBuffer->row_count;
            // Debug Code   
            for(auto i : workBuffer->table_data){
                cout << i.first << "|" << i.second.row_count << "|" << i.second.type << endl;
            }
            cout << "<test> Done " << qid << ":" << tname << endl;
            break;
        }else{// QueryIDError | NonInitTable
            string msg = "QueryIDError | NonInitTable, wait phase : "+to_string(21-MAX_TIME)+" {"+to_string(qid)+"|"+tname+"}";
            KETILOG(LOGTAG,msg);
            sleep(1);
            MAX_TIME--;
        }
    }

    return tableData;
}
    
string BufferManager::getTableDataToString(int qid, string tname)
{
    TableData queryResult = BufferManager::GetTableData(qid, tname);
    StorageEngineInstance::QueryResult result;

    bool flag = true;
    int row_count = 0;

    if (queryResult.valid)
    {
        for (auto iter = queryResult.table_data.begin(); iter != queryResult.table_data.end(); ++iter)
        {
            string col_name = (*iter).first;
            StorageEngineInstance::Column col;

            switch ((*iter).second.type)
            {
            case TYPE_STRING:
            {
                col.set_col_type(StorageEngineInstance::Column_ColType_TYPE_STRING);
                for (size_t i = 0; i < (*iter).second.strvec.size(); i++)
                {
                    col.add_string_col((*iter).second.strvec[i]);
                }
                if (flag)
                {
                    row_count = (*iter).second.strvec.size();
                    flag = false;
                }
                break;
            }
            case TYPE_INT:
            {
                col.set_col_type(StorageEngineInstance::Column_ColType_TYPE_INT);
                for (size_t i = 0; i < (*iter).second.intvec.size(); i++)
                {
                    col.add_int_col((*iter).second.intvec[i]);
                }
                if (flag)
                {
                    row_count = (*iter).second.intvec.size();
                    flag = false;
                }
                break;
            }
            case TYPE_FLOAT:
            {
                col.set_col_type(StorageEngineInstance::Column_ColType_TYPE_FLOAT);
                for (size_t i = 0; i < (*iter).second.floatvec.size(); i++)
                {
                    col.add_double_col((*iter).second.floatvec[i]);
                }
                if (flag)
                {
                    row_count = (*iter).second.floatvec.size();
                    flag = false;
                }
                break;
            }
            }

            result.mutable_query_result()->insert({col_name, col});
        }
        result.set_row_count(queryResult.row_count);
    }

			stringstream result_string;
			int length = 18;
			std::string line((length+1) * result.query_result_size() - 1, '-');
			//Proto Buffer 형식 결과를 임시로 string으로 저장 -> 나중엔 DB Connector Instance 에서 출력		
			result_string << "+"+line+"+\n";
			const auto& my_map = result.query_result();
			
			result_string << "|";
			for (const auto& entry : my_map) {
				result_string << setw(length) << left << entry.first << "|";
			}
			result_string << "\n";

			result_string <<  "+"+line+"+\n";

			for(int i=0; i<result.row_count(); i++){
				result_string << "|";
				for (const auto& entry : my_map) {
					switch(entry.second.col_type()){
						case StorageEngineInstance::Column::TYPE_STRING:
							result_string << setw(length) << left << entry.second.string_col(i) << "|";
							break;
						case StorageEngineInstance::Column::TYPE_INT:
							result_string << setw(length) << left << to_string(entry.second.int_col(i)) << "|";
							break;
						case StorageEngineInstance::Column::TYPE_FLOAT:
							result_string << setw(length) << left << to_string(entry.second.double_col(i)) << "|";
							break;
					}
				}
				result_string << "\n";
			}

			result_string <<  "+"+line+"+\n";

			return result_string.str();
}

int BufferManager::saveTableData(int qid, string tname, TableData table_data_, int offset, int length){
    string msg = "Save Table {" + to_string(qid) + "|" + tname + "}";
    KETILOG(LOGTAG,msg);

    int wid = DataBuff[qid]->tablename_workid_map[tname];
    WorkBuffer* workBuffer = DataBuff[qid]->work_buffer_list[wid];
    unique_lock<mutex> lock(workBuffer->mu);

    cout << offset << " " << length << endl;
    for(auto &coldata: table_data_.table_data){
        ColData column;
        if(coldata.second.type == TYPE_STRING){
            column.strvec.assign(coldata.second.strvec.begin()+offset, coldata.second.strvec.begin()+length); 
            column.isnull.assign(coldata.second.isnull.begin()+offset, coldata.second.isnull.begin()+length);
            column.row_count = length - offset;
        }else if(coldata.second.type == TYPE_INT){
            column.intvec.assign(coldata.second.intvec.begin()+offset, coldata.second.intvec.begin()+length); 
            column.isnull.assign(coldata.second.isnull.begin()+offset, coldata.second.isnull.begin()+length);
            column.row_count = length - offset;
        }else if(coldata.second.type == TYPE_FLOAT){
            column.floatvec.assign(coldata.second.floatvec.begin()+offset, coldata.second.floatvec.begin()+length); 
            column.isnull.assign(coldata.second.isnull.begin()+offset, coldata.second.isnull.begin()+length);
            column.row_count = length - offset;
        }else if(coldata.second.type == TYPE_EMPTY){
        }else{
            cout << "save table row type check plz... " << endl;
        }
        column.type = coldata.second.type;
        workBuffer->table_data.insert({coldata.first, column});
    }

    workBuffer->row_count = length - offset;
    DataBuff[qid]->work_buffer_list[wid]->status = WorkDone;
    DataBuff[qid]->work_buffer_list[wid]->cond.notify_all();
    
    //디버깅 출력
    // for(auto it = DataBuff[qid]->work_buffer_list.begin(); it != DataBuff[qid]->work_buffer_list.end(); it++){
    //     cout << "workID: " << (*it).first << " tableName: " << (*it).second->table_alias << endl;
    // }
    // for(auto it = DataBuff[qid]->table_status.begin(); it != DataBuff[qid]->table_status.end(); it++){
    //     cout << "tableName: " << (*it).first << " workID: " << (*it).second.first << "status: " << (*it).second.second << endl;
    // }
    
    return 1;
}

// int BufferManager::DeleteTableData(int qid, string tname){ 
//     int status = CheckTableStatus(qid,tname);
//     switch(status){
//         case QueryIDError:
//         case NonInitTable:{
//             return -1;
//         }case NotFinished:{
//             int wid = DataBuff[qid]->tablename_workid_map[tname];
//             WorkBuffer* workBuffer = DataBuff[qid]->work_buffer_list[wid];
            
//             unique_lock<mutex> lock(workBuffer->mu);
//             workBuffer->cond.wait(lock);

//             workBuffer->table_data.clear();
//         }case WorkDone:{
//             int wid = DataBuff[qid]->tablename_workid_map[tname];
//             WorkBuffer* workBuffer = DataBuff[qid]->work_buffer_list[wid];
//             unique_lock<mutex> lock(workBuffer->mu);

//             workBuffer->table_data.clear();
//             return 0;
//         }
//     }

//     return 1;
// }

void getColOffset(const char* row_data, int* col_offset_list, vector<int> return_datatype, vector<int> table_offlen){
    int col_type = 0, col_len = 0, col_offset = 0, new_col_offset = 0, tune = 0;
    int col_count = return_datatype.size();

    for(int i=0; i<col_count; i++){
        col_type = return_datatype[i];
        col_len = table_offlen[i];
        new_col_offset = col_offset + tune;
        col_offset += col_len;
        if(col_type == MySQL_VARSTRING){
            if(col_len < 256){//0~255 -> 길이표현 1자리
                char var_len[1];
                var_len[0] = row_data[new_col_offset];
                uint8_t var_len_ = *((uint8_t *)var_len);
                tune += var_len_ + 1 - col_len;
            }else{//0~65535 -> 길이표현 2자리
                char var_len[2];
                var_len[0] = row_data[new_col_offset];
                int new_col_offset_ = new_col_offset + 1;
                var_len[1] = row_data[new_col_offset_];
                uint16_t var_len_ = *((uint16_t *)var_len);
                tune += var_len_ + 2 - col_len;
            }
        }

        col_offset_list[i] = new_col_offset;
    }
}

void calculForReturnData(StorageEngineInstance::Snippet snippet){
    unordered_map<string, int> col_type, col_offlen;
    for (int i = 0; i < snippet.table_col_size(); i++){
        col_type.insert(make_pair(snippet.table_col(i), snippet.table_datatype(i)));
        col_offlen.insert(make_pair(snippet.table_col(i), snippet.table_offlen(i)));
    }
    
    vector<int> return_datatype, return_offlen;
    for (int i = 0; i < snippet.column_projection_size(); i++){
        //tpc-h 쿼리 동작만 수행하도록 작성 => 수정필요
        if(snippet.column_projection(i).value(0) == "CASE"){
            return_datatype.push_back(2);
            return_offlen.push_back(4);
        }if(snippet.column_projection(i).value(0) == "EXTRACT"){
            return_datatype.push_back(14);
            return_offlen.push_back(3);
        }if(snippet.column_projection(i).value(0) == "SUBSTRING"){
            return_datatype.push_back(254);
            return_offlen.push_back(2);
        }else{
            if(snippet.column_projection(i).value_size() == 1){
                return_datatype.push_back(col_type[snippet.column_projection(i).value(0)]);
                return_offlen.push_back(col_offlen[snippet.column_projection(i).value(0)]);
            }else{
                int multiple_count = 0;
                for (int j = 0; j < snippet.column_projection(i).value_size(); j++){
                    if(snippet.column_projection(i).value(j) == "*"){
                        multiple_count++;
                    }
                }
                if(multiple_count == 1){
                    return_datatype.push_back(246);
                    return_offlen.push_back(8);
                }else if(multiple_count == 2){
                    return_datatype.push_back(246);
                    return_offlen.push_back(9);
                }else{
                    return_datatype.push_back(col_type[snippet.column_projection(i).value(0)]);
                    return_offlen.push_back(col_offlen[snippet.column_projection(i).value(0)]);
                }
            }
        }
    }

    //return return_datatype, return_offlen
}