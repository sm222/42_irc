#include "parser.h"

//*                       *//
//*         ERROR         *//
//*                       *//



//! don't use need to modefiy
//! shoud be modefy for bad arg in cmd
//! @param user ERR_UNKNOWNERROR (400) 
void Parser::badCmd(userData& user) {
  Sock.SendData(user.userFD, string("400 ") + ServerName " " + user.recvString + " :ERR_UNKNOWNERROR" );
}

void  Parser::unknowCommand(userData& user) {
  Sock.SendData(user.userFD, string("421 ") + ServerName " " + user.recvString + " :unknow cmd" );
}


void  Parser::allReadyRegistered(userData& user) {
  Sock.SendData(user.userFD, ServerName " :You may not reregister");
}

void Parser::notInChannel(const userData& user, const string channel, const userData* ask) {
  const userData& msg = ask ? *ask : user;
  Sock.SendData(msg.userFD, ERR_USERNOTINCHANNEL(msg.nickName, channel));
  
}

//<client> :There was no such nickname"ERR_WASNOSUCHNICK (406) 
void Parser::noSuchNick(const userData& user, const string nick) {
  Sock.SendData(user.userFD, string("406 ") + user.nickName + " " + nick + " :There was no such nickname");
}

void    Parser::_sendChannel(const string message, const string channel, const bool user) {
  const vec_str& userList = _channels.Channel_Get_AllUsers(channel);
  for (size_t i = 0; i < userList.size(); i++) {
    const userData* tmpUser = Sock.GetUserByUsername(userList[i]);
    string s;
    if (tmpUser) {
      if (user)
        s = ":" + tmpUser->nickName + " ";
      Sock.SendData(tmpUser->userFD, s + message);
    }
  }
}

bool  Parser::_testInChannel(const userData& user, const string channelName, const userData* ask) {
  if (channelName.empty()) {
    // no name
    return false;
  }
  if (!_channels.Channel_AlreadyExist(channelName)) {
    Sock.SendData(ask ? ask->userFD : user.userFD, ERR_NOSUCHCHANNEL(channelName));
    return false;
  }
  if (!_channels.Channel_Get_IsUserInChannel(user.userName, channelName)) {
    notInChannel(user, channelName, ask);
    return false;
  }
  return true;
}

bool  Parser::_testOp(const userData& user, const string channelName) {
  if (!_testInChannel(user, channelName))
    return false;
  if (!_channels.Channel_Get_IsUserChannelOP(user.userName, channelName)) {
    Sock.SendData(user.userFD, ERR_CHANOPRIVSNEEDED(channelName));
    return false;
  }
  return true;
}

//*                       *//
//*           CMD         *//
//*                       *//

//?  * // topic

//TOPIC

  //332   "<client> <channel> :<topic>"
bool  Parser::_sendTopicTo(const string channel, const userData* user) {
  if (!_channels.Channel_AlreadyExist(channel))
    return false;
  if (user && _channels.Channel_Get_IsUserInChannel(user->userName, channel)) {
    Sock.SendData(user->userFD, string("332 ") + user->nickName + ' ' + channel + " " +  getTopic(channel));
    return true;
  }
  const vec_str& userList = _channels.Channel_Get_AllUsers(channel);
  for (size_t i = 0; i < userList.size(); i++) {
    const userData* tmpUser = Sock.GetUserByUsername(userList[i]);
    if (tmpUser) {
      Sock.SendData(tmpUser->userFD, string("332 ") + tmpUser->nickName + ' ' + channel + " " +  getTopic(channel));
    }
  }
  return true;
}

bool  Parser::setTopic(const userData& user, const string& channelName, const string& topic) {
  if (_channels.Channel_Get_CanUserChangeTopic(channelName)) {
    _channels.Channel_Set_Topic(channelName, topic);
    _sendTopicTo(channelName);
    return true;
  }
  else if (_testOp(user, channelName)) {
    _channels.Channel_Set_Topic(channelName, topic);
    _sendTopicTo(channelName);
    return true;
  }
  return false;
}

string  Parser::getTopic(const string& chanalName) {
  if (_channels.Channel_AlreadyExist(chanalName))
    return _channels.Channel_Get_Topic(chanalName);
  return ("");
}


//?  * // kick

void  Parser::kickUser(vectorIT& index, const string reasons, const userData &user) {
    Sock.SendData(user.userFD, reasons);
    Sock.KickUser(index);
}

short     Parser::_tryJoinChannel(const userData& user, const string channel, const string pass) {
  const string& _pass = _channels.Channel_Get_Password(channel);
  if (!_pass.empty() && _pass != pass) {
    Sock.SendData(user.userFD, ERR_PASSWDMISMATCH);
    return false;
  }
  if (!_channels.Channel_Join(user.userName, channel)) {
    if (_channels.Channel_Get_InviteOnly(channel))
      Sock.SendData(user.userFD, "473 " + user.nickName + " " + channel + " :Cannot join channel (+i)");
    return false;
  }
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

bool    Parser::joinChannel(const userData& user, const string& channel, const string& pass) {
  if (!_channels.Channel_AlreadyExist(channel)) {
    if (!_channels.Channel_Create(user.userName, channel)) {
      return false;    //! can't make chanel
    }
    _channels.Channel_Set_Password(channel, pass);
    setTopic(user, channel, string(":") + user.nickName + ", new channel");
  }
  else if (!_tryJoinChannel(user, channel, pass))
    return false;
  Sock.SendData(user.userFD, string(":") + user.nickName + " JOIN " + channel);
  _sendTopicTo(channel, &user);
  const vec_str& userList = Sock.channels.Channel_Get_AllUsers(channel);
  string  msg = "353 " + user.nickName + " = " + channel + " :";
  msg += _SendUserChannelStatus(userList, channel);
  Sock.SendData(user.userFD, msg);
  Sock.SendData(user.userFD, string("366 ") + user.nickName + " " + channel + " :End of /NAMES list");
  for (size_t i = 0; i < userList.size(); i++) {
    const userData* tmpUser = Sock.GetUserByUsername(userList[i]);
    if (user.userFD != tmpUser->userFD)
      Sock.SendData(tmpUser->userFD, string(":") + user.nickName + " JOIN " + channel);
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
  kickUser(index, ERR_PASSWDMISMATCH, user);
  std::cout << RED <<  "Bad password" << RESET << std::endl;
  return false;
}

//?  * // PRIVMSG
//PRIVMSG #a :awd



bool  Parser::privMsg(const string target, const string message, const string nick, bool self) {
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
  const userData*  sender = Sock.GetUserByNickname(nick);
  if (!_channels.Channel_Get_IsUserInChannel(sender->userName, target)) {
    //! user not in channel
    notInChannel(*sender, target);
    return false;
  }
  const vec_str userList =  Sock.channels.Channel_Get_AllUsers(target);
  for (size_t i = 0; i < userList.size(); i++) {
    const userData* tmpUser = Sock.GetUserByUsername(userList[i]);
    string msg = ":" + nick + " PRIVMSG " + target + " " + message;
    if ((tmpUser && tmpUser->nickName != nick) || self)
      Sock.SendData(tmpUser->userFD, msg);
  }
  return true;
}

//?  * // KICK


// Received: KICK #a bob :bozo lala

// :WiZ!jto@tolsun.oulu.fi KICK #Finnish John

bool  Parser::KickUserChannel(const userData &user, const string channel, const string nick, const string reson) {
  if (!_testOp(user, channel))
    return false;
  const userData* tmpUser = Sock.GetUserByNickname(nick);
  if (!tmpUser || !_testInChannel(*tmpUser, channel, &user)) {
    Sock.SendData(user.userFD, ERR_USERNOTINCHANNEL(user.nickName, channel));
    return false;
  }
  string msg = string(":") + user.nickName + " KICK " + channel + " " + tmpUser->nickName + " " + reson;
  const vec_str userList =  Sock.channels.Channel_Get_AllUsers(channel);
  for (size_t i = 0; i < userList.size(); i++) {
    const userData* usrMsg = Sock.GetUserByUsername(userList[i]);
    Sock.SendData(usrMsg->userFD, msg);
  }
  _channels.Channel_Leave(tmpUser->userName, channel);
  return true;
}


//?  * // PART

bool  Parser::userPart(const string channel , const string userName, const string reson) {
  const userData* leaver = Sock.GetUserByUsername(userName);
  if (_testInChannel(*leaver, channel)) {
    vec_str channelUser = _channels.Channel_Get_AllUsers(channel);
    for (size_t i = 0; i < channelUser.size(); i++) {
      const userData* tmpUser = Sock.GetUserByUsername(channelUser[i]);
      Sock.SendData(tmpUser->userFD, string(":") + leaver->nickName + " PART " + channel + " " + reson);
    }
    _channels.Channel_Leave(userName, channel);
    return true;
  }
  return false;
}

bool Parser::KickUserAllChannel(const userData& user, const string reson) {
  vec_str channelList = _channels.User_GetAllChannels(user.userName);
  if (channelList.size() == 0)
    return false;
  //*                                              //
  for (size_t i = 0; i < channelList.size(); i++) {
    userPart(channelList[i], user.userName, reson);
  }
  return true;
}


//?  * // invite

bool Parser::userInvite(userData& user, std::string nick, std::string channel){
  if (!_testOp(user, channel))
    return false;
  const userData* tmpUser = Sock.GetUserByNickname(nick);
  if (!tmpUser)
    return false;
  _channels.Channel_Invite(tmpUser->userName, channel);
  Sock.SendData(tmpUser->userFD, ":" + user.nickName + " INVITE " +  nick + " " +  channel);
  return true;
}



//?  * // MODE

/* 
*i  Définir/supprimer le canal sur invitation uniquement // ? --
*t  Définir/supprimer les restrictions de la commande TOPIC pour les opérateurs de canaux // ? --
*k  Définir/supprimer la clé du canal (mot de passe // !
*o  Donner/retirer le privilège de l’opérateur de cana // !
*l  Définir/supprimer la limite d’utilisateurs pour le canal
*/


bool    Parser::ModeI(const userData& user, const string channel, const bool mode) {
  if (!_testOp(user, channel))
    return false;
  _channels.Channel_Set_InviteOnly(channel, mode);
  return true;
}

bool    Parser::ModeT(const userData& user, const string channel, const bool mode) {
  if (!_testOp(user, channel))
    return false;
  _channels.Channel_Set_CanUserChangeTopic(channel, mode);
  return true;
}


bool    Parser::ModeK(const userData& user, const string channel, const string pass) {
  if (!_testOp(user, channel))
    return false;
  _channels.Channel_Set_Password(channel, pass);
  return true;
}

bool    Parser::ModeO(const userData& user, const string channel, const string nick, const bool mode) {
  if (!_testOp(user, channel))
    return false;
  const userData* tmpUser = Sock.GetUserByNickname(nick);
  if (!tmpUser) {
    Sock.SendData(user.userFD, ERR_NOLOGIN(nick));
    return false;
  }
  if (!_testInChannel(*tmpUser, channel, &user))
    return false;
  if (mode) {
    _channels.Channel_Set_Operator(tmpUser->userName, channel);
    _sendChannel(" MODE " + channel + " +o " + nick, channel, true);
  }
  else {
    _channels.Channel_Remove_Operator(tmpUser->userName, channel);
    _sendChannel(" MODE " + channel + " -o " + nick, channel, true);
  }
  return true;
}

bool   Parser::ModeL(const userData& user, const string channel, const int number) {
  if (!_testOp(user, channel))
    return false;
  _channels.Channel_Set_MaxUsersCount(channel, number);
  return true;
}


//join #a,#b,#c
//10.12.2.5
