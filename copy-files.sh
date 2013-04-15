#!/bin/sh

VENDOR=panasonic
DEVICE=p01d

BASE=../../../vendor/$VENDOR/$DEVICE/proprietary

echo "Copying device files from $1 ..."
for FILE in `cat proprietary-files.txt | grep -v ^# | grep -v ^$`; do
    DIR=`dirname $FILE`
    if [ ! -d $BASE/$DIR ]; then
        mkdir -p $BASE/$DIR
    fi
    cp $1/$FILE $BASE/$FILE
done

for FILE in `cat proprietary-files-need-renamed.txt | grep -v ^# | grep -v ^$ | cut -d':' -f1`; do
    DIR=`dirname $FILE`
    if [ ! -d $BASE/$DIR ]; then
        mkdir -p $BASE/$DIR
    fi
    cp $1/$FILE $BASE/$FILE
done
