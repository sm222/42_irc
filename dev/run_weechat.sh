# #!/bin/bash

# # Count the number of running Docker containers using the weechat/weechat image
# weechat_containers=$(docker ps --filter ancestor=weechat/weechat --format "{{.ID}}" | wc -l | xargs)

# # Default values 
# default_name="user_name_$weechat_containers"
# default_nickname="nick_name_$weechat_containers"
# default_ip=$(ifconfig -l | xargs -n1 ipconfig getifaddr | grep 10. | head -n 1)

# # Ask the user to enter a new IP address or press Enter to keep the default value
# echo -n "Enter new IP or press Enter (${default_ip}):"
# read new_ip
# IP=${new_ip:-$default_ip}

# # Ask the user to enter a new name or press Enter to keep the default name
# echo -n "Enter new name or press Enter (${default_name}):"
# read new_name
# NAME=${new_name:-$default_name}

# # Ask the user to enter a new nickname or press Enter to keep the default nickname
# echo -n "Enter new nick or press Enter (${default_nickname}):"
# read new_nickname
# NICKNAME=${new_nickname:-$default_nickname}

# # Commands to be executed in WeeChat
# commands="/server add O_O ${IP}/2000 -notls -password=a; /set irc.server.O_O.username \"${NAME}\"; /set irc.server.O_O.nicks \"${NICKNAME}\"; /connect O_O"

# # Launch WeeChat in Docker and execute the commands
# docker run -it --rm weechat/weechat weechat -r "$commands"



#!/bin/bash

# Flag to use default values without user input
use_default=false

# Parse flags
while getopts "d" opt; do
  case $opt in
    d) use_default=true ;;
    \?) echo "Invalid option: -$OPTARG" >&2 ;;
  esac
done

# Count the number of running Docker containers using the weechat/weechat image
weechat_containers=$(docker ps --filter ancestor=weechat/weechat --format "{{.ID}}" | wc -l | xargs)

# Default values
default_name="user_name_$weechat_containers"
default_nickname="nick_name_$weechat_containers"
default_ip=$(ifconfig -l | xargs -n1 ipconfig getifaddr | grep 10. | head -n 1)

# Functions to get user input or use default
get_input_or_default() {
  local input prompt default_var result_var
  prompt="$1"
  default_var="$2"
  result_var="$3"

  if [ "$use_default" = true ]; then
    eval $result_var="'$default_var'"
  else
    echo -n "$prompt ($default_var):"
    read input
    eval $result_var='${input:-$default_var}'
  fi
}

# Ask the user for inputs or use default values based on the flag
get_input_or_default "Enter new IP or press Enter" "$default_ip" IP
get_input_or_default "Enter new name or press Enter" "$default_name" NAME
get_input_or_default "Enter new nick or press Enter" "$default_nickname" NICKNAME

# Commands to be executed in WeeChat
commands="/server add O_O ${IP}/2000 -notls -password=a; /set irc.server.O_O.username \"${NAME}\"; /set irc.server.O_O.nicks \"${NICKNAME}\"; /connect O_O"

# Launch WeeChat in Docker and execute the commands
docker run -it --rm weechat/weechat weechat -r "$commands"
