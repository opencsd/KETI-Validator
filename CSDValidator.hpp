#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <string.h>
#include <unordered_map>
#include <sys/socket.h>
#include <cstdlib>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <condition_variable>
#include <queue>
#include <thread>
#include "TypeDef.hpp"
#include "HistogramTable.hpp"
#include "DBManager.hpp"
#include "StorageValidator.hpp"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h" 

float applyWeight(float value);
storageValidation executeCSDValidate(std::vector<querySnippetInfo> snippetInfo, int queryNum);
std::string CSDValidatorTemp(std::vector<querySnippetInfo> snippetInfo,std::string queryStatement, int queryNum,int optionID, optionInfo option, std::string userID, int simulationNnm, std::string returnJson);
double executeQEMU(std::vector<querySnippetInfo> snippetInfo, int queryNum, int tpchNum);







