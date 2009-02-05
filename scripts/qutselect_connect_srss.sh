#!/bin/sh
#
# This is a startup script for qutselect which initates a
# SRSS session to another sunray server via utswitch
#
# It receives the following inputs:
#
# $1 = PID of qutselect
# $2 = serverType (SRSS, RDP, VNC)
# $3 = 'true' if dtlogin mode was on while qutselect was running
# $4 = the resolution (either 'fullscreen' or 'WxH')
# $5 = the selected color depth (8, 16, 24)
# $6 = the current max. color depth (8, 16, 24)
# $7 = the selected keylayout (e.g. 'de' or 'en')
# $8 = the servername (hostname) to connect to
#

UTSWITCH=/opt/SUNWut/bin/utswitch
KILL=/opt/csw/bin/kill

#####################################################
# check that we have 8 command-line options at hand
if [ $# -lt 8 ]; then
   printf "ERROR: missing arguments!"
   exit 2
fi

# catch all arguments is some local variables
parentPID=$1
serverType=$2
dtlogin=$3
resolution=$4
colorDepth=$5
curDepth=$6
keyLayout=$7
serverName=$8

# execute utswitch
${UTSWITCH} -h ${serverName}
if [ $? != 0 ]; then
   printf "ERROR: ${UTSWITCH} returned invalid return code"
   exit 2
fi

# now we kill the qutselect gui to free the session for someone else
${KILL} $parentPID

return 0
