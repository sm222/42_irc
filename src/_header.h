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
#include <utility>          // utility 
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

enum e_ActionLv {
  e_notRegistred,
  e_setNickname,
  e_setUser,
  e_userRegistred
};

// +++ Defines +++

#define vectorIT    std::vector<pollfd>::iterator
#define ServerName  "42irc"

// base //
#define RPL_WELCOME(user, nick)                 "001 " + nick + " :Welcome to the 42irc " + user   //<client> :Welcome to the <networkname> Network, <nick>[!<user>@<host>]
#define RPL_YOURHOST(nick)                      "002 " + nick + " :Your host is " + ServerName + ", running version 1.0" //"Your host is <servername>, running version <ver>"
#define RPL_CREATED(nick)                       "003 " + nick + " :This server was created - " //"This server was created <date>"
#define RPL_MYINFO(nick)                        "004 " + nick + " :ircserv 1.0 +itkol #&" //"<servername> <version> <available user modes> <available channel modes>"
// info //
#define ERR_UNKNOWNERROR(cmd, reason)           "400 " + cmd + " :" + reason
#define ERR_UNKNOWNCOMMAND(cmd)                 "421 " + cmd + " :Unknown command"
#define ERR_NEEDMOREPARAMS(nick, cmd)           "461 " + nick + " " + cmd + " :Not enough parameters"
#define ERR_PASSWDMISMATCH(nick)                "464 " + nick + " :Password incorrect"
#define ERR_NOTREGISTERED                       "451 :You have not registered"
#define ERR_ALREADYREGISTRED                    "462 :Unauthorized command (already registered)"

//user
#define ERR_USERNOTINCHANNEL(nick, channel)     "441 " + nick + " " + channel + " :They aren't on that channel"
#define ERR_USERONCHANNEL(user, channel)        "443 " + user + " " + channel + " :is already on channel"
#define ERR_NOLOGIN(user)                       "444 " + user + " :User not logged in"

//nickName
#define ERR_NONICKNAMEGIVEN                     "431 :No nickname given"
#define ERR_ERRONEUSNICKNAME(nick)              "432 " + nick +  " :Erroneous nickname"
#define ERR_NICKNAMEINUSE(nick)                 "433 " + nick + " :Nickname is already in use"
#define ERR_WASNOSUCHNICK(nick)                 "406 " + nick + " :There was no such nickname"

//kick
#define ERR_NOSUCHNICK(nick)                    "401 " + nick + " :No such nick/channel"
#define ERR_NOTONCHANNEL(channel)               "442 " + channel + " :You're not on that channel"

//join
#define RPL_JOIN(nick, channel)                 ":" + nick + " JOIN " + channel
#define ERR_CHANNELISFULL(channel)              "471 " + channel + " :Cannot join channel (+l)"
#define ERR_INVITEONLYCHAN(channel)             "473 " + channel + " :Cannot join channel (+i)"
#define ERR_BADCHANNELKEY(channel)              "475 " + channel + " :Cannot join channel (+k)"

//channel
#define RPL_INVITING(nick, channel)             "341 " + channel + " " + nick
#define RPL_ENDOFNAMES(nick, channel)           "366 " + nick + " " + channel + " :End of NAMES list"
#define ERR_NOSUCHCHANNEL(channel)              "403 " + channel + " :No such channel"
#define ERR_CANNOTSENDTOCHAN(channel)           "404 " + channel + " :Cannot send to channel"
#define ERR_BADCHANNAME                         "479 :bad channel name/invalid charater"
#define ERR_CHANOPRIVSNEEDED(channel)           "482 " + channel + " :You're not channel operator"

//
#define RPL_AWAY(nick, msg)                     "301 " + nick + " :" + msg
#define RPL_PRIVMSG(nick, target, msg)          ":" + nick + " PRIVMSG " + target + " " + msg
#define ERR_NORECIPIENT(cmd)                    "411 :No recipient given (" + cmd + ")"
#define ERR_NOTEXTTOSEND                        "412 :No text to send"

//topic
#define RPL_TOPIC(user, channel, topic)         "332 " + user + " " + channel + " " + topic
#define ERR_TOOMANYTARGETS(target, mag)         target + ":407 recipients. " + msg
#define ERR_NOTOPLEVEL(mask)                    "413 " + mask + " :No toplevel domain specified"

//mode
#define ERR_UMODEUNKNOWNFLAG(nick)              "501 " + nick + " :Unknown MODE flag"
#define ERR_UNKNOWNMODE(char, channel)          "472 " + char + " :is unknown mode char to me for " + channel

// +++ Structs +++

// This is what we need to parse
struct userData {
    int         userFD;
    int         currentAction;  // <--- i added this so we can keep track of what we're currently waiting from the user
    std::string recvString;     // <--- Received Data
    std::string nickName;
    std::string userName;
    std::string IP;
};

