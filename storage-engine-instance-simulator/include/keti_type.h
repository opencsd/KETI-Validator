#ifndef _KETI_TYPE_H_
#define _KETI_TYPE_H_

typedef enum opertype
{
    KETI_DEFAULT = 0,
    KETI_GE,      // >=
    KETI_LE,      // <=
    KETI_GT,      // >
    KETI_LT,      // <
    KETI_ET,      // ==
    KETI_NE,      // !=, <>
    KETI_LIKE,    // RV로 스트링
    KETI_BETWEEN, // RV로 배열형식 [10,20] OR [COL1,20] --> extra
    KETI_IN,      // RV로 배열형식 [10,20,30,40] + 크기 --> extra
    KETI_IS,      // IS 와 IS NOT을 구분 RV는 무조건 NULL
    KETI_ISNOT,   // IS와 구분 필요 RV는 무조건 NULL
    KETI_NOT,     // ISNOT과 관련 없음 OPERATOR 앞에 붙는 형식 --> 혼자 들어오는 oper
    KETI_AND,     // AND --> 혼자 들어오는 oper
    KETI_OR,      // OR --> 혼자 들어오는 oper
    KETI_JOIN,    // 타입 나눠야함 left, right, inner, outer
    KETI_SUBSTRING,
    KETI_SET_UNION = 100,
    KETI_SET_UNIONALL = 101,
    KETI_SET_INTERSECT = 102, //MySQL 미지원
    KETI_SET_MINUS = 103, //MySQL 미지원
    KETI_SET_PLUS = 104  //MySQL 미지원
}KETI_OPER_TYPE;

typedef enum QueryEngineType{
    MySQL = 1,
    Oracle = 2
}KETI_QueryEngine_Type;

typedef enum Oracle_DataType{
    Oracle_NUMBER = 1,
    Oracle_DATE,
    Oracle_TIMESTAMP,
    Oracle_CHAR,
    Oracle_FLOAT,
}Oracle_DataType;

typedef enum MySQL_DataType{
    MySQL_BYTE = 1,
    MySQL_INT16 = 2,
    MySQL_INT32 = 3,
    MySQL_INT64 = 8,
    MySQL_FLOAT32 = 4,
    MySQL_DOUBLE = 5,
    MySQL_NEWDECIMAL = 246,
    MySQL_DATE = 14,
    MySQL_TIMESTAMP = 7,
    MySQL_STRING = 254,
    MySQL_VARSTRING = 15,
}MySQL_DataType;

typedef enum KETI_Type{
    KETI_INT8 = 0,
    KETI_INT16,
    KETI_INT32,
    KETI_INT64,
    KETI_FLOAT32,
    KETI_FLOAT64,
    KETI_NUMERIC,
    KETI_DATE,
    KETI_TIMESTAMP,
    KETI_STRING,
    KETI_COLUMN,
    KETI_PLUS = 100,
    KETI_MINUS,
    KETI_MULTIPLE,
    KETI_DIVIDE,
}KETI_Type;

typedef enum Work_Status_Type{
    NonInitQuery,
    NonInitTable,
    NotFinished,
    WorkDone,
}Work_Status_Type;

typedef enum vector_type{ //저장된 컬럼의 타입을 나타냄(BM,MQM에서 사용) -> 더 세세하게 나눠야함
    TYPE_EMPTY = 0,
    TYPE_STRING,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_BOOLEAN
}KETI_VECTOR_TYPE;

typedef enum scheduling_type{ //스케줄링을 수행하는 알고리즘 선택
    ROUND_ROBBIN = 0,
    ALGORITHM_AUTO_SELECT = 1,
    FILE_DISTRIBUTION = 2,
    CSD_METRIC_SCORE = 3
}KETI_SCHEDULING_TYPE;

#endif
