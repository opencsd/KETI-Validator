#include <string>
#include <vector>
#include "TypeDef.hpp"
#include <iostream>
#include <ctime>
#include <iomanip>
#include "HistogramTable.hpp"
#include "DBManager.hpp"

std::string  SSDValidatorTemp(std::string queryStatement,std::vector<querySnippetInfo> snippetInfo,int queryNum, optionInfo option, int optionID, std::string userID, int simulationNnm, std::string returnJson);
std::string getTimestamp();
storageValidation executeSSDValidate(std::vector<querySnippetInfo> snippetInfo, int queryNum);
 std::string convertTPCH1(std::string tpch);

 float applyWeight1(float value);