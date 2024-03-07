#!/bin/bash

# seting --<
speed=0.3
endl='\n'

# var --<
args=(
  "test"
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
exit