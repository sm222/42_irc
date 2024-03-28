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
  Sock.SendData(user.userFD, makeMessage(e_welcom, "Welcome to the 42irc %n", user));
  //std::cout << "name = " << user.userName << " ,nick name = " << user.nickName << std::endl;
  return true;
}

//?  * // join

short     Parser::_tryJoinChanel(const userData& user, const string name, const string pass) {
  const string& _pass = Sock.channels.Channel_Get_Password(name);
  if (!_pass.empty() && _pass != pass) {
    //! bad PASSWORD
    return false;
  }
  if (!Sock.channels.Channel_Join(user.userName, name))
    return false;
  return true;
}

bool    Parser::joinChanel(const userData& user, const string& name, const string& pass) {
  Channels&  chanRef = Sock.channels;
  if (!chanRef.Channel_AlreadyExist(name)) {
    if (!chanRef.Channel_Create(user.userName, name)) {
      //! can't make chanel
      return false;
    }
  }
  if (!_tryJoinChanel(user, name, pass)) {
    return false;
  }
  return true;
}

//! not final, old one stop using pls
bool    Parser::joinChanel(const userData& user, const vec_str& vec) {
  size_t  list = 1;

  if (vec.size() > 1 && vec[list][0] == '#') {
    const char  *tmp = vec[list].c_str() + 1;
    if (Sock.channels.Channel_AlreadyExist(tmp)) {
      Sock.channels.Channel_Join(user.userName, tmp);
      Sock.SendData(user.userFD, makeMessage(e_none, string(":%n JOIN ") + vec[list], user));
      return false;
    }
    else {
      Sock.channels.Channel_Create(user.userName, tmp);
      Sock.channels.Channel_Join(user.userName, tmp);
      Sock.SendData(user.userFD, makeMessage(e_none, string(":%n JOIN ") + vec[list], user));
      //? :Olivier JOIN #a    
      //?  ^         ^   ^    
      //?  |         |   |    
      //?  |       CMD   |    
      //?  NAME          |    
      //?          CHANEL NANE
    }
  }
  return true;
}

//?  * // pass


bool Parser::testPassWord(string &pass, userData &user, vectorIT& index) {
  if (user.currentAction > e_notConfim) {
    allReadyRegistered(user);
    return false;
  }
  else if (pass == Sock.GetPassword() && user.currentAction == e_notConfim){
    std::cout << GRN << "Valid password" << RESET << std::endl;
    return true;
  }
  kickUser(index, MSG_PassMisMatch, user);
  std::cout << RED <<  "Bad password" << RESET << std::endl;
  return false;
}
