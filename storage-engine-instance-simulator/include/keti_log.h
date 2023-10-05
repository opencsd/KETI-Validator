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

struct log_data {
    char msg[MSGMAAX];
};

struct message {
    long msg_type;
    log_data data;
};

inline void KETILOG(std::string id, std::string msg){
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

    std::cout << "[" << id << "] " << msg << std::endl;
}

#endif // LOG_MSG_H_INCLUDED