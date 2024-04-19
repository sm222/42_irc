#pragma once

#include "_header.h"
#include "Channels.h"
#include "parser.h"

#define BUFFER_SIZE 8196

//class Parser;

class Socket {
public:
    Socket(uint16_t port, const std::string password, const bool showDebug = false);
    ~Socket();
 
    // Channels
    Channels                                channels;

    void                                    SendData(const int& userFD, std::string data);
    void                                    SendData(const std::string& userName, std::string data);

    bool                                    doesThisNicknameExist(const std::string& nickname);
    bool                                    doesThisUsernameExist(const std::string& username);

    const std::string&                      GetPassword();
    void                                    KickUser(vectorIT& index);

    void                                    BroadcastToAll(const std::string& data);

    userData*                               GetUserByNickname(const std::string& nickName);
    userData*                               GetUserByUsername(const std::string& userName);



    // you shouldnt use this
    userData*                               GetUserByFD(const int& fd);
private:
    // +++ Sockets & Errors +++


    int                                     _getSocket(const uint16_t port, const std::string ip);
    void                                    _start();
    void                                    _rateLimit(const std::string& IP);




    void                                    _cleanup();
    size_t                                  _updatePolls(const bool needUpdate = false);
    
    // +++ Users Methods +++
    void                                    _acceptConnection();
    void                                    _newUser(const int& fd, const std::string& IP);
    void                                    _recvData(vectorIT& index);

    // +++ Server Data +++
    int                                     _fd;
    const std::string                       _password;
    const bool                              _showDebug;

    // +++ Polls & UserData +++
    std::vector<pollfd>                     _polls;
    std::map<int, userData>                 _users;
    std::map<std::string, rateLimiting>     _ratelimiter;   // anti spam measure

};


Socket* wtf(Socket* ptr = nullptr);
