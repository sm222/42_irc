#include "socket.h"
#include "signal.h"

// +++ Constructor +++
Socket::Socket(const uint16_t port, const std::string password, const bool showDebug) : _ip(""), _port(port), _password(password), _showDebug(showDebug), _fd(-1), _addrInfo(0) {
    _addrInfo = _getInfo(_ip, _port);
    _fd = _getSocket(_addrInfo);
}

// +++ Destructor +++
Socket::~Socket() {
   _cleanup();
}
// +++ Public +++
bool                Socket::doesThisUsernameExist(const std::string& username) {
    for (std::map<int, userData>::iterator i = _users.begin(); i != _users.end(); i++) {
        if (i->second.userName == username)
            return true;
    }
    return false;
}
bool                Socket::doesThisNicknameExist(const std::string& nickname) {
    for (std::map<int, userData>::iterator i = _users.begin(); i != _users.end(); i++) {
        if (i->second.nickName == nickname)
            return true;
    }
    return false;
}
void                Socket::KickUser(vectorIT& index) {
    close(index->fd);

    userData* user = GetUserByFD(index->fd);
    if (user && user->userName != "") {
        channels.SOCKETONLY_kickuserfromallchannels(user->userName);
    }
    _users.erase(index->fd);
    index->fd = -1;             // We set this to -1 to remove it AFTER the Vector for loop
    _updatePolls(true);         // Definitely remove the user from Polls listing on next iteration
    if (_showDebug)  std::cout << "[DEBUG] ["+ std::string(__FILE__) +"][KickUser] User removed with Success" << std::endl;
}
void                Socket::SendData(const int& userFD, std::string data) {

    if (_users.find(userFD) == _users.end()) { 
        if (_showDebug)  std::cout << "[DEBUG] ["+ std::string(__FILE__) +"][SendData] You're trying to send data to an invalid socket..." << std::endl;
    }
    else {
        ssize_t bytesSent = send(userFD, data.c_str(), data.size(), MSG_DONTWAIT);
        if (bytesSent == -1) {
            if (_showDebug)  std::cout << "[DEBUG] ["+ std::string(__FILE__) +"][SendData] Error while sending data... send returned -1 ..." << std::endl;
        }
    }
}
void                Socket::SendData(const std::string& userName, std::string data) {
    for (std::map<int, userData>::iterator i = _users.begin(); i != _users.end(); i++) {
        if (userName == i->second.userName) {
            SendData(i->first, data);
            break;
        }
    }
}
const std::string&  Socket::GetPassword() {
    return _password;
}
userData*           Socket::GetUserByUsername(const std::string& userName) {
    for (size_t i = 0; i < _users.size(); i++) {
        if (_users[i].userName == userName)
            return &_users[i];
    }
    return 0;
}
userData*           Socket::GetUserByNickname(const std::string& nickName) {
    for (size_t i = 0; i < _users.size(); i++) {
        if (_users[i].nickName == nickName)
            return &_users[i];
    }
    return 0;
}
void                Socket::BroadcastToAll(const std::string& data) {
    for (size_t i = 0; i < _users.size(); i++) {
        SendData(_users[i].userFD, data);
    }
}

void                Socket::Listen() {
    if (_showDebug)  std::cout << "[DEBUG] ["+ std::string(__FILE__) +"][Listen] Listening..." << std::endl;
    if (listen(_fd, SOMAXCONN) == -1) {
        _cleanup();
        throw AnyExcept("[-] [EXCEPT] ["+ std::string(__FILE__) +"][Listen] Error while Listening... [" + _ip + ":" + std::to_string(_port) + "]");
    }
    if (_showDebug)  std::cout << "[DEBUG] ["+ std::string(__FILE__) +"][Listen] Activated" << std::endl;
}
void                Socket::Bind() {

    if (_showDebug)  std::cout << "[DEBUG] ["+ std::string(__FILE__) +"][Bind] Trying to bind..." << std::endl;

    if (bind(_fd, _addrInfo->ai_addr, _addrInfo->ai_addrlen) == -1) {
        _cleanup();
        throw AnyExcept("[EXCEPT] ["+ std::string(__FILE__) +"][Bind] Binding Socket Failed...");
    }

    if (_showDebug)  std::cout << "[DEBUG] ["+ std::string(__FILE__) +"][Bind] Success" << std::endl;
}
void                Socket::Connect(const std::string& ip, const uint16_t& port) {

    addrinfo* userInfo = _getInfo(ip, port);

    if (_showDebug) std::cout << "[DEBUG] ["+ std::string(__FILE__) +"][Connect] Trying to connect: [" + _ip + ":" + std::to_string(_port) + "]" << std::endl;
    if (connect(_fd, userInfo->ai_addr, userInfo->ai_addrlen) < 0) {
        freeaddrinfo(userInfo);
        userInfo = 0;
        _cleanup();
        throw AnyExcept("[-] [EXCEPT] ["+ std::string(__FILE__) +"][Connect] Couldn't Connect... [" + _ip + ":" + std::to_string(_port) + "]\n");
    }

    // We don't need that anymore
    freeaddrinfo(userInfo);
    userInfo = 0;
    if (_showDebug) std::cout << "[DEBUG] ["+ std::string(__FILE__) +"][Connect] Success [" + _ip + ":" + std::to_string(_port) + "]" << std::endl;
}
void                Socket::SetNonBlocking() {
  
    // Grab the initials flags already in the sockets
    if (_showDebug) std::cout << "[DEBUG] ["+ std::string(__FILE__) +"][SetNonBlocking] Getting socket flags..." << std::endl;
    int flags = fcntl(_fd, F_GETFL, 0);
    if (flags == -1) {
        _cleanup();
        throw AnyExcept("[-] [EXCEPT] ["+ std::string(__FILE__) +"][SetNonBlocking] Couldn't get the initial socket flags");
    }
    if (_showDebug) std::cout << "[DEBUG] ["+ std::string(__FILE__) +"][SetNonBlocking] Done" << std::endl;

    // Modify the flags
    if (_showDebug) std::cout << "[DEBUG] ["+ std::string(__FILE__) +"][SetNonBlocking] Setting the new flags..." << std::endl;
    flags = flags & ~O_NONBLOCK;
    if (fcntl(_fd, F_SETFL, flags) != 0) {
        _cleanup();
        throw AnyExcept("[-] [EXCEPT] ["+ std::string(__FILE__) +"][SetNonBlocking] Couldn't modify socket flags");
    }
    if (_showDebug) std::cout << "[DEBUG] ["+ std::string(__FILE__) +"][SetNonBlocking] Done" << std::endl;



}
void                Socket::SetAddrReusable() {

    if (_showDebug)  std::cout << "[DEBUG] ["+ std::string(__FILE__) +"][_setAddrReusable] Trying to set the socket as Reusable..." << std::endl;

    int optval = 1;
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        _cleanup();
        throw AnyExcept("[-] [EXCEPT] ["+ std::string(__FILE__) +"][_setAddrReusable] Couldnt set the socket as Reusable...");
    }

    if (_showDebug)  std::cout << "[DEBUG] ["+ std::string(__FILE__) +"][_setAddrReusable] Done" << std::endl;

}
void                Socket::Start() {
    _newUser(_fd);  // Add the Server socket to poll, so we can receive New users requests

    int     ret;
    initSignal();
    while (_updatePolls() > 0) {

        ret = poll(_polls.data(), _polls.size(), -1);       // Waiting for events

        // ------------------------- Error Handling -------------------------

        // Signals
        if (*signalStop()) {
            _cleanup();
            throw AnyExcept("[+] Signal Received -> Goodbye");
        }

        // Poll Errors
        else if (ret == -1) {
            _cleanup();
            throw AnyExcept("[-] [EXCEPT] ["+ std::string(__FILE__) +"][Start] Error with Poll...");
        }

        // Server socket Error
        else if (_polls[0].revents & (POLLERR | POLLHUP | POLLNVAL)) {
            _cleanup();
            throw AnyExcept("[-] [EXCEPT] ["+ std::string(__FILE__) +"][Start] Error with Server socket...");
        }

        // -------------------------------------------------------------------

        // New Connection
        if (_polls[0].revents & POLLIN) { 
            _acceptConnection();
            ret--;
        }

        // +++ Clients Event +++
        for (vectorIT i = _polls.begin() + 1; ret > 0 && _polls.size() > 1 && i != _polls.end(); i++) {

            // Critical Error
            if      (i->revents & (POLLERR | POLLHUP | POLLNVAL)) { 
                KickUser(i);
                ret--;
            }

            // Receiving Data
            else if (i->revents & POLLIN) {
                _recvData(i);
                ret--;
            }

        }
    
    }
}

// +++ Private +++
userData*           Socket::GetUserByFD(const int& fd) {
    std::map<int, userData>::iterator i = _users.find(fd);
    if (i != _users.end()) {
        return &i->second;
    }
    return 0;
}
int                 Socket::_getSocket(addrinfo* AddrInfo) {

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
    return 0;
}
addrinfo*           Socket::_getInfo(const std::string ip, const uint16_t port) {

    // This attempt to get IP Data
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family     = AF_UNSPEC;
    hints.ai_socktype   = SOCK_STREAM;
    hints.ai_protocol   = IPPROTO_TCP;

    if (_showDebug)  std::cout << "[DEBUG] ["+ std::string(__FILE__) +"][_getInfo] Trying to IP Info..." << std::endl;

    std::string ipvalue;
    addrinfo*   result = 0;
    int         getAddrInfoResult;

    // Server Mode
    if (ip == "") {
        hints.ai_flags = AI_PASSIVE;
        getAddrInfoResult = getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &result);
    }

    // Client mode
    else {
        getAddrInfoResult = getaddrinfo(ip.c_str(), std::to_string(port).c_str(), &hints, &result);
    }

    // Check Return Value
    if (getAddrInfoResult != 0) {
        result = 0;
        _cleanup();
        throw AnyExcept("[-] [EXCEPT] ["+ std::string(__FILE__) +"][_getInfo] Couldnt Resolve IP... [" + ip + ":" + std::to_string(port) + "] Errno: [" + std::to_string(getAddrInfoResult) +  "]");
    }

    if (_showDebug)  std::cout << "[DEBUG] ["+ std::string(__FILE__) +"][_getInfo] Success" << std::endl;

    return result;
}
void                Socket::_newUser(const int& fd) {

    // Adding the socket info of the user in polls
    // Poll wants an array, and a vector is perfect for that
    pollfd newuser;
    newuser.fd = fd;
    newuser.events = POLLIN;
    newuser.revents = 0;

    // Data of the new user
    userData newUserData;
    newUserData.userFD = fd;
    newUserData.currentAction = 0;
    newUserData.nickName = "";
    newUserData.userName = "";
    newUserData.recvString = "";

    _users[fd] = newUserData;
    _polls.push_back(newuser);

    // Server
    if (fd == _fd)  { if (_showDebug) std::cout << "[DEBUG] Server online" << std::endl; }

    // New Client
    else            { if (_showDebug) std::cout << "[DEBUG] New User online" << std::endl; }
}
size_t              Socket::_updatePolls(const bool needUpdate) {
    static bool update;

    // Tell this function that we need to remove one user or more
    if (needUpdate) {
        update = true;
        return 0;
    }

    // Remove user from Poll listing
    if (update) {

        std::vector<pollfd> T;
        for (vectorIT i = _polls.begin(); i != _polls.end(); i++) {

            // If FD isnt already closed, keep user
            if (i->fd != -1) { T.push_back(*i); }

        }
        _polls = T;
        update = false;
    }

    return _polls.size();
}
void                Socket::_cleanup() {

    if (_fd != -1) {
        close(_fd);
    }

    if (_addrInfo) {
        freeaddrinfo(_addrInfo);
        _addrInfo = 0;
    }

    for (vectorIT i = _polls.begin(); i != _polls.end(); i++) {
        close(i->fd);
    }

    _users.clear();
    _polls.clear();
}
void                Socket::_acceptConnection() {
    sockaddr_in     clientAddr;
    socklen_t       clientInfo = sizeof(clientAddr);
    int newUser = accept(_fd, (sockaddr*)&clientAddr, &clientInfo);

    if  (newUser != -1) { _newUser(newUser); }

    else                { if (_showDebug)  std::cout << "[DEBUG] ["+ std::string(__FILE__) +"][_acceptConnection] Error while accepting new user..." << std::endl; }

 }
void                Socket::_recvData(vectorIT& index) {
    static Parser parser(*this);       // Parser instance starts here


    // I think there's a limit to message lens. if there's not, ill need to do something similar
    /*
        int bytesReceived = 0;
        do {
            bytesReceived = recv(*csock, &buffer[0], buffer.size(), 0);
            // append string from buffer.
            if ( bytesReceived == -1 ) { 
                // error 
            } else {
                rcv.append( buffer.cbegin(), buffer.cend() );
            }
        } while ( bytesReceived == MAX_BUF_LENGTH );
    
    */

    // Receiving Data part
    char buffer[BUFFER_SIZE];
    int len = recv(index->fd, buffer, BUFFER_SIZE - 1, MSG_DONTWAIT);

    // Errors
    if (len <= 0) {
        KickUser(index);
        return;
    }

    // Store the data, then do something with it
    buffer[len] = '\0';
    _users[index->fd].recvString = buffer;

    // Send Stuff to Parse Class
    parser.ParseData(_users[index->fd], index);
}
