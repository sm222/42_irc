#!/bin/bash

# seting --<
speed=0.3
endl=\r

# var --<
args=(
  "PASS a"
  "USER a 0 * :b"
  "PING A A A"
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