#pragma once

#include <thread>
#include <unordered_map>
#include <string.h>

#include <google/protobuf/util/field_mask_util.h>

#include "ip_config.h"
#include "keti_log.h"

#include "MergeQueryManager.h"
#include "SnippetScheduler.h"
#include "LBA2PBAQueryAgent.h"
#include "TableManager.h"
using google::protobuf::FieldMask;
using StorageEngineInstance::SnippetRequest;
using StorageEngineInstance::Snippet;
using StorageEngineInstance::TableBlockCount;

class SnippetManager {
/* Variables */
public:
    inline const static std::string LOGTAG = "Interface Container::Snippet Manager";

private:
    kQueue<SnippetRequest> SnippetQueue;
    unordered_map<int,string> QueryList; //key:queryID, value:table name

/* Methods */
public:
    static void PushQueue(SnippetRequest snippet){
      GetInstance().SnippetQueue.push_work(snippet);
    }

    static SnippetRequest PopQueue(){
      SnippetRequest blockResult = GetInstance().SnippetQueue.wait_and_pop();
      return blockResult;
    }

    static void InsertQueryID(int qid){
        GetInstance().QueryList.insert({qid,""});
    }

    static void EraseQueryID(int qid){
        GetInstance().QueryList.erase(qid);
    }

    static void SetTableName(int qid, string tname){
        GetInstance().QueryList[qid] = tname;
    }

    static string GetTableName(int qid){
        return GetInstance().QueryList[qid];
    }

private:
    SnippetManager() {
        /* Init Snippet Container */
        /* Start Thread */
        std::thread thd(&SnippetManager::runSnippetManager,this);
        thd.detach();
    };
    SnippetManager(const SnippetManager&);
    SnippetManager& operator=(const SnippetManager&){
        return *this;
    };
    
    static SnippetManager& GetInstance() {
        static SnippetManager _instance;
        return _instance;
    }

    void runSnippetManager(){
        while(1) {
            SnippetRequest snippetrequest = PopQueue();
            
            {
            // // Check Recv Snippet - Debug Code   
            // std::string test_json;
            // google::protobuf::util::JsonPrintOptions options;
            // options.always_print_primitive_fields = true;
            // options.always_print_enums_as_ints = true;
            // google::protobuf::util::MessageToJsonString(snippetrequest,&test_json,options);
            // std::cout << "Popped Snippet to JSON" << std::endl;
            // std::cout << test_json << std::endl << std::endl;
            }        

            /* Request Set MetaData */
            Snippet snippet = snippetrequest.snippet(); 
            if(snippetrequest.type() == StorageEngineInstance::SnippetRequest_SnippetType_SCAN_SNIPPET) {
                //KETILOG(LOGTAG,"# Request Monitoring Container Setting Metadata");
                //LBA2PBAQueryAgent::RequestPBA(snippetrequest.snippet().query_id(),snippetrequest.snippet().work_id(),snippetrequest.snippet().table_name(0));
            }

            /* Init Buffer Manager */
            KETILOG(LOGTAG,"# Init Buffer Manager");                
            
            // FieldMask mask;
            // mask.add_paths("query_ID");
            // mask.add_paths("work_ID");
            // mask.add_paths("table_alias");
            // mask.add_paths("column_alias");
            
            // // Snippet masked_snippet;
            // // masked_snippet.CopyFrom(snippetrequest.snippet());
            // Snippet masked_snippet = snippetrequest.snippet();
            // google::protobuf::util::FieldMaskUtil::TrimMessage(mask, &masked_snippet);
            

            //Init Buffer


            if(snippetrequest.type() == StorageEngineInstance::SnippetRequest_SnippetType_SCAN_SNIPPET){
                // GET CSD BlockInfo
                // string key = TableManager::makeKey(snippet.query_id(), snippet.work_id());
                // KETILOG(LOGTAG,"# TableManager Make Key");     
                // Response *data = TableManager::GetReturnData(key);
                // KETILOG(LOGTAG,"# TableManager Get Return Data");     
                // unique_lock<mutex> lock(data->mu);
                // if (!data->is_done)
                // {
                //         data->cond.wait(lock);
                // }
                // row_cnt = data -> row_count;
            }   
                        KETILOG(LOGTAG,"# Init Buffer Manager");         

            {
            // // Check Masked Snippet - Debug Code   
            // std::string test_json;
            // google::protobuf::util::JsonPrintOptions options;
            // options.always_print_primitive_fields = true;
            // options.always_print_enums_as_ints = true;
            // google::protobuf::util::MessageToJsonString(masked_snippet,&test_json,options);
            // std::cout << "Masked Snippet to JSON" << std::endl;
            // std::cout << test_json << std::endl << std::endl;
            }

            /* Send Snippet to Container */
            if(snippetrequest.type() == StorageEngineInstance::SnippetRequest_SnippetType_SCAN_SNIPPET) {
                KETILOG(LOGTAG,"# Send Snippet to Offloading Container");
                                cout<<snippetrequest.snippet().table_name(0)<<endl;
                TableManager::Table targetTable = TableManager::GetTable(snippetrequest.snippet().table_name(0));
                cout<<"check1"<<endl;
                BufferManager::InitWork(snippet, targetTable.tablerow);
                                cout<<"check2"<<endl;
                SetTableName(snippet.query_id(), targetTable.tablename);
                                cout<<"check3"<<endl;
                Object schedulingTarget;
                schedulingTarget.snippet = snippet;
                schedulingTarget.targetTable = targetTable;
                                cout<<"check4"<<endl;
                Scheduler::PushQueue(schedulingTarget);
            } 
            else { // Aggregation 
                KETILOG(LOGTAG,"# Send Snippet to Merging Container");
                            BufferManager::InitWork(snippet, 0);
                thread MergeQueryInstance = thread(&MergeQueryManager::RunSnippetWork, MergeQueryManager(snippetrequest));
                MergeQueryInstance.detach();
            }
        }
    }
};