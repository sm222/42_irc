#include "Channels.h"
#include "socket.h"


Channels::Channels() {}

// +++ Public +++


std::vector<std::string>    Channels::User_GetAllChannels(const std::string& userName) {
    std::vector<std::string> T;

    // Iterate all the Channels
    for (ChannelGroup::iterator i = _channelGroup.begin(); i != _channelGroup.end(); i++) {
        std::string     channelName = i->first;
        UsersMap&       currentChannel = i->second.second; // Skip to the userlist right away

        // Try to find Player, if exist, Pushback
        UsersMap::iterator j = currentChannel.find(userName);
        if (j != currentChannel.end()) {
            T.push_back(channelName);
        }
    }
    return T;
}


std::string                     Channels::Channel_Get_Password(const std::string& channelname) {
    ChannelMap* Chan = _getChannelByName(channelname);
    if (!Chan) return "";

    return Chan->first.Password;
}
void                            Channels::Channel_Set_Password(const std::string& channelname, const std::string& password) {
    ChannelMap* Chan = _getChannelByName(channelname);
    if (!Chan) return;
    Chan->first.Password = password;
}

int                             Channels::Channel_Get_MaxUsersCount(const std::string& channelname) {
    // -1 = No Limit
    // -2 = Doesn't exist

    ChannelMap* Chan = _getChannelByName(channelname);
    if (!Chan) return -2;
    return Chan->first.MaxUserCount;
}
void                            Channels::Channel_Set_MaxUsersCount(const std::string& channelname, const int maxCount) {
    ChannelMap* Chan = _getChannelByName(channelname);
    if (!Chan) return ;

    // Basic Protection
    if (maxCount < -1)  return;

    Chan->first.MaxUserCount = maxCount;
}

bool                            Channels::Channel_Get_IsUserInChannel(const std::string& userName, const std::string& channelName) {
    // True - Is in Channel
    // False - Not in channel

    std::vector<std::string> T = Channel_Get_AllUsers(channelName);
    for (unsigned int i = 0; i < T.size(); i++) {
        if (T[i] == userName)
            return true;
    }
    return false;
}
bool                            Channels::Channel_Get_IsUserChannelOP(const std::string& userName, const std::string& channelName) {
    // Return false if Channel doesnt exist
    ChannelMap* Chan = _getChannelByName(channelName);
    if (!Chan) {
      return false;
    }

    // Loop all the users
    for (UsersMap::iterator i = Chan->second.begin(); i != Chan->second.end(); i++) {
        // If user is OP && Is the right username
        if (i->second.in_Operator_List && userName == i->first) {
            return true;
        }
    }
    return false;
}

bool                            Channels::Channel_AlreadyExist(const std::string& channelname) {
    for (ChannelGroup::iterator i = _channelGroup.begin(); i != _channelGroup.end(); i++) {
        if (i->first == channelname)
            return true;
    }
    return false;
}

bool                            Channels::Channel_Get_CanUserChangeTopic(const std::string& channelname) {
    // Return False if channel doesnt exist
    ChannelMap* T =_getChannelByName(channelname);
    if (T) { return T->first.CanUserChangeTopic; }
    return false;
}

bool                            Channels::Channel_Remove_Operator(const std::string& user, const std::string& channelName) {
    ChannelMap* T = _getChannelByName(channelName);
    if (T) {
        for (UsersMap::iterator i = T->second.begin(); i != T->second.end(); i++) {
            if (user == i->first) {
                i->second = _createNewUserStats(true, false, false);
                return true;
            }
        }
    }
    return false;
}

int                             Channels::Channel_Get_CurrentUsersCount(const std::string& channelname) {
    ChannelMap* T =_getChannelByName(channelname);
    if (T) { return T->second.size(); }
    return 0;
}

bool                            Channels::Channel_Get_InviteOnly(const std::string& channelname) {
    // Return False if channel doesnt exist
    ChannelMap* T =_getChannelByName(channelname);
    if (T) { return T->first.InviteOnly; }
    return false;
}
void                            Channels::Channel_Set_CanUserChangeTopic(const std::string& channelname, const bool value) {
    ChannelMap* T =_getChannelByName(channelname);
    if (T) { T->first.CanUserChangeTopic = value; }
}
void                            Channels::Channel_Set_InviteOnly(const std::string& channelname, const bool value) {
    ChannelMap* T =_getChannelByName(channelname);
    if (T) { T->first.InviteOnly = value; }
}
std::string                     Channels::Channel_Get_Topic(const std::string& channelName) {
    // Return [NULL] if the channel doesnt exist
    ChannelMap* x = _getChannelByName(channelName);
    if (!x) return "-1";
    return x->first.Topic;
}

void                            Channels::Channel_Set_Topic(const std::string& channelName, const std::string& topic) {
    ChannelMap* T = _getChannelByName(channelName);
    if (T) {
        T->first.Topic = topic;
    }
}
bool                            Channels::Channel_Create(const std::string& channelMaker, const std::string& channelName) {
    // True     = Success
    // False    = Channel Already exist

    if (_getChannelByName(channelName)) return false;
    ChannelMap& T = _channelGroup[channelName];

    // Default Default for new Channel
    T.first.Topic = "";
    T.first.Password = "";
    T.first.MaxUserCount = -1;
    T.first.CanUserChangeTopic = true;
    T.first.InviteOnly = false;

    T.second[channelMaker] = _createNewUserStats(false, false, true);
    return true;
}
bool                            Channels::Channel_Join(const std::string& userName, const std::string& channelName) {
    // true = Success
    // false = Fail

    ChannelMap *T = _getChannelByName(channelName);

     // Channel Dont exist (False)
    if (!T) return false;                                                          

    // Channel on Invite Only && User NOT invited (False)
    if (T->first.InviteOnly && !_isUserInvited(T, userName)) { return false; }

    // Already in Channel (False)
    UsersMap::iterator i = T->second.find(userName);
    if (i != T->second.end()) return false;

    // Add to Channel
    T->second[userName] = _createNewUserStats(true, false, false);
    return true;

}
std::vector<std::string>        Channels::Channel_Get_AllUsers(const std::string& channelName) {
    // Return empty vector on error
    // only Users and Operators are considered users ( not Invited )

    std::vector<std::string> T;
    
    ChannelMap* Chan = _getChannelByName(channelName);
    if (!Chan) return T;
    for (UsersMap::iterator i = Chan->second.begin(); i != Chan->second.end(); i++) {
        if (i->second.in_User_list || i->second.in_Operator_List)
            T.push_back(i->first);
    }
    return T;
}
std::vector<std::string>        Channels::Channel_Get_AllOperators(const std::string& channelName) {
    std::vector<std::string> T;
    
    ChannelMap* Chan = _getChannelByName(channelName);
    if (!Chan) return T;

    for (UsersMap::iterator i = Chan->second.begin(); i != Chan->second.end(); i++) {
        if (i->second.in_Operator_List)
            T.push_back(i->first);
    }
    return T;
}
std::vector<ChannelAndTopic>    Channels::Channel_Get_AllChannelsAndTopicName() {
    std::vector<ChannelAndTopic> T;

    for (ChannelGroup::iterator Channel = _channelGroup.begin(); Channel != _channelGroup.end(); Channel++) {
        ChannelAndTopic A;
        A.ChannelName = Channel->first;
        A.TopicName = Channel->second.first.Topic;
        T.push_back(A);
    }
    return T;
}
bool                            Channels::Channel_Leave(const std::string& userName, const std::string& channelName) {
    ChannelMap* T = _getChannelByName(channelName);
    if (T) {
        for (UsersMap::iterator i = T->second.begin(); i != T->second.end(); i++) {
            if (userName == i->first) {
                T->second.erase(i);
                return true;
            }
        }
    }
    return false;
}
void                            Channels::Channel_Delete(const std::string& channelName) {
    ChannelGroup::iterator i = _channelGroup.find(channelName);
    if (i != _channelGroup.end()) { _channelGroup.erase(i); }
}

bool                            Channels::Channel_Set_Operator(const std::string& user, const std::string& channelName) {

    ChannelMap* T = _getChannelByName(channelName);
    if (T) {
        for (UsersMap::iterator i = T->second.begin(); i != T->second.end(); i++) {
            if (user == i->first) {
                i->second = _createNewUserStats(false, false, true);
                return true;
            }
        }
    }
    return false;

}
bool                            Channels::Channel_Invite(const std::string& invitedUser, const std::string& channelName) {
    ChannelMap* T = _getChannelByName(channelName);
    if (T) {
        for (UsersMap::iterator i = T->second.begin(); i != T->second.end(); i++) {
            if (invitedUser == i->first) {

                // Already in Channel -> Return false
                if (i->second.in_Operator_List || i->second.in_User_list) return false;

                i->second.in_Invited_List = true;
                return true;
            }
        }
    }
    return false;
}
bool                            Channels::Channel_Uninvite(const std::string& invitedUser, const std::string& channelName) {
    ChannelMap* T = _getChannelByName(channelName);
    if (T) {
        for (UsersMap::iterator i = T->second.begin(); i != T->second.end(); i++) {
            if (invitedUser == i->first) {
                i->second.in_Invited_List = false;
                return true;
            }
        }
    }
    return false;
}

void                            Channels::PrintChannelContent(const std::string& channelName) {
    ChannelMap* T = _getChannelByName(channelName);
    if (!T) {
        std::cout << channelName << " Does not exist ..." << std::endl;
        return;
    }

    else if (T->second.size() == 0) {
        std::cout << channelName << " has 0 Users..." << std::endl;
        return;
    }

    std::string choices[3];
    choices[0] = "User";
    choices[1] = "Invited";
    choices[2] = "Operator";
    std::string temp;

    std::cout << "\nUsers in " << channelName << ":" << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    for (UsersMap::iterator i = T->second.begin(); i != T->second.end(); i++) {

        if      (i->second.in_User_list)     temp = choices[0];
        else if (i->second.in_Invited_List)  temp = choices[1];
        else if (i->second.in_Operator_List) temp = choices[2];

        std::cout << "[" << i->first << "]          " << " Role: [" << temp << "]" << std::endl;
    }
    std::cout << "--------------------------------------" << std::endl;
}
void                            Channels::PrintALLChannelContent() {
    if (_channelGroup.size() == 0) {
        std::cout << "No Channel yet..." << std::endl;
        return;
    }

    std::string choices[3];
    choices[0] = "User";
    choices[1] = "Invited";
    choices[2] = "Operator";

    std::string temp;
    std::cout << "----------------------------------------------" << std::endl;
    for (ChannelGroup::iterator i = _channelGroup.begin(); i != _channelGroup.end(); i++) {

        std::cout << "--------------[ " << i->first << " ]--------------" << std::endl;
        for (UsersMap::iterator j = i->second.second.begin(); j != i->second.second.end(); j++) {

            if      (j->second.in_User_list)     temp = choices[0];
            else if (j->second.in_Invited_List)  temp = choices[1];
            else if (j->second.in_Operator_List) temp = choices[2];

            std::cout << "[" << j->first << "]          " << " Role: [" << temp << "]" << std::endl;
        }
    }
    std::cout << "----------------------------------------------" << std::endl;
}

// Socket uses this when user leave the server
void                            Channels::SOCKETONLY_kickuserfromallchannels(const std::string& userName) {
    // Iterate all the Channels
    for (ChannelGroup::iterator i = _channelGroup.begin(); i != _channelGroup.end(); i++) {
        std::string     channelName = i->first;
        UsersMap&       currentChannel = i->second.second; // Skip to the userlist right away

        // Try to find Player, if exist, Erase
        UsersMap::iterator j = currentChannel.find(userName);
        if (j != currentChannel.end()) {

            // Send Message to everyone except the kicked user
            if (currentChannel.size() > 1) {
                for (UsersMap::iterator each = currentChannel.begin(); each != currentChannel.end(); each++) {
                    if (each->first != userName && wtf()) {

                        userData* user = wtf()->GetUserByUsername(userName);
                        if (user) {
                            wtf()->SendData(user->userFD, ":" + userName + " PART " + channelName + " got hit by a car and died :( rip bozo ");
                        }
                    }
                }
            }

            // Remove user and delete channel if its now empty
            currentChannel.erase(j);
            if (currentChannel.size() == 0)
                _channelGroup.erase(i);
        }
    }
}

// +++ Private +++
bool                            Channels::_isUserInvited(ChannelMap* Channel, const std::string& user) {
    if (!Channel) return false;
    UsersMap::iterator i = Channel->second.find(user);
    if (i == Channel->second.end())
        return false;
    return true;
}
ChannelMap*                     Channels::_getChannelByName(const std::string& channelName) {
    ChannelGroup::iterator i = _channelGroup.find(channelName);
    if (i == _channelGroup.end()) return 0;
    return &i->second;
}
userState                       Channels::_createNewUserStats(const bool isUser, const bool isInv, const bool isOP) {
    userState   T;
    T.in_User_list        = isUser;
    T.in_Invited_List     = isInv;
    T.in_Operator_List    = isOP;
    return T;
}
