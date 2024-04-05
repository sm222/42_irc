#include "parser.h"

//*                       *//
//*         ERROR         *//
//*                       *//



//! don't use need to modefiy
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

void Parser::notInChannel(const userData& user, const string channel) {
  Sock.SendData(user.userFD, string("441 " + user.nickName + " " + channel + " :They aren't on that channel"));
}

//<client> :There was no such nickname"ERR_WASNOSUCHNICK (406) 
void Parser::noSuchNick(const userData& user, const string nick) {
  Sock.SendData(user.userFD, string("406 ") + user.nickName + " " + nick + " :There was no such nickname");
}


bool  Parser::_testInChannel(const userData& user, const string channelName) {
  if (channelName.empty()) {
    // no name
    return false;
  }
  if (!_channels.Channel_AlreadyExist(channelName)) {
    Sock.SendData(user.userFD, ERR_NOSUCHCHANNEL(channelName));
    return false;
  }
  if (!_channels.Channel_Get_IsUserInChannel(user.userName, channelName)) {
    notInChannel(user, channelName);
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

//?  * // mode

//! not use working on it
bool  Parser::setUserMode(userData& user, int type) {
  (void)type;
  Sock.channels.Channel_Get_IsUserChannelOP(user.userName, "test");
  return true;
}



//?  * // topic

//TOPIC

bool  Parser::setTopic(const userData& user, const string& channelName, const string& topic) {
  if (_testOp(user, channelName)) {
    _channels.Channel_Set_Topic(channelName, topic);
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

short     Parser::_tryJoinChannel(const userData& user, const string name, const string pass) {
  const string& _pass = _channels.Channel_Get_Password(name);
  if (!_pass.empty() && _pass != pass) {
    Sock.SendData(user.userFD, makeMessage(e_passmismatch, ":channel password incorrect", user));
    return false;
  }
  if (!_channels.Channel_Join(user.userName, name))
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
  if (!_channels.Channel_AlreadyExist(name)) {
    if (!_channels.Channel_Create(user.userName, name)) {
      return false;    //! can't make chanel
    }
    _channels.Channel_Set_Password(name, pass);
  }
  else if (!_tryJoinChannel(user, name, pass)) {
    return false;
  }
  Sock.SendData(user.userName, makeMessage(e_none, string(":%n JOIN ") + name, user));
  Sock.SendData(user.userFD, string("332 ") + user.nickName + " :"+ getTopic(name));
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
    if (tmpUser->nickName != nick || self)
      Sock.SendData(tmpUser->userFD, msg);
  }
  return true;
}

//?  * // KICK


// Received: KICK #a bob :bozo lala

// :WiZ!jto@tolsun.oulu.fi KICK #Finnish John

bool  Parser::KickUserChannel(const userData &user, const string channel, const string nick, const string reson) {
  //if (_channels.Channel_AlreadyExist(channel)) {
  //  if (_channels.Channel_Get_IsUserInChannel(user.userName, channel) && 
  //      _channels.Channel_Get_IsUserChannelOP(user.userName, channel)) {
  //      const userData* tmpUser = Sock.GetUserByNickname(nick);
  //      if (_channels.Channel_Get_IsUserInChannel(tmpUser->userName, channel)) {
  //        string msg = string(":") + user.nickName + " KICK " + channel + " " + tmpUser->nickName + " " + reson;
  //        const vec_str userList =  Sock.channels.Channel_Get_AllUsers(channel);
  //        for (size_t i = 0; i < userList.size(); i++) {
  //          const userData* usrMsg = Sock.GetUserByUsername(userList[i]);
  //          Sock.SendData(usrMsg->userFD, msg);
  //        }
  //        _channels.Channel_Leave(tmpUser->userName, channel);
  //        return true;
  //      }
  //      else
  //        noSuchNick(user, nick);
  //  }
  //  else
  //    std::cout << "// is not in chanel or op" << std::endl;
  //}
  //else
  //  std::cout << "//chanel don't exist\n";
  //return false;
  if (!_testOp(user, channel))
    return false;
  const userData* tmpUser = Sock.GetUserByNickname(nick);
  if (!tmpUser || !_testInChannel(*tmpUser, channel)) //! need to fix + send messe to bad person
    return false;
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



//?  * // MODE

/* 
*i  Définir/supprimer le canal sur invitation uniquement
*t  Définir/supprimer les restrictions de la commande TOPIC pour les opérateurs de canaux
*k  Définir/supprimer la clé du canal (mot de passe
*o  Donner/retirer le privilège de l’opérateur de cana
*l  Définir/supprimer la limite d’utilisateurs pour le canal
*/



bool    Parser::ModeI(const userData& user, const string mode, const string targerNick, const string channel) {
  (void)user;
  (void)mode;
  (void)targerNick;
  (void)channel;
  if (!_channels.Channel_AlreadyExist(channel)) {
  }
  if (!Sock.doesThisNicknameExist(targerNick))
    std::cout << "no";
  return true;
}
