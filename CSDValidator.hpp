#include <string>
#include <vector>
#include "TypeDef.hpp"
#include "HistogramTable.hpp"
#include "DBManager.hpp"
#include "StorageValidator.hpp"

float applyWeight(float value);
storageValidation executeCSDValidate(std::vector<querySnippetInfo> snippetInfo, int queryNum);
std::string CSDValidatorTemp(std::vector<querySnippetInfo> snippetInfo,std::string queryStatement, int queryNum,int optionID, optionInfo option, std::string userID, int simulationNnm, std::string returnJson);