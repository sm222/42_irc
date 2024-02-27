#pragma once
#include <iostream>
#include "_header.h"

enum channelEnum {
    CHANNEL_USERLIST,
    CHANNEL_INVITED_LIST,
    CHANNEL_OPERATOR_LIST
};

// User Role in Channel, Only can have one role at a time
struct userState {
    bool            in_User_list;
    bool            in_Invited_List;
    bool            in_Operator_List;
};

// Just Return All The Names
struct ChannelAndTopic {
    std::string     ChannelName;
    std::string     TopicName;
};

/*
//                                                            (Key)           (Value)
//                                              - - - - - [Users Map] -> UserState ( Role )
//                                             |
//                        - - - - - - - - ChannelMap    (Second)
//                       |
// channelGroup --- ChannelName
//                       |
//                        - - - - - - - - Topic Name    (First)

*/


//                                Username      Role
#define UsersMap        std::map<std::string, userState>     // Users in Channel
//                                  Topic    
#define ChannelMap      std::pair<std::string, UsersMap>    // Channel
//                                ChannelName
#define ChannelGroup    std::map<std::string, ChannelMap>    // All the Channels

class Channels {
public:

    Channels();

    // Create channel
    bool                            CreateChannel(const std::string& channelMaker, const std::string& channelName);
    uint8_t                         LeaveChannel(const std::string& userName, const std::string& channelName);
    
    // All Channels and their Topic Name
    std::vector<ChannelAndTopic>    GetAllChannelsAndTopicName();

    // Topic Name
    std::string                     GetChannelTopic(const std::string& channelName);
    uint8_t                         SetChannelTopic(const std::string& userName, const std::string& channelName, const std::string& topic);

    // User must be invited by OP
    uint8_t                         JoinChannel(const std::string& userName, const std::string& channelName);
    
    // Send Message to users in channel (You loop & used SendData with their username)
    std::vector<std::string>        GetAllUsersInChannel(const std::string& channelName);
    std::vector<std::string>        GetAllOPinChannel(const std::string& channelName);

    // ----- Operator only -----
    uint8_t                         SetUserAsOP(const std::string& currentOP,const std::string& newOP, const std::string& channelName);
    uint8_t                         KickUserFromChannel(const std::string& currentOP,const std::string& userToKick, const std::string& channelName);
    uint8_t                         CancelChannelInvite(const std::string& currentOP,const std::string& invitedUser, const std::string& channelName);
    uint8_t                         InviteUserToChannel(const std::string& currentOP, const std::string& invitedUser, const std::string& channelName);

    // ----- DO NOT USE -----
    void                            SOCKETONLY_kickuserfromallchannels(const std::string& userName);

    // ---- Testing & Debugging -----
    void                            PrintALLChannelContent();
    void                            PrintChannelContent(const std::string& channelName);

private:
    ChannelGroup                    _channelGroup;
    UsersMap::iterator              _getUserInChannelByName(const std::string& userName, const std::string& channelName);
    ChannelMap*                     _getChannelByName(const std::string& channelName);
    userState                       _createNewUserStats(const bool isUser, const bool isInv, const bool isOP);
};