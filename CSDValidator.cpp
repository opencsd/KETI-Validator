#include "CSDValidator.hpp"
#include "QEMUHandler.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <thread>

float adjust5percent(float value){
    static bool seedInitialized = false;
    if (!seedInitialized) {
        srand(static_cast<unsigned int>(time(nullptr)));
        seedInitialized = true;
    }

    // -5% ~ +5% 범위의 비율 생성
    float adjustmentFactor = (static_cast<float>(rand() % 11) - 5) / 100.0f;

    // 값 조정2
    return value * (1.0f + adjustmentFactor);
}

std::string CSDValidatorTemp(std::vector<querySnippetInfo> snippetInfo,std::string queryStatement, int queryNum,int optionID, optionInfo option, std::string userID, int simulationNnm, std::string returnJson){
    std::cout<<"---CSD VALIDATION START---\n";
    validationLog validateLog;
    validateLog.optionID = optionID;
    validateLog.queryStatement = queryStatement;
    validateLog.userID = userID;
    validateLog.validationID = simulationNnm;
    validateLog.SnippetCount = (int)snippetInfo.size();
    
    double QEMUTime = 0;

    QEMUTime = executeQEMU(snippetInfo, simulationNnm, queryNum); // 8 개일때 기준
    std::cout<<"EXECUTION QEMU TIME : "<<QEMUTime<<std::endl;

    storageValidation histogramValidate;
    float network = 0;
    int scannedrow = 0;
    int filteredrow = 0;
    int over10000row = 0;
    int suppCount = 0;
    for(int i=0;i<snippetInfo.size();i++){
        querySnippetInfo snippet = snippetInfo[i];
        if(snippet.snippetType == "CSD_Scan"){
            std::string tableName = snippet.tableName[0];
            float filterRatio;
            filterRatio = getFilteredRow(snippet);
            snippetInfo[i].filterRatio = filterRatio;
            std::cout<<"FILTER RATIO : "<<filterRatio<<std::endl;
            if(tableName == "lineitem"){
                over10000row++;
                network += lineitemSize * filterRatio * (snippet.projectionCount/lineitemCol);
                scannedrow += lineitemRow;
                filteredrow += filterRatio * lineitemRow;
                snippetInfo[i].filterCount = filterRatio * lineitemRow;
                snippetInfo[i].topTableRow = lineitemRow;
            }
            if(tableName == "region"){
                network += regionSize * filterRatio * (snippet.projectionCount / regionCol);
                scannedrow += regionRow;
                filteredrow += filterRatio * regionRow;
                snippetInfo[i].filterCount = filterRatio * regionRow;
                snippetInfo[i].topTableRow = regionRow;
            }
            if(tableName == "nation"){
                network += nationSize * filterRatio * (snippet.projectionCount / nationCol);
                scannedrow += nationRow;
                filteredrow += filterRatio * nationRow;
                snippetInfo[i].filterCount = filterRatio * nationRow;
                snippetInfo[i].topTableRow = nationRow;
            }
            if(tableName == "part"){
                over10000row++;
                network += partSize * filterRatio * (snippet.projectionCount / partCol);
                scannedrow += partRow;
                filteredrow += filterRatio * partRow;
                snippetInfo[i].filterCount = filterRatio * partRow;
                snippetInfo[i].topTableRow = partRow;
            }
            if(tableName == "partsupp"){
                over10000row++;
                network += partsuppSize * filterRatio * (snippet.projectionCount / partsuppCol);
                scannedrow += partsuppRow;
                filteredrow += filterRatio * partsuppRow;
                snippetInfo[i].filterCount = filterRatio * partsuppRow;
                snippetInfo[i].topTableRow = partsuppRow;
            }
            if(tableName == "customer"){
                over10000row++;
                network += customerSize * filterRatio * (snippet.projectionCount / customerCol);
                scannedrow += customerRow;
                filteredrow += filterRatio * customerRow;
                snippetInfo[i].filterCount = filterRatio * customerRow;
                snippetInfo[i].topTableRow = customerRow;
            }
            if(tableName == "orders"){
                over10000row++;
                network += ordersSize * filterRatio * (snippet.projectionCount / orderCol);
                scannedrow += ordersRow;
                filteredrow += filterRatio * ordersRow;
                snippetInfo[i].filterCount = filterRatio * ordersRow;
                snippetInfo[i].topTableRow = ordersRow;
            }
            if(tableName == "supplier"){
                suppCount++;
                network += supplierSize * filterRatio * (snippet.projectionCount / supplierCol);
                scannedrow += supplierRow;
                filteredrow += filterRatio * supplierRow;
                snippetInfo[i].filterCount = filterRatio * supplierRow;
                snippetInfo[i].topTableRow = supplierRow;
            }
            std::cout<<"SNIPPET INFO \n"<<snippetInfo[i].filterCount<<" "<<snippetInfo[i].filterRatio<<" "<<snippetInfo[i].snippetType<<" "<<snippetInfo[i].tableAlias<<"\n";
        }
    }

    int qemuScannedRow = over10000row * 10000 + 1250 * suppCount;
    double qemuScannedRatio = scannedrow / qemuScannedRow;
    double csdTime;
    std::cout<<"QEMU SCANNED RATIO : "<<qemuScannedRatio<<" QEMUTIME : "<<QEMUTime<<"\n";
    if(queryNum == 1 || queryNum == 12){
        csdTime = QEMUTime * qemuScannedRatio / qemuRedWeight;
    }
    else if(queryNum == 2 || queryNum == 5 || queryNum == 9){
        csdTime = QEMUTime * qemuScannedRatio / qemuOrangeWeight;
    }
    else if(queryNum == 3 || queryNum == 4 || queryNum == 8 || queryNum == 6|| queryNum == 7 || queryNum == 10 || queryNum == 14 || queryNum == 16 || queryNum == 17 || queryNum == 20 || queryNum == 22){
        csdTime = QEMUTime * qemuScannedRatio /qemuYellowWeight;
    }
    else if(queryNum == 11 || queryNum == 15){
        csdTime = QEMUTime * qemuScannedRatio / qemuBrownWeight;
    }
    else{
        csdTime = QEMUTime * qemuScannedRatio / qemuGreenWeight;
    }

    histogramValidate.filteredRow = filteredrow;
    histogramValidate.scannedRow = scannedrow;
    histogramValidate.networkUsage = network;
    

    validateLog.networkUsage = histogramValidate.networkUsage;
    validateLog.filteredRow = histogramValidate.filteredRow;
    validateLog.scannedRow = histogramValidate.scannedRow;
    std::cout<<"RESULT CSD NETWORK USAGE : "<<validateLog.networkUsage<<std::endl;
    std::cout<<"RESULT CSD SCANNED ROW : " <<validateLog.scannedRow<<std::endl;
    std::cout<<"RESULT CSD FILTERED ROW : "<<validateLog.filteredRow<<std::endl;
    //csd TIme -> QEMU time
    
    // if(queryNum == 1 | queryNum == 4 | queryNum == 5 | queryNum == 6| queryNum == 9 |queryNum == 12|queryNum == 13|queryNum == 17|queryNum == 18|queryNum == 19|queryNum == 20){
    //     csdTime = histogramValidate.scannedRow / csdBadScanWeight;
    // }
    // else if(queryNum == 7 | queryNum == 8 | queryNum == 10 | queryNum == 14 | queryNum == 15){
    //     csdTime = histogramValidate.scannedRow / csdNormalScanWeight;
    // }
    // else{
    //     csdTime = histogramValidate.scannedRow / csdGoodScanWeight;
    // }
    
    std::cout<<"RESULT CSD TIME : "<<csdTime<<std::endl;
    
    int csdCount = option.csdCount;
    csdCount = 8 / csdCount;
    csdTime = csdTime * csdCount;

    validateLog.executionTime = csdTime;
    // if(csdTime > 5){
    //     sleep(5);
    // }
    // else{
    //     sleep(csdTime);
    // }
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
        if(queryNum == 1 | queryNum == 2 | queryNum == 3| queryNum == 4 | queryNum == 9 | queryNum == 11 | queryNum == 16 | queryNum == 17 | queryNum == 18 | queryNum == 20 | queryNum == 22){
            cpuUsage = applyWeight(csdTime* csdRedCPUWeight);
        }
        else if(queryNum == 6 | queryNum == 7 | queryNum == 10 | queryNum == 19){
            cpuUsage = applyWeight(csdTime* csdOrangeCPUWeight);
        }
        else if(queryNum == 5 | queryNum == 8 | queryNum == 14 | queryNum == 15 | queryNum == 21){
            cpuUsage = applyWeight(csdTime * csdGreenCPUWeight);
        }
        else if(queryNum == 12){
            cpuUsage = applyWeight(csdTime * csdYellowCPUWeight);
        }
        else{
            cpuUsage = applyWeight(csdTime* csd13CPUWeight);
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

    for(int i=0;i<option.csdCount;i++){
        csdValidtion csdLog;
        csdLog.validationID = simulationNnm;
        csdLog.storageID = i;
        float cpuUsage;
        float powerUsage;
        if(queryNum == 1 | queryNum == 2 | queryNum == 4 | queryNum == 9 | queryNum == 11 | queryNum == 16 | queryNum == 17 | queryNum == 18 | queryNum == 20 | queryNum == 22){
            cpuUsage = applyWeight(csdTime* csdRedCPUWeight);
        }
        else if(queryNum == 6 | queryNum == 7 | queryNum == 10 | queryNum == 19){
            cpuUsage = applyWeight(csdTime* csdOrangeCPUWeight);
        }
        else if(queryNum == 5 | queryNum == 8 | queryNum == 14 | queryNum == 15 | queryNum == 21){
            cpuUsage == applyWeight(csdTime * csdGreenCPUWeight);
        }
        else if(queryNum == 3 || queryNum == 12){
            cpuUsage = applyWeight(csdTime * csdYellowCPUWeight);
        }
        else{
            cpuUsage = applyWeight(csdTime* csd13CPUWeight);
        }
        powerUsage = applyWeight( csdTime* csdpowerweight);
        csdLog.cpuUsage = adjust5percent(cpuUsage);
        csdLog.powerUsage = adjust5percent(powerUsage);

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
    float weight = 0;
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
            snippet.filterRatio = filterRatio;
            std::cout<<"FILTER RATIO : "<<filterRatio<<std::endl;
            if(tableName == "lineitem"){
                network += lineitemSize * filterRatio * (snippet.projectionCount/lineitemCol);
                scannedrow += lineitemRow;
                filteredrow += filterRatio * lineitemRow;
                snippet.filterCount = filterRatio * lineitemRow;
            }
            if(tableName == "region"){
                network += regionSize * filterRatio * (snippet.projectionCount / regionCol);
                scannedrow += regionRow;
                filteredrow += filterRatio * regionRow;
                snippet.filterCount = filterRatio * regionRow;
            }
            if(tableName == "nation"){
                network += nationSize * filterRatio * (snippet.projectionCount / nationCol);
                scannedrow += nationRow;
                filteredrow += filterRatio * nationRow;
                snippet.filterCount = filterRatio * nationRow;
            }
            if(tableName == "part"){
                network += partSize * filterRatio * (snippet.projectionCount / partCol);
                scannedrow += partRow;
                filteredrow += filterRatio * partRow;
                snippet.filterCount = filterRatio * partRow;
            }
            if(tableName == "partsupp"){
                network += partsuppSize * filterRatio * (snippet.projectionCount / partsuppCol);
                scannedrow += partsuppRow;
                filteredrow += filterRatio * partsuppRow;
                snippet.filterCount = filterRatio * partsuppRow;
            }
            if(tableName == "customer"){
                network += customerSize * filterRatio * (snippet.projectionCount / customerCol);
                scannedrow += customerRow;
                filteredrow += filterRatio * customerRow;
                snippet.filterCount = filterRatio * customerRow;
            }
            if(tableName == "orders"){
                network += ordersSize * filterRatio * (snippet.projectionCount / orderCol);
                scannedrow += ordersRow;
                filteredrow += filterRatio * ordersRow;
                snippet.filterCount = filterRatio * ordersRow;
            }
            if(tableName == "supplier"){
                network += supplierSize * filterRatio * (snippet.projectionCount / supplierCol);
                scannedrow += supplierRow;
                filteredrow += filterRatio * supplierRow;
                snippet.filterCount = filterRatio * supplierRow;
            }
            
        }
    }
    resultValidate.filteredRow = filteredrow;
    resultValidate.scannedRow = scannedrow;
    resultValidate.networkUsage = network;
    return resultValidate;
}

void convertTableFilterFormat(rapidjson::Value& filterValue, rapidjson::Document::AllocatorType& allocator) {
    if (filterValue.IsObject() && filterValue.HasMember("LV") && filterValue.HasMember("Operator") && filterValue.HasMember("RV")) {
        rapidjson::Value& lv = filterValue["LV"];
        rapidjson::Value& rv = filterValue["RV"];

        if (lv.IsObject() && lv.HasMember("type") && lv.HasMember("value") && lv["type"].IsArray() && lv["value"].IsArray() &&
            rv.IsObject() && rv.HasMember("type") && rv.HasMember("value") && rv["type"].IsArray() && rv["value"].IsArray()) {

            rapidjson::Value newFilter(rapidjson::kObjectType);

            // Convert Ch* to String
            rapidjson::Value lvValue(rapidjson::kStringType);
            lvValue.SetString(lv["value"][0].GetString(), allocator);
            newFilter.AddMember("LV", lvValue, allocator);

            newFilter.AddMember("Operator", filterValue["Operator"].GetInt(), allocator);

            // Convert Ch* to String
            std::string rvStringValue = rv["value"][0].GetString();
            if (rv["type"][0].GetInt() == 9 || rv["type"][0].GetInt() == 10) {
                rapidjson::Value rvValue(rapidjson::kStringType);
                rvValue.SetString(rvStringValue.c_str(), allocator);
                newFilter.AddMember("RV", rvValue, allocator);
            } else {
                // Try to convert the string to an integer
                try {
                    int rvIntValue = std::stoi(rvStringValue);
                    newFilter.AddMember("RV", rvIntValue, allocator);
                } catch (const std::invalid_argument& e) {
                    // Handle the case where conversion to integer fails
                    rapidjson::Value rvValue(rapidjson::kStringType);
                    rvValue.SetString(rvStringValue.c_str(), allocator);
                    newFilter.AddMember("RV", rvValue, allocator);
                }
            }

            filterValue = newFilter;
        }
    } else if (filterValue.IsArray()) {
        for (rapidjson::SizeType i = 0; i < filterValue.Size(); i++) {
            convertTableFilterFormat(filterValue[i], allocator);
        }
    }
}



double executeQEMU(std::vector<querySnippetInfo> snippetInfo, int queryNum, int tpchNum){
    std::vector<querySnippetInfo> csdScanSnippet;
    std::cout<<"EXECUTE QEMU\n";
    for(int i=0;i<snippetInfo.size();i++){
        if(snippetInfo[i].snippetType == "CSD_Scan"){
                csdScanSnippet.push_back(snippetInfo[i]);

        }
    }
    std::cout<<"CSD_SCAN NUM : "<<csdScanSnippet.size()<<std::endl;
    for(int i=0;i<csdScanSnippet.size();i++){
        if(csdScanSnippet[i].tableName[0] != "region"){
        int sock;
        sock = socket(PF_INET, SOCK_STREAM, 0);
        if(sock < 0){
                std::cout <<"Failed to create socket" <<std::endl;
                continue;
        }
        struct sockaddr_in serv_addr;
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = inet_addr("10.0.4.80");
        serv_addr.sin_port = htons(4445);

        if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
            std::cout <<ntohs(serv_addr.sin_port)<< " Failed to connect to server" << std::endl;
            close(sock);
            connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
            continue;
        }

        std::cout<<"Connect"<<std::endl;
        
        const char* snippetJson;
        snippetJson = csdScanSnippet[i].snippetOriginal.c_str();
        rapidjson::Document document;
        document.Parse(snippetJson);
        rapidjson::Value& snippetValue = document["snippet"];
        rapidjson::Document snippetDocument;
        snippetDocument.CopyFrom(snippetValue,snippetDocument.GetAllocator());

        snippetDocument["queryID"] = queryNum;
        rapidjson::Value csdName;
        csdName.SetString("1",snippetDocument.GetAllocator());
        snippetDocument.AddMember("csd_name",csdName, snippetDocument.GetAllocator());
        rapidjson::Value tpchID;
        tpchID.SetInt(tpchNum);
        snippetDocument.AddMember("TPCH_ID",tpchID,snippetDocument.GetAllocator());
        if(snippetDocument.IsObject() && snippetDocument.HasMember("tableFilter") && snippetDocument["tableFilter"].IsArray()){
            rapidjson::Value& tableFilterArray = snippetDocument["tableFilter"];
            for(int j = 0;j<tableFilterArray.Size();j++){
                convertTableFilterFormat(tableFilterArray[j], snippetDocument.GetAllocator());
                std::cout<<"CONVERT FILTER FORMAT DONE"<<"\n";
            }
        }

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        snippetDocument.Accept(writer);

        std::string sendSnippet = buffer.GetString();
        std::cout<<"\nCSD SEND SNIPPET FORMAT\n\n"<<sendSnippet<<"\n\n";
        size_t len = strlen(sendSnippet.c_str());
        send(sock, &len, sizeof(len),0);
        send(sock, (char *)sendSnippet.c_str(), strlen(sendSnippet.c_str()),0);
        close(sock);
    }
    }

    double returnTime = 0;
    double weightRatio = 1.0;
    double shrinkRatio = 1;
    int totalScanRow = 0;

    //GET RETURN TIME OF QEMU
    for(int i = 0;i<csdScanSnippet.size();i++){
        if(csdScanSnippet[i].tableName[0] != "region"){
        totalScanRow += csdScanSnippet[i].topTableRow;
        double tempTime = QEMUHandler::CalculateQEMU(queryNum,i);
        if(tempTime > returnTime){
            returnTime = tempTime;
        }
        }
    }

    return returnTime;
}

