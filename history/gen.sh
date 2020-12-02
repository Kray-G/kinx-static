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

if [ -f Makefile ]; then
	make
	if [ -f kiss ]; then cp -f kiss ../; fi
fi

cd ..
exit 0

