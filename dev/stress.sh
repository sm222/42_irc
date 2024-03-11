#!/bin/bash

#look at '--<' for category 

#    var --<
i=0
len=$#
argv=("$@")
name=""
sys=$(uname -s)
pwdtest=$(pwd)
txt=$pwdtest

# setting --<
speed=0.2
port=8080


function linux_ft_run() {
  gnome-terminal -- $pwdtest/.run.sh $pwdtest spam.sh
}

function mac_ft_run() {
  osascript -e "tell app \"Terminal\"
    do script \"$pwdtest/../ircserv\"
  end tell"
}

function linux_start() {
  gnome-terminal -- $pwdtest/../ircserv
}

function mac_start() {
  osascript -e "tell app \"Terminal\"
    do script \"$pwdtest/.run.sh spam.sh\"
  end tell"
}

if [ $sys == "Darwin" ]
  then
    mac_start
    mac_ft_run
    mac_ft_run
    mac_ft_run
  else
    linux_start
    sleep 1
    linux_ft_run
    linux_ft_run
    linux_ft_run
fi