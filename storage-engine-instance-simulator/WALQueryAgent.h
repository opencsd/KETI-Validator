// #pragma once

// #include <iostream>
// #include <sys/socket.h>
// #include <netinet/in.h> 
// #include <arpa/inet.h>
// #include <unistd.h>
// #include <string.h>

// #include "rapidjson/document.h"
// #include "rapidjson/writer.h"
// #include "rapidjson/stringbuffer.h"
// #include "rapidjson/prettywriter.h" 

// #include <grpcpp/grpcpp.h>
// #include "storage_engine_instance.grpc.pb.h"

// #include "keti_log.h"
// #include "ip_config.h"

// using namespace rapidjson;

// using grpc::Channel;
// using grpc::ClientContext;
// using grpc::Status;

// using StorageEngineInstance::WALManager;
// using StorageEngineInstance::WALRequest;
// using StorageEngineInstance::WALResponse;

// #define BUFF_SIZE 4096

// class WALManager_Interface {
    
// public:
//     WALManager_Interface(std::shared_ptr<Channel> channel) : stub_(WALManager::NewStub(channel)) {}

//     WALResponse RequestWAL(WALRequest walRequset) {
//         WALResponse walResponse;
//         ClientContext context;
        
//         Status status = stub_->RequestWAL(&context, walRequset, &walResponse);
//         // cout << "result : " << result.value() << endl;

//         if (!status.ok()) {
//             std::cout << status.error_code() << ": " << status.error_message() << std::endl;
//             std::cout << "RPC failed";
//         }

//         return walResponse;
//     }

// private:
//     std::unique_ptr<WALManager::Stub> stub_;
// };

// class WALQueryAgent {

// public:
//     inline const static std::string LOGTAG = "Monitoring Container::WAL Query Agent";

//     static void RequestWAL(int qid, int wid, string tname, MetaDataResponse* response){
//         KETILOG(LOGTAG, "# Request WAL Called");

//         string key = TableManager::makeKey(qid,wid);

//         WALRequest walRequest;
//         walRequest.set_req_key(key);
//         walRequest.set_type("table");
//         walRequest.set_value(tname);

//         WALManager_Interface walm(grpc::CreateChannel((string)CSD_WAL_MANAGER_IP+":"+to_string(CSD_WAL_MANAGER_PORT), grpc::InsecureChannelCredentials()));
//         WALResponse walResponse = walm.RequestWAL(walRequest);

//         StringBuffer buffer;
//         buffer.Clear();
//         Writer<StringBuffer> writer(buffer);
//         writer.StartObject();
//         writer.Key("blockList");
        
//         for(int i=0; i<walResponse.deleted_key_size(); i++){

//         }

//         for(int i=0; i<walResponse.inserted_key_size(); i++){

//         }

//         response->set_wal_json(buffer.GetString());
//     }

// private:
//     WALQueryAgent(){};
//     WALQueryAgent(const WALQueryAgent&);
//     WALQueryAgent& operator=(const WALQueryAgent&){
//         return *this;
//     }

//     static WALQueryAgent& GetInstance(){
//         static WALQueryAgent _instance;
//         return _instance;
//     }
// };

