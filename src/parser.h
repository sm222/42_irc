#pragma once
#include "_header.h"
#include "socket.h"
#include "Channels.h" // Not sure if it belongs there
#include <cstring>


using std::string;
//type use
typedef std::vector<std::string> vec_str;

class Socket;

class Parser {
public:
    Parser(Socket& socket);
    ~Parser();
    vec_str TokenizeMessage(string message);

    //TODO: v
    std::string makeMessage(t_code const type, const string msg , const userData& user);
    bool        setUserInfo(userData& user, vec_str vec);
    bool        joinChanel(const userData& user, const std::vector<string>& vec);
    bool        testPassWord(std::string &pass, userData &user, vectorIT& index);
    //*    //
    void        kickUser(vectorIT& index, const char* reasons, const userData &user);
    void        badCmd(userData &user);
    void        unknowCommand(userData &user);
    void        allReadyRegistered(userData &user);
    //*    //
    
    // Im calling this in my socket class, everytime we get new Data from Client
    void        ParseData(userData& user, vectorIT& index);

private:
    Socket&     Sock;
    short       _tryJoinChanel(const userData& user, const string name, const string pass);
    #define     MSG_PONG(user, ip) Sock.SendData(user, string("PONG ") + ip);
    #define     MSG_ReadyRegistered(user) allReadyRegistered(user);
    //          look if the user doing the acorder lever of Action
    #define     LV(userLv, askLv) (userLv >= askLv)
    //


};


/*
  vec[0][0]join
  vec[0][1] a 
  vec[1][0]





*/
