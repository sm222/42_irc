#include "parser.h"

Parser::Parser(Socket& socketClass) : Sock(socketClass) {}
Parser::~Parser(){}

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

//! shoud be modefy for bad arg in cmd
//! @param user ERR_UNKNOWNERROR (400) 
void Parser::badCmd(userData &user) {
  Sock.SendData(user.userFD, string("400 ") + ServerName " " + user.recvString + " :ERR_UNKNOWNERROR" );
}

void  Parser::unknowCommand(userData &user) {
  Sock.SendData(user.userFD, string("421 ") + ServerName " " + user.recvString + " :unknow cmd" );
}


void  Parser::allReadyRegistered(userData &user) {
  Sock.SendData(user.userFD, ServerName " :You may not reregister");
}


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

  //(void)user;
  //(void)vec;
  //Sock.

vec_str Parser::Tokenize(std::string message, char c){
  vec_str vec;
  size_t pos = 0;
  size_t old_pos = 0;
  size_t end = message.find_last_not_of("\n");

  if (end != std::string::npos)
    message.erase(end, 1);
  end = message.find_last_not_of("\r");
  if (end != string::npos)
    message.erase(end, 1);
  if (message.empty())
    return vec;
  while ((pos = message.find(c, old_pos)) != std::string::npos) {
    if (!isBetween(message, pos, '"')){
      std::string token = message.substr(old_pos, pos - old_pos);
      if (!token.empty())
        vec.push_back(token);
    }
    old_pos = pos + 1;
  }
  if (old_pos < message.length())
    vec.push_back(message.substr(old_pos));
  for (size_t i = 0; i < vec.size(); ++i)
      std::cout << "{" << i << "}" << vec[i] << "|" << std::endl;
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

void    Parser::ParseData(userData& user, vectorIT& index) {
    (void) index;
    Channels& AllChannels = Sock.channels;
    (void)AllChannels;
    vec_str split = Tokenize(user.recvString, ' ');

    if (split.empty()) {
      std::cout << "empty\n"; //! fix segfault
      return ;
    }

    std::cout << "user - " << user.userName << "currentAction\t\t\t\t\t\t\t>" << user.currentAction << std::endl;
    if (split[0] == "PASS" && LV(user.currentAction, e_notConfim)) {
      if (testPassWord(split[1], user, index))
        user.currentAction = 10;
    }
    else if (user.currentAction == 0) {
      kickUser(index, MSG_ErrSaslFail, user); //!if user send shit witout giving a valid password
    }
    //! user
    else if (split[0] == "USER" && LV(user.currentAction, e_notNameSet)) {
      if (setUserInfo(user, split))
        user.currentAction++;
        //!ERR_ALREADYREGISTERED (462)
    }
    //? PONG :)
    else if (split[0] == "PING" && LV(user.currentAction, e_ConfimUser)) {
      Sock.SendData(user.userFD, string("PONG ") + split[1]);
      std::cout << ORG << user.userName << RESET << " :PING, " << std::endl; //? dev can be remove
    }
    else if (split[0] == "JOIN") {
      joinChanel(user, split);
    }
    else if (split[0] == "KICK") {
      //KICK
    }
    else if (split[0] == "INVITE") {
      //KICK
    }
    else if (split[0] == "TOPIC") {
      //KICK
    }
    else if (split[0] == "MODE") {
      //KICK
    }
    else if (split[0] == "NICK") {
      user.nickName = split[1];
      Sock.SendData(user.userFD, string("NICK ") + split[1]);
    }
    //else
    //  unknowCommand(user);
    //dev messasge  *v*
    std::cout << "Received: " + user.recvString;    // Data Received
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
