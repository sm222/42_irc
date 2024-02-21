#include "socket.h"

// +++ Constructor +++
Socket::Socket(const std::string ip, const uint16_t port, const bool showDebug, const bool isServer) : _ip(ip), _port(port), _showDebug(showDebug), _isServer(isServer), _fd(-1), _addrInfo(nullptr) {
    _addrInfo = _getInfo(_ip, _port);
    _fd = _getSocket(_addrInfo);
}

// +++ Destructor +++
Socket::~Socket() {
   _cleanup();
}

// +++ Public +++
void            Socket::Listen() {

    // Start listening for incoming connections
    // Settings an actual value instead of SOMAXCONN(4096) might be good ...     <--------
    // New users enter a queue @ Listen

    if (_showDebug)  std::cout << "[DEBUG] ["+ std::string(__FILE__) +"][Listen] Listening..." << std::endl;
    if (listen(_fd, SOMAXCONN) == -1) {
        _cleanup();
        throw AnyExcept("[-] [EXCEPT] ["+ std::string(__FILE__) +"][Listen] Error while Listening... [" + _ip + ":" + std::to_string(_port) + "]");
    }
    if (_showDebug)  std::cout << "[DEBUG] ["+ std::string(__FILE__) +"][Listen] Activated" << std::endl;
}
void            Socket::Bind() {

    // Bind the socket, Tell the socket that we will be accepting connection from users

    if (_showDebug)  std::cout << "[DEBUG] ["+ std::string(__FILE__) +"][Bind] Trying to bind..." << std::endl;
    
    if (bind(_fd, _addrInfo->ai_addr, _addrInfo->ai_addrlen) == -1) {
        _cleanup();
        throw AnyExcept("[EXCEPT] ["+ std::string(__FILE__) +"][Bind] Binding Socket Failed...");
    }

    if (_showDebug)  std::cout << "[DEBUG] ["+ std::string(__FILE__) +"][Bind] Success !" << std::endl;
}
void            Socket::Connect(const std::string ip, const uint16_t port) {

    // Connect to an user ( if we're making the bonus, this will be needed )

    addrinfo* userInfo = _getInfo(ip, port);

    if (_showDebug) std::cout << "[DEBUG] ["+ std::string(__FILE__) +"][Connect] Trying to connect: [" + _ip + ":" + std::to_string(_port) + "]" << std::endl;
    if (connect(_fd, userInfo->ai_addr, userInfo->ai_addrlen) < 0) {
        freeaddrinfo(userInfo);
        userInfo = nullptr;
        _cleanup();
        throw AnyExcept("[-] [EXCEPT] ["+ std::string(__FILE__) +"][Connect] Couldn't Connect... [" + _ip + ":" + std::to_string(_port) + "]\n");
    }

    // We don't need that anymore
    freeaddrinfo(userInfo);
    userInfo = nullptr;
    if (_showDebug) std::cout << "[DEBUG] ["+ std::string(__FILE__) +"][Connect] Success ! [" + _ip + ":" + std::to_string(_port) + "]" << std::endl;
}

// +++ Private +++
int             Socket::_getSocket(addrinfo* AddrInfo) {

    // Return the Socket

    // This is a for loop, since sometimes, you can get multiple address from a simple ip / domain
    // IPV4, IPV6, Ect..

    // Attempt to get the Right IP
    int serverSocket = -1;
    if (_showDebug)  std::cout << "[DEBUG] ["+ std::string(__FILE__) +"][_getSocket] Trying to get socket..." << std::endl;
    for (addrinfo* T = AddrInfo; T; T = T->ai_next) {

        // Create Socket
        serverSocket = socket(T->ai_family, T->ai_socktype, T->ai_protocol);
        if (serverSocket == -1) { 
            if (_showDebug)  std::cout << "[DEBUG] ["+ std::string(__FILE__) +"][_getSocket] Socket Attempted Failed... Continue" << std::endl;
            continue; 
        }

        // Found a valid socket
        if (_showDebug)  std::cout << "[DEBUG] ["+ std::string(__FILE__) +"][_getSocket] Success" << std::endl;
        return serverSocket;
    }

    _cleanup();
    throw AnyExcept("[-] [EXCEPT] ["+ std::string(__FILE__) +"][_getSocket] Couldn't find a viable socket.. [" + _ip + ":" + std::to_string(_port) + "]");
}
addrinfo*       Socket::_getInfo(const std::string ip, const uint16_t port) {
    addrinfo    hints;                  // Used to "filter" those options
    addrinfo*   result = nullptr;       // IP data

    // This attempt to get IP Data
    memset(&hints, 0, sizeof(hints));
    hints.ai_family     = AF_INET;                          //  IPV4
    hints.ai_socktype   = SOCK_STREAM;                      //  TCP socket
    hints.ai_flags      = _isServer ? AI_PASSIVE : 0;       //  Server = AI_PASSIVE, Client = 0

    if (_showDebug)  std::cout << "[DEBUG] ["+ std::string(__FILE__) +"][_getInfo] Trying to IP Info..." << std::endl;

    if (getaddrinfo(ip.c_str(), std::to_string(port).c_str(), &hints, &result) != 0) {
        result = nullptr;
        _cleanup();

        // getaddrinfo cause some "still reachable" if it fails
        // https://i.imgur.com/ODliCy6.png
        // https://stackoverflow.com/questions/41829655/valgrind-memory-leaks-because-of-getaddrinfo

        throw AnyExcept("[-] [EXCEPT] ["+ std::string(__FILE__) +"][_getInfo] Couldnt Resolve IP... [" + ip + ":" + std::to_string(port) + "]");
    }

    if (_showDebug)  std::cout << "[DEBUG] ["+ std::string(__FILE__) +"][_getInfo] Success" << std::endl;

    return result;
}
void            Socket::_cleanup() {

    // Just in case we forget to Catch... oO

    // Close FD
    if (_fd != -1)
        close(_fd);

    // Free Address Info
    if (_addrInfo) {
        freeaddrinfo(_addrInfo);
        _addrInfo = nullptr;
    }
}