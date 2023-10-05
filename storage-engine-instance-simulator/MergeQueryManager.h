#pragma once

#include "BufferManager.h"

using namespace std;

struct T{
    string varString;
    int64_t varInt;
    double varFloat;
    bool isnull;
    bool boolean;
    int type;//0 string, 1 int, 2 float, 3 boolean (KETI_VECTOR_TYPE)
    
    T(){
        varString = "";
        varInt = 0;
        varFloat = 0;
        isnull = false;
        boolean = false;
        type = -1;
    }
};

class MergeQueryManager{	
public:
    MergeQueryManager(StorageEngineInstance::SnippetRequest snippet_){
        this->snippet = snippet_.snippet();
        this->snippetType = snippet_.type();
        this->group_by_table.clear();
        this->group_by_key.clear();
        this->ordered_index.clear();
    };

    void RunSnippetWork();//스니펫 작업 수행

    string makeGroupbyKey(int row_index){
        string key = "";
        for(int g=0; g<snippet.group_by_size(); g++){
            string groupby_col_name = snippet.group_by(g);
            int groupby_col_type = target_table.table_data[groupby_col_name].type;
            string k;
            switch(groupby_col_type){
            case TYPE_STRING:
                key += "|" + target_table.table_data[groupby_col_name].strvec[row_index];
                break;
            case TYPE_INT:
                key += "|" + to_string(target_table.table_data[groupby_col_name].intvec[row_index]);
                break;
            case TYPE_FLOAT:
                key += "|" + to_string(target_table.table_data[groupby_col_name].floatvec[row_index]);
                break;
            }
        }
        return key;
    }

    inline const static std::string LOGTAG = "Merging Container::Merge Query Manager";

private:
    StorageEngineInstance::Snippet snippet;//스니펫
    int snippetType;//스니펫 작업 타입
    int tableCnt;//작업 대상 테이블 개수
    bool isGroupby;//그룹바이 여부
    bool isOrderby;//오더바이 여부
    bool isLimit;//리미트 여부
    bool smallFlag;//true = derived is smaller, false = driving is smaller
    TableData driving_table;//table1(좌항)
    TableData derived_table;//table2(우항)
    unordered_map<string,vector<int>> hash_table;//key:index 해시 조인 시(driving, derived 로우가 적은 쪽)
    TableData target_table;//프로젝션 대상 테이블(driving_table X derived_table)
    map<string,int> group_by_key;//<그룹바이기준컬럼,그룹바이 테이블 인덱스>
    vector<TableData> group_by_table;//그룹바이된 테이블 컬럼(group by)
    TableData result_table;//결과 테이블(projected)->버퍼저장
    vector<int> ordered_index;//result_table의 정렬 후 인덱스 순서
    TableData order_by_table;//결과 테이블(ordered)->버퍼저장

private:
    void Aggregation();//column projection, 모든 스니펫이 기본적으로 수행
    T Projection(int groupIndex, int projectionIndex, int rowIndex);//프로젝션 수행,Aggregation 호출
    T Postfix(int groupIndex, int projectionIndex, int rowIndex, int start, int end);//실제 postfix 계산
    void GroupBy();//group by, 그룹바이 절이 있으면 수행
    void OrderBy();//order by, 오더바이 절이 있으면 수행
    void createHashTable();//create hash table for hash join
    void InnerJoin_nestedloop();//inner join (nested loop join)
    void InnerJoin_hash();//inner join (hash join)
    void LeftOuterJoin_nestedloop();//left outer join (nested loop join)
    void LeftOuterJoin_hash();//left outer join (hash join)
    void RightOuterJoin();//right outer join
    void FullOuterJoin();//full outer join
    void DependencySemiJoin();//dependency exist/in
    void DependencyAntiJoin();//dependency not exist/not in
    void NonDependencySemiJoin();//non dependency exist/in
    void NonDependencyAntiJoin();//non dependency not exist/not in
    void DependencySubquery();//dependency not exist/not in
    void NonDependencySubquery();//non dependency exist/in
    void Filtering();//single table filtering (having)
    void Union();//single table filtering (having)
    void UnionAll();//single table filtering (having)
    void CrossJoin();//cartesian product
};
