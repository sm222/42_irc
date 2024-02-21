#include "socket.h"

int main(int argc, char **args) {


    try {
        Socket serverSocket("127.0.0.1", 80, true);
        // Need to implement .SetNonBlock(), man fcntl
        serverSocket.Bind();
        serverSocket.Listen();
        
        // Need to implement Read & Send
            // then, we add our socket FD to poll ( or receive, but i strongly suggest poll)
            // & we wait for poll to trigger
            // it will means someone connected, crashed, or sent data


        // Here is what it should looks like
        // https://i.imgur.com/28XAJjj.png

        // i also added stuff in the drawio

    }
    catch (const std::exception& e) { std::cout << e.what() << std::endl; }

    return 0;
}