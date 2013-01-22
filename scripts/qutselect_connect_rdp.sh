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
# $8 = the domain (e.g. 'FZR', used for RDP)
# $9 = the username
# $10 = the servername (hostname) to connect to
#

if [ `uname -s` = "SunOS" ]; then
   RDESKTOP=/opt/csw/bin/rdesktop
   UTTSC=/opt/SUNWuttsc/bin/uttsc
   UTACTION=/opt/SUNWut/bin/utaction
   XVKBD=/usr/openwin/bin/xvkbd
   PKILL=/usr/bin/pkill
else
   RDESKTOP=/usr/local/bin/rdesktop
   UTTSC=/opt/SUNWuttsc/bin/uttsc
   UTACTION=/opt/SUNWut/bin/utaction
   XVKBD=/usr/openwin/bin/xvkbd
   PKILL=/usr/bin/pkill
fi

#####################################################
# check that we have 10 command-line options at hand
if [ $# -lt 10 ]; then
   printf "ERROR: missing arguments!"
   exit 2
fi

# catch all arguments is some local variables
parentPID="${1}"
serverType="${2}"
dtlogin="${3}"
resolution="${4}"
colorDepth="${5}"
curDepth="${6}"
keyLayout="${7}"
domain="${8}"
username="${9}"
serverName="${10}"

# read the password from stdin
read password

# before we go and connect to the windows (rdp) server we
# go and add an utaction call so that on a smartcard removal
# the windows desktop will be locked.
if [ "x${dtlogin}" = "xtrue" ]; then
   ${PKILL} -u ${USER} -f "utaction.*xvkbd" >/dev/null 2>&1
   ${UTACTION} -d "$XVKBD -text '\Ml'" &
fi

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

   # add client name
   cmdArgs="$cmdArgs -n `hostname`"

   # keyboard
   if [ "x${keyLayout}" = "xde" ]; then
      cmdArgs="$cmdArgs -l de-DE"
   else
      cmdArgs="$cmdArgs -l en-US"
   fi

   # add domain
   if [ "x${domain}" != "xNULL" ]; then
     cmdArgs="$cmdArgs -d ${domain}"
   else
     cmdArgs="$cmdArgs -d FZR"
   fi
   
   # add username
   if [ "x${username}" != "xNULL" ]; then
     cmdArgs="$cmdArgs -u ${username}"
   else
     if [ "x${domain}" != "xNULL" ]; then
       cmdArgs="$cmdArgs -u ${domain}\\"
     fi
   fi

   # enbaled enhanced network security
   cmdArgs="$cmdArgs -N on"

   # add the usb path as a local path
   cmdArgs="$cmdArgs -r disk:USB=/tmp/SUNWut/mnt/${USER}/"

   # output the cmdline so that users can replicate it
   if [ "x${dtlogin}" != "xtrue" ]; then
      echo ${UTTSC} ${cmdArgs} ${serverName}
   fi

   # run uttsc finally
   if [ "x${password}" != "xNULL" ]; then
     cmdArgs="$cmdArgs -i"
     echo ${password} | ${UTTSC} ${cmdArgs} ${serverName} &
   else
     ${UTTSC} ${cmdArgs} ${serverName} &
   fi

   ret=$?
   if [ $ret != 0 ]; then
      if [ $ret -eq 211 ]; then
        cmdArgs=""
        echo "WARNING: couldn't start uttsc, retrying with rdesktop"
      else
        echo "ERROR: uttsc returned invalid return code"
        exit 2
     fi
   fi
fi

if [ -z "${cmdArgs}" ]; then

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
   if [ "x${domain}" != "xNULL" ]; then
     cmdArgs="$cmdArgs -d ${domain}"
   else
     cmdArgs="$cmdArgs -d FZR"
   fi

   # add username
   if [ "x${username}" != "xNULL" ]; then
     cmdArgs="$cmdArgs -u ${username}"
   else
     if [ "x${domain}" != "xNULL" ]; then
       cmdArgs="$cmdArgs -u ${domain}\\"
     fi
   fi

   if [ "x${SUN_SUNRAY_TOKEN}" != "x" ]; then
      # add the usb path as a local path
      cmdArgs="$cmdArgs -r disk:USB=/tmp/SUNWut/mnt/${USER}/"
   fi

   # if we are not in dtlogin mode we go and
   # output the rdesktop line that is to be executed
   if [ "x${dtlogin}" != "xtrue" ]; then
      echo ${RDESKTOP} ${cmdArgs} ${serverName}
   fi

   # run rdesktop finally
   if [ "x${password}" != "xNULL" ]; then
     cmdArgs="$cmdArgs -p -"
     echo ${password} | ${RDESKTOP} ${cmdArgs} ${serverName} &
   else
     ${RDESKTOP} ${cmdArgs} ${serverName} &
   fi

   if [ $? != 0 ]; then
      echo "ERROR: rdesktop returned invalid return code"
      exit 2
   fi

fi

return 0
