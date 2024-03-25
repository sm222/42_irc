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

/// <<<<<<<<<<<<<<<<<<<<<<<         DEV
# ifndef COLORS
# define RED	"\x1B[31m"
# define GRN	"\x1B[32m"
# define YEL	"\x1B[33m"
# define BLU	"\x1B[34m"
# define MAG	"\x1B[35m"
# define CYN	"\x1B[36m"
# define WHT	"\x1B[37m"
# define ORG	"\x1b[38;5;202m"
# define PIK	"\x1b[38;5;176m"
# define TOX	"\x1b[38;5;190m"
# define RESET	"\x1B[0m"
# define CLE	"\e[1;1H\e[2J"
# endif

// +++ enum +++

enum e_ActionLv {
  e_notConfim,
  e_notNameSet,
  e_ConfimUser
};

// +++ Defines +++

#define vectorIT    std::vector<pollfd>::iterator
#define ServerName  "42irc"

// base //
#define MSG_Welcome         (user) "001 " ServerName " :Welcome to the 42irc " + user             //<client> :Welcome to the <networkname> Network, <nick>[!<user>@<host>]

// info //
#define MSG_RplTopic        (channel, topic) "332 " ServerName " " + channel + " " + topic        //<client> <channel> :<topic>

// error //
#define MSG_ERR_UNKNOWNERROR     "400 " ServerName " :SASL authentication failed"
#define MSG_PassMisMatch         "464 " ServerName " :password incorect"                                                //<client> :Password incorrect
#define MSG_ErrSaslFail          "904 " ServerName " :SASL authentication failed"


typedef enum e_type {
  e_none = -1,   // -1
  e_welcom,      // 001
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
