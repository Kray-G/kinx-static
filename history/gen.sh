#!/bin/bash

if [ "$1" == "" ]; then
	echo Needs a directory name.
	exit 1
fi
if [ ! -d $1 ]; then
	echo Dirctory not found: $1
	exit 1
fi

DIR=$1
cd $DIR

if [ -f kinxstatic.y ]; then
	echo Generating a parser.
	../../utility/kmyacc -v -d -m ../../utility/kmyacc.c.parser -b kxs kinxstatic.y
fi

cd ..
exit 0

