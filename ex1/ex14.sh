#!/bin/bash

name="$1"
bankLog="$2"

grep -w $name $bankLog # printing the lines where name is in the log 
grep -w $name $bankLog | awk '{SUM += $2} END {print "Total balance: " SUM}' # calc the sum numbers in the bank log and print it
