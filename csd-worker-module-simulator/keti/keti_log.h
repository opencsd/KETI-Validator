#ifndef LOG_MSG_H_INCLUDED
#define LOG_MSG_H_INCLUDED

#pragma once

#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>

#define MSQID 12345
#define MSGMAAX 4096

using namespace std;

struct log_data {
    char msg[MSGMAAX];
};

struct message {
    long msg_type;
    log_data data;
};

typedef enum DEBUGG_LEVEL {
    TRACE = 0,
    DEBUG = 1,
    METRIC = 2,
    INFO = 3,
    WARN = 4,
    ERROR = 5,
    FATAL = 6
}KETI_DEBUGG_LEVEL;

class KETILOG {
    public:
        static void SetDefaultLogLevel(){
            GetInstance().LOG_LEVEL = DEBUG;
        }
        
        static void SetLogLevel(int level){
            GetInstance().LOG_LEVEL = level;
        }

        static int GetLogLevel(){
            return GetInstance().LOG_LEVEL;
        }

        static void TRACELOG(std::string id, const char msg[]){
        //     key_t key = MSQID;
        //     int msqid;
        //     std::string log_str;
        // retry:
        //     message mess;    
        //     mess.msg_type=1;
        //     log_str = "[" + id + "] " + msg;
        //     memcpy(mess.data.msg,log_str.c_str(),MSGMAAX);

        //     if((msqid=msgget(key,IPC_CREAT|0666))==-1){
        //         printf("msgget failed %s\n",strerror(errno));
        //         goto retry;
        //     }
            
        //     if(msgsnd(msqid,&mess,sizeof(log_data),0)==-1){
        //         printf("msgsnd failed %s\n",strerror(errno));
        //         goto retry;
        //     }
            if(GetInstance().LOG_LEVEL <= TRACE){
                printf("[%s] %s", id.c_str(), msg);
            }
        }

        static void DEBUGLOG(std::string id, const char msg[]){
            if(GetInstance().LOG_LEVEL <= DEBUG){
                printf("[%s] %s", id.c_str(), msg);
            }
        }

        static void DEBUGLOG(std::string id, string msg){
            if(GetInstance().LOG_LEVEL <= INFO){
                printf("[%s] %s", id.c_str(), msg.c_str());
            }
        }

        static void INFOLOG(std::string id, const char msg[]){
            if(GetInstance().LOG_LEVEL <= INFO){
                printf("[%s] %s", id.c_str(), msg);
            }
        }

        static void INFOLOG(std::string id, string msg){
            if(GetInstance().LOG_LEVEL <= INFO){
                printf("[%s] %s", id.c_str(), msg.c_str());
            }
        }

        static void WARNLOG(std::string id, const char msg[]){
            if(GetInstance().LOG_LEVEL <= WARN){
                printf("[%s] %s", id.c_str(), msg);
            }
        }

        static void WARNLOG(std::string id, string msg){
            if(GetInstance().LOG_LEVEL <= ERROR){
                printf("[%s] %s", id.c_str(), msg.c_str());
            }
        }

        static void ERRORLOG(std::string id, const char msg[]){
            if(GetInstance().LOG_LEVEL <= ERROR){
                printf("[%s] %s", id.c_str(), msg);
            }
        }

        static void ERRORLOG(std::string id, string msg){
            if(GetInstance().LOG_LEVEL <= FATAL){
                printf("[%s] %s", id.c_str(), msg.c_str());
            }
        }

        static void FATALLOG(std::string id, const char msg[]){
            if(GetInstance().LOG_LEVEL <= FATAL){
                printf("[%s] %s", id.c_str(), msg);
            }
        }

        static void FATALLOG(std::string id, string msg){
            if(GetInstance().LOG_LEVEL <= FATAL){
                printf("[%s] %s", id.c_str(), msg.c_str());
            }
        }

        static bool IsLogLevelUnder(int level){
            bool flag = (GetInstance().LOG_LEVEL <= level);
            return flag;
        }

    private:
        KETILOG(){};
        KETILOG(const KETILOG&);
        KETILOG& operator=(const KETILOG&){
            return *this;
        }

        static KETILOG& GetInstance(){
            static KETILOG _instance;
            return _instance;
        }

        int LOG_LEVEL;
};

#endif // LOG_MSG_H_INCLUDED