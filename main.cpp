#include "RestHandler.hpp"
#include <thread>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "HistogramTable.hpp"
#include "keti_log.h"
#include "QEMUHandler.hpp"

std::unique_ptr<Rest_Handler_Instance> restHandler;

int main(int argc, char** argv){
    
    if (argc >= 2) {
      KETILOG::SetLogLevel(std::stoi(argv[1]));
  }else if (getenv("LOG_LEVEL") != NULL){
      std::string env = getenv("LOG_LEVEL");
      int log_level;
      if (env == "TRACE"){
          log_level = DEBUGG_LEVEL::TRACE;
      }else if (env == "DEBUG"){
          log_level = DEBUGG_LEVEL::DEBUG;
      }else if (env == "INFO"){
          log_level = DEBUGG_LEVEL::INFO;
      }else if (env == "WARN"){
          log_level = DEBUGG_LEVEL::WARN;
      }else if (env == "ERROR"){
          log_level = DEBUGG_LEVEL::ERROR;
      }else if (env == "FATAL"){
          log_level = DEBUGG_LEVEL::FATAL;
      }else{
          log_level = DEBUGG_LEVEL::DEBUG;
      }
      KETILOG::SetLogLevel(log_level);
    }else{
      KETILOG::SetDefaultLogLevel();
    } 
    KETILOG::SetDefaultLogLevel();


    DBManager& dbManager = DBManager::getInstance();
    dbManager.initialize("10.0.4.80","root","ketilinux","keti_opencsd");

    std::string queryState = "show tables;";
    
    try{
        sql::ResultSet *resultSet = dbManager.executeQuery(queryState);
        while(resultSet->next()){
            std::cout<<resultSet->getString(1)<<std::endl;
        }
        delete resultSet;
    } catch (sql::SQLException& e){
        std::cerr <<"SQLException: "<<e.what();
    }

    
    QEMUHandler::InitQEMUHandler();
    InitHistogram();
    InitQueryParser();
    //QueryParserMain("SELECT ps_partkey,Sum(ps_supplycost * ps_availqty) AS value FROM   partsupp, supplier,nation WHERE  ps_suppkey = s_suppkey AND s_nationkey = n_nationkey AND n_name = 'MOZAMBIQUE' GROUP  BY ps_partkey HAVING Sum(ps_supplycost * ps_availqty) > (SELECT Sum(ps_supplycost * ps_availqty) * 0.0001000000 FROM   partsupp, supplier, nation  WHERE  ps_suppkey = s_suppkey AND s_nationkey = n_nationkey AND n_name = 'MOZAMBIQUE') ORDER  BY value DESC;", 0, 123);

    utility::string_t port = U("40000");
    utility::string_t address = U("http://0.0.0.0:");
    address.append(port);

    web::uri_builder uri(address);
    auto addr = uri.to_uri().to_string();
    restHandler = std::unique_ptr<Rest_Handler_Instance>(new Rest_Handler_Instance(addr));
    restHandler -> open().wait();

    while(true){

    }
}


// int main(){
//     DBManager& dbManager = DBManager::getInstance();
//     dbManager.initialize("localhost","keti","ketilinux","keti_db");

//     InitHistogram();
//     InitQueryParser();
//     //QueryParserMain("SELECT ps_partkey,Sum(ps_supplycost * ps_availqty) AS value FROM   partsupp, supplier,nation WHERE  ps_suppkey = s_suppkey AND s_nationkey = n_nationkey AND n_name = 'MOZAMBIQUE' GROUP  BY ps_partkey HAVING Sum(ps_supplycost * ps_availqty) > (SELECT Sum(ps_supplycost * ps_availqty) * 0.0001000000 FROM   partsupp, supplier, nation  WHERE  ps_suppkey = s_suppkey AND s_nationkey = n_nationkey AND n_name = 'MOZAMBIQUE') ORDER  BY value DESC;", 0, 123);

//     utility::string_t port = U("40000");
//     utility::string_t address = U("http://0.0.0.0:");
//     address.append(port);

//     web::uri_builder uri(address);
//     auto addr = uri.to_uri().to_string();
//     restHandler = std::unique_ptr<Rest_Handler_Instance>(new Rest_Handler_Instance(addr));
//     restHandler -> open().wait();

//     while(true){

//     }
// }