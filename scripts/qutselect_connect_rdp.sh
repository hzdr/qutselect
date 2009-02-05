#!/bin/sh
#
# This is a startup script for qutselect which initates a
# RDP session to a windows server either via rdesktop or uttsc
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

RDESKTOP=/opt/csw/bin/rdesktop
UTTSC=/opt/SUNWuttsc/bin/uttsc
UTACTION=/opt/SUNWut/bin/utaction
XVKBD=/usr/openwin/bin/xvkbd

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

# before we go and connect to the windows (rdp) server we
# go and add an utaction call so that on a smartcard removal
# the windows desktop will be locked.
${UTACTION} -d "$XVKBD -text '\Ml'" &

# variable to prepare the command arguments
cmdArgs=""

# now we find out which RDP client we use (rdesktop/uttsc)
if [ "x${SUN_SUNRAY_TOKEN}" != "x" ] && [ -x ${UTTSC} ]; then

   # if we end up here we go and prepare all arguments for the
   # uttsc call
   
   # resolution
   if [ "x${resolution}" = "xfullscreen" ]; then
      cmdArgs="$cmdArgs -m"

      # if we are in dtlogin mode we go and disable the pulldown header
      if [ "x${dtlogin}" = "xtrue" ]; then
         cmdArgs="$cmdArgs -b"
      fi
   else
      cmdArgs="$cmdArgs -g ${resolution}"
   fi

   # color depth
   cmdArgs="$cmdArgs -A ${colorDepth}"

   # sound 
   cmdArgs="$cmdArgs -r sound:low"

   # disable compression (save CPU time)
   cmdArgs="$cmdArgs -z"

   # add client name
   cmdArgs="$cmdArgs -n `hostname`"

   # keyboard
   if [ "x${keyLayout}" = "xde" ]; then
      cmdArgs="$cmdArgs -l de-DE"
   else
      cmdArgs="$cmdArgs -l en-US"
   fi

   # add domain
   cmdArgs="$cmdArgs -d FZR -u FZR\\"

   # add the usb path as a local path
   cmdArgs="$cmdArgs -r disk:USB=/tmp/SUNWut/mnt/${USER}/"

   ${UTTSC} ${cmdArgs} ${serverName}
   if [ $? != 0 ]; then
      printf "ERROR: uttsc returned invalid return code"
      exit 2
   fi

else

   # resolution
   if [ "x${resolution}" = "xfullscreen" ]; then
      cmdArgs="$cmdArgs -f"
   else
      cmdArgs="$cmdArgs -g ${resolution}"
   fi

   # color depth
   cmdArgs="$cmdArgs -a ${colorDepth}"

   # sound 
   cmdArgs="$cmdArgs -r sound:local"

   # disable encryption (saves CPU time)
   cmdArgs="$cmdArgs -E"

   # enable LAN speed features
   cmdArgs="$cmdArgs -x lan"

   # add client name
   cmdArgs="$cmdArgs -n `hostname`"

   # keyboard
   if [ "x${keyLayout}" = "xde" ]; then
      cmdArgs="$cmdArgs -k de"
   else
      cmdArgs="$cmdArgs -k en-US"
   fi

   # add domain
   cmdArgs="$cmdArgs -d FZR"

   # add the usb path as a local path
   cmdArgs="$cmdArgs -r disk:USB=/tmp/SUNWut/mnt/${USER}/"

   ${RDESKTOP} ${cmdArgs} ${serverName}
   if [ $? != 0 ]; then
      printf "ERROR: rdesktop returned invalid return code"
      exit 2
   fi

fi

return 0
