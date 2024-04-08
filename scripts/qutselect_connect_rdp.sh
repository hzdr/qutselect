#!/bin/sh
# shellcheck shell=dash disable=SC3010,SC3020
#
# This is a startup script for qutselect which initates a
# RDP session to a windows server
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
#

RDESKTOP=/usr/bin/rdesktop
XFREERDP=/usr/local/bin/xfreerdp
TLSSOPASSWORD=/opt/thinlinc/bin/tl-sso-password
TLBESTWINSERVER=/opt/thinlinc/bin/tl-best-winserver
ZENITY=/bin/zenity

#####################################################
# check that we have 10 command-line options at hand
if [[ $# -lt 10 ]]; then
   echo "ERROR: missing arguments!"
   exit 2
fi

# catch all arguments is some local variables
#parentPID="${1}"
#serverType="${2}"
dtlogin="${3}"
resolution="${4}"
colorDepth="${5}"
#curDepth="${6}"
keyLayout="${7}"
domain="${8}"
username="${9}"
serverName="${10}"

# if this is a ThinLinc session we can grab the password
# using the tl-sso-password command in case the user wants
# to connect to one of our servers (FZR domain)
if [[ -x ${TLSSOPASSWORD} ]] &&
  ${TLSSOPASSWORD} -c 2>/dev/null; then
  if [[ "${domain}" = "FZR" ]]; then
    password=$(${TLSSOPASSWORD})
  fi
elif [[ -x ${ZENITY} ]]; then
  password=$(${ZENITY} --password --title="${serverName}" --timeout=20)
fi

# read the password from stdin if not specified yet
if [[ -z "${password}" ]]; then
  read -r password
fi

# if the serverName contains more than one server we go and
# check via the check_nrpe command which server to prefer
serverList=$(echo "${serverName}" | tr -s ',' ' ')
numServers=$(echo "${serverList}" | wc -w)
if [[ "${numServers}" -gt 1 ]]; then
  # check if we can find a suitable binary
  if [[ -x ${TLBESTWINSERVER} ]]; then
    # shellcheck disable=SC2086
    bestServer=$(${TLBESTWINSERVER} ${serverList})
    res=$?
  else
    # as an alternative we search for the tool in the scripts subdir
    # this tool also allows to override the username via -u
    if [[ -x "scripts/tl-best-winserver" ]]; then
      # shellcheck disable=SC2086
      bestServer=$(scripts/tl-best-winserver -u "${username}" ${serverList})
      res=$?
    else
      # we don't have tl-best-winserver so lets simply take the first
      # one in the list
      bestServer=$(echo "${serverList}" | awk '{ print $1 }')
      res=0
    fi
  fi
  if [[ $res -eq 0 ]]; then
    serverName=${bestServer}
  fi
fi

# variable to prepare the command arguments
cmdArgs=""
res=2

# now we find out which RDP client we use (xfreerdp/rdesktop)

## XFREERDP
# if $cmdArgs is empty and xfreerdp exists use that one
if [[ -z "${cmdArgs}" ]] && [[ -x ${XFREERDP} ]]; then

  # resolution
  if [[ "${resolution}" == "fullscreen" ]]; then
     cmdArgs="$cmdArgs /f"

     # enable multi monitor support, but only if the two displays
     # are not mirrored (offset = 0)
     for r in $(xrandr | grep " connected" | cut -d " " -f3); do
       x=$(echo "${r}" | cut -d "+" -f2)

       # check the x-offset for being non-zero and if so
       # enable multimon support
       if [[ $x -ne 0 ]]; then
         cmdArgs="$cmdArgs /multimon"
         break
       fi
     done

  else
     cmdArgs="$cmdArgs /size:${resolution}"
  fi

  # color depth
  #cmdArgs="$cmdArgs /bpp:${colorDepth}"
  cmdArgs="$cmdArgs /bpp:32"

  # keyboard
  if [[ "${keyLayout}" == "de" ]]; then
     cmdArgs="$cmdArgs /kbd:0x407" # German
  else
     cmdArgs="$cmdArgs /kbd:0x409" # US
  fi

  # add domain
  if [[ "${domain}" != "NULL" ]]; then
    cmdArgs="$cmdArgs /d:${domain}"
  else
    cmdArgs="$cmdArgs /d:FZR"
  fi

  # add username
  if [[ "${username}" != "NULL" ]]; then
    cmdArgs="$cmdArgs /u:${username}"
  elif [[ "${domain}" != "NULL" ]]; then
    cmdArgs="$cmdArgs /u:${domain}\\"
  fi

  # set the window title to the server name we connect to
  cmdArgs="$cmdArgs /t:${username}@${serverName}"

  # ignore the certificate in case of encryption
  cmdArgs="$cmdArgs /cert-ignore"

  # add the usb path as a local path. if TLSESSIONDATA is set
  # we are in a thinlinc session and thus have to forward
  # ${HOME}/thindrives/mnt instead
  if [ -n "${TLSESSIONDATA}" ]; then
    mkdir -p "${TLSESSIONDATA}/drives"
    cmdArgs="$cmdArgs /drive:USB,${TLSESSIONDATA}/drives/"
  else
    cmdArgs="$cmdArgs /drive:USB,/run/usbmount/"
  fi

  # enable sound redirection
  cmdArgs="$cmdArgs /sound:sys:pulse"

  # enable audio input redirection
  cmdArgs="$cmdArgs /microphone:sys:pulse"

  # performance optimization options
  cmdArgs="$cmdArgs +auto-reconnect +fonts +window-drag -menu-anims -themes +wallpaper +heartbeat /dynamic-resolution /gdi:hw /rfx /gfx:avc444 +gfx-thin-client /video /network:auto"

  # exception for old servers with weak security footprints
  if [[ "${serverName}" == "fwpdev01" ]]; then
    cmdArgs="$cmdArgs /tls-seclevel:0"
  fi

  # if we are not in dtlogin mode we go and
  # output the rdesktop line that is to be executed
  if [[ "${dtlogin}" != "true" ]]; then

    # add clipboard synchronization (only required in non-dtlogin mode)
    cmdArgs="$cmdArgs /clipboard"

    echo "${XFREERDP} ${cmdArgs} /v:${serverName}"
  else
    # disable the full-screen toggling in case we are in dtlogin mode
    cmdArgs="$cmdArgs -toggle-fullscreen"
  fi

  # increase logging
  cmdArgs="$cmdArgs /log-level:INFO"

  # run xfreerdp finally
  if [[ "${password}" != "NULL" ]]; then
    cmdArgs="$cmdArgs /from-stdin"
    # shellcheck disable=SC2086
    echo "${password}" | ${XFREERDP} ${cmdArgs} /v:"${serverName}" >/tmp/xfreerdp-${USER}-$$.log 2>&1 &
    res=$?
  else
    # shellcheck disable=SC2086
    ${XFREERDP} ${cmdArgs} /v:"${serverName}" >/tmp/xfreerdp-${USER}-$$.log 2>&1 &
    res=$?
  fi

  if [[ ${res} != 0 ]]; then
     cmdArgs=""
     echo "WARNING: couldn't start xfreerdp, retrying with other remote desktop"
  fi
fi

## RDESKTOP
# if $cmdArgs is empty and rdesktop exists use that one
if [[ -z "${cmdArgs}" ]] && [[ -x ${RDESKTOP} ]]; then

   # resolution
   if [[ "${resolution}" == "fullscreen" ]]; then
      cmdArgs="$cmdArgs -f"
   else
      cmdArgs="$cmdArgs -g ${resolution}"
   fi

   # color depth
   cmdArgs="$cmdArgs -a ${colorDepth}"

   # sound 
   cmdArgs="$cmdArgs -r sound:local"

   # enable LAN speed features
   cmdArgs="$cmdArgs -x lan"

   # add client name
   cmdArgs="$cmdArgs -n $(hostname)"

   # set the window title to the server name we connect to
   cmdArgs="$cmdArgs -T ${username}@${serverName}"

   # keyboard
   if [[ "${keyLayout}" == "de" ]]; then
      cmdArgs="$cmdArgs -k de"
   else
      cmdArgs="$cmdArgs -k en-US"
   fi

   # add domain
   if [[ "${domain}" != "NULL" ]]; then
     cmdArgs="$cmdArgs -d ${domain}"
   else
     cmdArgs="$cmdArgs -d FZR"
   fi

   # add username
   if [[ "${username}" != "NULL" ]]; then
     cmdArgs="$cmdArgs -u ${username}"
   else
     if [[ "${domain}" != "NULL" ]]; then
       cmdArgs="$cmdArgs -u ${domain}\\"
     fi
   fi

   # add the usb path as a local path. if TLSESSIONDATA is set
   # we are in a thinlinc session and thus have to forward
   # ${HOME}/thindrives/mnt instead
   if [[ -n "${TLSESSIONDATA}" ]]; then
     cmdArgs="$cmdArgs -r disk:USB=${TLSESSIONDATA}/drives/"
   else
     cmdArgs="$cmdArgs -r disk:USB=/mnt/$(hostname)"
   fi

   # if we are not in dtlogin mode we go and
   # output the rdesktop line that is to be executed
   if [[ "${dtlogin}" != "true" ]]; then
      echo "${RDESKTOP} ${cmdArgs} ${serverName}"
   fi

   # run rdesktop finally
   if [[ "${password}" != "NULL" ]]; then
     cmdArgs="$cmdArgs -p -"
     # shellcheck disable=SC2086
     echo ${password} | ${RDESKTOP} ${cmdArgs} "${serverName}" >/tmp/rdesktop-${USER}-$$.log 2>&1 &
     res=$?
   else
     # shellcheck disable=SC2086
     ${RDESKTOP} ${cmdArgs} "${serverName}" >/tmp/rdesktop-${USER}-$$.log 2>&1 &
     res=$?
   fi

   if [[ ${res} != 0 ]]; then
      echo "ERROR: rdesktop returned invalid return code"
   fi
fi

exit ${res}
