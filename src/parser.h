#pragma once
#include "_header.h"
#include "socket.h"
#include "Channels.h" // Not sure if it belongs there
#include <cstring>

class Socket;

class Parser {
public:
    Parser(Socket& socket);
    ~Parser();
    std::string makeMessage(t_code const type, const std::string msg , const userData& user);
    // Im calling this in my socket class, everytime we get new Data from Client
    void        ParseData(userData& user, vectorIT& index); 

private:
    Socket&     Sock;
};