#include "parser.h"
#include <iostream>

Parser::Parser(Socket& socketClass) : Sock(socketClass) {}
Parser::~Parser(){}

/// @brief use to build a message to send
/// @param type 
/// @param msg %u = user.userName, %n = user.nickName, %i could add ip ?
/// @param user 
/// @return 
std::string  Parser::makeMessage(t_code const type, const std::string msg, const userData& user) {
    std::string result = MType[type];
    result += " " + user.userName + " ";
    for (size_t i = 0; i < msg.size(); i++) {
      if (msg[i] == '%' && msg[i + 1] == 'u') {
        result += user.userName;
        i++;
      }
      else if (msg[i] == '%' && msg[i + 1] == 'n') {
        result += user.nickName;
        i++;
      }
      else
        result += msg[i];
    }
    return (result);
}

bool Parser::setUserInfo(userData& user) {
  size_t  i = 0;
  while (5 + i < user.recvString.size() && user.recvString[5 + i] != ' ') {i++;}
  user.userName = user.recvString.substr(5, i);
  while (i < user.recvString.size() && user.recvString[i] != ':') {i++;}
  user.nickName = user.recvString.substr(i + 1, i - user.recvString.size());
  std::cout << "name = " << user.userName << " ,nick name = " << user.nickName << std::endl;
  Sock.SendData(user.userFD, makeMessage(e_welcom, ":Welcome to the 42irc %n", user));
  return true;
}

// ! not final, use as templet
bool    Parser::joinChanel(const userData& user, const std::string chanelName) {
  std::string tmp;
  if (Sock.channels.Channel_AlreadyExist(chanelName) == true) {
    Sock.channels.Channel_Join(user.userName, chanelName);
    tmp = makeMessage(e_rplTopic, chanelName, user);
    tmp += " " + Sock.channels.Channel_Get_Topic(chanelName);
    return true;
  }
    Sock.channels.Channel_Create(user.userName, chanelName);
  return true;
}

vec_str Parser::TokenizeMessage(std::string message){
  vec_str vec;
  size_t pos = 0;
  size_t old_pos = 0;
  // bool inQuote = false;
  message.erase(message.find_last_not_of("\n"), 1);
  message.erase(message.find_last_not_of("\r"), 1);
  while ((pos = message.find(" ", old_pos)) != std::string::npos) {
      std::string token = message.substr(old_pos, pos - old_pos);
      if (!token.empty())
          vec.push_back(token);
      old_pos = pos + 1;
  }
  if (old_pos < message.length())
      vec.push_back(message.substr(old_pos));
  for (size_t i = 0; i < vec.size(); ++i) 
      std::cout << "{" << i << "}" << vec[i] << "|" << std::endl;
  return (vec);
}

void TEST(std::string str){
  std::cout << str << std::endl;
}

void    Parser::ParseData(userData& user, vectorIT& index) {
    Channels& AllChannels = Sock.channels;
    (void)AllChannels;
    vec_str split = TokenizeMessage(user.recvString);
    if (split[0] == "PASS"){
      if (split[1] == Sock.GetPassword()){
        std::cout << "Valid password" << std::endl;
      }
      else{
        Sock.SendData(user.userFD, makeMessage(e_passmismatch, "Server :password incorect", user));
        Sock.KickUser(index);
        std::cout << "Bad password" << std::endl;
      }
    }

    if (split[0] == "PING"){
      Sock.SendData(user.userFD, std::string("PONG ") + split[1]);
      std::cout << user.userName << " :PING" << std::endl;
    }

    if (split[0] == "USER"){
      setUserInfo(user);
    }

    // index is pretty much only used to kick user. 
    //user.currentAction = 1;                       // Action Index, step 1 = wait 4 password, step 2 = ask for username..Etc
                                                    // im not using it anywhere, its for you guys to use
    // Little example how i think you should use currentAction
  /*
    // Step 1 -> Check password         ( you can probably use an Enum for this )
    if (user.currentAction == 0) {

        // If the user DIDNT enter the right password, kick his ass
        if (user.recvString != Sock.GetPassword()) {
            Sock.SendData(user.userFD, "eat shit and die\n");
            Sock.KickUser(index);
            std::cout << "USER KICKED" << std::endl;
            return;
        }
        // Otherwise, go to next step
        else {
            user.currentAction++;
        }
    }
    // Step 2 -> Save user
    else if (user.currentAction == 2) {
        // Empty Username, We dont increment, so user will be forced to actually send something
        if (user.recvString == "") {
            Sock.SendData(user.userFD, "Are you fucking dumb ? Enter something bozo");
        }
        // Correct Username
        else {
            user.userName = user.recvString;
            Sock.SendData(user.userFD, "Welcome" + user.userName);
            user.currentAction++;   // Next step ...
        }
    }
  */
  (void)index;
  /*
    use to split on the data
  */
  //* shit way to get the user so weechat stop crying
  //? send a pong so the weechat don't stop the connection
    //user.userName = "userName";                   // Set username
    //user.nickName = "nickName";                   // Set nickname
    //user.userFD;                                  // Hold user FD
    //user.recvString;                              // Data received just now

    //Sock.KickUser(index);                           // Kick user
    //Sock.GetPassword();                             // To check if user entered correct password, return the password
    //Sock.SendData(user.userFD, "nice scam sir");    // You can easily send data with this

    std::cout << "Received: " + user.recvString;    // Data Received
}
