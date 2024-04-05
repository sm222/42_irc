#pragma once
#include "_header.h"
#include "socket.h"
#include "Channels.h" // Not sure if it belongs there
#include <cstring>
#include <iostream>
#include <algorithm>
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

    //TODO: v
    std::string makeMessage(t_code const type, const string msg , const userData& user);
    bool        setUserInfo(userData& user, vec_str vec);
    bool        joinChannel(const userData& user, const string& name, const string& pass);
    bool        testPassWord(std::string &pass, userData &user, vectorIT& index);
    bool        setTopic(const userData&  user, const string& chanelName, const string& topic);
    string      getTopic(const string& chanalName);
    bool        setUserMode(userData& user, int type);
    bool        privMsg(const string chanel, const string message, const string nick, bool self = false);
    bool        KickUserChannel(const userData &user, const string channel, const string nick, const string reson);
    bool        KickUserAllChannel(const userData& user, const string reson);
    bool        userPart(const string channel , const string userName, const string reson);
    bool        ModeI(const userData&  user, const string mode, const string targerNick, const string channel);
    //*    //
    void        kickUser(vectorIT& index, const string reasons, const userData &user);
    void        badCmd(userData &user);
    void        notInChannel(const userData& user, const string channel);
    void        unknowCommand(userData &user);
    void        allReadyRegistered(userData &user);
    void        noSuchNick(const userData& user, const string nick);
    //*    //

    void        fnJOIN(vec_str& vec, userData& user);
    void        fnPMSG(vec_str& vec, userData& user);
    void        fnKICK(vec_str& vec, userData& user);
    void        fnPART(vec_str& vec, userData& user);
    void        fnQUIT(vec_str& vec, userData& user);
    void        fnNICK(vec_str& vec, userData& user, vectorIT& index);
    void        fnUSER(vec_str& vec, userData& user, vectorIT& index);
    void        fnPASS(vec_str& vec, userData& user, vectorIT& index);
    
    // Im calling this in my socket class, everytime we get new Data from Client
    void        ParseData(userData& user, vectorIT& index);

private:
    Socket&     Sock;
    vectorIT*   _index;
    Channels&   _channels;
    //*ft v
    string      _SendUserChannelStatus(const vec_str& userList, const string& name);
    short       _tryJoinChannel(const userData& user, const string name, const string pass);
    bool        _testOp(const userData& user, const string channelName);
    bool        _testInChannel(const userData& user, const string channelName);
    //          look if the user doing the acorder lever of Action
    #define     LV(userLv, askLv) (userLv >= askLv)
    //


};


/*
  vec[0][0]join
  vec[0][1] a 
  vec[1][0]





*/
