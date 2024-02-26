#pragma once
#include <iostream>
#include "_header.h"

enum channelEnum {
    CHANNEL_USERLIST,
    CHANNEL_INVITED_LIST,
    CHANNEL_OPERATOR_LIST
};

struct userChannelState {
    userData&   user;

    bool        in_User_list;
    bool        in_Invited_List;
    bool        in_Operator_List;
};


class Channels {
public:

    Channels();

    userChannelState*   isUserInChannel(const userData& user, const std::string channelName);
    void                JoinChannel(const std::string channelName);
    bool                doesChannelExist(const std::string channelName);                      //

    void                JoinSubChannel(userData& user, const std::string channelName, const channelEnum Enum);



private:


    std::map<std::string, std::vector<userChannelState>>  _channelGroup;
};