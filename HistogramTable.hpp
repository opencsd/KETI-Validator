#pragma once
#include <iostream>
#include <map>
#include <sstream>
#include <vector>
#include "TypeDef.hpp"
struct histogram{
    std::string columnName;
    std::string dataType;
    float minValue;
    float maxValue;
    float bucketNum;
    long count;
    bool identifier = false;
    std::map<std::string, std::string> dataMap;
};

void InitHistogram();
std::map<std::string, std::string> makeDataMap(std::string input);
std::map<std::string, std::string> makeDataMapFloat(std::string input);
std::map<std::string, std::string> makeDataMapDate(std::string input);
float getFilteredRow(querySnippetInfo snippet);
histogram Stringtohisto(std::string str);
int dateToInt(std::string date);
