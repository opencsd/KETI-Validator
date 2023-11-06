#include "DB_Connector_Instance.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h" 
#include "keti_util.h"
#include "thread"

#include <ctime>

using namespace rapidjson;

DB_Connector_Instance::DB_Connector_Instance() : storageEngineInterface_(grpc::CreateChannel("10.0.4.86:40001", grpc::InsecureChannelCredentials())), meta_data_manager_("10.0.4.86:40001")
{
    //ctor
}
DB_Connector_Instance::DB_Connector_Instance(utility::string_t url):m_listener(url), storageEngineInterface_(grpc::CreateChannel("10.0.4.86:40001", grpc::InsecureChannelCredentials())), meta_data_manager_("10.0.4.86:40001")
{
    cout << "url: " << url << endl;
    m_listener.support(methods::GET, std::bind(&DB_Connector_Instance::handle_get, this, std::placeholders::_1));
    m_listener.support(methods::PUT, std::bind(&DB_Connector_Instance::handle_put, this, std::placeholders::_1));
    m_listener.support(methods::POST, std::bind(&DB_Connector_Instance::handle_post, this, std::placeholders::_1));
    m_listener.support(methods::DEL, std::bind(&DB_Connector_Instance::handle_delete, this, std::placeholders::_1));
}
DB_Connector_Instance::~DB_Connector_Instance()
{
    //dtor
}

void DB_Connector_Instance::handle_error(pplx::task<void>& t)
{
    try
    {
        t.get();
    }
    catch(...)
    {
        // Ignore the error, Log it if a logger is available
    }
}


//
// Get Request 
//
void DB_Connector_Instance::handle_get(http_request message)
{
    struct timespec  begin, end;
    clock_gettime(CLOCK_MONOTONIC, &begin);
    std::string uri_path = message.relative_uri().to_string();
    cout<<uri_path<<endl;

    auto body_json = message.extract_string();
    std::string json = utility::conversions::to_utf8string(body_json.get());
    cout << json << endl;
    
    Document document;
    document.Parse(json.c_str());

    if(!uri_path.find("/simulate/log/compare/")){

    }
    else if(!uri_path.find("/simulate/log/option/")){

    }
    else if(!uri_path.find("/simulate/option")){

    }
    else if(uri_path == "/simulate/log"){

    }

    // std::string uri_path = message.relative_uri().to_string();
    // cout<<uri_path<<endl;

    // clock_t start, end;
    // double time;

    // start = clock();

    // auto body_json = message.extract_string();
    // std::string json = utility::conversions::to_utf8string(body_json.get());
    
    // Document document;
    // document.Parse(json.c_str());
    
    // Parsed_Query parsed_query(document["query"].GetString());
    
    // keti_log("DB Connector Instance","\t------------:: STEP 1 ::------------");
    // keti_log("DB Connector Instance","Recv Query");
    // query_planner_.Parse(meta_data_manager_,parsed_query);
    // std::string rep = plan_executer_.Execute_Query(storageEngineInterface_,parsed_query) + "\n";
    
    // message.reply(status_codes::OK,rep);

    // end = clock();
    // time = (double)(end-start)/CLOCKS_PER_SEC;
    
    // keti_log("DB Connector Instance","End Query time :" + std::to_string(time));

    // return;

};

//
// A POST request
//
void DB_Connector_Instance::handle_post(http_request message)
{    

    std::string uri_path = message.relative_uri().to_string();
    //cout<<uri_path<<endl;

    auto body_json = message.extract_string();
    std::string json = utility::conversions::to_utf8string(body_json.get());
    //cout << json << endl;
    
    Document document;
    document.Parse(json.c_str());

    if(uri_path == "/simulate/template"){

    }
    else if(uri_path == "/simulate/simulation"){

    }
    else if(uri_path == "/simulate/option/new"){

    }
    else if(uri_path == "/simulate"){
            struct timespec  begin, end;
            clock_gettime(CLOCK_MONOTONIC, &begin);
            Parsed_Query parsed_query(document["query"].GetString());
            keti_log("DB Connector Instance","Recv Query");
            query_planner_.Parse(meta_data_manager_,parsed_query);
            std::string rep = plan_executer_.Execute_Query(storageEngineInterface_,parsed_query) + "\n";
    
            clock_gettime(CLOCK_MONOTONIC, &end);
            double time = (double)(end.tv_sec - begin.tv_sec) + (double)(end.tv_nsec - begin.tv_nsec)/1000000000;

            message.reply(status_codes::OK,/*rep + */"End Query time : " + std::to_string(time) + "s");
    
            keti_log("DB Connector Instance","End Query time : " + std::to_string(time) + "s");
    }
    else if(uri_path == "/simulate/recording/CSD"){
        struct timespec begin, end;
        clock_gettime(CLOCK_MONOTONIC, &begin);
        keti_log("Validator","Validator Receive Query, Option");
        Parsed_Query parsed_query(document["query"].GetString());
        query_planner_.Parse(meta_data_manager_,parsed_query);
        keti_log("Validator",parsed_query.Get_Parsed_Query());
        keti_log("Validator","Option Name : Option1");
        keti_log("Validator","DBMS : Mysql");
        keti_log("Validator","Storage Type : CSD");
        keti_log("Validator","CSD Count : 8");
        keti_log("Validator","CSD Kind : NGD");
        keti_log("Validator","Block Count : 15");
        keti_log("Validator","Algorithm : CSD Metric Score");
        keti_log("Validator","Using Index : False");
        keti_log("Validator","Validation Start");
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        cout<<"----------------------------------------------------------------"<<endl;
        keti_log("Validator","CSD Validator Start...");
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        keti_log("Validator","Send Snippet (WorkID : 0) to CSD Validator");
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        keti_log("Validator","Send Snippet (WorkID : 1) to CSD Validator");
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        keti_log("Validator","Send Snippet (WorkID : 2) to CSD Validator");
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        keti_log("Validator","Send Snippet (WorkID : 3) to Storage Validator");
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        keti_log("Validator","Send Snippet (WorkID : 4) to Storage Validator");
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        keti_log("Validator","Send Snippet (WorkID : 5) to Storage Validator");
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        keti_log("Validator","Send Snippet (WorkID : 6) to Storage Validator");
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        keti_log("CSD Validator","Scanning Histogram Table Partsupp, (WorkId : 0)");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        keti_log("CSD Validator","Scanned Row : 800000, Filtered Row : 800000");
        keti_log("CSD Validator","Scanning Histogram Table supplier, (WorkId : 1)");
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        keti_log("CSD Validator","Scanned Row : 10000, Filtered Row : 10000");
        keti_log("CSD Validator","Scanning Histogram Table nation, (WorkId : 2)");
        keti_log("CSD Validator","Scanned Row : 25, Filtered Row : 1");

        cout<<"----------------------------------------------------------------"<<endl;

        keti_log("Validator","Storage Validator Start...");
        keti_log("Storage Validator","Join Snippet, (WorkId : 3)");
        keti_log("Storage Validator","Row : 800000, 10000");
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        keti_log("Storage Validator","Calculated Row (WorkId : 3) : 800000");

        keti_log("Storage Validator","Join Snippet, (WorkId : 4)");
        keti_log("Storage Validator","Row : 800000, 1");
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        keti_log("Storage Validator","Calculated Row (WorkId : 4) : 32480");

        keti_log("Storage Validator","Join Snippet, (WorkId : 5)");
        keti_log("Storage Validator","Row : 800000, 10000");
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        keti_log("Storage Validator","Calculated Row (WorkId : 5) : 800000");
        
        keti_log("Storage Validator","Join Snippet, (WorkId : 6)");
        keti_log("Storage Validator","Row : 800000, 1");
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        keti_log("Storage Validator","Calculated Row (WorkId : 6) : 1");

        keti_log("Storage Validator","Join Snippet, (WorkId : 7)");
        keti_log("Storage Validator","Row : 32480, 1");
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        keti_log("Storage Validator","Calculated Row (WorkId : 7) : 812");
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        keti_log("Validator","Validatoion Finish...");
        cout<<"----------------------------------------------------------"<<endl;
        keti_log("Validator","Storage CPU Usage      : 2112 Tick");
        keti_log("Validator","Storage Power Usage    : 800.99 W");
        keti_log("Validator","Storage execution Time : 13.8295 s");
        cout<<"----------------------------------------------------------"<<endl;
        keti_log("Validator","CSD Total CPU Usage   : 3487 Tick");
        keti_log("Validator","CSD Total Power Usage : 161.48 W");
        keti_log("Validator","CSD Execution Time    : 3.2280 s");      
        cout<<"----------------------------------------------------------"<<endl;  
        keti_log("Validator","Total Network Usage      : 96.67 MB");
        keti_log("Validator","Total Scanned Row Count  : 1620050 Row");
        keti_log("Validator","Total Filtered Row Count : 812 Row");
        cout<<"----------------------------------------------------------"<<endl;

        clock_gettime(CLOCK_MONOTONIC, &end);
        double time = (double)(end.tv_sec - begin.tv_sec) + (double)(end.tv_nsec - begin.tv_nsec)/1000000000;
        keti_log("Validator","End Validation time : " + std::to_string(time) + "s");
        cout<<"----------------------------------------------------------"<<endl<<endl;
    }
    else if(uri_path == "/simulate/recording/SSD"){
        struct timespec begin, end;
        clock_gettime(CLOCK_MONOTONIC, &begin);
        keti_log("Validator","Validator Receive Query, Option");
        Parsed_Query parsed_query(document["query"].GetString());
        query_planner_.Parse(meta_data_manager_,parsed_query);
        
        keti_log("Validator",parsed_query.Get_Parsed_Query());
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        keti_log("Validator","Option Name : Option2");
        keti_log("Validator","DBMS : Mysql");
        keti_log("Validator","Storage Type : SSD");
        keti_log("Validator","CSD Count : ");
        keti_log("Validator","CSD Kind : ");
        keti_log("Validator","Block Count : ");
        keti_log("Validator","Algorithm : ");
        keti_log("Validator","Using Index : ");
        keti_log("Validator","Validation Start");
        cout<<"----------------------------------------------------------------"<<endl;
        keti_log("SSD Validator","SSD Validator Start...");
        std::this_thread::sleep_for(std::chrono::milliseconds(5960));
        keti_log("Validator","Validatoion Finish...");
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        cout<<"----------------------------------------------------------"<<endl;
        keti_log("Validator","Host CPU Usage           : 5960 Tick");
        keti_log("Validator","Host Power Usage         : 3367.98 W");
        keti_log("Validator","Total Network Usage      : 194.09 MB");
        keti_log("Validator","Total Scanned Row Count  : 1620050 Row");
        keti_log("Validator","Total Filtered Row Count : 838 Row");
        keti_log("Validator","Query execution Time     : 59.64 s");
        cout<<"----------------------------------------------------------"<<endl;

        clock_gettime(CLOCK_MONOTONIC, &end);
        double time = (double)(end.tv_sec - begin.tv_sec) + (double)(end.tv_nsec - begin.tv_nsec)/1000000000;
        keti_log("Validator","End Validation time : " + std::to_string(time) + "s");
        cout<<"----------------------------------------------------------"<<endl<<endl;
    }
    return;
};

//
// A DELETE request
//
void DB_Connector_Instance::handle_delete(http_request message)
{
    std::string uri_path = message.relative_uri().to_string();
    cout<<uri_path<<endl;

    auto body_json = message.extract_string();
    std::string json = utility::conversions::to_utf8string(body_json.get());
    cout << json << endl;
    
    Document document;
    document.Parse(json.c_str());

    if(!uri_path.find("/simulate/option/")){

    }
    else if(!uri_path.find("/simulate/log/")){

    }

    ucout <<  message.to_string() << endl;
    message.reply(status_codes::NotFound,U("SUPPORT ONLY GET API"));
    return;
};


//
// A PUT request 
//
void DB_Connector_Instance::handle_put(http_request message)
{
    std::string uri_path = message.relative_uri().to_string();
    cout<<uri_path<<endl;

    auto body_json = message.extract_string();
    std::string json = utility::conversions::to_utf8string(body_json.get());
    cout << json << endl;
    
    Document document;
    document.Parse(json.c_str());

    if(!uri_path.find("/simulate/option/")){

    }


    ucout <<  message.to_string() << endl;
    message.reply(status_codes::NotFound,U("SUPPORT ONLY GET API"));
    return;
};
