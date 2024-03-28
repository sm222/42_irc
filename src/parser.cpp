#include "parser.h"
#include "_header.h"
#include <cctype>


//FUNCTION

bool	isBetween(std::string str, size_t pos, char c)
{
	bool	between_c;

	between_c = false;
	if (pos == 0 || str.empty() || str.length() >= pos)
		return (false);
	for (size_t i = 0; i <= pos; i++) {
		if (str[i] == c && !between_c)
			between_c = !between_c;
	}
	if (between_c)
		return (true);
	return (false);
}

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

//CLASS

Parser::Parser(Socket& socketClass) : Sock(socketClass) {}
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
  size_t end = message.find_last_of("\n");

  if (end != std::string::npos)
    message.erase(end, 1);
  end = message.find_last_of("\r");
  if (end != std::string::npos)
    message.erase(end, 1);
  if (message.empty())
    return vec;
  while ((pos = message.find(c, old_pos)) != std::string::npos) {
      std::string token = message.substr(old_pos, pos - old_pos);
      if (!token.empty())
        vec.push_back(token);
    old_pos = pos + 1;
  }
  if (old_pos < message.length())
    vec.push_back(message.substr(old_pos));
  return (vec);
}

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

void Parser::fnPASS(vec_str vec, userData& user, vectorIT& index){
  std::cout << RED "|fnPASS" RESET << std::endl;
  if (testPassWord(vec[1], user, index))
    user.currentAction = 10;
}

void Parser::fnUSER(vec_str vec, userData& user){
  std::cout << RED "|fnUSER" RESET << std::endl;
  if (isValidStr(vec[1], "-_")){
    if (setUserInfo(user, vec))
        user.currentAction++;
  }
  // else
    // !ERR_ALREADYREGISTERED (462)
}

void Parser::fnNICK(vec_str vec, userData& user){
  std::cout << RED "|fnNICK" RESET << std::endl;
  if (isValidStr(vec[1], "-_")){
    user.nickName = vec[1];
    // Sock.SendData(user.userFD,"NICK " + vec[1]);
  }
  else{
    // makeMessage(e_errornickname, "Erroneus nickname", user);
    Sock.SendData(user.userFD, "432 PRIVMSG :Erroneus nickname"); //"<client> <nick> :Erroneus nickname"
  }
  //   // message nickname bad char
}

void Parser::fnJOIN(vec_str vec, userData& user){
  std::cout << RED "|fnJOIN" RESET << std::endl;
  vec_str channel;
  vec_str key;

  (void)user;
  if (vec.size() < 2)
    return;
  if (!vec[1].empty()){
    channel = Tokenize(vec[1], ',');
    if (!vec[2].empty())
      key = Tokenize(vec[2], ',');
    for (size_t i = 0; i < channel.size(); i++) {
        if (!isValidStr(channel[i], "#&") || (channel[i][0] != '#' && channel[i][0] != '&') || channel[i].length() == 1){
          if (!key.empty() && i < key.size())
            key.erase(key.begin() + i);
          channel.erase(channel.begin() + i);
          i--;
        }
    }
  }
  if (!channel.empty()){
    std::string tmp("");
    for (size_t i = 0; i < channel.size(); i++) {
        if (i < key.size())
          tmp = key[i];
        joinChannel(user, channel[i], tmp);
    }
  }
  print_vec(channel, "CHANNEL");
  print_vec(key, "KEY");
}

/// ####################################################################################################################

void    Parser::ParseData(userData& user, vectorIT& index) {
    (void) index;
    Channels& AllChannels = Sock.channels;
    (void)AllChannels;
    vec_str token = Tokenize(user.recvString, ' ');
    print_vec(token, "KEY");

    if (token.empty()) {
      std::cout << "empty\n"; //! fix segfault
      return ;
    }

    // std::cout << "user - " << user.userName << "currentAction\t\t\t\t\t\t\t>" << user.currentAction << std::endl;
    if (token[0] == "PASS" && LV(user.currentAction, e_notConfim)) {
      fnPASS(token, user, index);
    }
    else if (user.currentAction == 0) {
      kickUser(index, MSG_ErrSaslFail, user); //!if user send shit witout giving a valid password
    }
    //! user
    else if (token[0] == "USER" && LV(user.currentAction, e_notNameSet)) {
      fnUSER(token, user);
    }
    //? PONG :)
    else if (token[0] == "PING" && LV(user.currentAction, e_ConfimUser)) {
      Sock.SendData(user.userFD, string("PONG ") + token[1]);
      std::cout << ORG << user.userName << RESET << " :PING " << std::endl; //? dev can be remove
    }
    else if (token[0] == "JOIN") {
      fnJOIN(token, user);
    }
    else if (token[0] == "KICK") {
      //KICK
    }
    else if (token[0] == "INVITE") {
      //KICK
    }
    else if (token[0] == "TOPIC") {
      //KICK
    }
    else if (token[0] == "MODE") {
      //KICK
    }
    else if (token[0] == "NICK") {
      fnNICK(token, user);
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
