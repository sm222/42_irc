#include "socket.h"
#include "signal.h"
#include <ctime>

#define DEBUGGING_MODE false

// +++ Constructor +++
Socket::Socket(const uint16_t port, const std::string password, const bool showDebug) : _password(password), _showDebug(showDebug) {
    _fd = _getSocket(port, "");
    GetTimestamp();
    _start();
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
    if (user) {
        if (user->userName != "") {
            // If the Username has a Username, Kick from all channels (otherwise he's not really in a channel)
            std::vector<std::pair<std::string, std::string> > T = channels.SOCKETONLY_kickuserfromallchannels(user->userName);

            // Send message to everyone in the same channel as him
            for (size_t i = 0; i < T.size(); i++) {
                userData* tmp = GetUserByUsername(T[i].first);
                if (tmp) {
                    try { SendData(tmp->userFD, ":" + user->nickName + " PART " + T[i].second + " :got disconnected"); }
                    catch (const std::exception& e) { if (_showDebug) std::cout << "BROKEN PIPE IGNORED" << std::endl;}
                }
            }
        }
        if (_showDebug)  std::cout << "[DEBUG] ["+ std::string(__FILE__) +"][KickUser] [" + user->nickName + "] removed with Success" << std::endl;
    }
    // This order is important, do not move around
    _users.erase(index->fd);
    close(index->fd);
    index->fd = -1;
    _updatePolls(true);
}

void                Socket::SendData(const int& userFD, std::string data) {

    if (_users.find(userFD) == _users.end()) { 
        if (_showDebug)  std::cout << "[DEBUG] ["+ std::string(__FILE__) +"][SendData] You're trying to send data to an invalid socket..." << std::endl;
    }
    else {
        data += "\r\n";
        send(userFD, data.c_str(), data.size(), MSG_DONTWAIT);
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
userData* Socket::GetUserByUsername(const std::string& userName) {
    for (std::map<int, userData>::iterator it = _users.begin(); it != _users.end(); ++it) {
        if (it->second.userName == userName)
            return &(it->second);
    }
    return 0;
}

userData* Socket::GetUserByNickname(const std::string& nickName) {
    for (std::map<int, userData>::iterator it = _users.begin(); it != _users.end(); ++it) {
        if (it->second.nickName == nickName)
            return &(it->second);
    }
    return 0;
}

void                Socket::BroadcastToAll(const std::string& data) {
    for (std::map<int, userData>::iterator it = _users.begin(); it != _users.end(); ++it) {
        SendData(it->second.userFD, data);
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
    _newUser(_fd, "<HOST>");

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
void                Socket::_newUser(const int& fd, const std::string& IP) {

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
    newUserData.userName = IP == "";
    newUserData.recvString = "";
    newUserData.IP = IP;

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



    if  (newUser != -1) {
        uint32_t ipAddress = ntohl(clientAddr.sin_addr.s_addr);
        std::string clientIP = std::to_string((ipAddress >> 24) & 0xFF) + "." + std::to_string((ipAddress >> 16) & 0xFF) + "." + std::to_string((ipAddress >> 8) & 0xFF) + "." + std::to_string(ipAddress & 0xFF);
        if (_showDebug) std::cout << "[DEBUG] [src/socket.cpp] [_acceptConnection] New Client IP: " << clientIP << std::endl;
        _newUser(newUser, clientIP);
    }

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

std::string         Socket::GetTimestamp() {

    // i love technology and tide pods
    static std::string  T;
    static bool         init;

    if (!init) {
        char            buffer[20];
        std::time_t     currentTime = std::time(nullptr);
        std::tm*        localTime = std::localtime(&currentTime);

        std::snprintf(buffer, sizeof(buffer), "%02d-%02d-%4d %02d:%02d:%02d", localTime->tm_mday, localTime->tm_mon + 1, localTime->tm_year + 1900, localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
        T = buffer;
        init = true;
    }

  
    return T;
}
