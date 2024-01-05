#include <mutex>
#include <queue>
#include <condition_variable>

using namespace std;

template <typename T>
class WorkQueue
{
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

  int length(){
	  int ret;
	  unique_lock<mutex> lock(work_mutex);

    ret = work.size();

    lock.unlock();
	  return ret;
  }
};