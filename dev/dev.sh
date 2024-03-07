#!/bin/bash
echo $#

i=0
len=$#
argv=("$@")
while [ $i -lt $len ]
  do
  name=${argv[${i}]}
  gnome-terminal -x "ls"
  ./"$name".sh | nc -w 5 127.0.0.1 8080
  i=$((i + 1))
done

#args=("$@") 
## get number of elements 
#ELEMENTS=${#args[@]} 
# 
## echo each element in array
## for loop 
#for (( i=0;i<$ELEMENTS;i++)); do 
#    echo ${args[${i}]} 
#done