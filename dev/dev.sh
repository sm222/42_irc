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
    gnome-terminal -x "ls"
    ./"$name".sh | nc -w 5 127.0.0.1 $port
    i=$((i + 1))
  done
}

function mac_ft() {
  while [ $i -lt $len ]
    do
    name=${argv[${i}]}
    osascript -e "tell app \"Terminal\"
      do script \"nc -l $port\"
    end tell"
    sleep $speed
    ./"$name".sh | nc -w 5 127.0.0.1 $port
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

#args=("$@") 
## get number of elements 
#ELEMENTS=${#args[@]} 
# 
## echo each element in array
## for loop 
#for (( i=0;i<$ELEMENTS;i++)); do 
#    echo ${args[${i}]} 
#done