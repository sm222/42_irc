#!/bin/bash

# - - - - - - - - - - - - #
# - use to pipe in irc  - #
# - - - - - - - - - - - - #

# seting --<
#  add more if needed
#  those may change
speed=0.3
endl='\n'
dest="1.1.1.1"
name="user"
nic="bozo"

# var --<
#  all the step you want the bot to do
args=(
  "connect $dest"
  "a"
  "b"
)
# -- -- --

i=0
j=${#args[@]}

# code --<

while [ $i -lt $j ]
  do
  printf "${args[$i]}$endl"
  i=$((i + 1))
  sleep $speed
done