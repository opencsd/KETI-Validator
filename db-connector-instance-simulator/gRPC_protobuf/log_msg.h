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


#endif // LOG_MSG_H_INCLUDED