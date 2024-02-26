#include "Channels.h"
#include <exception.h>

Channels::Channels() {}


// kick from all the shit


// remove from channel
// remove from subchannel

// isUserInSubChannel
// destroy channel

// Send message to subchannel
// Send message to entire channel

bool                Channels::doesChannelExist(const std::string channelName) {
    return _channelGroup.find(channelName) != _channelGroup.end() ? true : false;
}
userChannelState*   Channels::isUserInChannel(const userData& user, const std::string channelName) {
    // Channel don't exist -> User can't be in it
    if (!doesChannelExist(channelName)) return 0;

    std::vector<userChannelState> &T = _channelGroup[channelName];

    for (std::vector<userChannelState>::iterator i = T.begin(); i != T.end(); i++) {
        if (user.userFD == i->user.userFD)
            return &(*i);
    }
    return 0;

}


void                Channels::JoinChannel(const std::string channelName) {

    // Already Exist -> No Action
   if (doesChannelExist(channelName)) return;

    // Create Channel
    _channelGroup[channelName];

}
void                Channels::JoinSubChannel(userData& user, const std::string channelName, const channelEnum Enum) {

    // Create's it if it doesnt exist
    JoinChannel(channelName);

    // Grab the user if it already exists
    userChannelState* T = isUserInChannel(user, channelName);

    // Not in Channel -> Add to channel & Add flag
    if (!T) {
        userChannelState node = {user, false, false, false};
        switch (Enum) {
            case CHANNEL_USERLIST:      node.in_User_list = true;       break;
            case CHANNEL_INVITED_LIST:  node.in_Invited_List = true;    break;
            case CHANNEL_OPERATOR_LIST: node.in_Operator_List = true;   break;
            default: throw AnyExcept("[-][JoinSubChannel] Enum is bad [0]");
        }

        _channelGroup[channelName].push_back(node);
    }

    // Already Exist -> Adjust flag
    else {
         switch
          (Enum) {
            case CHANNEL_USERLIST:      T->in_User_list = true;       break;
            case CHANNEL_INVITED_LIST:  T->in_Invited_List = true;    break;
            case CHANNEL_OPERATOR_LIST: T->in_Operator_List = true;   break;
            default: throw AnyExcept("[-][JoinSubChannel] Enum is bad [1]");
        }
    }
}