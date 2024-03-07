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

# function --<
function linux_ft() {
  while [ $i -lt $len ]
    do
    name=${argv[${i}]}
    echo "runing $name"
    gnome-terminal -- nc -l $port
    ./"$name" | nc -w 5 127.0.0.1 $port
    i=$((i + 1))
  done
}

function mac_ft() {
  while [ $i -lt $len ]
    do
    name=${argv[${i}]}
    echo "runing $name"
    osascript -e "tell app \"Terminal\"
      do script \"nc -l $port && kill -9 \$$\"
    end tell"
    sleep $speed
    ./"$name" | nc -w 5 127.0.0.1 $port
    i=$((i + 1))
  done
}

# code --<

if [ $sys == "Darwin" ]
  then
    mac_ft
  else
    linux_ft
fi
