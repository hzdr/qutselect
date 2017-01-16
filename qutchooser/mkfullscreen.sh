#!/bin/sh

LOG="/var/log/pfx.log"

WINID=""

if  [ "$1" = "" ];
then
   echo "No string to locate window" >> $LOG 2>&1
   exit 255
fi

while [ "$WINID" = "" ]; do
    WINID=`xdotool search "$1" 2>&1 | grep -v "Defaulting"`
    echo "Lookup window $1, ID=[$WINID]" >> $LOG 2>&1
    sleep 1
done

sleep 1

echo "Resizing window [$WINID]" >> $LOG 2>&1
DPYW=`xwininfo -root | grep geometry | sed "s/  -geometry //g" | sed "s/+0//g" | awk -Fx '{ print $1; }'`
DPYH=`xwininfo -root | grep geometry | sed "s/  -geometry //g" | sed "s/+0//g" | awk -Fx '{ print $2; }'`

xdotool windowsize $WINID $DPYW $DPYH > /dev/null 2>&1
xdotool windowmove $WINID 0 0 > /dev/null 2>&1

