#pragma once

#include "_header.h"
#include "parser.h"

#define BUFFER_SIZE 8196

//class Parser;

class Socket {
public:
    Socket(const std::string ip, uint16_t port, const std::string password, const bool showDebug = false);
    ~Socket();
 
    // Socket 
    void                        Bind();
    void                        Start();
    void                        Listen();
    void                        SetNonBlocking();
    void                        SetAddrReusable();
    void                        Connect(const std::string& ip, const uint16_t& port);

    // Getter for "friend" class

    // broadcast
    void                        BroadcastToAll(const std::string& data);
    userData*                   GetUserByNickname(const std::string& nickName);
    userData*                   GetUserByUsername(const std::string& userName);
    void                        SendData(const int& userFD, std::string data);
    void                        KickUser(vectorIT& index);
    const std::string&          GetPassword();

private:
    // +++ Sockets & Errors +++
    void                        _cleanup();
    int                         _getSocket(addrinfo* AddrInfo);
    size_t                      _updatePolls(const bool needUpdate = false);
    addrinfo*                   _getInfo(const std::string ip, const uint16_t port);
    
    // +++ Users Methods +++
    void                        _acceptConnection();
    void                        _newUser(const int& fd);
    void                        _recvData(vectorIT& index);

    // +++ Server Data +++
    const std::string           _ip;
    const uint16_t              _port;
    const std::string           _password;
    const bool                  _showDebug;

    // +++ Polls & UserData +++
    std::vector<pollfd>         _polls;
    std::map<int, userData>     _users;

    // +++ Free & Close +++
    int                         _fd;
    addrinfo*                   _addrInfo;

};