#include "CSDValidator.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <thread>

std::string CSDValidatorTemp(std::vector<querySnippetInfo> snippetInfo,std::string queryStatement, int queryNum,int optionID, optionInfo option, std::string userID, int simulationNnm, std::string returnJson){
    std::cout<<"---CSD VALIDATION START---\n";
    validationLog validateLog;
    validateLog.optionID = optionID;
    validateLog.queryStatement = queryStatement;
    validateLog.userID = userID;
    validateLog.validationID = simulationNnm;
    validateLog.SnippetCount = (int)snippetInfo.size();
    storageValidation histogramValidate = executeCSDValidate(snippetInfo,queryNum);
    validateLog.networkUsage = histogramValidate.networkUsage;
    validateLog.filteredRow = histogramValidate.filteredRow;
    validateLog.scannedRow = histogramValidate.scannedRow;
    std::cout<<"RESULT CSD NETWORK USAGE : "<<validateLog.networkUsage<<std::endl;
    std::cout<<"RESULT CSD SCANNED ROW : " <<validateLog.scannedRow<<std::endl;
    std::cout<<"RESULT CSD FILTERED ROW : "<<validateLog.filteredRow<<std::endl;
    float csdTime;
    if(queryNum == 1 | queryNum == 4 | queryNum == 5 | queryNum == 6| queryNum == 9 |queryNum == 12|queryNum == 13|queryNum == 17|queryNum == 18|queryNum == 19|queryNum == 20){
        csdTime = histogramValidate.scannedRow / csdBadScanWeight;
    }
    else if(queryNum == 7 | queryNum == 8 | queryNum == 10 | queryNum == 14 | queryNum == 15){
        csdTime = histogramValidate.scannedRow / csdNormalScanWeight;
    }
    else{
        csdTime = histogramValidate.scannedRow / csdGoodScanWeight;
    }
    
    std::cout<<"RESULT CSD TIME : "<<csdTime/8<<std::endl;
    int csdCount = option.csdCount;
    csdTime = csdTime / csdCount;
    validateLog.executionTime = csdTime;
    if(csdTime > 5){
        sleep(5);
    }
    else{
        sleep(csdTime);
    }
    DBManager& dbManager = DBManager::getInstance();
    std::string queryState = "insert into validation_csd_metric (validation_id, storage_id, csd_cpu_usage_predict, csd_power_usage_predict) values (";
    std::string jsonCode = "\"Validation_CSD_Metric\" : {\n[\n";
    srand(static_cast<unsigned int>(time(0)));
    for(int i=0;i<csdCount;i++){
        csdValidtion csdLog;
        csdLog.validationID = simulationNnm;
        csdLog.storageID = i;
        float cpuUsage;
        float powerUsage;
        if(queryNum == 1 | queryNum == 3 | queryNum == 4 | queryNum == 10 | queryNum == 15 | queryNum == 17 | queryNum == 20){
            cpuUsage = applyWeight(csdTime* csdBadCPUweight);
        }
        else if(queryNum == 6 | queryNum == 7 | queryNum == 8 | queryNum == 12 | queryNum == 19){
            cpuUsage = applyWeight(csdTime* CSDNormalCPUweight);
        }
        else{
            cpuUsage = applyWeight(csdTime* CSDGoodCPUWeight);
        }
        powerUsage = applyWeight( csdTime* csdpowerweight);
        csdLog.cpuUsage = cpuUsage;
        csdLog.powerUsage = powerUsage;
        std::cout<<"RESULT CSD"<<i+1<<" CPU : "<<csdLog.cpuUsage<<std::endl;
        std::cout<<"RESULT CSD"<<i+1<<" POWER : "<<csdLog.powerUsage<<std::endl;

        jsonCode += "{\n\"Simulation_ID\" : " + std::to_string(csdLog.validationID) + ",\n\"CSD_Name\" : " + std::to_string(csdLog.storageID)+ ",\n\"CPU_Usage\" : " + std::to_string(csdLog.cpuUsage) + ",\n\"Power_Usage\" : " + std::to_string(csdLog.powerUsage) + "\n}";
        if(i<csdCount-1){
            jsonCode += ",";
        }
    }
    jsonCode += "\n]\n},\n";
    returnJson += jsonCode;
    std::cout<<"---CSD VALIDATION FINISHED---\n";
    returnJson = StorageValidatorMain(validateLog,snippetInfo, queryNum, option,optionID, userID, simulationNnm, returnJson);

    for(int i=0;i<csdCount;i++){
        csdValidtion csdLog;
        csdLog.validationID = simulationNnm;
        csdLog.storageID = i;
        float cpuUsage;
        float powerUsage;
        if(queryNum == 1 | queryNum == 3 | queryNum == 4 | queryNum == 10 | queryNum == 15 | queryNum == 17 | queryNum == 20){
            cpuUsage = applyWeight(csdTime* csdBadCPUweight);
        }
        else if(queryNum == 6 | queryNum == 7 | queryNum == 8 | queryNum == 12 | queryNum == 19){
            cpuUsage = applyWeight(csdTime* CSDNormalCPUweight);
        }
        else{
            cpuUsage = applyWeight(csdTime* CSDGoodCPUWeight);
        }
        powerUsage = applyWeight( csdTime* csdpowerweight);
        csdLog.cpuUsage = cpuUsage;
        csdLog.powerUsage = powerUsage;

        std::string newQueryState = queryState + std::to_string(csdLog.validationID) + ", " + std::to_string(csdLog.storageID) + ", " + std::to_string(csdLog.cpuUsage) + ", " + std::to_string(csdLog.powerUsage) + ")";
        //std::cout<<newQueryState<<std::endl;
        try{
            sql::ResultSet *resultSet = dbManager.executeQuery(newQueryState);
            delete resultSet;
        } catch (sql::SQLException& e){
            if(e.what() != "")
                std::cerr<<e.what();
        }
    }
    return returnJson;
}

float applyWeight(float value) {
    float weight = static_cast<float>(rand()) / RAND_MAX * 0.06 - 0.03;
    float result = value * (1.0 + weight);

    return result;
}

storageValidation executeCSDValidate(std::vector<querySnippetInfo> snippetInfo, int queryNum){
    storageValidation resultValidate;
    float network = 0;
    int scannedrow = 0;
    int filteredrow = 0;
    for(int i=0;i<snippetInfo.size();i++){
        querySnippetInfo snippet = snippetInfo[i];
        if(snippet.snippetType == "CSD_Scan"){
            std::string tableName = snippet.tableName[0];
            float filterRatio;
            filterRatio = getFilteredRow(snippet);
            std::cout<<"FILTER RATIO : "<<filterRatio<<std::endl;
            if(tableName == "lineitem"){
                network += lineitemSize * filterRatio * (snippet.projectionCount/lineitemCol);
                scannedrow += lineitemRow;
                filteredrow += filterRatio * lineitemRow;
            }
            if(tableName == "region"){
                network += regionSize * filterRatio * (snippet.projectionCount / regionCol);
                scannedrow += regionRow;
                filteredrow += filterRatio * regionRow;
            }
            if(tableName == "nation"){
                network += nationSize * filterRatio * (snippet.projectionCount / nationCol);
                scannedrow += nationRow;
                filteredrow += filterRatio * nationRow;
            }
            if(tableName == "part"){
                network += partSize * filterRatio * (snippet.projectionCount / partCol);
                scannedrow += partRow;
                filteredrow += filterRatio * partRow;
            }
            if(tableName == "partsupp"){
                network += partsuppSize * filterRatio * (snippet.projectionCount / partsuppCol);
                scannedrow += partsuppRow;
                filteredrow += filterRatio * partsuppRow;
            }
            if(tableName == "customer"){
                network += customerSize * filterRatio * (snippet.projectionCount / customerCol);
                scannedrow += customerRow;
                filteredrow += filterRatio * customerRow;
            }
            if(tableName == "orders"){
                network += ordersSize * filterRatio * (snippet.projectionCount / orderCol);
                scannedrow += ordersRow;
                filteredrow += filterRatio * ordersRow;
            }
            if(tableName == "supplier"){
                network += supplierSize * filterRatio * (snippet.projectionCount / supplierCol);
                scannedrow += supplierRow;
                filteredrow += filterRatio * supplierRow;
            }
        }
    }
    resultValidate.filteredRow = filteredrow;
    resultValidate.scannedRow = scannedrow;
    resultValidate.networkUsage = network;
    return resultValidate;
}