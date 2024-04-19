#include "parser.h"
#include "_header.h"
#include <cctype>
#include <cstddef>
#include <cstring>
#include <ostream>
#include <string>
#include <vector>


//FUNCTION

/// @brief check if str isalpha/isdigit and dont start with a digit
/// @param str a string 
/// @param other a string to add chars in the filter
/// @return bool
bool isValidStr(std::string str, std::string other, bool printable = false) {
  if (str.empty() || isdigit(str[0]))
    return false;
  for (size_t i = 0; i < str.length(); i++) {
    if (!printable && !isalnum(str[i]) && other.find(str[i]) == std::string::npos)
      return false;
    else if (printable && !isprint(str[i])) {
      return false;
    }
  }
  return true;
}

int charCount(std::string str, char c) {
  return std::count(str.begin(), str.end(), c);
}

bool strDoubleChar(std::string str) {
  for (size_t i = 0; i < str.length(); i++) {
    for (size_t j = i + 1; j < str.length(); j++) {
      if (str[i] == str[j])
        return true;
    }
  }
  return false;
}

void print_vec(vec_str vec, std::string name) {
  if (vec.empty())
    return;
  std::cout << MAG "|" RESET << name;
  for (size_t i = 0; i < vec.size(); ++i)
      std::cout <<  "[" << i << "]" YEL << vec[i] << RESET "|";
  std::cout << std::endl;
}

std::string supchar(const std::string &str, char c) {
	std::string tmp = "";
  if (str.empty())
    return tmp;
	for (size_t i = 0; i < str.length(); i++)
		if (str[i] != c)
			tmp += str[i];
	return tmp;
}

//CLASS

Parser::Parser(Socket& socketClass) : Sock(socketClass), _channels(Sock.channels) {}
Parser::~Parser() {}

vec_str Parser::Tokenize(std::string message, char c) {
  vec_str vec;
  size_t pos = 0;
  size_t old_pos = 0;
  message = supchar(message, '\r');
  message = supchar(message, '\n');
  size_t point = message.find(':', 0);
  std::string _text;
  std::string _cmds;

  if (message.empty())
    return vec;
  if (point != std::string::npos){
    _text = message.substr(message.find(':', 0));
    _cmds = message.substr(0, message.find(':', 0));
  }else {
    _cmds = message;
  }
  while ((pos = _cmds.find(c, old_pos)) != std::string::npos) {
    std::string token = _cmds.substr(old_pos, pos - old_pos);
    if (!token.empty())
      vec.push_back(token);
    old_pos = pos + 1;
  }
  if (old_pos < _cmds.length())
    vec.push_back(_cmds.substr(old_pos));
  if (point != std::string::npos)
    vec.push_back(_text);
  return (vec);
}

/*
* - asuming the currentAction = 0 at start, step 0 is to confim the password
*                                                *
*  password - 0                                  *
*            |                                   *
*            get user info - 1                   *
*                          |                     *
*                          ? next step 2         *
*                                                *
*/

bool Parser::chaIsValid(std::string str) {
  if (str.length() == 1 || !isValidStr(str, "#&") || (str[0] != '#' && str[0] != '&')) {
    return false;
  }
  return true;
}

bool Parser::keyIsValid(std::string str){
  return isValidStr(str, "!@$%?&*");
}

void Parser::fnPASS(vec_str& vec, userData& user) {
  if (vec.size() < 2){
    Sock.SendData(user.userFD, ERR_NEEDMOREPARAMS(user.nickName, user.recvString)); 
    return;
  }
  if (user.currentAction == e_userRegistred) {
    Sock.SendData(user.userFD, ERR_ALREADYREGISTRED);
    return;
  }
  if (testPassWord(vec[1], user, *_index)) {
    user.currentAction = 1;
    user.userName.clear();
    user.nickName.clear();
  }
}

void Parser::fnUSER(vec_str& vec, userData& user) {
  if (vec.size() < 5) {
    Sock.SendData(user.userFD, ERR_NEEDMOREPARAMS(user.nickName, user.recvString)); 
    return;
  }
  if (!user.userName.empty() || user.userName == vec[1]) {
    Sock.SendData(user.userFD, ERR_ALREADYREGISTRED);
    return;
  }
  if (isValidStr(vec[1], "-_")) {
    if (!Sock.GetUserByUsername(vec[1])) {
      user.userName = vec[1];
      user.currentAction++;
      Sock.SendData(user.userFD, RPL_WELCOME(user.userName, user.nickName));
      return;
    }
    kickUser(*_index, "904 " ServerName " :user alrady use", user);
    return;
  }
  kickUser(*_index, "904 " ServerName " :invalide character for user", user);
}

void Parser::fnNICK(vec_str& vec, userData& user) {
  if (vec.size() < 2) {
    Sock.SendData(user.userFD, ERR_NEEDMOREPARAMS(user.nickName, user.recvString));
    return;
  }
  if (!user.nickName.empty()) {
    if (!Sock.doesThisNicknameExist(vec[1])) {
      sendAllChannel(user, ":" + user.nickName + " NICK " + vec[1]);
      user.nickName = vec[1];
    }
    else {
      Sock.SendData(user.userFD, ERR_NICKNAMEINUSE(vec[1]));
    }
    return;
  }
  if (isValidStr(vec[1], "-_")) {
    if (!Sock.GetUserByNickname(vec[1])) {
      user.nickName = vec[1];
      user.currentAction++;
      return;
    }
    kickUser(*_index, ERR_NICKNAMEINUSE(vec[1]), user);
    return;
  }
  else
    kickUser(*_index, ERR_ERRONEUSNICKNAME(vec[1]), user);
}

void Parser::fnJOIN(vec_str& vec, userData& user) {
  vec_str channel;
  vec_str key;

  //error verification
  if (vec.size() < 2) {
    Sock.SendData(user.userFD, ERR_NEEDMOREPARAMS(user.nickName, user.recvString));
    return;
  }
  if (user.recvString.find(':') != std::string::npos) {
      Sock.SendData(user.userFD, "479 :invalid character");
      return;
  }
  //channel and key logical
  if (vec.size() >= 2 && !vec[1].empty()) {
    channel = Tokenize(vec[1], ',');
    if (vec.size() >= 3 && !vec[2].empty())
      key = Tokenize(vec[2], ',');
    for (size_t i = 0; i < channel.size(); i++) {
      if (!chaIsValid(channel[i])){
        if (!key.empty() && i < key.size())
          key.erase(key.begin() + i);
        channel.erase(channel.begin() + i);
        Sock.SendData(user.userFD, ERR_BADCHANNAME);
        i--;
      }
    }
  }
  //join channel
  if (!channel.empty()) {
    for (size_t i = 0; i < channel.size(); i++) {
        std::string tmp("");
        if (i < key.size())
          tmp = key[i];
        joinChannel(user, channel[i], tmp);
    }
  }
}

void Parser::fnPMSG(vec_str& vec, userData& user) {
  if (vec.size() < 3){
    Sock.SendData(user.userFD, ERR_NEEDMOREPARAMS(user.nickName, user.recvString));
    return;
  }
  if (vec.size() >= 3)
    privMsg(vec[1], vec[2], user.nickName);
}

/*
  kick multi
  topic only user ask
*/

//KICK #a,#a,#a bob,bob,bob : reason
void Parser::fnKICK(vec_str& vec, userData& user) {
  vec_str channel;
  vec_str usr;
  std::string reason("");
  //error verification
  if (vec.size() < 3) {
    Sock.SendData(user.userFD, ERR_NEEDMOREPARAMS(user.nickName, user.recvString));
    return;
  }
  if (user.recvString.find(':') != std::string::npos) {
      Sock.SendData(user.userFD, "479 :invalid character");
      return;
  }
  if (vec.size() >= 4)
    reason = vec[3]; //KickUserChannel(user, vec[1], vec[2], reason);
  //channel and key logical
  if (vec.size() >= 2 && !vec[1].empty()) {
    channel = Tokenize(vec[1], ',');
    if (vec.size() >= 3 && !vec[2].empty())
      usr = Tokenize(vec[2], ',');
    for (size_t i = 0; i < channel.size(); i++) {
      if (!chaIsValid(channel[i])){
        if (!usr.empty() && i < usr.size())
          usr.erase(usr.begin() + i);
        channel.erase(channel.begin() + i);
        Sock.SendData(user.userFD, ERR_BADCHANNAME);
        i--;
      }
    }
  }
  //join channel
  if (!channel.empty()){
    for (size_t i = 0; i < channel.size(); i++) {
        std::string tmp("");
        if (i < usr.size())
          KickUserChannel(user, channel[i], usr[i], reason);
    }
  }
}

//PART #channel 
//PART #channel :reason
void Parser::fnPART(vec_str& vec, userData& user) {
  vec_str channel;

  if (vec.size() < 2) {
    Sock.SendData(user.userFD, ERR_NEEDMOREPARAMS(user.nickName, user.recvString));
    return;
  }
  if (vec.size() >= 2 && !vec[1].empty())
    channel = Tokenize(vec[1], ',');
  if (channel.size() > 0){
    for (size_t i = 0; i < channel.size(); i++) {
      if (!chaIsValid(channel[i])){
        channel.erase(channel.begin() + i);
        Sock.SendData(user.userFD, ERR_BADCHANNAME);
        i--;
      }
    }
  }
  if (!channel.empty()){
    for (size_t i = 0; i < channel.size(); i++) {
      std::string tmp = "";
      if (vec.size() == 3)
        tmp = vec[2];
      userPart(channel[i], user.userName, tmp);
    }
  }
}

//QUIT *reason
void Parser::fnQUIT(vec_str& vec, userData& user) {
  if (vec.size() == 1) {
    Sock.SendData(user.userFD, ":" + user.nickName + " QUIT");
  }
  else if (vec.size() >= 2) {
    Sock.SendData(user.userFD, ":" + user.nickName + " QUIT " + vec[1]);
  }
  KickUserAllChannel(user, vec[1]);
}

//MODE #CHANNEL +/-ITKOL
//MODE #CHANNEL +/-ITKOL param param param
void Parser::fnMODE(vec_str& vec, userData& user) {
  bool mode = false;
  if (vec.size() < 2) {
    Sock.SendData(user.userFD, ERR_NEEDMOREPARAMS(user.nickName, user.recvString));
    return;
  }else if(vec.size() == 2) {
    Mode(user, vec[1]);
    return;
  }
  else if (vec.size() < 3 && vec[2].find_first_of("kol") != std::string::npos){
    Sock.SendData(user.userFD, ERR_NEEDMOREPARAMS(user.nickName, user.recvString));
    return;
  }
  if (vec[2][0] == '+') {
    mode = true;
  }else if (vec[2][0] != '-') {
    Sock.SendData(user.userFD, ERR_UMODEUNKNOWNFLAG(user.nickName));
    return;
  }
  if (strDoubleChar(vec[2])) {
    Sock.SendData(user.userFD, ERR_UMODEUNKNOWNFLAG(user.nickName));
    return;
  }
  size_t count = 3;
  for (size_t i = 1; i < vec[2].length(); i++) {
    if (vec[2][i] == 'k' || vec[2][i] == 'o' || vec[2][i] == 'l')
      count++;
  }
  if (vec.size() < count) {
    Sock.SendData(user.userFD, ERR_NEEDMOREPARAMS(user.nickName, vec[1]));
    return;
  }
  size_t j = 3;
  for (size_t i = 1; i < vec[2].length(); i++) {
    switch (vec[2][i]) {
      case 'i':
        ModeI(user, vec[1], mode);
        break;
      case 't':
        ModeT(user, vec[1], !mode);
        break;
      case 'k':
        if (mode == true && vec.size() > j && !vec[j].empty())
          ModeK(user, vec[1], vec[j]);
        else
          ModeK(user, vec[1], "");
        j++;
        break;
      case 'o':
        if (vec.size() > j && !vec[j].empty())
          ModeO(user, vec[1], vec[j], mode);
        else 
          Sock.SendData(user.userFD, ERR_NEEDMOREPARAMS(user.nickName, user.nickName + " " + vec[1]));
        j++;
        break;
      case 'l':
        if (mode == true && vec.size() > j && !vec[j].empty()) {
          try {
            ModeL(user, vec[1], stoi(vec[j]));
          } catch (const std::exception& e) {
            Sock.SendData(user.userFD, ":invalide number");
          }
        }
        else
          ModeL(user, vec[1], -1);
        j++;
        break;
      default:
        Sock.SendData(user.userFD, ERR_NEEDMOREPARAMS(user.nickName, user.recvString));
        break;
    }
  }
}

//topic #channel :topic
void Parser::fnTOPIC(vec_str& vec, userData& user) {
  if (vec.size() < 2){
    Sock.SendData(user.userFD, ERR_NEEDMOREPARAMS(user.nickName, user.recvString));
    return;
  }
  else if (vec.size() == 2)
    _sendTopicTo(vec[1]);
  else if (isValidStr(vec[2], "", true)) {
    setTopic(user, vec[1], vec[2]);
  } 
}

//INVITE nick #channel
void Parser::fnINVITE(vec_str& vec, userData& user) {
  if (vec.size() < 3) {
    Sock.SendData(user.userFD, ERR_NEEDMOREPARAMS(user.nickName, user.recvString));
    return;
  }
  else
    userInvite(user, vec[1], vec[2]);
}

/// ####################################################################################################################

void    Parser::ParseData(userData& user, vectorIT& index) {
    _channels = Sock.channels;
    _index = &index;
    vec_str token = Tokenize(user.recvString, ' ');
    if (token.empty()) {
      return ;
    }
    // if(token[0] != "PING")
    //   print_vec(token, "token");
    if (token[0] == "PING" && LV(user.currentAction, e_userRegistred)) {if (token.size() >= 2) Sock.SendData(user.userFD, "PONG " + token[1]); }
    else if (token[0] == "PASS" && LV(user.currentAction, e_notRegistred)) { fnPASS(token, user); }
    else if (user.currentAction == 0) { kickUser(index, ERR_PASSWDMISMATCH(user.nickName), user); } //!if user send shit witout giving a valid password
    else if (token[0] == "NICK" && LV(user.currentAction, e_setNickname))    { fnNICK(token, user); }
    else if (token[0] == "USER" && LV(user.currentAction, e_setUser)) { fnUSER(token, user); }
    else if (token[0] == "PRIVMSG" && LV(user.currentAction, e_userRegistred)) { fnPMSG(token, user);}
    else if (token[0] == "JOIN" && LV(user.currentAction, e_userRegistred))    { fnJOIN(token, user); }
    else if (token[0] == "KICK" && LV(user.currentAction, e_userRegistred))    { fnKICK(token, user); }
    else if (token[0] == "MODE" && LV(user.currentAction, e_userRegistred))    { fnMODE(token, user); }
    else if (token[0] == "PART" && LV(user.currentAction, e_userRegistred))    { fnPART(token, user); }
    else if (token[0] == "QUIT" && LV(user.currentAction, e_userRegistred))    { fnQUIT(token, user); }
    else if (token[0] == "TOPIC" && LV(user.currentAction, e_userRegistred))   { fnTOPIC(token, user); }
    else if (token[0] == "INVITE" && LV(user.currentAction, e_userRegistred))  { fnINVITE(token, user); }
    // std::cout << "Received: " + user.recvString;    // Data Received
}
