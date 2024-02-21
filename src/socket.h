#pragma once

#include "_header.h"

class Socket {
public:
    Socket(const std::string ip, uint16_t port, const bool showDebug, const bool isServer = true);
    ~Socket();
 
    void                Bind();
    void                Listen();
    void                Connect(const std::string ip, const uint16_t port);

private:

    int                 _getSocket(addrinfo* AddrInfo);
    addrinfo*           _getInfo(const std::string ip, const uint16_t port);
    void                _cleanup();

    // +++ Data +++
    const std::string   _ip;
    const uint16_t      _port;
    const bool          _isServer;
    const bool          _showDebug;

    // +++ Free & Close +++
    int                 _fd;
    addrinfo*           _addrInfo;
};