#pragma once
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

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h" 

#include "storage_engine_instance.grpc.pb.h"
#include "TableManager.h"
using StorageEngineInstance::MetaDataResponse;
using StorageEngineInstance::Snippet;

#include "ip_config.h"

//#define BUFF_SIZE 4096

using namespace std;
using namespace rapidjson;

using StorageEngineInstance::Snippet;
using StorageEngineInstance::MetaDataResponse_PBAInfo;

struct vectortype{
  string strvec;
  int64_t intvec;
  double floatvec;
  int type; //0 string, 1 int, 2 float
};

struct filtervalue {
    vector<int> type;
    vector<string> value;
};

typedef filtervalue lv;
typedef filtervalue rv;


struct filterstruct{
    lv LV;
    int filteroper;
    rv RV; 
};


struct Projection{
    string value;
    int type; // 0 int, 1 float, 2 string, 3 column
};

template <typename T>
class WorkQueue{
  condition_variable work_available;
  mutex work_mutex;
  queue<T> work;

public:
  void push_work(T item){
    unique_lock<mutex> lock(work_mutex);

    bool was_empty = work.empty();
    work.push(item);

    lock.unlock();

    if (was_empty){
      work_available.notify_one();
    }    
  }

  T wait_and_pop(){
    unique_lock<mutex> lock(work_mutex);
    while (work.empty()){
      work_available.wait(lock);
    }

    T tmp = work.front();
	
    work.pop();
    return tmp;
  }

  bool is_empty(){
    return work.empty();
  }

  void qclear(){
    work = queue<T>();
  }

  int get_size(){
    return work.size();
  }
};

struct Object{
  StorageEngineInstance::Snippet snippet;
  TableManager::Table targetTable;
  //StorageEngineInstance::MetaDataResponse metadata;
};

class Scheduler{
  public: 
    static void PushQueue(Object scheduling_target){
      GetInstance().SnippetQueue.push_work(scheduling_target);
    }

    static Object PopQueue(){
      Object scheduling_target = GetInstance().SnippetQueue.wait_and_pop();
      return scheduling_target;
    }

    static MetaDataResponse GetSchedulingTarget(int qid, int wid){
      MetaDataResponse schedulerInfo = GetInstance().getSchedulingTarget(qid, wid);
      return schedulerInfo;
    }

  private:
    Scheduler() {
      std::thread Running_Scheduler_Thread(&Scheduler::runScheduler,this);
      Running_Scheduler_Thread.detach();
    };
    Scheduler(const Scheduler&);
    ~Scheduler() {};
    Scheduler& operator=(const Scheduler&){
        return *this;
    }

    static Scheduler& GetInstance(){
        static Scheduler scheduler;
        return scheduler;
    }

    void runScheduler();
    void scheduling(Object &scheduling_target, string sst_name, string bestCSD); //sst파일 단위 스케줄링
    string bestCSD(Object &scheduling_target, string sst_name, MetaDataResponse_PBAInfo &pba_info); //최적 CSD 선정
    string DPG(Object &scheduling_target, string sst_name, MetaDataResponse_PBAInfo pba_info); //CSD 병렬 처리 우선
    string RR(Object &scheduling_target, string sst_name, MetaDataResponse_PBAInfo pba_info); //CSD 순서대로
    void serialize(StringBuffer &buff, Snippet &snippet, string bestcsd); // snippet -> json 구성
    void sendSnippetToCSD(string snippet_json, string csdIP); // CSD 전달
    MetaDataResponse getSchedulingTarget(int qid, int wid);
  
  public:
    inline const static std::string LOGTAG = "Offloading Container::Snippet Scheduler";

  private:
    thread Running_Scheduler_Thread;
    WorkQueue<Object> SnippetQueue;
};
