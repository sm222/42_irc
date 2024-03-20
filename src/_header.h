#pragma once

// Socket
#include <sys/socket.h>     // socket, bind, listen, connect
#include <netdb.h>          // getaddrinfo, freeaddrinfo, struct addrinfo
#include <fcntl.h>          // setNonBlocking
#include <poll.h>           // Poll

// Basic Stuff
#include <iostream>         // cout
#include <unistd.h>         // close
#include <string.h>         // memset
#include <vector>           // vector
#include <map>              // map 

// Others
#include "exception.h"

// +++ Defines +++

#define vectorIT    std::vector<pollfd>::iterator


#define MSG_Welcome         (client, user) "001 " + client + " :Welcome to the 42irc " + user             //<client> :Welcome to the <networkname> Network, <nick>[!<user>@<host>]
#define MSG_RplTopic        (client, channel, topic) "332 " + client + " " + channel + " " + topic        //<client> <channel> :<topic>
#define MSG_PassMisMatch    "464 Server :password incorect"                                               //<client> :Password incorrect

typedef enum e_type {
  e_welcom,     // 001
  e_rplTopic,    // 332
  e_passmismatch //464
} t_code;

const char* const MType[] = {
  "001",
  "332",
  "464",
  ""
};

// +++ Structs +++

// This is what we need to parse
struct userData {
    int         userFD;
    int         currentAction;  // <--- i added this so we can keep track of what we're currently waiting from the user
    std::string recvString;     // <--- Received Data
    std::string nickName;
    std::string userName;
};
