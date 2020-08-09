#!/bin/bash

if [ $#  == 2 ];then # check if there are 2 args or 1
  cd "$1"
  filename="$2"
else
  filename="$1"
fi

for i in $(ls -v); do

  if [[ $i = $filename ]]; then # if we found file with the same name
    echo "`cat $i`" # printing the file data in new line

  elif [[ -d $i ]]; then cd $i # if directory get inside
    for j in $(ls -v); do # go over files in sorted way

      if [[ $j = $filename ]]; then # find file with same filename
          echo "`cat $j`" # printing the file data in new line
      fi
    done
    cd .. # go out from the inner file

  fi
done
