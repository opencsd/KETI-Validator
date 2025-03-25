#include "QueryParser.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <rapidjson/document.h>
#include <thread>

using namespace rapidjson;

int simulationNum = 0;
int queryNum = 11;
void InitQueryParser(){
    DBManager& dbManager = DBManager::getInstance();
    std::string queryState = "select max(validation_id) as max_validation_id from validation_log";
    sql::ResultSet *resultSet = dbManager.executeQuery(queryState);
    while(resultSet -> next()){
        simulationNum = resultSet -> getInt("max_validation_id");
    }
    if(simulationNum == NULL){
        simulationNum = 0;
    }
    std::cout<<"VALIDATION ID : "<<simulationNum<<std::endl;
}

std::string QueryParserMain(std::string queryStatement, int optionID, std::string userID){
    simulationNum++;
    std::string returnJson= "";
    optionInfo option;
    std::vector<querySnippetInfo> snippetInfo;
    option = getOptionInfo(optionID, userID);
    sleep(1);
    std::cout<<"---QUERY PARSER---"<<std::endl;
    std::cout<<"VALIDATION ID : "<<simulationNum<<std::endl;
    snippetInfo = getSnippetInfo(queryStatement);
    sleep(1);
    std::cout<<"SNIPPET COUNT : "<<snippetInfo.size()<<std::endl;
    returnJson = "{\n\"Validation_Option\":{\n\"Option_ID\" : " + std::to_string(option.optionID) + "\",\n\"Option_Name\" : \"" + option.optionName + "\",\n\"DBMS_Type\" : \"" + option.DBType + "\",\n\"Storage_Type\" : \"" + option.storageType + "\",\n\"CSD_Count\" : " + std::to_string(option.csdCount) + ",\n\"CSD_Type\" : \"" + option.csdType + "\",\n\"Block_Count\" : " + std::to_string(option.blockCount) + ",\n\"Scheduling_Algorithm\" : \"" + option.schedulingAlgorithm + "\",\n\"Using_Index\" : " + std::to_string(option.usingIndex) + "\n},\n";
    
    sleep(1);
    returnJson = returnJson + " \"Validation_Snippet\" : {\n[\n";
    for(int i=0;i <snippetInfo.size();i++){
        std::string snippetJson = "";
        snippetJson = "{\n\"validation_id\" : " + std::to_string(snippetInfo[i].validationID) + ",\n\"Work_ID\" : " + std::to_string(snippetInfo[i].workID) + ",\n\"Snippet_Type\" : \"" + snippetInfo[i].snippetType + "\"" + ",\n\"Projection\" : " + std::to_string(snippetInfo[i].projectionCount) + ",\n\"Filter\" : " + std::to_string(snippetInfo[i].filterCount) + ",\n\"Group_By\" : " + std::to_string(snippetInfo[i].groupByCount) + ",\n\"Order_By\" : " + std::to_string(snippetInfo[i].orderByCount) + ",\n\"Having\" : " + std::to_string(snippetInfo[i].havingCount) + ",\n\"Limit\" : " + std::to_string(snippetInfo[i].limitExist) + "}\n";
        if(i < snippetInfo.size() - 1)
            snippetJson += ",";
        returnJson += snippetJson;
        sleep(0.5);
    }
    returnJson += "]\n},\n";
    std::cout<<"COMPLETE SNIPPET INFO IN DB"<<std::endl;

    std::string resultJson;
    if(option.storageType == "csd"){ //CSD Validator
        resultJson = CSDValidatorTemp(snippetInfo,queryStatement,queryNum,optionID, option, userID, simulationNum, returnJson);
    }
    else{ //SSD Validator
        resultJson = SSDValidatorTemp(queryStatement,snippetInfo,queryNum,option,optionID,userID,simulationNum, returnJson);
    }
    resultJson = "{\"validation_id\" : " + std::to_string(simulationNum) + "}";
    InsertSnippetDB(snippetInfo);
    return resultJson;
}

optionInfo getOptionInfo(int optionID, std::string userID){
    optionInfo option;
    DBManager& dbManager = DBManager::getInstance();
    std::string queryState = "select * from validation_option where  option_id = " +std::to_string(optionID) + ";";
    std::cout<<"---OPTION ANALYZE---"<<std::endl;
    std::cout<<"USER ID : "<<userID<<" OPTION ID : "<<optionID<<std::endl;
    sql::ResultSet *resultSet = dbManager.executeQuery(queryState);
    while (resultSet->next()) {
        // 처리 로직 작성
        option.optionID = resultSet->getInt("option_id");
        std::cout<<"OPTION ID : "<<resultSet->getInt("option_id")<<std::endl;
        option.optionName = resultSet -> getString("option_name");
        std::cout<<"OPTION NAME : "<<resultSet -> getString("option_name")<<std::endl;
        option.DBType = (resultSet->getString("dbms_type"));
        std::cout<<"DBMS TYPE : "<< resultSet->getString("dbms_type")<<std::endl;
        option.storageType = (resultSet->getString("storage_type"));
        std::cout<<"STORAGE TYPE : "<<resultSet->getString("storage_type")<<std::endl;
        option.csdCount = resultSet->getInt("csd_count");
        std::cout<<"CSD COUNT : "<<resultSet->getInt("csd_count")<<std::endl;
        option.csdType = (resultSet->getString("csd_type"));
        std::cout<<"CSD TYPE : "<<resultSet->getString("csd_type")<<std::endl;
        option.blockCount = resultSet->getInt("block_count");
        std::cout<<"BLOCK COUNT : "<<resultSet->getInt("block_count")<<std::endl;
        option.schedulingAlgorithm = (resultSet->getString("scheduling_algorithm"));
        std::cout<<"SCHEDULING ALGORITHM : "<<resultSet->getString("scheduling_algorithm")<<std::endl;
        option.usingIndex = resultSet->getInt("using_index");
        std::cout<<"USING INDEX : "<<(resultSet->getInt("using_index"))<<std::endl<<std::endl;
    }
    // ResultSet 메모리 해제
    delete resultSet;
    return option;
}

std::vector<querySnippetInfo> getSnippetInfo(std::string queryStatement){
    std::vector<querySnippetInfo> snippetInfoVector;
    if(queryStatement == "TPC-H_01"){
        querySnippetInfo info0, info1;
        info0 = parseSnippet("/usr/local/snippet/tpch01/tpch01-0.json");
        info1 = parseSnippet("/usr/local/snippet/tpch01/tpch01-1.json");

        snippetInfoVector.push_back(info0);
        snippetInfoVector.push_back(info1);
        queryNum = 1;
    }
    else if(queryStatement == "TPC-H_02"){
        querySnippetInfo info0,info1,info2,info3, info4, info5, info6, info7, info8, info9, info10, info11, info12;
        info0 = parseSnippet("/usr/local/snippet/tpch02/tpch02-0.json");
        info1 = parseSnippet("/usr/local/snippet/tpch02/tpch02-1.json");
        info2 = parseSnippet("/usr/local/snippet/tpch02/tpch02-2.json");
        info3 = parseSnippet("/usr/local/snippet/tpch02/tpch02-3.json");
        info4 = parseSnippet("/usr/local/snippet/tpch02/tpch02-4.json");
        info5 = parseSnippet("/usr/local/snippet/tpch02/tpch02-5.json");
        info6 = parseSnippet("/usr/local/snippet/tpch02/tpch02-6.json");
        info7 = parseSnippet("/usr/local/snippet/tpch02/tpch02-7.json");
        info8 = parseSnippet("/usr/local/snippet/tpch02/tpch02-8.json");
        info9 = parseSnippet("/usr/local/snippet/tpch02/tpch02-9.json");
        info10 = parseSnippet("/usr/local/snippet/tpch02/tpch02-10.json");
        info11 = parseSnippet("/usr/local/snippet/tpch02/tpch02-11.json");
        info12 = parseSnippet("/usr/local/snippet/tpch02/tpch02-12.json");

        snippetInfoVector.push_back(info0);
        snippetInfoVector.push_back(info1);
        snippetInfoVector.push_back(info2);
        snippetInfoVector.push_back(info3);
        snippetInfoVector.push_back(info4);
        snippetInfoVector.push_back(info5);
        snippetInfoVector.push_back(info6);
        snippetInfoVector.push_back(info7);
        snippetInfoVector.push_back(info8);
        snippetInfoVector.push_back(info9);
        snippetInfoVector.push_back(info10);
        snippetInfoVector.push_back(info11);
        snippetInfoVector.push_back(info12);
        queryNum = 2;
    }
    else if(queryStatement == "TPC-H_03"){
        querySnippetInfo info0,info1,info2,info3, info4, info5;
        info0 = parseSnippet("/usr/local/snippet/tpch03/tpch03-0.json");
        info1 = parseSnippet("/usr/local/snippet/tpch03/tpch03-1.json");
        info2 = parseSnippet("/usr/local/snippet/tpch03/tpch03-2.json");
        info3 = parseSnippet("/usr/local/snippet/tpch03/tpch03-3.json");
        info4 = parseSnippet("/usr/local/snippet/tpch03/tpch03-4.json");
        info5 = parseSnippet("/usr/local/snippet/tpch03/tpch03-5.json");

        snippetInfoVector.push_back(info0);
        snippetInfoVector.push_back(info1);
        snippetInfoVector.push_back(info2);
        snippetInfoVector.push_back(info3);
        snippetInfoVector.push_back(info4);
        snippetInfoVector.push_back(info5);
        queryNum = 3;
    }
    else if(queryStatement == "TPC-H_04"){
        querySnippetInfo info0,info1,info2;
        info0 = parseSnippet("/usr/local/snippet/tpch04/tpch04-0.json");
        info1 = parseSnippet("/usr/local/snippet/tpch04/tpch04-1.json");
        info2 = parseSnippet("/usr/local/snippet/tpch04/tpch04-2.json");

        snippetInfoVector.push_back(info0);
        snippetInfoVector.push_back(info1);
        snippetInfoVector.push_back(info2);
        queryNum = 4;
    }
    else if(queryStatement == "TPC-H_05"){
        querySnippetInfo info0,info1,info2,info3, info4, info5, info6, info7, info8, info9, info10, info11;

        info0 = parseSnippet("/usr/local/snippet/tpch05/tpch05-0.json");
        info1 = parseSnippet("/usr/local/snippet/tpch05/tpch05-1.json");
        info2 = parseSnippet("/usr/local/snippet/tpch05/tpch05-2.json");
        info3 = parseSnippet("/usr/local/snippet/tpch05/tpch05-3.json");
        info4 = parseSnippet("/usr/local/snippet/tpch05/tpch05-4.json");
        info5 = parseSnippet("/usr/local/snippet/tpch05/tpch05-5.json");
        info6 = parseSnippet("/usr/local/snippet/tpch05/tpch05-6.json");
        info7 = parseSnippet("/usr/local/snippet/tpch05/tpch05-7.json");
        info8 = parseSnippet("/usr/local/snippet/tpch05/tpch05-8.json");
        info9 = parseSnippet("/usr/local/snippet/tpch05/tpch05-9.json");
        info10 = parseSnippet("/usr/local/snippet/tpch05/tpch05-10.json");
        info11 = parseSnippet("/usr/local/snippet/tpch05/tpch05-11.json");

        snippetInfoVector.push_back(info0);
        snippetInfoVector.push_back(info1);
        snippetInfoVector.push_back(info2);
        snippetInfoVector.push_back(info3);
        snippetInfoVector.push_back(info4);
        snippetInfoVector.push_back(info5);
        snippetInfoVector.push_back(info6);
        snippetInfoVector.push_back(info7);
        snippetInfoVector.push_back(info8);
        snippetInfoVector.push_back(info9);
        snippetInfoVector.push_back(info10);
        snippetInfoVector.push_back(info11);
        queryNum =5;
    }
    else if(queryStatement == "TPC-H_06"){
        querySnippetInfo info0,info1;

        info0 = parseSnippet("/usr/local/snippet/tpch06/tpch06-0.json");
        info1 = parseSnippet("/usr/local/snippet/tpch06/tpch06-1.json");

        snippetInfoVector.push_back(info0);
        snippetInfoVector.push_back(info1);
        queryNum = 6;
    }
    else if(queryStatement == "TPC-H_07"){
        querySnippetInfo info0,info1,info2,info3, info4, info5, info6, info7, info8, info9, info10, info11, info12, info13, info14;
        info0 = parseSnippet("/usr/local/snippet/tpch07/tpch07-0.json");
        info1 = parseSnippet("/usr/local/snippet/tpch07/tpch07-1.json");
        info2 = parseSnippet("/usr/local/snippet/tpch07/tpch07-2.json");
        info3 = parseSnippet("/usr/local/snippet/tpch07/tpch07-3.json");
        info4 = parseSnippet("/usr/local/snippet/tpch07/tpch07-4.json");
        info5 = parseSnippet("/usr/local/snippet/tpch07/tpch07-5.json");
        info6 = parseSnippet("/usr/local/snippet/tpch07/tpch07-6.json");
        info7 = parseSnippet("/usr/local/snippet/tpch07/tpch07-7.json");
        info8 = parseSnippet("/usr/local/snippet/tpch07/tpch07-8.json");
        info9 = parseSnippet("/usr/local/snippet/tpch07/tpch07-9.json");
        info10 = parseSnippet("/usr/local/snippet/tpch07/tpch07-10.json");
        info11 = parseSnippet("/usr/local/snippet/tpch07/tpch07-11.json");
        info12 = parseSnippet("/usr/local/snippet/tpch07/tpch07-12.json");
        info13 = parseSnippet("/usr/local/snippet/tpch07/tpch07-13.json");
        info14 = parseSnippet("/usr/local/snippet/tpch07/tpch07-14.json");

        snippetInfoVector.push_back(info0);
        snippetInfoVector.push_back(info1);
        snippetInfoVector.push_back(info2);
        snippetInfoVector.push_back(info3);
        snippetInfoVector.push_back(info4);
        snippetInfoVector.push_back(info5);
        snippetInfoVector.push_back(info6);
        snippetInfoVector.push_back(info7);
        snippetInfoVector.push_back(info8);
        snippetInfoVector.push_back(info9);
        snippetInfoVector.push_back(info10);
        snippetInfoVector.push_back(info11);
        snippetInfoVector.push_back(info12);
        snippetInfoVector.push_back(info13);
        snippetInfoVector.push_back(info14);
        queryNum = 7;
    }
    else if(queryStatement == "TPC-H_08"){
        querySnippetInfo info0,info1,info2,info3, info4, info5, info6, info7, info8, info9, info10, info11, info12, info13, info14, info15, info16;
        info0 = parseSnippet("/usr/local/snippet/tpch08/tpch08-0.json");
        info1 = parseSnippet("/usr/local/snippet/tpch08/tpch08-1.json");
        info2 = parseSnippet("/usr/local/snippet/tpch08/tpch08-2.json");
        info3 = parseSnippet("/usr/local/snippet/tpch08/tpch08-3.json");
        info4 = parseSnippet("/usr/local/snippet/tpch08/tpch08-4.json");
        info5 = parseSnippet("/usr/local/snippet/tpch08/tpch08-5.json");
        info6 = parseSnippet("/usr/local/snippet/tpch08/tpch08-6.json");
        info7 = parseSnippet("/usr/local/snippet/tpch08/tpch08-7.json");
        info8 = parseSnippet("/usr/local/snippet/tpch08/tpch08-8.json");
        info9 = parseSnippet("/usr/local/snippet/tpch08/tpch08-9.json");
        info10 = parseSnippet("/usr/local/snippet/tpch08/tpch08-10.json");
        info11 = parseSnippet("/usr/local/snippet/tpch08/tpch08-11.json");
        info12 = parseSnippet("/usr/local/snippet/tpch08/tpch08-12.json");
        info13 = parseSnippet("/usr/local/snippet/tpch08/tpch08-13.json");
        info14 = parseSnippet("/usr/local/snippet/tpch08/tpch08-14.json");
        info15 = parseSnippet("/usr/local/snippet/tpch08/tpch08-15.json");
        info16 = parseSnippet("/usr/local/snippet/tpch08/tpch08-16.json");

        snippetInfoVector.push_back(info0);
        snippetInfoVector.push_back(info1);
        snippetInfoVector.push_back(info2);
        snippetInfoVector.push_back(info3);
        snippetInfoVector.push_back(info4);
        snippetInfoVector.push_back(info5);
        snippetInfoVector.push_back(info6);
        snippetInfoVector.push_back(info7);
        snippetInfoVector.push_back(info8);
        snippetInfoVector.push_back(info9);
        snippetInfoVector.push_back(info10);
        snippetInfoVector.push_back(info11);
        snippetInfoVector.push_back(info12);
        snippetInfoVector.push_back(info13);
        snippetInfoVector.push_back(info14);        
        snippetInfoVector.push_back(info15);
        snippetInfoVector.push_back(info16); 
        queryNum = 8;
    }
    else if(queryStatement == "TPC-H_09"){
        querySnippetInfo info0,info1,info2,info3, info4, info5, info6, info7, info8, info9, info10, info11;
        info0 = parseSnippet("/usr/local/snippet/tpch09/tpch09-0.json");
        info1 = parseSnippet("/usr/local/snippet/tpch09/tpch09-1.json");
        info2 = parseSnippet("/usr/local/snippet/tpch09/tpch09-2.json");
        info3 = parseSnippet("/usr/local/snippet/tpch09/tpch09-3.json");
        info4 = parseSnippet("/usr/local/snippet/tpch09/tpch09-4.json");
        info5 = parseSnippet("/usr/local/snippet/tpch09/tpch09-5.json");
        info6 = parseSnippet("/usr/local/snippet/tpch09/tpch09-6.json");
        info7 = parseSnippet("/usr/local/snippet/tpch09/tpch09-7.json");
        info8 = parseSnippet("/usr/local/snippet/tpch09/tpch09-8.json");
        info9 = parseSnippet("/usr/local/snippet/tpch09/tpch09-9.json");
        info10 = parseSnippet("/usr/local/snippet/tpch09/tpch09-10.json");
        info11 = parseSnippet("/usr/local/snippet/tpch09/tpch09-11.json");

        snippetInfoVector.push_back(info0);
        snippetInfoVector.push_back(info1);
        snippetInfoVector.push_back(info2);
        snippetInfoVector.push_back(info3);
        snippetInfoVector.push_back(info4);
        snippetInfoVector.push_back(info5);
        snippetInfoVector.push_back(info6);
        snippetInfoVector.push_back(info7);
        snippetInfoVector.push_back(info8);
        snippetInfoVector.push_back(info9);
        snippetInfoVector.push_back(info10);
        snippetInfoVector.push_back(info11);
        queryNum =9;
    }
    else if(queryStatement == "TPC-H_10"){
        querySnippetInfo info0,info1,info2,info3, info4, info5, info6, info7;
        info0 = parseSnippet("/usr/local/snippet/tpch10/tpch10-0.json");
        info1 = parseSnippet("/usr/local/snippet/tpch10/tpch10-1.json");
        info2 = parseSnippet("/usr/local/snippet/tpch10/tpch10-2.json");
        info3 = parseSnippet("/usr/local/snippet/tpch10/tpch10-3.json");
        info4 = parseSnippet("/usr/local/snippet/tpch10/tpch10-4.json");
        info5 = parseSnippet("/usr/local/snippet/tpch10/tpch10-5.json");
        info6 = parseSnippet("/usr/local/snippet/tpch10/tpch10-6.json");
        info7 = parseSnippet("/usr/local/snippet/tpch10/tpch10-7.json");

        snippetInfoVector.push_back(info0);
        snippetInfoVector.push_back(info1);
        snippetInfoVector.push_back(info2);
        snippetInfoVector.push_back(info3);
        snippetInfoVector.push_back(info4);
        snippetInfoVector.push_back(info5);
        snippetInfoVector.push_back(info6);
        snippetInfoVector.push_back(info7);
        queryNum = 10;
    }
    else if(queryStatement == "TPC-H_11"){
        querySnippetInfo info0,info1,info2,info3, info4, info5, info6, info7;
        info0 = parseSnippet("/usr/local/snippet/tpch11/tpch11-0.json");
        info1 = parseSnippet("/usr/local/snippet/tpch11/tpch11-1.json");
        info2 = parseSnippet("/usr/local/snippet/tpch11/tpch11-2.json");
        info3 = parseSnippet("/usr/local/snippet/tpch11/tpch11-3.json");
        info4 = parseSnippet("/usr/local/snippet/tpch11/tpch11-4.json");
        info5 = parseSnippet("/usr/local/snippet/tpch11/tpch11-5.json");
        info6 = parseSnippet("/usr/local/snippet/tpch11/tpch11-6.json");
        info7 = parseSnippet("/usr/local/snippet/tpch11/tpch11-7.json");

        snippetInfoVector.push_back(info0);
        snippetInfoVector.push_back(info1);
        snippetInfoVector.push_back(info2);
        snippetInfoVector.push_back(info3);
        snippetInfoVector.push_back(info4);
        snippetInfoVector.push_back(info5);
        snippetInfoVector.push_back(info6);
        snippetInfoVector.push_back(info7);
        queryNum = 11;
    }
    else if(queryStatement == "TPC-H_12"){
        querySnippetInfo info0,info1,info2;
        info0 = parseSnippet("/usr/local/snippet/tpch12/tpch12-0.json");
        info1 = parseSnippet("/usr/local/snippet/tpch12/tpch12-1.json");
        info2 = parseSnippet("/usr/local/snippet/tpch12/tpch12-2.json");

        snippetInfoVector.push_back(info0);
        snippetInfoVector.push_back(info1);
        snippetInfoVector.push_back(info2);
        queryNum =12 ;
    }
    else if(queryStatement == "TPC-H_13"){
        querySnippetInfo info0,info1,info2,info3;
        info0 = parseSnippet("/usr/local/snippet/tpch13/tpch13-0.json");
        info1 = parseSnippet("/usr/local/snippet/tpch13/tpch13-1.json");
        info2 = parseSnippet("/usr/local/snippet/tpch13/tpch13-2.json");
        info3 = parseSnippet("/usr/local/snippet/tpch13/tpch13-3.json");

        snippetInfoVector.push_back(info0);
        snippetInfoVector.push_back(info1);
        snippetInfoVector.push_back(info2);
        snippetInfoVector.push_back(info3);
        queryNum = 13;
    }
    else if(queryStatement == "TPC-H_14"){
        querySnippetInfo info0,info1,info2,info3;
        info0 = parseSnippet("/usr/local/snippet/tpch14/tpch14-0.json");
        info1 = parseSnippet("/usr/local/snippet/tpch14/tpch14-1.json");
        info2 = parseSnippet("/usr/local/snippet/tpch14/tpch14-2.json");
        info3 = parseSnippet("/usr/local/snippet/tpch14/tpch14-3.json");

        snippetInfoVector.push_back(info0);
        snippetInfoVector.push_back(info1);
        snippetInfoVector.push_back(info2);
        snippetInfoVector.push_back(info3);
        queryNum = 14;
    }
    else if(queryStatement == "TPC-H_15"){
        querySnippetInfo info0,info1,info2,info3, info4, info5;
        info0 = parseSnippet("/usr/local/snippet/tpch15/tpch15-0.json");
        info1 = parseSnippet("/usr/local/snippet/tpch15/tpch15-1.json");
        info2 = parseSnippet("/usr/local/snippet/tpch15/tpch15-2.json");
        info3 = parseSnippet("/usr/local/snippet/tpch15/tpch15-3.json");
        info4 = parseSnippet("/usr/local/snippet/tpch15/tpch15-4.json");
        info5 = parseSnippet("/usr/local/snippet/tpch15/tpch15-5.json");

        snippetInfoVector.push_back(info0);
        snippetInfoVector.push_back(info1);
        snippetInfoVector.push_back(info2);
        snippetInfoVector.push_back(info3);
        snippetInfoVector.push_back(info4);
        snippetInfoVector.push_back(info5);
        queryNum = 15;
    }
    else if(queryStatement == "TPC-H_16"){
        querySnippetInfo info0,info1,info2,info3, info4;
        info0 = parseSnippet("/usr/local/snippet/tpch16/tpch16-0.json");
        info1 = parseSnippet("/usr/local/snippet/tpch16/tpch16-1.json");
        info2 = parseSnippet("/usr/local/snippet/tpch16/tpch16-2.json");
        info3 = parseSnippet("/usr/local/snippet/tpch16/tpch16-3.json");
        info4 = parseSnippet("/usr/local/snippet/tpch16/tpch16-4.json");

        snippetInfoVector.push_back(info0);
        snippetInfoVector.push_back(info1);
        snippetInfoVector.push_back(info2);
        snippetInfoVector.push_back(info3);
        snippetInfoVector.push_back(info4);
        queryNum = 16;
    }
    else if(queryStatement == "TPC-H_17"){
        querySnippetInfo info0,info1,info2,info3;
        info0 = parseSnippet("/usr/local/snippet/tpch17/tpch17-0.json");
        info1 = parseSnippet("/usr/local/snippet/tpch17/tpch17-1.json");
        info2 = parseSnippet("/usr/local/snippet/tpch17/tpch17-2.json");
        info3 = parseSnippet("/usr/local/snippet/tpch17/tpch17-3.json");

        snippetInfoVector.push_back(info0);
        snippetInfoVector.push_back(info1);
        snippetInfoVector.push_back(info2);
        snippetInfoVector.push_back(info3);
        queryNum = 17;
    }
    else if(queryStatement == "TPC-H_18"){
        querySnippetInfo info0,info1,info2,info3, info4, info5, info6, info7;
        info0 = parseSnippet("/usr/local/snippet/tpch18/tpch18-0.json");
        info1 = parseSnippet("/usr/local/snippet/tpch18/tpch18-1.json");
        info2 = parseSnippet("/usr/local/snippet/tpch18/tpch18-2.json");
        info3 = parseSnippet("/usr/local/snippet/tpch18/tpch18-3.json");
        info4 = parseSnippet("/usr/local/snippet/tpch18/tpch18-4.json");
        info5 = parseSnippet("/usr/local/snippet/tpch18/tpch18-5.json");
        info6 = parseSnippet("/usr/local/snippet/tpch18/tpch18-6.json");
        info7 = parseSnippet("/usr/local/snippet/tpch18/tpch18-7.json");

        snippetInfoVector.push_back(info0);
        snippetInfoVector.push_back(info1);
        snippetInfoVector.push_back(info2);
        snippetInfoVector.push_back(info3);
        snippetInfoVector.push_back(info4);
        snippetInfoVector.push_back(info5);
        snippetInfoVector.push_back(info6);
        snippetInfoVector.push_back(info7);
        queryNum = 18;
    }
    else if(queryStatement == "TPC-H_19"){
        querySnippetInfo info0,info1,info2,info3, info4, info5, info6, info7, info8, info9, info10;
        info0 = parseSnippet("/usr/local/snippet/tpch19/tpch19-0.json");
        info1 = parseSnippet("/usr/local/snippet/tpch19/tpch19-1.json");
        info2 = parseSnippet("/usr/local/snippet/tpch19/tpch19-2.json");
        info3 = parseSnippet("/usr/local/snippet/tpch19/tpch19-3.json");
        info4 = parseSnippet("/usr/local/snippet/tpch19/tpch19-4.json");
        info5 = parseSnippet("/usr/local/snippet/tpch19/tpch19-5.json");
        info6 = parseSnippet("/usr/local/snippet/tpch19/tpch19-6.json");
        info7 = parseSnippet("/usr/local/snippet/tpch19/tpch19-7.json");
        info8 = parseSnippet("/usr/local/snippet/tpch19/tpch19-8.json");
        info9 = parseSnippet("/usr/local/snippet/tpch19/tpch19-9.json");
        info10 = parseSnippet("/usr/local/snippet/tpch19/tpch19-10.json");

        snippetInfoVector.push_back(info0);
        snippetInfoVector.push_back(info1);
        snippetInfoVector.push_back(info2);
        snippetInfoVector.push_back(info3);
        snippetInfoVector.push_back(info4);
        snippetInfoVector.push_back(info5);
        snippetInfoVector.push_back(info6);
        snippetInfoVector.push_back(info7);
        snippetInfoVector.push_back(info8);
        snippetInfoVector.push_back(info9);
        snippetInfoVector.push_back(info10);
        queryNum = 19;
    }
    else if(queryStatement == "TPC-H_20"){
        querySnippetInfo info0,info1,info2,info3, info4, info5, info6, info7, info8;
        info0 = parseSnippet("/usr/local/snippet/tpch20/tpch20-0.json");
        info1 = parseSnippet("/usr/local/snippet/tpch20/tpch20-1.json");
        info2 = parseSnippet("/usr/local/snippet/tpch20/tpch20-2.json");
        info3 = parseSnippet("/usr/local/snippet/tpch20/tpch20-3.json");
        info4 = parseSnippet("/usr/local/snippet/tpch20/tpch20-4.json");
        info5 = parseSnippet("/usr/local/snippet/tpch20/tpch20-5.json");
        info6 = parseSnippet("/usr/local/snippet/tpch20/tpch20-6.json");
        info7 = parseSnippet("/usr/local/snippet/tpch20/tpch20-7.json");
        info8 = parseSnippet("/usr/local/snippet/tpch20/tpch20-8.json");

        snippetInfoVector.push_back(info0);
        snippetInfoVector.push_back(info1);
        snippetInfoVector.push_back(info2);
        snippetInfoVector.push_back(info3);
        snippetInfoVector.push_back(info4);
        snippetInfoVector.push_back(info5);
        snippetInfoVector.push_back(info6);
        snippetInfoVector.push_back(info7);
        snippetInfoVector.push_back(info8);
        queryNum = 20;
    }
    else if(queryStatement == "TPC-H_21"){
        querySnippetInfo info0,info1,info2,info3, info4, info5, info6, info7, info8, info9, info10;
        info0 = parseSnippet("/usr/local/snippet/tpch21/tpch21-0.json");
        info1 = parseSnippet("/usr/local/snippet/tpch21/tpch21-1.json");
        info2 = parseSnippet("/usr/local/snippet/tpch21/tpch21-2.json");
        info3 = parseSnippet("/usr/local/snippet/tpch21/tpch21-3.json");
        info4 = parseSnippet("/usr/local/snippet/tpch21/tpch21-4.json");
        info5 = parseSnippet("/usr/local/snippet/tpch21/tpch21-5.json");
        info6 = parseSnippet("/usr/local/snippet/tpch21/tpch21-6.json");
        info7 = parseSnippet("/usr/local/snippet/tpch21/tpch21-7.json");
        info8 = parseSnippet("/usr/local/snippet/tpch21/tpch21-8.json");
        info9 = parseSnippet("/usr/local/snippet/tpch21/tpch21-9.json");
        info10 = parseSnippet("/usr/local/snippet/tpch21/tpch21-10.json");

        snippetInfoVector.push_back(info0);
        snippetInfoVector.push_back(info1);
        snippetInfoVector.push_back(info2);
        snippetInfoVector.push_back(info3);
        snippetInfoVector.push_back(info4);
        snippetInfoVector.push_back(info5);
        snippetInfoVector.push_back(info6);
        snippetInfoVector.push_back(info7);
        snippetInfoVector.push_back(info8);
        snippetInfoVector.push_back(info9);
        snippetInfoVector.push_back(info10);
        queryNum = 21;
    }
    else{
        querySnippetInfo info0,info1,info2,info3, info4, info5, info6, info7, info8, info9, info10;
        info0 = parseSnippet("/usr/local/snippet/tpch22/tpch22-0.json");
        info1 = parseSnippet("/usr/local/snippet/tpch22/tpch22-1.json");
        info2 = parseSnippet("/usr/local/snippet/tpch22/tpch22-2.json");
        info3 = parseSnippet("/usr/local/snippet/tpch22/tpch22-3.json");
        info4 = parseSnippet("/usr/local/snippet/tpch22/tpch22-4.json");
        info5 = parseSnippet("/usr/local/snippet/tpch22/tpch22-5.json");
        info6 = parseSnippet("/usr/local/snippet/tpch22/tpch22-6.json");

        snippetInfoVector.push_back(info0);
        snippetInfoVector.push_back(info1);
        snippetInfoVector.push_back(info2);
        snippetInfoVector.push_back(info3);
        snippetInfoVector.push_back(info4);
        snippetInfoVector.push_back(info5);
        snippetInfoVector.push_back(info6);
        queryNum = 22;
    }
    return snippetInfoVector;
}

querySnippetInfo parseSnippet(std::string filename){
    querySnippetInfo returnInfo;
    std::ifstream file(filename);
    if(!file.is_open()){
        std::cerr<<"Error opening File : "<<filename<<std::endl;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string fileContent = buffer.str();
    returnInfo.snippetOriginal = fileContent.c_str();
    Document document;
    document.Parse(fileContent.c_str());
    returnInfo.snippetOriginal = fileContent.c_str();
    if(document.HasParseError()){
        std::cerr<<"Error parsing Json in file : "<<filename << std::endl;
    }

    if(document["type"].GetInt() == 0){
        returnInfo.snippetType = "CSD_Scan";
    }
    else if(document["type"].GetInt() == 1){
        returnInfo.snippetType = "Aggregation";
    }
    else if(document["type"].GetInt() == 2){
        returnInfo.snippetType = "Storage_Filter";
    }
    else if(document["type"].GetInt() == 3){
        returnInfo.snippetType = "Inner_Join";
    }
    else if(document["type"].GetInt() == 4){
        returnInfo.snippetType = "Left_Outer_Join";
    }
    else if(document["type"].GetInt() == 5){
        returnInfo.snippetType = "Right_Outer_Join";
    }
    else if(document["type"].GetInt() == 6){
        returnInfo.snippetType = "Cross_Join";
    }
    else if(document["type"].GetInt() == 7){
        returnInfo.snippetType = "Union";
    }
    else if(document["type"].GetInt() == 8){
        returnInfo.snippetType = "In";
    }
    else if(document["type"].GetInt() == 9){
        returnInfo.snippetType = "Depend_Inner_Join";
    }
    else if(document["type"].GetInt() == 10){
        returnInfo.snippetType = "Depend_Exist";
    }
    else{
        returnInfo.snippetType = "Depend_In";
    }

    const Value& value = document["snippet"];
    if(value.HasMember("tableFilter") & returnInfo.snippetType != "Storage_Filter" & returnInfo.snippetType != "Union"){
        returnInfo.filterCount = value["tableFilter"].Size();
        for(int i = 0; i<value["tableFilter"].Size();i++){ // not 연산 처리 필요
            const Value& tempValue = value["tableFilter"][i];
            if(tempValue.HasMember("Operator") == true && tempValue.HasMember("RV") == false){ // not이 들어가지 않는 경우 
                returnInfo.filterInfo.push_back(std::to_string(tempValue["Operator"].GetInt()));
            }
            else if(tempValue.HasMember("Operator") == true && tempValue.HasMember("RV") == true && tempValue.HasMember("LV") == false){ // substring 이 들어가는 경우
                const Value& tempValuerv = tempValue["RV"];
                returnInfo.filterInfo.push_back(std::to_string(tempValue["Operator"].GetInt()));
                returnInfo.filterInfo.push_back(std::to_string(tempValuerv["type"][0].GetInt()));
                returnInfo.filterInfo.push_back(std::to_string(tempValuerv["value"].Size()));
            }
            else{
                const Value& tempValuelv = tempValue["LV"];
                const Value& tempValuerv = tempValue["RV"];
                if(tempValuerv["type"].Size() != 0 & tempValuelv["type"].Size() != 0){
                    returnInfo.filterInfo.push_back(std::to_string(tempValuelv["type"][0].GetInt()));
                    returnInfo.filterInfo.push_back(tempValuelv["value"][0].GetString());
                    returnInfo.filterInfo.push_back(std::to_string(tempValue["Operator"].GetInt()));
                    if(returnInfo.filterInfo.back() == "9"){ // in 처리를 위한 if문
                        std::string inString = "";
                        returnInfo.filterInfo.push_back(std::to_string(tempValuerv["type"][0].GetInt()));
                        for(int j = 0;j<tempValuerv["value"].Size();j++){
                            inString += tempValuerv["value"][j].GetString();
                            if(j != tempValuerv["value"].Size()-1){
                                inString += "|";
                            } // -> 20|40|22|30||
                        }
                        returnInfo.filterInfo.push_back(inString);
                    }
                    else if(returnInfo.filterInfo.back() == "8"){
                        std::string inString = "";
                        returnInfo.filterInfo.push_back(std::to_string(tempValuerv["type"][0].GetInt()));
                        inString += tempValuerv["value"][0].GetString();
                        inString += "|";
                        inString += tempValuerv["value"][1].GetString();
                        returnInfo.filterInfo.push_back(inString);
                    }
                    else if(returnInfo.filterInfo.back() == "16"){
                        returnInfo.filterInfo.push_back(std::to_string(tempValuerv["type"][0].GetInt()));
                        returnInfo.filterInfo.push_back(tempValuerv["value"][1].GetString());
                    }
                    else{
                        returnInfo.filterInfo.push_back(std::to_string(tempValuerv["type"][0].GetInt()));
                        returnInfo.filterInfo.push_back(tempValuerv["value"][0].GetString());
                    }
                }
            }
        }
        for(int i=0;i<returnInfo.filterInfo.size();i++){
            std::cout<<"Table Filter Info "<<i<<" "<<returnInfo.filterInfo[i]<<std::endl;
        }
        std::cout<<"Table Filter Count "<<returnInfo.filterCount<<std::endl; 
    }
    if(value.HasMember("tableFilter") & returnInfo.snippetType == "Union"){
        returnInfo.filterCount = value["tableFilter"].Size();
        const Value& tempValue = value["tableFilter"][0];
        returnInfo.filterInfo.push_back(std::to_string(tempValue["Operator"].GetInt()));
    }
    if(value.HasMember("groupBy")){
        returnInfo.groupByCount = value["groupBy"].Size();
    }
    if(value.HasMember("orderBy")){
        const Value& tempValue = value["orderBy"];
        returnInfo.orderByCount = tempValue["columnName"].Size();
    }
    if(value.HasMember("tableFilter") & returnInfo.snippetType == "Storage_Filter"){
        returnInfo.havingCount = value["tableFilter"].Size();
        returnInfo.filterCount = value["tableFilter"].Size();
        for(int i = 0; i<value["tableFilter"].Size();i++){ // not 연산 처리 필요
            const Value& tempValue = value["tableFilter"][i];
            if(tempValue.HasMember("Operator") == true && tempValue.HasMember("RV") == false){ // not이 들어가지 않는 경우 
                returnInfo.filterInfo.push_back(std::to_string(tempValue["Operator"].GetInt()));
            }
            else if(tempValue.HasMember("Operator") == true && tempValue.HasMember("RV") == true && tempValue.HasMember("LV") == false){ // substring 이 들어가는 경우
                const Value& tempValuerv = tempValue["RV"];
                returnInfo.filterInfo.push_back(std::to_string(tempValue["Operator"].GetInt()));
                returnInfo.filterInfo.push_back(std::to_string(tempValuerv["type"][0].GetInt()));
                returnInfo.filterInfo.push_back(std::to_string(tempValuerv["value"].Size()));
            }
            else{
                const Value& tempValuelv = tempValue["LV"];
                const Value& tempValuerv = tempValue["RV"];
                if(tempValuerv["type"].Size() != 0 & tempValuelv["type"].Size() != 0){
                    returnInfo.filterInfo.push_back(std::to_string(tempValuelv["type"][0].GetInt()));
                    returnInfo.filterInfo.push_back(tempValuelv["value"][0].GetString());
                    returnInfo.filterInfo.push_back(std::to_string(tempValue["Operator"].GetInt()));
                    if(returnInfo.filterInfo.back() == "9"){ // in 처리를 위한 if문
                        std::string inString = "";
                        returnInfo.filterInfo.push_back(std::to_string(tempValuerv["type"][0].GetInt()));
                        for(int j = 0;j<tempValuerv["value"].Size();j++){
                            inString += tempValuerv["value"][j].GetString();
                            if(j != tempValuerv["value"].Size()-1){
                                inString += "|";
                            } // -> 20|40|22|30||
                        }
                        returnInfo.filterInfo.push_back(inString);
                    }
                    else if(returnInfo.filterInfo.back() == "8"){
                        std::string inString = "";
                        returnInfo.filterInfo.push_back(std::to_string(tempValuerv["type"][0].GetInt()));
                        inString += tempValuerv["value"][0].GetString();
                        inString += "|";
                        inString += tempValuerv["value"][1].GetString();
                        returnInfo.filterInfo.push_back(inString);
                    }
                    else if(returnInfo.filterInfo.back() == "16"){
                        returnInfo.filterInfo.push_back(std::to_string(tempValuerv["type"][0].GetInt()));
                        returnInfo.filterInfo.push_back(tempValuerv["value"][1].GetString());
                    }
                    else{
                        returnInfo.filterInfo.push_back(std::to_string(tempValuerv["type"][0].GetInt()));
                        returnInfo.filterInfo.push_back(tempValuerv["value"][0].GetString());
                    }
                }
            }
        }
    }
    if(value.HasMember("limit")){
        returnInfo.limitExist = 1;
    }
    if(value.HasMember("columnProjection")){
        returnInfo.projectionCount = value["columnProjection"].Size();
    }
    if(value.HasMember("workID")){
        returnInfo.workID = value["workID"].GetInt();
    }
    if(value.HasMember("tableAlias")){
        returnInfo.tableAlias = value["tableAlias"].GetString();
    }
    if(value.HasMember("tableName")){
        if(value["tableName"].Size() == 2){
            returnInfo.tableName[0] = value["tableName"][0].GetString();
            returnInfo.tableName[1] = value["tableName"][1].GetString();
            std::cout<<"TABLE NAME : "<<returnInfo.tableName[0]<<" TABLE NAME : "<<returnInfo.tableName[1]<<std::endl;
        }
        else{
            returnInfo.tableName[0] = value["tableName"][0].GetString();
            returnInfo.tableName[1] = "none";
            std::cout<<"TABLE NAME : "<<returnInfo.tableName[0]<<" TABLE NAME : "<<returnInfo.tableName[1]<<std::endl;
        }
    }
    returnInfo.validationID = simulationNum;
    return returnInfo;
}

void InsertSnippetDB(std::vector<querySnippetInfo> snippetVector){
    DBManager& dbManager = DBManager::getInstance();
    for(int i=0;i<snippetVector.size();i++){
        int snippetType = 0;
            if(snippetVector[i].snippetType  == "CSD_Scan"){
        snippetType  = 0;
    }
    else if(snippetVector[i].snippetType  == "Aggregation"){
        snippetType  = 1;
    }
    else if(snippetVector[i].snippetType  == "Storage_Filter"){
        snippetType  = 2;
    }
    else if(snippetVector[i].snippetType  == "Inner_Join"){
        snippetType  = 3;
    }
    else if(snippetVector[i].snippetType  == "Left_Outer_Join"){
        snippetType  = 4;
    }
    else if(snippetVector[i].snippetType  == "Right_Outer_Join"){
        snippetType  = 5;
    }
    else if(snippetVector[i].snippetType  == "Cross_Join"){
        snippetType  = 6;
    }
    else if(snippetVector[i].snippetType  == "Union"){
        snippetType  = 7;
    }
    else if(snippetVector[i].snippetType  == "In"){
        snippetType  = 8;
    }
    else if(snippetVector[i].snippetType  == "Depend_Inner_Join"){
        snippetType  = 9;
    }
    else if(snippetVector[i].snippetType  == "Depend_Exist"){
        snippetType  = 10;
    }
    else{
        snippetType  = 11;
    }
        std::string queryState = "insert into validation_snippet (validation_id, work_id, snippet_type, projection_count, filter_count, group_by_count, order_by_count,limit_exist) values (";
        queryState = queryState + std::to_string(snippetVector[i].validationID) + "," +std::to_string(snippetVector[i].workID) + "," + std::to_string(snippetType) + "," + std::to_string(snippetVector[i].projectionCount) + "," + std::to_string(snippetVector[i].filterCount) + "," + std::to_string(snippetVector[i].groupByCount) + "," +std::to_string(snippetVector[i].orderByCount) + "," + std::to_string(snippetVector[i].limitExist) +")";

        try{
            sql::ResultSet *resultSet = dbManager.executeQuery(queryState);
            while(resultSet->next()){
            std::cout<<resultSet->getString(1)<<std::endl;
        }
            delete resultSet;
        } catch (sql::SQLException& e){
            std::cerr<<e.what();
        }
    }
}