#!/bin/bash

sys=$(uname -s)

i=1
len=$#
argv=("$@")
name=""
sys=$(uname -s)

speed=0.2
port=2000

echo $# $sys

# code
while [ $i -lt $len ]
  do
    name=${argv[0]}
    file=${argv[${i}]}
    test -f $name/$file
    if [ $? == 0 ]
      then
        "$name/$file" | nc -w 5 127.0.0.1 $port
        echo '✅'
      else
        echo 'no file > ' \""$name"\"
        echo '❌'
    fi
    i=$((i + 1))
  done
exit