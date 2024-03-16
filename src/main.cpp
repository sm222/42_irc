#include "socket.h"
#include "Channels.h"

int main(int argc, char **args) {
    (void)argc;
    (void)args;

    //  Votre binaire devra être appelé comme ceci:
    //      ./ircserv <port> <password>

    try {
        Socket serverSocket(2001, "password");
        serverSocket.SetAddrReusable();
        serverSocket.SetNonBlocking();
        serverSocket.Bind();
        serverSocket.Listen();
        serverSocket.Start();
    }
    catch (const std::exception& e) { std::cout << e.what() << std::endl; }
    return 0;
}


