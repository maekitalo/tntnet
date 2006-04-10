#!/bin/bash
#
# print insert-statements for testdata for newsdb
#
# usage:
#
# ./newsdata.sh
#   generates articles 1-20 (startvalue=1, count=20)
#
# ./newsdata.sh 5
#   generates articles 1-5 (startvalue=1, count=5)
#
# ./newsdata.sh 100 5
#   generates articles 100-104 (startvalue=100, count=5)
#

if [ ! -z "$2" ]
  then
    START=$1
    COUNT=$2
elif [ ! -z "$1" ]
  then
    START=1
    COUNT=$1
else
  START=1
  COUNT=20
fi

END=$((START+COUNT))

for ((i = $START; i < $END; i = i + 1))
  do
    cat <<SQL
insert into article (id, ctime, mtime, title, short_text, long_text)
 values ($i, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP, 'Article $i', 'This is Article $i',
   'This is the long version of Article $i. We have blah blah here.');
SQL
  done
