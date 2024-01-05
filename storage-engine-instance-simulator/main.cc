#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <unistd.h>

#include "ip_config.h"
#include "SnippetManager.h"
#include "BufferManager.h"

#include <grpcpp/grpcpp.h>
#include "storage_engine_instance.grpc.pb.h"
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;
using StorageEngineInstance::InterfaceContainer;
using StorageEngineInstance::Request;
using StorageEngineInstance::Result;
using StorageEngineInstance::Snippet;
using StorageEngineInstance::SnippetRequest;

using namespace std;

// Logic and data behind the server's behavior.
class InterfaceContainerServiceImpl final : public InterfaceContainer::Service
{
  Status SetSnippet(ServerContext *context,
                    ServerReaderWriter<Result, SnippetRequest> *stream) override
  {
    SnippetRequest snippet_request;
    bool flag = true;

    while (stream->Read(&snippet_request))
    {
      if (flag)
      {
        string msg = "==:Set Snippet:== {" + to_string(snippet_request.snippet().query_id()) + "}";
        //KETILOG("Interface Container", msg);
        SnippetManager::InsertQueryID(snippet_request.snippet().query_id());
        flag = false;
      }

      // Check Recv Snippet - Debug Code
      {
        std::string test_json;
        google::protobuf::util::JsonPrintOptions options;
        options.always_print_primitive_fields = true;
        options.always_print_enums_as_ints = true;
        google::protobuf::util::MessageToJsonString(snippet_request, &test_json, options);
        std::cout << endl
                  << test_json << std::endl
                  << std::endl;
      }

      SnippetManager::PushQueue(snippet_request);
    }
    cout<<snippet_request.snippet().table_alias()<<endl;
    SnippetManager::SetTableName(snippet_request.snippet().query_id(), snippet_request.snippet().table_alias());

    return Status::OK;
  }

  Status Run(ServerContext *context, const Request *request, Result *result) override
  {
    int query_id = request->query_id();
    string table_name = SnippetManager::GetTableName(query_id); // 최종 테이블 네임
    cout<<"======="<<table_name<<"======"<<endl;

    //KETILOG("Interface Container", "==:RUN:== {" + to_string(query_id) + "}");

    string msg = "==:Get Query Result:== {" + to_string(request -> query_id()) + "|" + table_name + "}";
    //KETILOG("Merging Container", msg);

    string result_string = BufferManager::GetTableDataToString(request -> query_id(), table_name);
    result->set_value(result_string);

    SnippetManager::EraseQueryID(query_id);
    
    msg = "==:End Query:== {" + to_string(query_id) + "}";
    //KETILOG("Merging Container", msg);
    BufferManager::EndQuery(query_id);
    //end here
    return Status::OK;
  }
};

void RunServer()
{
  std::string server_address("0.0.0.0:" + std::to_string(SE_INTERFACE_CONTAINER_PORT));
  InterfaceContainerServiceImpl service;

  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case, it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  //KETILOG("Interface Container", "Storage Engine Interface Server Listening on " + server_address);

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

int main(int argc, char **argv)
{
    TableManager::InitTableManager();
  BufferManager::InitBufferManager(); // Run Buffer Manager TCP/IP Server
  RunServer();

  return 0;
}