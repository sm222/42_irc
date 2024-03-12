#include "socket.h"
#include "Channels.h"
#include "parsingAv.hpp"

int main(int argc, char **args) {
    //  Votre binaire devra être appelé comme ceci:
    //      ./ircserv <port> <password>
    valueAv value;

    if (!parsingAv(argc, args, value))
      return 1;
    try {
        Socket serverSocket(value.port, value.pass.c_str(), true);
        serverSocket.SetAddrReusable();
        serverSocket.SetNonBlocking();
        serverSocket.Bind();
        serverSocket.Listen();
        serverSocket.Start();
    }
    catch (const std::exception& e) { std::cout << e.what() << std::endl; }
    return 0;
}


