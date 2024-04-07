#include "parser.h"
#include "_header.h"
#include <cctype>
#include <string>
#include <vector>
#include <xlocale/_stdio.h>


//FUNCTION

/// @brief check if str isalpha/isdigit and dont start with a digit
/// @param str a string 
/// @param other a string to add chars in the filter
/// @return bool
bool isValidStr(std::string str, std::string other){
  if (isdigit(str[0]))
    return false;
  for (size_t i = 0; i < str.length(); i++) {
    if (!isalnum(str[i]) && other.find(str[i]) == std::string::npos)
      return false;
  }
  return true;
}

int charCount(std::string str, char c){
  return std::count(str.begin(), str.end(), c);
}

void print_vec(vec_str vec, std::string name){
  if (vec.empty())
    return;
  std::cout << RED "|" RESET << name;
  for (size_t i = 0; i < vec.size(); ++i)
      std::cout <<  "[" << i << "]" YEL << vec[i] << RESET "|";
  std::cout << std::endl;
}

std::string supchar(const std::string &str, char c){
	std::string tmp = "";
  if (str.empty())
    return tmp;
	for (size_t i = 0; i < str.length(); i++)
		if (str[i] != c)
			tmp += str[i];
	return tmp;
}

//CLASS

Parser::Parser(Socket& socketClass) : Sock(socketClass), _channels(Sock.channels) {
}
Parser::~Parser(){}

/// @brief use to build a message to send
/// @param type 
/// @param msg %u = user.userName, %n = user.nickName, %i
/// @param user 
/// @return 
string  Parser::makeMessage(t_code const type, const string msg, const userData& user) {
    string result;
    if (type > -1) {
      result = MType[type];
      result += " " + user.nickName + " ";
    }
    else
      result = "";
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
    std::cout << RED "|" << CYN  << result << RESET << "\n";
    return (result);
}

vec_str Parser::Tokenize(std::string message, char c){
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

// void test(vec_str& vec, userData& user){
//   std::string msg = user.recvString.substr(user.recvString.find(':', 0));
//   int index;
//   for (int i = 0; i < vec.size(); i++) {
//     if (vec[i].find(':', 0) != std::string::npos){
//       index = i;
//       break;
//     }
//   }
//   vec_str tmp;
//   for (int i = 0; i <= index; i++) {
//     if (i == index)
//       tmp.push_back(msg);
//     else
//       tmp.push_back(vec[i]);
//   }
//   vec.clear();
//   vec = tmp;
// }

/*
* - asuming the currentAction = 0 at start, step 0 is to confim the password
*
*                                                *
*  password - 0                                  *
*            |                                   *
*            get user info - 1                   *
*                          |                     *
*                          ? next step 2         *
*                                                *
*                                                *
*                                                *
*                                                *
*                                                *
*/
bool Parser::chaIsValid(std::string str){
   if (str.length() == 1 || !isValidStr(str, "#&") || (str[0] != '#' && str[0] != '&')){
    return false;
   }
   return true;
}

bool Parser::keyIsValid(std::string str){
   return isValidStr(str, "!@$%?&*");
}

void Parser::fnPASS(vec_str& vec, userData& user, vectorIT& index){
  std::cout << RED "|fnPASS" RESET << std::endl;
  if (vec.size() < 2){
    Sock.SendData(user.userFD, ERR_NEEDMOREPARAMS(user.recvString)); 
    return;
  }
  if (testPassWord(vec[1], user, index))
    user.currentAction = 10;
}

void Parser::fnUSER(vec_str& vec, userData& user, vectorIT& index){
  std::cout << RED "|fnUSER" RESET << std::endl;
  if (vec.size() < 2){
    Sock.SendData(user.userFD, ERR_NEEDMOREPARAMS(user.recvString)); 
    return;
  }
  if (isValidStr(vec[1], "-_")){
    if (!Sock.GetUserByUsername(vec[1])){
      user.userName = vec[1];
      user.currentAction++;
      Sock.SendData(user.userFD, makeMessage(e_welcom, "Welcome to the 42irc %u", user));
      return;
    }
    kickUser(index, "904 " ServerName " :user alrady use", user);
    return;
  }
  kickUser(index, "904 " ServerName " :invalide character for user", user);
  // else
    // !ERR_ALREADYREGISTERED (462)
}

void Parser::fnNICK(vec_str& vec, userData& user, vectorIT& index){
  std::cout << RED "|fnNICK" RESET << std::endl;
  if (vec.size() < 2){
    Sock.SendData(user.userFD, ERR_NEEDMOREPARAMS(user.recvString)); 
    return;
  }
  if (!user.nickName.empty()){
    if (!Sock.doesThisNicknameExist(vec[1])){
      user.nickName = vec[1];
      //send all user new nick name
    }
    else{
      kickUser(index, ERR_NICKNAMEINUSE(vec[1]), user);
    }
    return;
  }
  if (isValidStr(vec[1], "-_")){
    if (!Sock.GetUserByNickname(vec[1])){
      user.nickName = vec[1];
      return;
    }
    kickUser(index, ERR_NICKNAMEINUSE(vec[1]), user);
    return;
  }
  else
    kickUser(index, ERR_ERRONEUSNICKNAME(vec[1]), user);
}

void Parser::fnJOIN(vec_str& vec, userData& user){
  std::cout << RED "|fnJOIN" RESET << std::endl;
  vec_str channel;
  vec_str key;

  //error verification
  if (vec.size() < 2){
    Sock.SendData(user.userFD, ERR_NEEDMOREPARAMS(user.recvString));
    return;
  }
  if (user.recvString.find(':') != std::string::npos){
      Sock.SendData(user.userFD, "479 :invalid character");
      return;
  }

  //channel and key logical
  if (vec.size() >= 2 && !vec[1].empty()){
    channel = Tokenize(vec[1], ',');
    if (vec.size() >= 3 && !vec[2].empty())
      key = Tokenize(vec[2], ',');
    for (size_t i = 0; i < channel.size(); i++) {
        if (channel[i].length() == 1 || !isValidStr(channel[i], "#&") || (channel[i][0] != '#' && channel[i][0] != '&')){
          if (!key.empty() && i < key.size())
            key.erase(key.begin() + i);
          channel.erase(channel.begin() + i);
          i--;
        }else
          Sock.SendData(user.userFD, "479 :invalid character");
    }
  }

  //join channel
  if (!channel.empty()){
    for (size_t i = 0; i < channel.size(); i++) {
        std::string tmp("");
        if (i < key.size())
          tmp = key[i];
        joinChannel(user, channel[i], tmp);
    }
  }
}

void Parser::fnPMSG(vec_str& vec, userData& user){
  std::cout << RED "|fnPMSG" RESET << std::endl;
  if (vec.size() < 3){
    Sock.SendData(user.userFD, ERR_NEEDMOREPARAMS(user.recvString)); 
    return;
  }
  if (vec.size() == 3)
    privMsg(vec[1], vec[2], user.nickName);
}

//KICK #a bob : reason
void Parser::fnKICK(vec_str& vec, userData& user){
  std::cout << RED "|fnKICK" RESET << std::endl;
  if (vec.size() < 3){
    Sock.SendData(user.userFD, ERR_NEEDMOREPARAMS(user.recvString)); 
    return;
  }
  if (vec.size() == 4)
    KickUserChannel(user, vec[1], vec[2], "");
  else if (vec.size() == 4)
    KickUserChannel(user, vec[1], vec[2], vec[3]);
  else if (vec.size() == 3)
    KickUserChannel(user, vec[1], vec[2], "");
}

//PART #channel *reason
void Parser::fnPART(vec_str& vec, userData& user){
  vec_str channel;

  if (vec.size() < 2){
    Sock.SendData(user.userFD, ERR_NEEDMOREPARAMS(user.recvString));
    return;
  }
  if (vec.size() >= 2 && !vec[1].empty())
    channel = Tokenize(vec[1], ',');
  if (channel.size() > 0){
    for (size_t i = 0; i < channel.size(); i++) {
        if (!isValidStr(channel[i], "#&") || (channel[i][0] != '#' && channel[i][0] != '&') || channel[i].length() == 1){
          channel.erase(channel.begin() + i);
          i--;
        }else
          Sock.SendData(user.userFD, ERR_BADCHANNAME);
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
void Parser::fnQUIT(vec_str& vec, userData& user){
  if (vec.size() == 2){ 
    Sock.SendData(user.userFD, ":" + user.nickName + " QUIT");
    KickUserAllChannel(user, vec[1]);
  }else{
    Sock.SendData(user.userFD, ":" + user.nickName + " QUIT " + vec[1]);
    KickUserAllChannel(user, vec[1]);
  }
}

// void Parser::fnMODE(vec_str& vec, userData& user){
//   //MODE #CHANNEL +/-ITKOL
//   std::string flag = "";
//   if (vec.size() < 3){
//     Sock.SendData(user.userFD, ERR_NEEDMOREPARAMS(user.recvString)); 
//     return;
//   }
//   if (vec[2][0] == '+' || vec[2][0] == '-'){
//     flag += vec[2][0];
//   }else{
//     Sock.SendData(user.userFD, ERR_UMODEUNKNOWNFLAG);
//     return;
//   }
//   if (vec.size() < 3 && vec[2].find_first_of("tkol") != std::string::npos){
//     Sock.SendData(user.userFD, ERR_NEEDMOREPARAMS(user.recvString)); 
//     return;
//   }

//   for (size_t i = 1; i < vec[2].length(); i++) {
//     switch (vec[2][0]) {
//       case 'i':
//         //ModeI(user, flag, channel);
//         break;
//       case 't':
//         //ModeT(user, flag, );
//         break;
//       case 'k':
//         //ModeK(user, flag, );
//         break;
//       case 'o':
//         //ModeO(user, flag, );
//         break;
//       case 'l':
//         //ModeL(user, flag, );
//         break;
//     }
//   }
// }

/// ####################################################################################################################

void    Parser::ParseData(userData& user, vectorIT& index) {
    _channels = Sock.channels;
    _index = &index;
    vec_str token = Tokenize(user.recvString, ' ');
    if(token[0] != "PING")
      print_vec(token, "token");

    if (token.empty()) {
      std::cout << "empty\n"; //! fix segfault
      return ;
    }

    // std::cout << "user - " << user.userName << "currentAction\t\t\t\t\t\t\t>" << user.currentAction << std::endl;
    if (token[0] == "PASS" && LV(user.currentAction, e_notConfim)) {
      fnPASS(token, user, index);
    }
    else if (user.currentAction == 0) {
      kickUser(index, ERR_PASSWDMISMATCH, user); //!if user send shit witout giving a valid password
    }
    else if (token[0] == "USER" && LV(user.currentAction, e_notNameSet)) {
      fnUSER(token, user, index);
    }
    else if (token[0] == "PING" && LV(user.currentAction, e_ConfimUser)) {
      Sock.SendData(user.userFD, string("PONG ") + token[1]);
    }
    else if (token[0] == "JOIN") {
      fnJOIN(token, user);
    }
    else if (token[0] == "KICK") {
      fnKICK(token, user);
    }
    else if (token[0] == "INVITE") { //INVITE|[1]nick_name_0|[2]#b|
      
    }
    else if (token[0] == "TOPIC") {
    }
    else if (token[0] == "MODE") {
    }
    else if (token[0] == "NICK") {
      fnNICK(token, user, index);
    }
    else if (token[0] == "PRIVMSG") { //PRIVMSG #a :awd
      fnPMSG(token, user);
    }
    else if (token[0] == "PART") {
      fnPART(token, user);
    }
    else if (token[0] == "QUIT") {
      fnQUIT(token, user);
    }
    //else
    //  unknowCommand(user);
    //dev messasge  *v*

    // std::cout << "Received: " + user.recvString;    // Data Received
}




//** example v

   //user set user
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
    //user.userName = "userName";                   // Set username
    //user.nickName = "nickName";                   // Set nickname
    //user.userFD;                                  // Hold user FD
    //user.recvString;                              // Data received just now

    //Sock.KickUser(index);                           // Kick user
    //Sock.GetPassword();                             // To check if user entered correct password, return the password
    //Sock.SendData(user.userFD, "nice scam sir");    // You can easily send data with this
