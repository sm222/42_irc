#include "parser.h"


Parser::Parser(Socket& socketClass) : Sock(socketClass) {}
Parser::~Parser(){}

void    Parser::ParseData(userData& user, vectorIT& index) {
    (void)user;
    (void)index;
    (void)Sock;
    std::cout << user.recvString;
}