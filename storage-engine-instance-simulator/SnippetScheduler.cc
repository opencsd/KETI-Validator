#include "SnippetScheduler.h"
#include "keti_log.h"
using StorageEngineInstance::Snippet;
using StorageEngineInstance::MetaDataResponse_PBAInfo;

void Scheduler::runScheduler(){ //스니펫 큐 확인해서 스니펫 들어오면 스케줄링 수행
    while (1){
        Object scheduling_target = Scheduler::PopQueue();

        string id = "{" + to_string(scheduling_target.snippet.query_id()) + "|" + to_string(scheduling_target.snippet.work_id()) + "}";
        KETILOG(LOGTAG, "# Snippet Scheduling Start " + id);
        vector<TableManager::SSTFile> SSTList = scheduling_target.targetTable.SSTList;

        int sock;

        

        for(int i =0; i < SSTList.size(); i++){
            //Scheduler::scheduling(scheduling_target, SSTList[i].filename, SSTList[i].csdName);

            sock = socket(PF_INET, SOCK_STREAM, 0);
            if(sock < 0){
                cout <<"Failed to create socket" <<endl;
                continue;
            }
            string bestcsd = SSTList[i].csdName;
            struct sockaddr_in serv_addr;
            memset(&serv_addr, 0, sizeof(serv_addr));
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_addr.s_addr = inet_addr(CSD_PROXY_IP);
            int csdNum = stoi(bestcsd);
            switch (csdNum)
            {
            case 1:
                serv_addr.sin_port = htons(CSD_1_PORT);
                break;
            case 2:
                serv_addr.sin_port = htons(CSD_2_PORT);
                break;
            case 3:
                serv_addr.sin_port = htons(CSD_3_PORT);
                break;
            case 4:
                serv_addr.sin_port = htons(CSD_4_PORT);
                break;
            case 5:    
                serv_addr.sin_port = htons(CSD_5_PORT);
                break;
            case 6:
                serv_addr.sin_port = htons(CSD_6_PORT);
                break;
            case 7:
                serv_addr.sin_port = htons(CSD_7_PORT);
                break;
            case 8:
                serv_addr.sin_port = htons(CSD_8_PORT);
                break;
            default:
                cout<<"server address error"<<endl;
                break;
            }


            if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
            // handle error
            cout <<ntohs(serv_addr.sin_port)<< " Failed to connect to server" << endl;
            close(sock);
            //sock = socket(PF_INET, SOCK_STREAM, 0);
            //memset(&serv_addr, 0, sizeof(serv_addr));
            //serv_addr.sin_family = AF_INET;
            //serv_addr.sin_addr.s_addr = inet_addr(LOCALHOST);
            //serv_addr.sin_port = htons(CSD_PROXY_PORT);
            connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
            continue;
            }
            cout<<"Connect"<<endl;

            StringBuffer snippetbuf;
            snippetbuf.Clear();
            serialize(snippetbuf, scheduling_target.snippet, bestcsd);
            
            KETILOG(LOGTAG, "# Send Snippet(" + SSTList[i].filename + ") to CSD Worker Module [CSD" + bestcsd + "]");
            string snippetJson = snippetbuf.GetString();
            // std::thread trd(&Scheduler::sendSnippetToCSD, &Scheduler::GetInstance(), snippetJson ,csdIP);
            // trd.detach();

            size_t len = strlen(snippetJson.c_str());
            send(sock, &len, sizeof(len), 0);
            send(sock, (char *)snippetJson.c_str(), strlen(snippetJson.c_str()), 0);
            cout<<SSTList[i].filename<<endl;
            close(sock);
        }
    }
}

void Scheduler::scheduling(Object &scheduling_target, string sst_name, string bestCSD){
    //get best csd
    string bestcsd = bestCSD;
    KETILOG(LOGTAG, "got best CSD");
    StringBuffer snippetbuf;
    snippetbuf.Clear();
    serialize(snippetbuf, scheduling_target.snippet, bestcsd);

    //send snippet
    KETILOG(LOGTAG, "# Send Snippet(" + sst_name + ") to CSD Worker Module [CSD" + bestcsd + "]");
    string snippetJson = snippetbuf.GetString();
    // CSD IP 형식 : "10.1.${id}.2" 
    //         ex) 10.1.1.2 ~ 10.1.8.2
     string csdIP = "";
    // std::thread trd(&Scheduler::sendSnippetToCSD, &Scheduler::GetInstance(), snippetJson ,csdIP);
    // trd.detach();
    sendSnippetToCSD(snippetJson,csdIP);

}

void Scheduler::serialize(StringBuffer &buff, Snippet &snippet, string bestcsd) {
    Writer<StringBuffer> writer(buff);

    // writer.StartObject();
    // writer.Key("Snippet");
    writer.StartObject();

    writer.Key("queryID");
    writer.Int(snippet.query_id());

    writer.Key("workID");
    writer.Int(snippet.work_id());

    writer.Key("tableName");
    writer.String(snippet.table_name(0).c_str());

    writer.Key("tableCol");
    writer.StartArray();
    for (int i = 0; i < snippet.table_col_size(); i++){
        writer.String(snippet.table_col(i).c_str());
    }
    writer.EndArray();

    writer.Key("tableFilter");
    writer.StartArray();
    if(snippet.table_filter().size() > 0){
        for (int i = 0; i < snippet.table_filter().size(); i++)
        {
            writer.StartObject();
            if (snippet.table_filter()[i].lv().type().size() > 0)
            {
                writer.Key("LV");
                
                if(snippet.table_filter()[i].lv().type(0) == 10){
                    writer.String(snippet.table_filter()[i].lv().value(0).c_str());
                }
            }
            writer.Key("OPERATOR");
            writer.Int(snippet.table_filter()[i].operator_());

            if(snippet.table_filter()[i].operator_() == 8 || snippet.table_filter()[i].operator_() == 9){
                writer.Key("EXTRA");
                writer.StartArray();

                for(int j = 0; j < snippet.table_filter()[i].rv().type().size(); j++){
                    if(snippet.table_filter()[i].rv().type(j) != 10){
                        if(snippet.table_filter()[i].rv().type(j) == 7 || snippet.table_filter()[i].rv().type(j) == 3){
                            string tmpstr = snippet.table_filter()[i].rv().value(j);
                            int tmpint = atoi(tmpstr.c_str());
                            writer.Int(tmpint);
                        }else if(snippet.table_filter()[i].rv().type(j) == 9){
                            string tmpstr = snippet.table_filter()[i].rv().value(j);
                            tmpstr = "+" + tmpstr;
                            writer.String(tmpstr.c_str());
                        }else if(snippet.table_filter()[i].rv().type(j) == 4 || snippet.table_filter()[i].rv().type(j) == 5){
                            string tmpstr = snippet.table_filter()[i].rv().value(j);
                            double tmpfloat = stod(tmpstr);
                            writer.Double(tmpfloat);
                        }else{
                            string tmpstr = snippet.table_filter()[i].rv().value(j);
                            tmpstr = "+" + tmpstr;
                            writer.String(tmpstr.c_str());
                        }
                    }else{
                        writer.String(snippet.table_filter()[i].rv().value(j).c_str());
                    }
                }
                writer.EndArray();
            } else if (snippet.table_filter()[i].rv().type().size() > 0)
            {
                writer.Key("RV");

                if(snippet.table_filter()[i].rv().type(0) != 10){
                    if(snippet.table_filter()[i].rv().type(0) == 7 || snippet.table_filter()[i].rv().type(0) == 3){
                        string tmpstr = snippet.table_filter()[i].rv().value(0);
                        int tmpint = atoi(tmpstr.c_str());
                        writer.Int(tmpint);
                    }else if(snippet.table_filter()[i].rv().type(0) == 9){
                        string tmpstr = snippet.table_filter()[i].rv().value(0);
                        tmpstr = "+" + tmpstr;
                        writer.String(tmpstr.c_str());
                    }else if(snippet.table_filter()[i].rv().type(0) == 4 || snippet.table_filter()[i].rv().type(0) == 5){
                        string tmpstr = snippet.table_filter()[i].rv().value(0);
                        double tmpfloat = stod(tmpstr);
                        writer.Double(tmpfloat);
                    }else{
                        string tmpstr = snippet.table_filter()[i].rv().value(0);
                        tmpstr = "+" + tmpstr;
                        writer.String(tmpstr.c_str());
                    }
                }else{
                    writer.String(snippet.table_filter()[i].rv().value(0).c_str());
                }
            }

            writer.EndObject();
        }
    }
    writer.EndArray();
    
    // writer.Key("columnFiltering");
    // writer.StartArray();
    // for (int i = 0; i < snippet.column_filtering_size(); i++)
    // {
    //     writer.String(snippet.column_filtering()[i].c_str());
    // }
    // writer.EndArray();

    writer.Key("columnProjection");
    writer.StartArray();
    for (int i = 0; i < snippet.column_projection().size(); i++){
        writer.StartObject();
        writer.Key("selectType");
        writer.Int(snippet.column_projection(i).select_type()); 

        for(int j = 0; j < snippet.column_projection(i).value_size(); j++){
            if(j==0){
                writer.Key("value");
                writer.StartArray();
                writer.String(snippet.column_projection(i).value(j).c_str());
            }
            else{
                writer.String(snippet.column_projection(i).value(j).c_str());
            }
        }
        writer.EndArray();
        for(int j = 0; j < snippet.column_projection(i).value_type_size(); j++){
            if(j == 0){
                writer.Key("valueType");
                writer.StartArray();
                writer.Int(snippet.column_projection(i).value_type(j));
            }else{
                writer.Int(snippet.column_projection(i).value_type(j));
            }
        }
        writer.EndArray();

        if(snippet.column_projection(i).value_size() == 0){
            writer.Key("value");
            writer.StartArray();
            writer.EndArray();
            writer.Key("valueType");
            writer.StartArray();
            writer.EndArray();
        }
        writer.EndObject();
    }
    writer.EndArray();

    writer.Key("tableOffset");
    writer.StartArray();
    for (int i = 0; i < snippet.table_offset_size(); i++){
        writer.Int(snippet.table_offset()[i]);
    }
    writer.EndArray();

    writer.Key("tableOfflen");
    writer.StartArray();
    for (int i = 0; i < snippet.table_offlen_size(); i++){
        writer.Int(snippet.table_offlen()[i]);
    }
    writer.EndArray();

    writer.Key("tableDatatype");
    writer.StartArray();
    for (int i = 0; i < snippet.table_datatype_size(); i++){
        writer.Int(snippet.table_datatype()[i]);
    }
    writer.EndArray();

    // writer.Key("pba");
    // writer.RawValue(pba.c_str(), strlen(pba.c_str()), kObjectType);

    writer.Key("primaryKey");
    writer.Int(snippet.pk_num());

    writer.Key("csdName");
    writer.String(bestcsd.c_str());
    writer.EndObject();

    // string csdIP = "10.1."+bestcsd+".2"; // CSD ip
    // writer.Key("csdIP");
    // writer.String(csdIP.c_str());
    // writer.EndObject();
}

void Scheduler::sendSnippetToCSD(string snippet_json, string csdIP){
    int sock;
    struct sockaddr_in serv_addr;
    sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;

    /*Send Snippet To CSD Proxy*/
    // 
     serv_addr.sin_addr.s_addr = inet_addr(CSD_PROXY_IP);
     serv_addr.sin_port = htons(CSD_PROXY_PORT);
     connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    cout<<"Send"<<endl;
     {
         // Check CSD Snippet Request - Debug Code  
         cout << endl << snippet_json.c_str() << endl << endl;
     }
    

     size_t len = strlen(snippet_json.c_str());
     send(sock, &len, sizeof(len), 0);
     send(sock, (char *)snippet_json.c_str(), strlen(snippet_json.c_str()), 0);
}

string Scheduler::bestCSD(Object &scheduling_target, string sst_name, MetaDataResponse_PBAInfo &pba_info){
    string bestcsd;
    
    const auto& my_map = pba_info.csd_pba_map();

    for (const auto& entry : my_map) {
       bestcsd = entry.first; 
    }

    return bestcsd;
}

string Scheduler::DPG(Object &scheduling_target, string sst_name, MetaDataResponse_PBAInfo pba_info){
    string bestcsd;

    return bestcsd;
}

string Scheduler::RR(Object &scheduling_target, string sst_name, MetaDataResponse_PBAInfo pba_info){
    string bestcsd;

    return bestcsd;
}

vector<string> ScheduleSplit(string str, char Delimiter)
{
    istringstream iss(str); // istringstream에 str을 담는다.
    string buffer;          // 구분자를 기준으로 절삭된 문자열이 담겨지는 버퍼

    vector<string> result;

    // istringstream은 istream을 상속받으므로 getline을 사용할 수 있다.
    while (getline(iss, buffer, Delimiter))
    {
        result.push_back(buffer); // 절삭된 문자열을 vector에 저장
    }

    return result;
}

MetaDataResponse Scheduler::getSchedulingTarget(int qid, int wid){
    MetaDataResponse schedulerinfo;

    string key = TableManager::makeKey(qid, wid);
    Response *data = TableManager::GetReturnData(key);
    unique_lock<mutex> lock(data->mu);
    if (!data->is_done)
    {
      data->cond.wait(lock);
    }
    schedulerinfo.CopyFrom(*data->metadataResponse);

    return schedulerinfo;
}