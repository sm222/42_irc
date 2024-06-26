#pragma once
#include "_header.h"
#include "socket.h"
#include "Channels.h" // Not sure if it belongs there
#include <cstring>
#include <iostream>
#include <algorithm>
#include <string>
// #include <cctype>



using std::string;
//type use
typedef std::vector<std::string> vec_str;

class Socket;

class Parser {
public:
    Parser(Socket& socket);
    ~Parser();
    vec_str Tokenize(std::string message, char c);
    //*                                                                                                           /
    string      getTopic(const string& chanalName);
    bool        KickUserAllChannel(const userData& user, const string reson);
    bool        testPassWord(std::string &pass, userData &user, vectorIT& index);
    bool        userInvite(userData& user, std::string nick, std::string channel);
    bool        userPart(const string channel , const string userName, const string reson);
    bool        joinChannel(const userData& user, const string& channel, const string& pass);
    bool        setTopic(const userData&  user, const string& chanelName, const string& topic);
    bool        privMsg(const string chanel, const string message, const string nick, bool self = false);
    bool        KickUserChannel(const userData &user, const string channel, const string nick, const string reson);
    // !mode                                                                                                      /
    bool        Mode (const userData& user, const string channel);
    bool        ModeI(const userData& user, const string channel, const bool mode);
    bool        ModeT(const userData& user, const string channel, const bool mode);
    bool        ModeL(const userData& user, const string channel, const int number);
    bool        ModeK(const userData& user, const string channel, const string pass);
    bool        ModeO(const userData& user, const string channel, const string nick, const bool mode);
    //*                                                                                                           /
    void        allReadyRegistered(userData &user);
    void        kickUser(vectorIT& index, const string reasons, const userData &user);
    void        notInChannel(const userData& user, const string channel, const userData* ask = NULL);
    //*                                                                                                           /
    bool        chaIsValid(std::string str);
    bool        keyIsValid(std::string str);
    void        fnJOIN(vec_str& vec, userData& user);
    void        fnPMSG(vec_str& vec, userData& user);
    void        fnKICK(vec_str& vec, userData& user);
    void        fnPART(vec_str& vec, userData& user);
    void        fnQUIT(vec_str& vec, userData& user);
    void        fnMODE(vec_str& vec, userData& user);
    void        fnNICK(vec_str& vec, userData& user);
    void        fnUSER(vec_str& vec, userData& user);
    void        fnPASS(vec_str& vec, userData& user);
    void        fnTOPIC(vec_str& vec, userData& user);
    void        fnINVITE(vec_str& vec, userData& user);
    void        sendAllChannel(userData& user, std::string message);
    //*                                                                                                           /
    // Im calling this in my socket class, everytime we get new Data from Client                                  /
    void        ParseData(userData& user, vectorIT& index);
private:
    Socket&     Sock;
    vectorIT*   _index;
    Channels&   _channels;
    //* private v                                                                                                 /
    string      _SendUserChannelStatus(const vec_str& userList, const string& name);
    short       _tryJoinChannel(const userData& user, const string channel, const string pass);
    bool        _testOp(const userData& user, const string channelName);
    bool        _testInChannel(const userData& user, const string channelName, const userData* ask = NULL);
    bool        _sendTopicTo(const string channel, const userData* user = NULL);
    void        _sendChannel(const string message, const string channel, const bool user = false);
    //          look if the user doing the acorder lever of Action                                                /
    #define     LV(userLv, askLv) (userLv >= askLv)
};

