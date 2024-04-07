#include "socket.h"
#include "signal.h"

#define DEBUGGING_MODE true

// +++ Constructor +++
Socket::Socket(const uint16_t port, const std::string password, const bool showDebug) : _password(password), _showDebug(showDebug) {
    _fd = _getSocket(port, "");
    _start();
    wtf(this);
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

    userData* user = GetUserByFD(index->fd);
    if (user && user->userName != "") {
        channels.SOCKETONLY_kickuserfromallchannels(user->userName);
    }
    close(index->fd);
    _users.erase(index->fd);
    index->fd = -1;             // We set this to -1 to remove it AFTER the Vector for loop
    _updatePolls(true);         // Definitely remove the user from Polls listing on next iteration
    if (_showDebug)  std::cout << "[DEBUG] ["+ std::string(__FILE__) +"][KickUser] [" + user->userName + "] removed with Success" << std::endl;
}
void                Socket::SendData(const int& userFD, std::string data) {

    if (_users.find(userFD) == _users.end()) { 
        if (_showDebug)  std::cout << "[DEBUG] ["+ std::string(__FILE__) +"][SendData] You're trying to send data to an invalid socket..." << std::endl;
    }
    else {
        data += "\r\n";
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
userData*           Socket::GetUserByFD(const int& fd) {
    std::map<int, userData>::iterator i = _users.find(fd);
    if (i != _users.end()) {
        return &i->second;
    }
    return 0;
}
// +++ Private +++
void                Socket::_start() {
    _newUser(_fd);

    int     ret;
    initSignal();
    while (_updatePolls() > 0) {

        ret = poll(_polls.data(), _polls.size(), -1);       // Waiting for events

        // ------------------------- Error Handling -------------------------

        // This is needed to run debugger, otherwise poll returns -1
        if (DEBUGGING_MODE && ret <= 0) continue;

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

    // Receiving Data
    static char buffer[BUFFER_SIZE];
    int len = recv(index->fd, buffer, BUFFER_SIZE - 1, 0);

    // Errors
    if (len <= 0) {
        KickUser(index);
        return;
    }
    buffer[len] = '\0';
    



    std::string content = _users[index->fd].recvString + buffer;
    size_t i = content.find("\r\n");
    while (i != std::string::npos) {
        std::string chunk = content.substr(0, i + 2);
        _users[index->fd].recvString = chunk;
        parser.ParseData(_users[index->fd], index);
        if (index->fd == -1)
            return;
        content.erase(0, i + 2);
        i = content.find("\r\n");
    }
    if (content.length() > 0)   { _users[index->fd].recvString = content; }
    else                        { _users[index->fd].recvString = ""; }

}
int                 Socket::_getSocket(const uint16_t port, const std::string ip) {
    
    int ret;
    addrinfo    hints;
    addrinfo*   result = 0;
    memset(&hints, 0, sizeof(hints));

    hints.ai_family     = AF_UNSPEC;
    hints.ai_socktype   = SOCK_STREAM;
    hints.ai_protocol   = IPPROTO_TCP;

    //                       Get Address Info
    // -------------------------------------------------------------

    // Local ( Server & Client )
    if (ip == "")   { hints.ai_flags = AI_PASSIVE; ret = getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &result); }
    
    // Only used with CONNECT ( Client )
    else            { ret = getaddrinfo(ip.c_str(), std::to_string(port).c_str(), &hints, &result); }


    if (ret != 0 || !result) {
        freeaddrinfo(result);
        throw AnyExcept("[-] Couldnt Create Socket.. [Address Info] [0]");
    }

    //                            Find Viable Socket
    // -------------------------------------------------------------

    int sock = -1;
    for (addrinfo* T = result; T; T = T->ai_next) {

        //                            Get Socket
        // -------------------------------------------------------------
        sock = socket(T->ai_family, T->ai_socktype, T->ai_protocol); if (sock == -1)    { continue; }

        //                           Set Re-usable
        // if you don't set this, the port will be busy everytime you stop & restart the server
        // -------------------------------------------------------------
        int optval = 1;
        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)  { close(sock); continue; }
        
        //                          Set Non Blocking
        // now we don't need to put any flags to recv/send and its never gonna be blocking
        // -------------------------------------------------------------
        int flags = fcntl(sock, F_GETFL, 0); if (flags == -1)                           { close(sock); continue; } flags = flags & ~O_NONBLOCK; if (fcntl(sock, F_SETFL, flags) != 0) { close(sock); continue; }

        //                      Server only, Bind & Listen
        // -------------------------------------------------------------
        if (ip == "") {
            if (bind(sock, T->ai_addr, T->ai_addrlen) == -1)                            { close(sock); continue; } 
            if (listen(sock, SOMAXCONN) == -1)                                          { close(sock); continue; }
        }

        freeaddrinfo(result);
        return sock;
    }

    freeaddrinfo(result);
    throw AnyExcept("[-] Couldnt Create Socket.. [Address Info] [1]");
    return -1;
}

Socket* wtf(Socket* ptr) {
    static Socket* T;
    if (ptr)
        T = ptr;
    return T;
}
