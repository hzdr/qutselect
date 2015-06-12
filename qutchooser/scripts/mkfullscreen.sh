#!/bin/sh

WINID=""

while [ "$WINID" = "" ]; do
    WINID=`xdotool search "$1" 2>&1 | grep -v "Defaulting"`
    sleep 1
done

sleep 1

DPYW=`xwininfo -root | grep geometry | sed "s/  -geometry //g" | sed "s/+0//g" | awk -Fx '{ print $1; }'`
DPYH=`xwininfo -root | grep geometry | sed "s/  -geometry //g" | sed "s/+0//g" | awk -Fx '{ print $2; }'`

xdotool windowsize $WINID $DPYW $DPYH > /dev/null 2>&1
xdotool windowmove $WINID 0 0 > /dev/null 2>&1


