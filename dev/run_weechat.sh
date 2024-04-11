#!/bin/bash

# Flag to use default values without user input
use_default=false

# IP address provided as a non-flag argument
provided_ip=""

# Parse flags and non-flag arguments
while getopts ":d" opt; do
  case $opt in
    d) use_default=true ;;
    \?) echo "Invalid option: -$OPTARG" >&2; exit 1 ;;
  esac
done

# Check for non-flag argument for IP address
shift $((OPTIND - 1))
if [ ! -z "$1" ]; then
  provided_ip=$1
fi

# Count the number of running Docker containers using the weechat/weechat image
weechat_containers=$(docker ps --filter ancestor=weechat/weechat --format "{{.ID}}" | wc -l | xargs)

# Default values
default_name="user_name_$weechat_containers"
default_nickname="nick_name_$weechat_containers"
default_ip=$(ifconfig -l | xargs -n1 ipconfig getifaddr | grep 10. | head -n 1)

# Use provided IP if available, otherwise use detected default IP
final_ip=${provided_ip:-$default_ip}

# Functions to get user input or use default
get_input_or_default() {
  local input prompt default_var result_var
  prompt="$1"
  default_var="$2"
  result_var="$3"

  if [ "$use_default" = true ]; then
    eval $result_var="'$default_var'"
  else
    echo -n "$prompt ($default_var): "
    read input
    eval $result_var='${input:-$default_var}'
  fi
}

# Ask the user for inputs or use default values based on the flag
get_input_or_default "Enter new IP or press Enter" "$final_ip" IP
get_input_or_default "Enter new name or press Enter" "$default_name" NAME
get_input_or_default "Enter new nick or press Enter" "$default_nickname" NICKNAME

# Commands to be executed in WeeChat
commands="/server add O_O ${IP}/2000 -notls -password=a; /set irc.server.O_O.username \"${NAME}\"; /set irc.server.O_O.nicks \"${NICKNAME}\"; /connect O_O"

# Launch WeeChat in Docker and execute the commands
docker run -it --rm weechat/weechat weechat -r "$commands"
