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

// Hold The Channels Options
struct ChannelData {
    std::string     Topic;
    bool            InviteOnly;
    bool            CanUserChangeChannel;
};


/*
//                                                            (Key)           (Value)
//                                              - - - - - [Users Map] -> UserState ( Role )
//                                             |
//                        - - - - - - - - ChannelMap    (Second)
//                       |
// channelGroup --- ChannelName
//                       |
//                        - - - - - - - - ChannelData    (First)

*/


//                                Username      Role
#define UsersMap        std::map<std::string, userState>     // Users in Channel
//                                  Topic    
#define ChannelMap      std::pair<ChannelData, UsersMap>    // Channel
//                                ChannelName
#define ChannelGroup    std::map<std::string, ChannelMap>    // All the Channels

class Channels {
public:

    Channels();

    // Basic Stuff
    bool                            Channel_AlreadyExist(const std::string& channelname);
    bool                            Channel_Create(const std::string& channelMaker, const std::string& channelName);
    bool                            Channel_Join(const std::string& userName, const std::string& channelName);
    bool                            Channel_Leave(const std::string& userName, const std::string& channelName);

    // Getter
    bool                            Channel_Get_IsUserInChannel(const std::string& userName, const std::string& channelName);
    bool                            Channel_Get_IsUserChannelOP(const std::string& userName, const std::string& channelName);
    bool                            Channel_Get_InviteOnly(const std::string& channelname);
    bool                            Channel_Get_CanUserChangeTopic(const std::string& channelname);
    std::string                     Channel_Get_Topic(const std::string& channelName);

    // Setter
    void                            Channel_Set_InviteOnly(const std::string& channelname, const bool value);
    void                            Channel_Set_CanUserChangeTopic(const std::string& channelname, const bool value);
    bool                            Channel_Set_Topic(const std::string& channelName, const std::string& topic);

    // Informations
    std::vector<ChannelAndTopic>    Channel_Get_AllChannelsAndTopicName();

    // Operators Only
    bool                            Channel_Set_Operator(const std::string& user, const std::string& channelName);
    bool                            Channel_Uninvite(const std::string& invitedUser, const std::string& channelName);
    bool                            Channel_Invite(const std::string& invitedUser, const std::string& channelName);
    
    // Broadcast to Channel (   You Loop the Vector & Use ---> Sock.SendData(User, Msg)   )
    std::vector<std::string>        Channel_Get_AllUsers(const std::string& channelName);
    std::vector<std::string>        Channel_Get_AllOperators(const std::string& channelName);

    // ---- Testing & Debugging -----
    void                            PrintALLChannelContent();
    void                            PrintChannelContent(const std::string& channelName);

    // ----- DO NOT USE -----
    void                            SOCKETONLY_kickuserfromallchannels(const std::string& userName);
private:
    ChannelGroup                    _channelGroup;
    UsersMap::iterator              _getUserInChannelByName(const std::string& userName, const std::string& channelName);
    ChannelMap*                     _getChannelByName(const std::string& channelName);
    userState                       _createNewUserStats(const bool isUser, const bool isInv, const bool isOP);
    bool                            _isUserInvited(ChannelMap* Channel, const std::string& channelName);
};