#include "Channels.h"

Channels::Channels() {}

// +++ Public +++
bool                            Channels::DoesChannelAlreadyExist(const std::string& channelname) {
    for (ChannelGroup::iterator i = _channelGroup.begin(); i != _channelGroup.end(); i++) {
        if (i->first == channelname)
            return true;
    }
    return false;
}
std::string                     Channels::GetChannelTopic(const std::string& channelName) {
    // Return [NULL] if the channel doesnt exist
    ChannelMap* x = _getChannelByName(channelName);
    if (!x) return "[NULL]";
    return x->first;
}
uint8_t                         Channels::SetChannelTopic(const std::string& userName, const std::string& channelName, const std::string& topic) {
    // 0 -> Success
    // 1 -> Channel don't exist
    // 2 -> User not in channel
    // 3 -> User not Operator

    try {
        UsersMap::iterator   userIT = _getUserInChannelByName(userName, channelName);
        if (!userIT->second.in_Operator_List) return 3;
        _channelGroup[channelName].first = topic;
    }
    catch (const int& e) {
        switch(e){
            case 0: return 1;
            case 1: return 2;
        }
    }
    return 0;
}
bool                            Channels::CreateChannel(const std::string& channelMaker, const std::string& channelName) {
    // True     = Success
    // False    = Channel Already exist

    if (_getChannelByName(channelName)) return false;
    ChannelMap& T = _channelGroup[channelName];
    T.first = "";
    T.second[channelMaker] = _createNewUserStats(false, false, true);
    return true;
}
uint8_t                         Channels::LeaveChannel(const std::string& userName, const std::string& channelName) {
    // 0 = Success
    // 1 = Channel doesnt exist
    // 2 = User not in channel
    // 3 = Only in invited list

    try {
        UsersMap::iterator   userIT = _getUserInChannelByName(userName, channelName);
        if (!userIT->second.in_Operator_List && !userIT->second.in_User_list) return 3;   // Only in invited
        _channelGroup[channelName].second.erase(userIT);
    }
    catch (const int& e) {
        switch (e) {
            case 0: return 1;   // Channel doesnt exist
            case 1: return 2;   // User not in channel
        }
    }

    return 0; // Success
}
uint8_t                         Channels::JoinChannel(const std::string& userName, const std::string& channelName) {
    // 0 -> Success
    // 1 -> Channel don't exist
    // 2 -> User not invited    --- User need invite to join
    // 3 -> Already in channel

    try {
        UsersMap::iterator T = _getUserInChannelByName(userName, channelName);
        if (T->second.in_User_list || T->second.in_Operator_List) return 3;
        T->second.in_Invited_List = false;  // Remove from Invited list
        T->second.in_User_list = true;      // Add to userlist
    }
    catch (const int& e) {
        switch (e) {
            case 0: return 1;               // Channel don't exist
            case 1: return 2;               // User not invited
        }
    }
    return 0;
}
std::vector<std::string>        Channels::GetAllUsersInChannel(const std::string& channelName) {
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
std::vector<std::string>        Channels::GetAllOPinChannel(const std::string& channelName) {
    std::vector<std::string> T;
    
    ChannelMap* Chan = _getChannelByName(channelName);
    if (!Chan) return T;

    for (UsersMap::iterator i = Chan->second.begin(); i != Chan->second.end(); i++) {
        if (i->second.in_Operator_List)
            T.push_back(i->first);
    }
    return T;
}
std::vector<ChannelAndTopic>    Channels::GetAllChannelsAndTopicName() {
    std::vector<ChannelAndTopic> T;

    for (ChannelGroup::iterator Channel = _channelGroup.begin(); Channel != _channelGroup.end(); Channel++) {
        ChannelAndTopic A;
        A.ChannelName = Channel->first;
        A.TopicName = Channel->second.first;
        T.push_back(A);
    }
    return T;
}
uint8_t                         Channels::KickUserFromChannel(const std::string& currentOP,const std::string& userToKick, const std::string& channelName) {
    // 0 -> Success
    // 1 -> Channel don't exist

    // --- userToKick ERRORS ---
    // 2 -> usertoKick not in channel
    // 3 -> userToKick in Invited list only
    // 4 -> userToKick in Operator

    // --- currentOP ERRORS ---
    // 5 -> currentOP is not Operator 
    // 6 -> currentOP is not in channel


    // currentOP Status
    try {
        UsersMap::iterator OP = _getUserInChannelByName(currentOP, channelName);
        if (!OP->second.in_Operator_List) return 5;
    }
    catch (const int& e) {
        switch (e) {
            case 0: return 1;
            case 1: return 6;
        }
    }

    // userToKick Status
    try {
        UsersMap::iterator _newOP = _getUserInChannelByName(userToKick, channelName);
        if (_newOP->second.in_Invited_List) return 3;
        if (_newOP->second.in_Operator_List) return 4;
        _channelGroup[channelName].second.erase(_newOP);   // Kicking the user here
    }
    catch (const int& e) {
        switch (e) {
            case 0: return 1;
            case 1: return 2;
        }
    }

    return 0;
}
uint8_t                         Channels::SetUserAsOP(const std::string& currentOP,const std::string& newOP, const std::string& channelName) {
    // 0 -> Success
    // 1 -> Channel don't exist

    // --- NEW OP ERRORS ---
    // 2 -> Only invited 
    // 3 -> Already Operator
    // 4 -> Not in channel

    // --- CURRENTOP ERRORS ---
    // 5 -> is not Operator ( i hope you call him bozo )
    // 6 -> Not in channel
    //

    // newOP Status
    try {
        UsersMap::iterator _newOP = _getUserInChannelByName(newOP, channelName);
        if (_newOP->second.in_Invited_List) return 2;
        if (_newOP->second.in_Operator_List) return 3;
    }
    catch (const int& e) {
        switch (e) {
            case 0: return 1;
            case 1: return 4;
        }
    }

    // currentOP Status
    try {
        UsersMap::iterator OP = _getUserInChannelByName(currentOP, channelName);
        if (!OP->second.in_Operator_List) return 5;
    }
    catch (const int& e) {
        switch (e) {
            case 0: return 1;   // i love copy pasting
            case 1: return 6;
        }
    }

    _channelGroup[channelName].second[newOP] = _createNewUserStats(false, false, true);
    return 0;
}
uint8_t                         Channels::InviteUserToChannel(const std::string& currentOP, const std::string& invitedUser, const std::string& channelName) {
    // 0 -> Success
    // 1 -> Channel don't exist

    // --- CURRENTOP ERRORS ---
    // 2 -> not in channel
    // 3 -> not Operator

    // --- invitedUser ERRORS ---
    // 4 -> already in channel
    // 5 -> already invited

    // currentOP Status
    try {
        UsersMap::iterator OP = _getUserInChannelByName(currentOP, channelName);
        if (!OP->second.in_Operator_List) return 3; // not OP
    }
    catch (const int& e) {
        switch (e) {
            case 0: return 1;   // channel dont exist
            case 1: return 2;   // user not in channel
        }
    }

    UsersMap::iterator i = _channelGroup[channelName].second.find(invitedUser);
    // Already listed
    if (i != _channelGroup[channelName].second.end()) {
        if (i->second.in_Operator_List || i->second.in_User_list) return 4;
        if (i->second.in_Invited_List) return 5;
    }

    _channelGroup[channelName].second[invitedUser] = _createNewUserStats(false, true, false);
    return 0;
}
uint8_t                         Channels::CancelChannelInvite(const std::string& currentOP,const std::string& invitedUser, const std::string& channelName) {
    // 0 -> Success
    // 1 -> Channel don't exist

    // --- currentOP ERRORS ---
    // 2 -> currentOP not Operator
    // 3 -> currentOP not in channel

    // --- invitedUser ERRORS ---
    // 4 -> invitedUser already in channel
    // 5 -> invitedUser Not in channel

    // currentOP Status
    try {
        UsersMap::iterator OP = _getUserInChannelByName(currentOP, channelName);
        if (!OP->second.in_Operator_List) return 2;
    }
    catch (const int& e) {
        switch (e) {
            case 0: return 1;
            case 1: return 3;
        }
    }

    // invitedUser Status
    try {
        UsersMap::iterator InvitedUser = _getUserInChannelByName(invitedUser, channelName);
        if (InvitedUser->second.in_Operator_List || InvitedUser->second.in_User_list) return 4;
        InvitedUser->second.in_Invited_List = false;
    }
    catch (const int& e) {
        switch (e) {
            case 0: return 1;
            case 1: return 5;
        }
    }

    return 0;
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
        UsersMap        currentChannel = i->second.second; // Skip to the userlist right away

        // Try to find Player, if exist, Erase
        UsersMap::iterator j = currentChannel.find(userName);
        if (j != currentChannel.end()) {
            currentChannel.erase(j);
        }
    }
}

// +++ Private +++
UsersMap::iterator              Channels::_getUserInChannelByName(const std::string& userName, const std::string& channelName) {
    // Object -> Success
    // Throw 0 -> Channel don't exist
    // Throw 1 -> User not in channel
    
    ChannelMap* T = _getChannelByName(channelName);
    if (!T) throw 0;

    UsersMap::iterator i = T->second.find(userName);
    if (i == T->second.end()) throw 1;

    return i;
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