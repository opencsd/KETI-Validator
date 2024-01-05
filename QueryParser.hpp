#include <string>
#include "DBManager.hpp"
#include <vector>
#include "SSDValidator.hpp"
#include "CSDValidator.hpp"


void InitQueryParser();
querySnippetInfo parseSnippet(std::string filename);
std::string QueryParserMain(std::string queryStatement, int optionID, std::string userID);
optionInfo getOptionInfo(int optionID, std::string userID);
std::vector<querySnippetInfo> getSnippetInfo(std::string queryStatement);
void InsertSnippetDB(std::vector<querySnippetInfo> snippetVector);