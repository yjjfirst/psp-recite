#!/bin/sh

FILES=$(cat wordlist)
for file_name in $FILES
do
#	echo $file_name
	mp3_name=/mnt/iso/${file_name:0:1}/$file_name.mp3
	dst_name=/home/martin/mp3/${file_name:0:1}/$file_name.mp3
	cp -rf $mp3_name $dst_name 
done

