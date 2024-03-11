#!/bin/bash

#look at '--<' for category 

#    var --<
i=0
len=$#
argv=("$@")
name=""
sys=$(uname -s)

# setting --<
speed=0.2
port=8080

# debug --<

#function --<
function linux_ft_run() {
  gnome-terminal -- $pwdtest/.run.sh $txt
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
    do script \"$pwdtest/.run.sh $txt\"
  end tell"
}

function join_by {
  local d=${1-} f=${2-}
  if shift 2; then
    printf %s "$f" "${@/#/$d}"
  fi
}

# code --<
#  start
if [ $len == 0 ]
  then
  echo 'how to use -- ./dev.sh *.sh'
  exit 1
fi
#  debug
echo $# $sys
#  set value
pwdtest=$(pwd)
txt=$pwdtest
# remaking argv
while [ $i -lt $# ]
  do
  txt=$(join_by  ' ' $txt ' ' ${argv[${i}]})
  i=$((i + 1))
done
#   - - - - -   #
sleep 1
if [ $sys == "Darwin" ]
  then
    mac_start
    mac_ft_run
  else
    linux_start
    linux_ft_run
fi
