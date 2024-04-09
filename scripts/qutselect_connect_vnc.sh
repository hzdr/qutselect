#!/bin/bash
# shellcheck shell=dash disable=SC3010,SC3020
#
# This is a startup script for qutselect which initates a
# VNC session to a windows server via 'vncviewer'
#
# It receives the following inputs:
#
# $1 = PID of qutselect
# $2 = serverType (RDP, VNC)
# $3 = 'true' if dtlogin mode was on while qutselect was running
# $4 = the resolution (either 'fullscreen' or 'WxH')
# $5 = the selected color depth (8, 16, 24)
# $6 = the current max. color depth (8, 16, 24)
# $7 = the selected keylayout (e.g. 'de' or 'en')
# $8 = the domain (e.g. 'FZR', used for RDP)
# $9 = the username
# $10 = the servername (hostname) to connect to

if [[ -x /opt/thinlinc/lib/tlclient/vncviewer ]]; then
  VNCVIEWER=/opt/thinlinc/lib/tlclient/vncviewer
elif [[ -x /lib/tlclient/vncviewer ]]; then
  VNCVIEWER=/lib/tlclient/vncviewer
elif [[ -x /usr/bin/vncviewer ]]; then
  VNCVIEWER=/usr/bin/vncviewer
else
  VNCVIEWER=vncviewer
fi

#####################################################
# check that we have 10 command-line options at hand
if [[ $# -lt 10 ]]; then
   printf "ERROR: missing arguments!"
   exit 2
fi

# catch all arguments is some local variables
#parentPID="${1}"
#serverType="${2}"
dtlogin="${3}"
resolution="${4}"
#colorDepth="${5}"
#curDepth="${6}"
#keyLayout="${7}"
#domain="${8}"
#username="${9}"
serverName="${10}"

# read the password from stdin
read -r password

# variable to prepare the command arguments
cmdArgs="-shared -menukey="

# resolution
if [[ "${resolution}" == "fullscreen" ]]; then
  cmdArgs="$cmdArgs -fullscreen -fullscreensystemkeys"
fi

# color depth
#cmdArgs="$cmdArgs -depth ${colorDepth}"

# disable compression (save CPU time)
#cmdArgs="$cmdArgs -compresslevel 0"

# run vncviewer finally
if [[ "${password}" != "NULL" ]]; then
  #cmdArgs="$cmdArgs -autopass"
  if [[ "${dtlogin}" != "true" ]]; then
    echo "${VNCVIEWER} ${cmdArgs} ${serverName}"
  fi
  # shellcheck disable=SC2086
  VNC_PASSWORD="${password}" ${VNCVIEWER} ${cmdArgs} "${serverName}" 2>/dev/null >/dev/null
  res=$?
else
  # make sure a password dialog pops up
  #cmdArgs="$cmdArgs -xrm vncviewer*passwordDialog:true"
  if [[ "${dtlogin}" != "true" ]]; then
    echo "${VNCVIEWER} ${cmdArgs} ${serverName}"
  fi
  # shellcheck disable=SC2086
  ${VNCVIEWER} ${cmdArgs} "${serverName}" &>/dev/null
  res=$?
fi

if [[ ${res} != 0 ]]; then
  echo "ERROR: ${VNCVIEWER} returned invalid return code (${res})"
fi

exit ${res}
