#!/bin/bash

if [ $# -lt 1 ]; then
	echo "player.sh <audiofile>"
	exit
fi

TMP_WAV=tmp_file.wav

echo "Will convert files [$*]"
sox --magic -S -V3 "$*" -G --norm -b 8 -c 1 -r 8000 "$TMP_WAV"
sox_es=$?

if [ $sox_es -ne 0 ]; then
	echo "File conversion failed. Status: [$sox_es]"
	exit
fi

time ./wpwm.pl "$TMP_WAV"
rm -rfv "$TMP_WAV"
