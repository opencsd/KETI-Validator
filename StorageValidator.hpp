#include "TypeDef.hpp"
#include <vector>
#include <ctime>
#include "DBManager.hpp"
#include "keti_log.h"
#include <thread>
#include <chrono>
std::string StorageValidatorMain(validationLog csdLog, std::vector<querySnippetInfo> snippetInfo, int queryNum, optionInfo option, int optionID, std::string userID, int simulationNnm, std::string returnJson);
std::string getTimestampp();
 std::string convertTPCH2(std::string tpch);
 float applyWeight2(float value);