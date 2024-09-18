#!/bin/bash
# shellcheck shell=dash disable=SC3010,SC3020
#
# This is a startup script for qutselect which initates a
# ThinLinc session to a thinlinc server
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

#TLCLIENT=/opt/thinlinc/bin/tlclient
TLCLIENT=/usr/bin/thinlinc

#####################################################
# check that we have 10 command-line options at hand
if [[ $# -lt 10 ]]; then
   printf "ERROR: missing arguments!"
   exit 2
fi

# catch all arguments is some local variables
#parentPID="${1}"
#serverType="${2}"
#dtlogin="${3}"
#resolution="${4}"
#colorDepth="${5}"
#curDepth="${6}"
#keyLayout="${7}"
#domain="${8}"
username="${9}"
serverName="${10}"

# read the password from stdin
read -r password

# check if the hostname is the same like the 
# server we should connect to and if yes we go and exit immediately
if [[ "$(hostname)" != "${serverName}" ]]; then

  # variable to prepare the command arguments
  cmdArgs=""

  # hide the options tab when the gui pops up
  cmdArgs="$cmdArgs -h options"

  # username
  cmdArgs="$cmdArgs -u $username"

  # execute tlclient
  if [[ "${password}" != "NULL" ]]; then
    # use '-P cat' to read in the password using stdin rather
    # than supplying it on command-line
    cmdArgs="$cmdArgs -P cat"
    # shellcheck disable=SC2086
    echo ${password} | ${TLCLIENT} ${cmdArgs} ${serverName} >/tmp/tlinc-${USER}-$$.log 2>&1 &
    res=$?
  else
    # shellcheck disable=SC2086
    ${TLCLIENT} ${cmdArgs} ${serverName} >/tmp/tlinc-${USER}-$$.log 2>&1 &
    res=$?
  fi

  # check return value of tlclient
  if [[ ${res} != 0 ]]; then
    echo "ERROR: ${TLCLIENT} returned invalid return code"
    exit 2
  fi
fi

exit 0
