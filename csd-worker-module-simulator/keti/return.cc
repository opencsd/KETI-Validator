#include "return.h"
#include "httplib.h"

void Return::ReturnResult(){
    while (1){
        MergeResult mergeResult = ReturnQueue.wait_and_pop();
                
        SendDataToBufferManager(mergeResult);
    }
}

void Return::SendDataToBufferManager(MergeResult &mergeResult){
    float temp_size = float(mergeResult.length) / float(1024);
    memset(msg, '\0', sizeof(msg));
    sprintf(msg,"Send Data to Buffer Manager (Return Buffer Size : %.1fK)\n\n",temp_size);
    KETILOG::DEBUGLOG(LOGTAG, msg);

    if(mergeResult.total_block_count == mergeResult.current_block_count){
        if(KETILOG::GetLogLevel() == METRIC){
            httplib::Client cli("localhost",40502);
            auto res = cli.Get("/endCSD");
        }
        
        memset(msg, '\0', sizeof(msg));
        sprintf(msg,"Snippet {ID : %d-%d} Done\n",mergeResult.query_id,mergeResult.work_id);
        KETILOG::INFOLOG(LOGTAG, msg);
    }

    StringBuffer block_buf;
    PrettyWriter<StringBuffer> writer(block_buf);

    writer.StartObject();

    writer.Key("queryID");
    writer.Int(mergeResult.query_id);

    writer.Key("workID");
    writer.Int(mergeResult.work_id);

    writer.Key("rowCount");
    writer.Int(mergeResult.row_count);

    writer.Key("rowOffset");
    writer.StartArray();
    for (int i = 0; i < mergeResult.row_offset.size(); i++){
        writer.Int(mergeResult.row_offset[i]);
    }
    writer.EndArray();

    writer.Key("length");
    writer.Int(mergeResult.length);

    writer.Key("returnDatatype");
    writer.StartArray();
    for (int i = 0; i < mergeResult.projection_datatype.size(); i++){
        writer.Int(mergeResult.projection_datatype[i]);
    }
    writer.EndArray();

    writer.Key("returnOfflen");
    writer.StartArray();
    for (int i = 0; i < mergeResult.projection_length.size(); i++){
        writer.Int(mergeResult.projection_length[i]);
    }
    writer.EndArray();

    writer.Key("csdName");
    writer.String(mergeResult.csd_name.c_str());

    writer.Key("resultBlockCount");
    writer.Int(mergeResult.result_block_count);

    writer.EndObject();

    string block_buf_ = block_buf.GetString();

    int sockfd;
    struct sockaddr_in serv_addr;
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(BUFF_M_IP);
    serv_addr.sin_port = htons(BUFF_M_PORT);

    connect(sockfd,(const sockaddr*)&serv_addr,sizeof(serv_addr));
	
	size_t len = strlen(block_buf_.c_str());
	send(sockfd, &len, sizeof(len), 0);
    send(sockfd, (char*)block_buf_.c_str(), len, 0);

    static char cBuffer[PACKET_SIZE];
    if (recv(sockfd, cBuffer, PACKET_SIZE, 0) == 0){
        KETILOG::FATALLOG(LOGTAG,"client recv Error");
        return;
    };

    len = mergeResult.length;
    send(sockfd,&len,sizeof(len),0);
    send(sockfd, mergeResult.data, BUFF_SIZE, 0);
    
    close(sockfd);
}