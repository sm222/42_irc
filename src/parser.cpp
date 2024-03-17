#include "parser.h"

/// @brief use to pritn the memorry from a int or char ptr
/// @param ptr adress
/// @param size width of the pointer (length of the array)
/// @param type  0 int , 1 char
/// @param name exp : 'A' 'B' 'C'
/// @details 00	'\0' 	(null character)
/// @details 1 	  SOH	(start of heading)
/// @details 2 	  STX	(start of text)
/// @details 3 	  ETX	(end of text)
/// @details 4 	  EOT	(end of transmission)
/// @details 5 	  ENQ	(enquiry)
/// @details 6 	  ACK	(acknowledge)
/// @details 7 	  BEL	'\a' (bell)
/// @details 8 	  BS 	'\b' (backspace)
/// @details 9 	  HT 	'\t' (horizontal tab)
/// @details 10	  LF 	'\n' (new line)
/// @details 11	  VT 	'\v' (vertical tab)
/// @details 12	  FF 	'\f' (form feed)
/// @details 13	  CR 	'\r' (carriage ret)
/// @details 14	  SO 	(shift out)
/// @details 15	  SI 	(shift in)
/// @details 16	  DLE	(data link escape)
/// @details 17	  DC1	(device control 1)
/// @details 18	  DC2	(device control 2)
/// @details 19	  DC3	(device control 3)
/// @details 20	  DC4	(device control 4)
/// @details 21	  NAK	(negative ack.)
/// @details 22	  SYN	(synchronous idle)
/// @details 23	  ETB	(end of trans. blk)
/// @details 24	  CAN	(cancel)
/// @details 25	  EM 	(end of medium)
/// @details 26	  SUB	(substitute)
/// @details 27	  ESC	(escape)
/// @details 28	  FS 	(file separator)
/// @details 29	  GS 	(group separator)
/// @details 30	  RS 	(record separator)
/// @details 31	  US 	(unit separator)
void	Ct_mprintf(void *ptr, size_t size, int type, int name)
{
	size_t	i;

	i = 0;
	if (!ptr)
		return ;
	printf("[%c]", name);
	if (type == 0)
	{
		while (i < size)
			printf("%12d | ", ((char *)ptr)[i++]);
	}
	else if (type == 1)
	{
		while (i < size)
		{
			if (((char *)ptr)[i] > 31 && ((char *)ptr)[i] < 127)
				printf("%3c | ", ((char *)ptr)[i]);
			else if (((char *)ptr)[i] == 0)
				printf(" \\0 | ");
			else if (((char *)ptr)[i] == 1)
				printf("SHO | ");
			else if (((char *)ptr)[i] == 2)
				printf("STX | ");
			else if (((char *)ptr)[i] == 3)
				printf("ETX | ");
			else if (((char *)ptr)[i] == 4)
				printf("EOT | ");
			else if (((char *)ptr)[i] == 5)
				printf("ENQ | ");
			else if (((char *)ptr)[i] == 6)
				printf("ACK | ");
			else if (((char *)ptr)[i] == 7)
				printf(" \\a | ");
			else if (((char *)ptr)[i] == 8)
				printf(" \\b | ");
			else if (((char *)ptr)[i] == 9)
				printf(" \\t | ");
			else if (((char *)ptr)[i] == 10)
				printf(" \\n | ");
			else if (((char *)ptr)[i] == 11)
				printf(" \\v | ");
			else if (((char *)ptr)[i] == 12)
				printf(" \\f | ");
			else if (((char *)ptr)[i] == 13)
				printf(" \\r | ");
			else if (((char *)ptr)[i] == 14)
				printf(" SO | ");
			else if (((char *)ptr)[i] == 15)
				printf(" SI | ");
			else if (((char *)ptr)[i] == 16)
				printf("DLE | ");
			else if (((char *)ptr)[i] == 17)
				printf("DC1 | ");
			else if (((char *)ptr)[i] == 18)
				printf("DC2 | ");
			else if (((char *)ptr)[i] == 19)
				printf("DC3 | ");
			else if (((char *)ptr)[i] == 20)
				printf("DC4 | ");
			else if (((char *)ptr)[i] == 21)
				printf("NAK | ");
			else if (((char *)ptr)[i] == 22)
				printf("SYN | ");
			else if (((char *)ptr)[i] == 23)
				printf("ETB | ");
			else if (((char *)ptr)[i] == 24)
				printf("CAN | ");
			else if (((char *)ptr)[i] == 25)
				printf(" EM | ");
			else if (((char *)ptr)[i] == 26)
				printf("SUB | ");
			else if (((char *)ptr)[i] == 27)
				printf("ESC | ");
			else if (((char *)ptr)[i] == 28)
				printf(" FS | ");
			else if (((char *)ptr)[i] == 29)
				printf(" GS | ");
			else if (((char *)ptr)[i] == 30)
				printf(" RS | ");
			else if (((char *)ptr)[i] == 31)
				printf(" US | ");
			else
				printf("%3d | ", ((char *)ptr)[i]);
			i++;
		}
	}
	printf("\n");
}


Parser::Parser(Socket& socketClass) : Sock(socketClass) {}
Parser::~Parser(){}

std::string  Parser::makeMessage(t_code const type, const std::string msg, const userData& user) {
    std::string result = MType[type];
    result += " " + user.userName + " " + msg;
    return (result);
}

void    Parser::ParseData(userData& user, vectorIT& index) {
    Channels& AllChannels = Sock.channels;
    AllChannels.CreateChannel("Some Looser", "BozoChannel");

    
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
  Sock.channels
  // shit way to get the user so weechar stop crying
  if (std::strncmp(user.recvString.c_str(), "USER ", 5) == 0) {
    size_t  i = 0;
    // USER paul 0 * Full_name
    //std::string::substr()
    while (5 + i < user.recvString.size() && user.recvString[5 + i] != ' ') {i++;}
    user.userName = user.recvString.substr(5, i);
    std::cout << user.userName << "<<" << std::endl;
    Ct_mprintf((char *)user.userName.c_str(), user.userName.size(), 1, 'A');
    std::string tmp = makeMessage(e_welcom, ":Welcome to the 42irc", user);
    std::cout << " > > " << tmp << std::endl;
    Sock.SendData(user.userFD, tmp);
  }
  // send a pong so the weechat don't stop the connection
  if (std::strncmp(user.recvString.c_str(), "PING ", 5) == 0) {
    std::string tmp = "PONG ";
    tmp += user.recvString.c_str() + 5;
    Sock.SendData(user.userFD, tmp);
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
