#!/bin/bash

# insert the number of line (at beginging of each line) and counting words per line (at end of each line)
# than getting the first number and the last number in evry row which contains the wanted word ($2)
# the numbers will be before : (the line number) and after the last : (number of words per line)
awk -F ' ' '{print $0": "NF}' $1 | grep -n -w $2 | awk -F ":" '{print $1 $NF}'

