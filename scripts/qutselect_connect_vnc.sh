#!/bin/sh
#
# This is a startup script for qutselect which initates a
# VNC session to a windows server via 'vncviewer'
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

VNCVIEWER=/usr/bin/vncviewer

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

# variable to prepare the command arguments
cmdArgs=""

# resolution
if [ "x${resolution}" = "xfullscreen" ]; then
  cmdArgs="$cmdArgs -fullscreen"
fi

# color depth
cmdArgs="$cmdArgs -depth ${colorDepth}"

# disable compression (save CPU time)
cmdArgs="$cmdArgs -compresslevel 0"

# make sure a password dialog pops up
cmdArgs="$cmdArgs -xrm vncviewer*passwordDialog:true"

${VNCVIEWER} ${cmdArgs} ${serverName}

if [ $? != 0 ]; then
   printf "ERROR: ${VNCVIEWER} returned invalid return code"
   exit 2
fi

return 0
