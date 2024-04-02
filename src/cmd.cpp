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

short     Parser::_tryJoinChannel(const userData& user, const string name, const string pass) {
  const string& _pass = Sock.channels.Channel_Get_Password(name);
  if (!_pass.empty() && _pass != pass) {
    Sock.SendData(user.userFD, makeMessage(e_passmismatch, ":channel password incorrect", user));
    return false;
  }
  bool bozo = Sock.channels.Channel_Join(user.userName, name);
  std::cout << "print bozo " << bozo << std::endl; 
  if (!bozo)
    return false;
  return true;
}

string   Parser::_SendUserChannelStatus(const vec_str& userList, const string& name) {
  string msg;
  for (size_t i = 0; i < userList.size(); i++) {
    const userData* tmpUser = Sock.GetUserByUsername(userList[i]);
    if (Sock.channels.Channel_Get_IsUserChannelOP(userList[i], name)) {
      msg += "@" + tmpUser->nickName + " ";
    }
    else {
      msg += tmpUser->nickName + " ";
    }
  }
  return msg;
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
  Sock.SendData(user.userFD, string("332 ") + user.nickName + " :boze");
  const vec_str& userList = Sock.channels.Channel_Get_AllUsers(name);
  for (size_t i = 0; i < userList.size(); i++) {
    const userData* tmpUser = Sock.GetUserByUsername(userList[i]);
    string  msg = "353 " + tmpUser->nickName + " = " + name + " :";
    msg += _SendUserChannelStatus(userList, name);
    Sock.SendData(userList[i], msg);
    Sock.SendData(userList[i], string("366 ") + tmpUser->nickName + " " + name + " :End of /NAMES list");
  }
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

//?  * // PRIVMSG
//PRIVMSG #a :awd


bool  Parser::privMsg(const string target, const string message, const string nick) {
  //* send mesagge to one user if no channel find
  if (!Sock.channels.Channel_AlreadyExist(target)) {
      if (Sock.doesThisNicknameExist(target)) {
        const userData *tmpUser = Sock.GetUserByNickname(target);
        Sock.SendData(tmpUser->userFD, string(":") + nick + " PRIVMSG " + target + " " + message);
        return true;
      }
    return false;
  }
  //* send message to all user
  const vec_str userList =  Sock.channels.Channel_Get_AllUsers(target);
  for (size_t i = 0; i < userList.size(); i++) {
    const userData* tmpUser = Sock.GetUserByUsername(userList[i]);
    string msg = ":" + nick + " PRIVMSG " + target + " " + message;
    if (tmpUser->nickName != nick)
      Sock.SendData(tmpUser->userFD, msg);
  }
  return true;
}
