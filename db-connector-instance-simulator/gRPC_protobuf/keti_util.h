#ifndef KETI_UTIL_H_INCLUDED
#define KETI_UTIL_H_INCLUDED

#pragma once

#include <iostream>
#include "log_msg.h"

inline void keti_log(std::string id, std::string msg){
//    key_t key = MSQID;
//    int msqid;
//    std::string log_str;
//retry:
//    message mess;    
//    mess.msg_type=1;
//    log_str = "[" + id + "] " + msg;
//    memcpy(mess.data.msg,log_str.c_str(),MSGMAAX);

//    if((msqid=msgget(key,IPC_CREAT|0666))==-1){
//        printf("msgget failed %s\n",strerror(errno));
//        goto retry;
//    }
    
//    if(msgsnd(msqid,&mess,sizeof(log_data),0)==-1){
//        printf("msgsnd failed %s\n",strerror(errno));
//        goto retry;
//    }

    std::cout << "[" << id << "] " << msg << std::endl;
}

#endif // KETI_UTIL_H_INCLUDED
