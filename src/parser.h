#pragma once
#include "_header.h"
#include "socket.h"
#include "Channels.h" // Not sure if it belongs there
#include <cstring>


//type use
typedef std::vector<std::string> vec_str;

class Socket;

class Parser {
public:
    Parser(Socket& socket);
    ~Parser();
    vec_str TokenizeMessage(std::string message);

    //TODO: v
    std::string makeMessage(t_code const type, const std::string msg , const userData& user);
    bool        setUserInfo(userData& user);
    bool        joinChanel(const userData& user, const std::string chanelName);
    bool        testPassWord(std::string &pass, userData &user, vectorIT& index);
    //*    //
    void        kickUser(vectorIT& index, const char* reasons, userData &user);
    void        badCmd(userData &user);
    void        allReadyRegistered(userData &user);
    //*    //
    
    // Im calling this in my socket class, everytime we get new Data from Client
    void        ParseData(userData& user, vectorIT& index); 

private:
    Socket&     Sock;
    #define MSG_PONG(user, ip) Sock.SendData(user, std::string("PONG ") + ip);
    #define MSG_ReadyRegistered(user) allReadyRegistered(user);
    //look if the user doing the acorder lever of Action
    #define LV(userLv, askLv) (userLv >= askLv)
    //


};


/*
  vec[0][0]join
  vec[0][1] a 
  vec[1][0]





*/
