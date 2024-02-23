#pragma once
#include "_header.h"
#include "socket.h"

class Socket;

class Parser {
public:
    Parser(Socket& socket);
    ~Parser();

    // Im calling this in my socket class, everytime we get new Data from Client
    void        ParseData(userData& user, vectorIT& index); 

private:
    Socket&     Sock;
};