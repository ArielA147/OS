#!/bin/bash

if [ -d "$1" ]; then cd "$1" # check if there is directory in $1 -> if yes go there.
fi

l=1 # nuber of the current line - start with 1

#finding all the .txt file (sorted)
for f in $(ls -v *.txt); do
  echo $l $f is a file
  l=$((l+1));
done

# finding all directory (sorted)
for i in $(ls -d -v */); do
  echo $l ${i%%/} is a directory
  l=$((l+1));
done
