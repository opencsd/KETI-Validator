#pragma once
#include <string>
#include <iostream>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <mysql_driver.h>

class DBManager {
    public:
        static DBManager& getInstance(){
            static DBManager instance;
            return instance;
        }

        void initialize(const std::string& host, const std::string& user, const std::string& password, const std::string& database){
            driver = sql::mysql::get_mysql_driver_instance();
            con = driver -> connect("tcp://10.0.4.87:30702",user,password);
            con -> setSchema(database);
        }

        sql::ResultSet* executeQuery(const std::string& query){
            sql::Statement *stmt = con -> createStatement();
            sql::ResultSet *res = stmt-> executeQuery(query);
            return res;
        }

    private:
        DBManager(){}
        ~DBManager(){
            delete con;
        }

        DBManager(const DBManager&) = delete;
        DBManager& operator=(const DBManager&) = delete;
        sql::mysql::MySQL_Driver *driver;
        sql::Connection *con;
};