#include "RestHandler.hpp"
#include <iostream>
#include <string>
#include <boost/property_tree/json_parser.hpp>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h" 
#include <boost/asio.hpp>
#include <iostream>
#include <sstream>  
 Rest_Handler_Instance::Rest_Handler_Instance(utility::string_t url):m_listener(url){
     std::cout << "URI: " << url << std::endl;
     m_listener.support(web::http::methods::GET, std::bind(&Rest_Handler_Instance::handle_get, this, std::placeholders::_1));
     m_listener.support(web::http::methods::PUT, std::bind(&Rest_Handler_Instance::handle_put, this, std::placeholders::_1));
     m_listener.support(web::http::methods::POST, std::bind(&Rest_Handler_Instance::handle_post, this, std::placeholders::_1));
     m_listener.support(web::http::methods::DEL, std::bind(&Rest_Handler_Instance::handle_delete, this, std::placeholders::_1));
 }

 void Rest_Handler_Instance::handle_get(web::http::http_request message){

 }

 void Rest_Handler_Instance::handle_put(web::http::http_request message){

 }

 void Rest_Handler_Instance::handle_post(web::http::http_request message){
    DBManager& dbManager = DBManager::getInstance();

     std::string uri_path = message.relative_uri().to_string();
     std::cout<<uri_path<<"\n";
    // 밑에 주석 부분 지워야 디버깅 로그 저장됨
     auto body_json = message.extract_string();
     std::string json = utility::conversions::to_utf8string(body_json.get());
     //std::stringstream buffer;
     //std::streambuf *oldCoutStreamBuf = std::cout.rdbuf(buffer.rdbuf());
     rapidjson::Document document;
     document.Parse(json.c_str());
     std::string resultJson;
     if(uri_path == "/validator/run"){
            const rapidjson::Value& value = document;
            std::string queryStatement = document["Query_Statement"].GetString();
            int optionID = document["Option_ID"].GetInt();
            std::string userID = document["User_ID"].GetString();
            std::cout<<"REST HANDLER\n USER_ID : "<<userID<<" OPTION_ID : "<<optionID<<" QUERY STATEMENT : "<<queryStatement<<"\n";
            // std::this_thread::sleep_for(std::chrono::milliseconds{300});
            resultJson = QueryParserMain(queryStatement, optionID, userID);
            message.reply(web::http::status_codes::OK,resultJson);
            std::cout<<"\n---VALIDATION FINISHED---"<<std::endl;
            //std::cout.rdbuf(oldCoutStreamBuf);
            //std::string debugLog = buffer.str();
            //std::cout<<"DEBUG LOG \n"<<debugLog<<std::endl;
            std::string valiId = "";
            for (char character : resultJson){
                if(std::isdigit(character)){
                    valiId += character;
                }
            }
            int valiInt = std::stoi(valiId);
            //std::string queryState = "insert into validation_debug_log (validation_id, validation_log) values (";
            //queryState = queryState + std::to_string(valiInt) + ",\"" + debugLog + "\")";
            //sql::ResultSet *resultSet = dbManager.executeQuery(queryState);
            //delete resultSet;
     }

     else if(uri_path == "/validator/snippet"){

     }

     else if(uri_path == "/validator/newOption"){

     }
     else if(uri_path == "/validator/getOption"){
        
     }
     
     
 }

 void Rest_Handler_Instance::handle_delete(web::http::http_request message){

 }


/*
namespace asio = boost::asio;
using tcp = asio::ip::tcp;

Rest_Handler_Instance::Rest_Handler_Instance(){

}

boost::property_tree::ptree Rest_Handler_Instance::parseJson(const std::string& jsonStr) {
    boost::property_tree::ptree tree;
    std::istringstream iss(jsonStr);
    boost::property_tree::read_json(iss, tree);
    return tree;
}

void Rest_Handler_Instance::handleRequest(tcp::socket socket, const std::string& requestBody) {
    try {
        auto jsonRequest = parseJson(requestBody);
        std::cout<<"REQUEST BODY : "<<requestBody<<"\n";
        std::string resultJson;
        // URI에 따라 다른 처리
        std::string uri = jsonRequest.get<std::string>("uri");
        std::cout<<"URL : "<<uri<<"\n";
        if(uri == "validator/run"){
            std::string queryStatement = jsonRequest.get<std::string>("Query_Statement");
            int optionID = jsonRequest.get<int>("Option_ID");
            std::string userID = jsonRequest.get<std::string>("User_ID");
            std::cout<<"REST HANDLER\n USER_ID : "<<userID<<" OPTION_ID : "<<optionID<<"\n";
            resultJson = QueryParserMain(queryStatement, optionID, userID);
        }

        // 간단한 응답 생성 (상태 코드만)
        std::string httpResponse = resultJson;
        asio::write(socket, asio::buffer(httpResponse));
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

void Rest_Handler_Instance::startServer(asio::io_service& ioService, short port) {
    tcp::acceptor acceptor(ioService, tcp::endpoint(tcp::v4(), port));

    while (true) {
        tcp::socket socket(ioService);
        acceptor.accept(socket);

        // 요청을 받음
        asio::streambuf request;
        boost::system::error_code ec;
        size_t s = asio::read(socket,request,ec);
        std::cout<<"SIZE T"<<s<<"\n";
        std::istream requestStream(&request);
        std::string requestLine;
        std::getline(requestStream, requestLine, '\r');

        // requestLine에서 uri를 추출하는 코드 추가
        size_t uriStart = requestLine.find(" ");
        size_t uriEnd = requestLine.find(" ", uriStart + 1);
        std::string uri = requestLine.substr(uriStart + 1, uriEnd - uriStart - 1);

        // 나머지 요청을 받음
        asio::streambuf requestBodyBuffer;
        asio::read(socket, requestBodyBuffer);
        std::string requestBody(asio::buffers_begin(requestBodyBuffer.data()), asio::buffers_end(requestBodyBuffer.data()));

        std::thread t(&Rest_Handler_Instance::handleRequest,this,std::move(socket), requestBody);
        t.detach();
    }
}
*/