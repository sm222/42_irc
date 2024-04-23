#pragma once

#include "_header.h"
#include "Channels.h"
#include "parser.h"

#define BUFFER_SIZE 512


class Socket {
public:
    Socket(uint16_t port, const std::string password, const bool showDebug = false);
    ~Socket();
 
    Channels                                channels;

    void                                    SendData(const int& userFD, std::string data);
    void                                    SendData(const std::string& userName, std::string data);

    bool                                    doesThisNicknameExist(const std::string& nickname);
    bool                                    doesThisUsernameExist(const std::string& username);

    const std::string&                      GetPassword();
    std::string                             GetTimestamp();
    void                                    KickUser(vectorIT& index);

    void                                    BroadcastToAll(const std::string& data);

    userData*                               GetUserByNickname(const std::string& nickName);
    userData*                               GetUserByUsername(const std::string& userName);
    userData*                               GetUserByFD(const int& fd);
private:
    // +++ Sockets & Errors +++
    void                                    _start();
    void                                    _cleanup();
    size_t                                  _updatePolls(const bool needUpdate = false);
    int                                     _getSocket(const uint16_t port, const std::string ip);
    
    // +++ Users Methods +++
    void                                    _acceptConnection();
    void                                    _recvData(vectorIT& index);
    void                                    _newUser(const int& fd, const std::string& IP);

    // +++ Server Data +++
    int                                     _fd;
    const std::string                       _password;
    const bool                              _showDebug;

    // +++ Polls & UserData +++
    std::vector<pollfd>                     _polls;
    std::map<int, userData>                 _users;

};
