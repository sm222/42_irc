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

typedef enum e_type {
  e_welcom,     // 001
  e_rplTopic    // 332
} t_code;

const char* const MType[] = {
  "001",
  "332",
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
