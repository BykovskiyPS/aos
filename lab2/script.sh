#!/bin/bash
if [ $# -ne 1 ]
then 
    echo Usage: $0 count
    exit
fi
father=0
son=0
count=$1
while [ $count -gt 0 ]
do
    count=`expr $count - 1`
    if [ "`./t02`" = "#" ]
    then
        father=`expr $father + 1`
    else
        son=`expr $son + 1`
    fi
done
echo father=$father
echo son=$son