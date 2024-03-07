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
echo $# $sys

#function --<
function linux_ft() {
  gnome-terminal -- ./run.sh $@
}

function mac_ft() {
  osascript -e "tell app \"Terminal\"
    do script \"$pwdtest/run.sh $txt\"
  end tell"
}

function join_by {
  local d=${1-} f=${2-}
  if shift 2; then
    printf %s "$f" "${@/#/$d}"
  fi
}

# code --<


pwdtest=$(pwd)
txt=$pwdtest
while [ $i -lt $# ]
  do
  txt=$(join_by  ' ' $txt ' ' ${argv[${i}]})
  i=$((i + 1))
done

sleep 1
if [ $sys == "Darwin" ]
  then
    mac_ft
  else
    linux_ft
fi