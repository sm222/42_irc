#include "parser.h"

//*                       *//
//*         ERROR         *//
//*                       *//



//! don't use need to modefiy
//! don't use need to modefiy
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

//*                       *//
//*           CMD         *//
//*                       *//

//?  * // mode

//! not use working on it
bool  Parser::setUserMode(userData& user, int type) {
  (void)type;
  Sock.channels.Channel_Get_IsUserChannelOP(user.userName, "test");
  return true;
}



//?  * // topic

//TOPIC

bool  Parser::setTopic(const userData& user, const string& chanelName, const string& topic) {
  if (Sock.channels.Channel_AlreadyExist(chanelName)) {
    if (!Sock.channels.Channel_Get_CanUserChangeTopic(chanelName) && !Sock.channels.Channel_Get_IsUserChannelOP(user.userName, chanelName))
      return false;
    Sock.channels.Channel_Set_Topic(chanelName, topic);
    return true;
  }
  return false;
}

string  Parser::getTopic(const string& chanalName) {
  if (Sock.channels.Channel_AlreadyExist(chanalName))
    return Sock.channels.Channel_Get_Topic(chanalName);
  return ("");
}


//?  * // kick

void  Parser::kickUser(vectorIT& index, const string reasons, const userData &user) {
    Sock.SendData(user.userFD, reasons);
    Sock.KickUser(index);
}

/// @brief not final but work for now
/// @param user 
/// @param vec 
/// @return 
bool Parser::setUserInfo(userData& user, vec_str vec) {
  //if (user.currentAction > e_notNameSet) {
  //  allReadyRegistered(user);
  //  return false;
  //}
  user.userName = vec[1];
  Sock.SendData(user.userFD, makeMessage(e_welcom, "Welcome to the 42irc %u", user));
  //std::cout << "name = " << user.userName << " ,nick name = " << user.nickName << std::endl;
  return true;
}

short     Parser::_tryJoinChannel(const userData& user, const string name, const string pass) {
  const string& _pass = Sock.channels.Channel_Get_Password(name);
  if (!_pass.empty() && _pass != pass) {
    Sock.SendData(user.userFD, makeMessage(e_passmismatch, ":Password incorrect", user));
    return false;
  }
  bool bozo = Sock.channels.Channel_Join(user.userName, name);
  std::cout << "print bozo " << bozo << std::endl; 
  if (!bozo)
    return false;
  return true;
}

bool    Parser::joinChannel(const userData& user, const string& name, const string& pass) {
  std::cout << RED "|" RESET << "name:" << name << "pass:" << pass << std::endl; 
  Channels&  chanRef = Sock.channels;
  if (!chanRef.Channel_AlreadyExist(name)) {
    if (!chanRef.Channel_Create(user.userName, name)) {
      //! can't make chanel
      return false;
    }
    chanRef.Channel_Set_Password(name, pass);
  }
  else if (!_tryJoinChannel(user, name, pass)) {
    return false;
  }
  Sock.SendData(user.userName, makeMessage(e_none, string(":%n JOIN ") + name, user));
  // sucsess!
  return true;
}


bool Parser::testPassWord(string &pass, userData &user, vectorIT& index) {
  if (user.currentAction > e_notConfim) {
    allReadyRegistered(user);
    return false;
  }
  else if (pass == Sock.GetPassword()){
    std::cout << GRN << "Valid password" << RESET << std::endl;
    return true;
  }
  kickUser(index, MSG_PassMisMatch, user);
  std::cout << RED <<  "Bad password" << RESET << std::endl;
  return false;
}
