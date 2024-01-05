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

typedef enum KETI_SELECT_TYPE{
      COL_NAME = 0, //DEFAULT
      SUM = 1,
      AVG = 2,
      COUNT = 3,
      COUNTSTAR = 4,
      TOP = 5,
      MIN = 6,
      MAX = 7,
}KETI_SELECT_TYPE;

typedef enum KETI_VALUE_TYPE{
    INT8 = 0,
    INT16 = 1,
    INT32 = 2,
    INT64 = 3,
    FLOAT32 = 4,
    DOUBLE = 5,
    NUMERIC = 6,
    DATE = 7,
    TIMESTAMP = 8,
    STRING = 9,
    COLUMN = 10,
    OPERATOR = 11,
}KETI_VALUE_TYPE;

typedef enum STACK_TYPE{
    INT_ = 50,
    Long_ = 51,
    FLOAT_ = 52,
    DOUBLE_ = 53,
    STRING_ = 54,
    DECIMAL_ = 55,
}STACK_TYPE;

typedef enum opertype
{
    GE = 1,      // >=
    LE,      // <=
    GT,      // >
    LT,      // <
    ET,      // ==
    NE,      // !=
    LIKE,    // RV로 스트링
    BETWEEN, // RV로 배열형식 [10,20] OR [COL1,20] --> extra
    IN,      // RV로 배열형식 [10,20,30,40] + 크기 --> extra
    IS,      // IS 와 IS NOT을 구분 RV는 무조건 NULL
    ISNOT,   // IS와 구분 필요 RV는 무조건 NULL
    NOT,     // ISNOT과 관련 없음 OPERATOR 앞에 붙는 형식 --> 혼자 들어오는 oper
    AND,     // AND --> 혼자 들어오는 oper
    OR,      // OR --> 혼자 들어오는 oper
    ALL,
    SUBSTRING = 16,
}opertype;

/*     
                            Scan Type
  +--------------------+-----------------------------------------+
  | Full_Scan_Filter   | full table scan / only scan (no filter) |
  | Full_Scan          | full table scan / scan and filter       |
  | Index_Scan_Filter  | index pk scan   / only scan (no filter) |
  | Index_Scan         | index pk scan   / scan and filter       |
  +--------------------+-----------------------------------------+
*/

enum Scan_Type{
  Full_Scan_Filter,
  Full_Scan,
  Index_Scan_Filter,
  Index_Scan
};