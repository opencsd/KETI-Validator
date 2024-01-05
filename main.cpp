#include "RestHandler.hpp"
#include <thread>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "HistogramTable.hpp"

std::unique_ptr<Rest_Handler_Instance> restHandler;

int main(){
    DBManager& dbManager = DBManager::getInstance();
    dbManager.initialize("localhost","keti","ketilinux","keti_db");

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