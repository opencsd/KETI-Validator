#pragma once
#include <string.h>
#include <vector>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <sstream>

#include "merge_manager.h"
struct ScanResult;
struct FilterResult;

extern WorkQueue<Result> FilterQueue;
extern WorkQueue<Result> MergeQueue;

class Filter{
public:
    Filter(){}
    vector<string> column_filter;
    unordered_map<string, int> newstartptr;
    unordered_map<string, int> newlengthraw;
    unordered_map<string,string> joinmap;
    char data[BUFF_SIZE];
    struct RowFilterData{
        vector<int> startoff;
        vector<int> offlen;
        vector<int> datatype;
        vector<string> ColName;
        vector<int> varcharlist;
        unordered_map<string,int> ColIndexmap;
        int offsetcount;
        int rowoffset;
        char *rowbuf;
    };
    
    RowFilterData rowfilterdata;
    int BlockFilter(Result &scanResult);//*
    void Filtering();
    void SavedRow(char *row, int startoff, Result &filterresult, int nowlength);//*
    vector<string> split(string str, char Delimiter);
    bool LikeSubString(string lv, string rv);
    bool LikeSubString_v2(string lv, string rv);
    bool InOperator(string lv, Value& rv,unordered_map<string, int> typedata,char *rowbuf);
    bool InOperator(int lv, Value& rv,unordered_map<string, int> typedata, char *rowbuf);
    bool BetweenOperator(int lv, int rv1, int rv2);
    bool BetweenOperator(string lv, string rv1, string rv2);
    bool IsOperator(string lv, char* nonnullbit, int isnot);
    bool isvarc(vector<int> datatype, int ColNum, vector<int>& varcharlist);
    void makedefaultmap(vector<string> ColName, vector<int> startoff, vector<int> offlen, vector<int> datatype, int ColNum, unordered_map<string, int> &startptr, unordered_map<string, int> &lengthRaw, unordered_map<string, int> &typedata);
    void makenewmap(int isvarchar, int ColNum, unordered_map<string, int> &newstartptr, unordered_map<string, int> &newlengthraw, vector<int> datatype, unordered_map<string, int> lengthRaw, vector<string> ColName, int &iter, vector<int> startoff, vector<int> offlen, char *rowbuf);
    void compareGE(string LV, string RV, bool &CV, bool &TmpV, bool &canSaved, bool isnot);
    void compareGE(int LV, int RV, bool &CV, bool &TmpV, bool &canSaved, bool isnot);
    void compareLE(string LV, string RV, bool &CV, bool &TmpV, bool &canSaved, bool isnot);
    void compareLE(int LV, int RV, bool &CV, bool &TmpV, bool &canSaved, bool isnot);
    void compareGT(string LV, string RV, bool &CV, bool &TmpV, bool &canSaved, bool isnot);
    void compareGT(int LV, int RV, bool &CV, bool &TmpV, bool &canSaved, bool isnot);
    void compareLT(string LV, string RV, bool &CV, bool &TmpV, bool &canSaved, bool isnot);
    void compareLT(int LV, int RV, bool &CV, bool &TmpV, bool &canSaved, bool isnot);
    void compareET(string LV, string RV, bool &CV, bool &TmpV, bool &canSaved, bool isnot);
    void compareET(int LV, int RV, bool &CV, bool &TmpV, bool &canSaved, bool isnot);
    void compareNE(string LV, string RV, bool &CV, bool &TmpV, bool &canSaved, bool isnot);
    void compareNE(int LV, int RV, bool &CV, bool &TmpV, bool &canSaved, bool isnot);
    int typeLittle(unordered_map<string, int> typedata, string colname, char *rowbuf);
    void JoinOperator(string colname);
    string ItoDec(int inum);
    string typeBig(string colname, char *rowbuf);
    string typeDecimal(string colname, char *rowbuf);
    void sendfilterresult(Result &filterresult);
    void GetColumnoff(string ColName);

    int row_offset;    
};
