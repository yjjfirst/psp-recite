#!/bin/sh

FILES=$(ls /home/martin/mp3/)
for file_name in $FILES
do
	cd /home/martin/mp3
	zip $file_name.zip $file_name/*

done
