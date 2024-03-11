#!/bin/bash

# - - - - - - - - - - - - #
# - use to pipe in irc  - #
# - - - - - - - - - - - - #

# seting --<
#  add more if needed
#  those may change
speed=0.0001
endl='\n'
dest="1.1.1.1"
name="user"
nic="bozo"

# var --<
#  all the step you want the bot to do
args=(
  "connect $dest"
)
# -- -- --

i=0
j=${#args[@]}
k=0
l=10000000000
# code --<

while [ $i -lt $j ]
  do
  printf "${args[$i]}$endl"
  i=$((i + 1))
  sleep $speed
  while [ $k -lt $l ]
    do
    echo "<$$>" $k
    sleep $speed
    k=$((k + 1))
  done
done