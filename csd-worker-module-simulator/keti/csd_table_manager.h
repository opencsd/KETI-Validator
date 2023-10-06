 #include <unordered_map>
#include <iostream>

using namespace std;

struct TableRep{
    string dev_name;
    bool blocks_maybe_compressed;
    bool blocks_definitely_zstd_compressed;
    bool immortal_table;
    uint32_t read_amp_bytes_per_bit;
    string compression_name;
};

class CSDTableManager{
  public:
    static void InitCSDTableManager(){
      GetInstance().initCSDTableManager();
    }

    static TableRep GetTableRep(string table_name){
      return GetInstance().getTableRep(table_name);
    }

  private:
    CSDTableManager(){};

    CSDTableManager(const CSDTableManager&);
    ~CSDTableManager() {};
    CSDTableManager& operator=(const CSDTableManager&){
        return *this;
    };

    static CSDTableManager& GetInstance() {
        static CSDTableManager csdTableManager;
        return csdTableManager;
    }

    void initCSDTableManager();
    TableRep getTableRep(string table_name);

    inline const static std::string LOGTAG = "CSD Table Manager";
    char msg[200];

  private:
    unordered_map<string,struct TableRep> table_rep_;// key=table_name
};