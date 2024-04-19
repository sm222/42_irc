#include "socket.h"
#include "Channels.h"
#include <stdlib.h>

int main(int argc, char **args) {
    // ++++++++++++++ Args Counts ++++++++++++++
    if (argc != 3) {
        std::cout << "./ircserv <port> <password>" << std::endl;
        return 1;
    }
    // ++++++++++++++ Port ++++++++++++++
    uint16_t port;
    try {
        int value = std::atoi(args[1]);
        if (value < 1 || value > 65535) {
            std::cout << "[-] Port can only be between 0 and 65535" << std::endl;
            return 1;
        }
        port = static_cast<uint16_t>(value);
    } catch (const std::exception& e) {
        std::cout << "[-] Port Argument is messed up" << std::endl;
        return 1;
    }
    // ++++++++++++++ Password ++++++++++++++
    if (strlen(args[2]) > 100) {
        std::cout << "[-] Anti Dave Password Policy - 100 Chars max Password" << std::endl;
        return 1;
    }
    for (size_t i = 0; args[2][i]; i++) {
        if (!isascii(args[2][i])) {
            std::cout << "[-] Password can only be ascii chars" << std::endl;
            return 1;
        }
    }
    // ++++++++++++++ Start ++++++++++++++
    std::string pw = args[2];
    try {
        Socket serverSocket(port, pw);
    }
    catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}


