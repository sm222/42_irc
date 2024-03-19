#include "parser.h"
#include <iostream>

static int  nb_word(const char *str, char c) {
	int i;
	int size;

  i = 0;
  size = 0;
  while (str[i]) {
    while (str[i] && str[i] == c)
      i++;
    if (str[i])
      size++;
    while (str[i] && str[i] != c)
      i++;
  }
  return size;
}

static std::string ft_strdup_c(const char *str, char c) {
  size_t        i;
  std::string   ret;

  i = 0;
  while (str[i] && str[i] != c) {
    ret += str[i];
    i++;
  }
  return ret;
}

splitData ft_split(char const *s, char c) {
  size_t	i;
  size_t  nbW;
  splitData  ret;

  i = 0;
  if (!s)
    throw std::invalid_argument("split fail");
  nbW = nb_word(s + i, c);
  while (nbW--)
  {
    while (s[i] && s[i] == c)
      i++;
    ret.push_back(ft_strdup_c(s + i, c));
    while (s[i] && s[i] != c)
      i++;
  }
  return ret;
}

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

void    Parser::ParseData(userData& user, vectorIT& index) {
    Channels& AllChannels = Sock.channels;
    (void)AllChannels;
    splitData split = ft_split(user.recvString.c_str(), ' ');
    for (size_t i = 0; i < split.size(); i++) {
      std::cout << '[' << i << ']' << split[i] << std::endl; }
    if (std::strncmp(split[0].c_str(), "PASS", 4) == 0) {
      std::string str = Sock.GetPassword();
      if (str != split[1]) {
        Sock.SendData(user.userFD, "get fuck");
        Sock.KickUser(index);
      }
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
  //* shit way to get the user so weechar stop crying
  if (std::strncmp(split[0].c_str(), "USER", 4) == 0) {
    setUserInfo(user);
  }
  //? send a pong so the weechat don't stop the connection
  else if (std::strncmp(split[0].c_str(), "PING", 4) == 0) {
    std::string tmp = "PONG ";
    tmp += user.recvString.c_str() + 5;
    Sock.SendData(user.userFD, tmp);
  }
  else if (std::strncmp(user.recvString.c_str(), "JOIN", 4) == 0) {
    std::string tmp = user.recvString.c_str() + 5;
    size_t  i = 0;
    while (i < tmp.size()) {
      if (tmp[i] == '#') {
        size_t  j = 1;
        while (i + j < tmp.size() && std::isalpha(tmp[i + j])) {j++;}
        joinChanel(user, tmp.substr(i + 1, j));
      }
    i++;
    }
  }
    //user.userName = "userName";                   // Set username
    //user.nickName = "nickName";                   // Set nickname
    //user.userFD;                                  // Hold user FD
    //user.recvString;                              // Data received just now

    //Sock.KickUser(index);                           // Kick user
    //Sock.GetPassword();                             // To check if user entered correct password, return the password
    //Sock.SendData(user.userFD, "nice scam sir");    // You can easily send data with this

    std::cout << "Received: " + user.recvString;    // Data Received
}
