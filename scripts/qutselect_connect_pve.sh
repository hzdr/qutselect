#!/bin/bash
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
# $8 = the domain (e.g. 'FZR', used for RDP) or proxy server (for PVE)
# $9 = the username
# $10 = the servername (hostname) to connect to
#

if [[ -x /usr/local/bin/remote-viewer ]]; then
  REMOTEVIEWER=/usr/local/bin/remote-viewer
elif [[ -x /usr/bin/remote-viewer ]]; then
  REMOTEVIEWER=/usr/bin/remote-viewer
else
  REMOTEVIEWER=remote-viewer
fi

TLSSOPASSWORD=/opt/thinlinc/bin/tl-sso-password
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
#colorDepth="${5}"
#curDepth="${6}"
#keyLayout="${7}"
proxy="${8}"
username="${9}@hzdr.de"
serverName="${10}"

# fallback to fwppve.fz-rossendorf.de as PVE proxy
if [[ -z ${proxy} ]] || [[ "${proxy}" == "NULL" ]]; then
  proxy=fwppve.fz-rossendorf.de
fi

# if this is a ThinLinc session we can grab the password
# using the tl-sso-password command in case the user wants
# to connect to one of our servers (FZR domain)
if [[ -x ${TLSSOPASSWORD} ]] &&
  ${TLSSOPASSWORD} -c 2>/dev/null; then
  password=$(${TLSSOPASSWORD})
fi

# read the password from stdin if not specified yet
if [[ -z "${password}" ]]; then
  read -r password
fi

# if we still have not a password yet we query it from the user via
# zenity
if [[ -x ${ZENITY} ]] && [[ "${password}" == "NULL" ]]; then
  password=$(${ZENITY} --password --title="${username}@${serverName}" --timeout=20)
fi

# variable to prepare the command arguments
cmdArgs=""
res=2

## REMOTEVIEWER

# resolution
if [[ "${resolution}" == "fullscreen" ]]; then
  cmdArgs="$cmdArgs --full-screen"
fi

# add the usb path as a local path. if TLSESSIONDATA is set
# we are in a thinlinc session and thus have to forward
# ${HOME}/thindrives/mnt instead
if [[ -n "${TLSESSIONDATA}" ]]; then
  mkdir -p "${TLSESSIONDATA}/drives"
  cmdArgs="$cmdArgs --spice-shared-dir=${TLSESSIONDATA}/drives/"
else
  cmdArgs="$cmdArgs --spice-shared-dir=/run/usbmount/"
fi

#####
# AUTHENTICATION
# get access ticket and csrf from PVE
if ! RESPONSE=$(curl -L -f -s -S -k -d "username=${username}&password=${password}"  "https://${proxy}/api2/json/access/ticket") || [[ -z "${RESPONSE}" ]]; then
  echo "ERROR: authentication failed"
  exit 1
fi

# extract ticket and csrf
TICKET=$(echo "${RESPONSE}" | jq -r '.data.ticket')
CSRF=$(echo "${RESPONSE}" | jq -r '.data.CSRFPreventionToken')
if [[ -z "${TICKET}" ]] || [[ -z "${CSRF}" ]]; then
  echo "ERROR: could not retrieve authentication ticket/csrf"
	exit 1
fi

#####
# GET VM INFO from cluster
if ! RESPONSE=$(curl -L -f -s -S -k -b "PVEAuthCookie=${TICKET}" -H "CSRFPreventionToken: ${CSRF}" "https://${proxy}/api2/json/cluster/resources") || [[ -z "${RESPONSE}" ]]; then
  echo "ERROR: vm info request for ${serverName} failed"
  exit 1
fi

# search in response for vmid und vmnode depending on if
# the servername was a name or simply vm number
if [[ ! ${serverName} =~ ^[0-9]+$ ]]; then
  # search by name
  VMID=$(echo "${RESPONSE}" | jq -r ".data[] | select(.name==\"${serverName}\") | .vmid")
  VMNAME=$(echo "${RESPONSE}" | jq -r ".data[] | select(.name==\"${serverName}\") | .name")
  VMNODE=$(echo "${RESPONSE}" | jq -r ".data[] | select(.name==\"${serverName}\") | .node")
  VMSTATUS=$(echo "${RESPONSE}" | jq -r ".data[] | select(.name==\"${serverName}\") | .status")
  VMTYPE=$(echo "${RESPONSE}" | jq -r ".data[] | select(.name==\"${serverName}\") | .type")
else
  # search by id
  VMID=$(echo "${RESPONSE}" | jq -r ".data[] | select(.vmid==${serverName}) | .vmid")
  VMNAME=$(echo "${RESPONSE}" | jq -r ".data[] | select(.vmid==${serverName}) | .name")
  VMNODE=$(echo "${RESPONSE}" | jq -r ".data[] | select(.vmid==${serverName}) | .node")
  VMSTATUS=$(echo "${RESPONSE}" | jq -r ".data[] | select(.vmid==${serverName}) | .status")
  VMTYPE=$(echo "${RESPONSE}" | jq -r ".data[] | select(.vmid==${serverName}) | .type")
fi

if [[ -z ${VMID} ]] || [[ -z ${VMNODE} ]] || [[ -z ${VMSTATUS} ]] || [[ -z ${VMNAME} ]] || [[ -z ${VMTYPE} ]]; then
  echo "ERROR: could not get all info for ${serverName}"
  exit 1
fi

#####
# START VM (if not running yet)
if [[ ${VMSTATUS} == "stopped" ]]; then
  echo "WARNING: VM not running. Trying to start"
  RESPONSE=$(curl -L -d "" -f -s -S -k -b "PVEAuthCookie=${TICKET}" -H "CSRFPreventionToken: ${CSRF}" "https://${VMNODE}:8006/api2/json/nodes/${VMNODE}/${VMTYPE}/${VMID}/status/start")
  echo "Waiting 10 seconds before trying Spice connection ..."
  sleep 10
fi

#####
# GET SPICE CONFIGURATION
if ! RESPONSE=$(curl -L -f -s -S -k -b "PVEAuthCookie=${TICKET}" -H "CSRFPreventionToken: ${CSRF}" "https://${VMNODE}:8006/api2/json/nodes/${VMNODE}/${VMTYPE}/${VMID}/spiceproxy" -d "proxy=${VMNODE}") || [[ -z ${RESPONSE} ]]; then
  echo "ERROR: could not get spice config (mayba proxmox api changed?)"
  exit 1
fi

# PARSING JSON RESPONSE
SPICE_ATTENTION=$(echo "${RESPONSE}" | jq -r '.data."secure-attention"')
SPICE_DELETE=$(echo "${RESPONSE}" | jq -r '.data."delete-this-file"')
SPICE_PROXY=$(echo "${RESPONSE}" | jq -r '.data.proxy')
SPICE_TYPE=$(echo "${RESPONSE}" | jq -r '.data.type')
SPICE_CA=$(echo "${RESPONSE}" | jq -r '.data.ca')
SPICE_FULLSCREEN=$(echo "${RESPONSE}" | jq -r '.data."toggle-fullscreen"')
SPICE_TITLE=$(echo "${RESPONSE}" | jq -r '.data.title')
SPICE_HOST=$(echo "${RESPONSE}" | jq -r '.data.host')
SPICE_PASSWORD=$(echo "${RESPONSE}" | jq -r '.data.password')
SPICE_SUBJECT=$(echo "${RESPONSE}" | jq -r '.data."host-subject"')
SPICE_CURSOR=$(echo "${RESPONSE}" | jq -r '.data."release-cursor"')
SPICE_PORT=$(echo "${RESPONSE}" | jq -r '.data."tls-port"')

# if we are in dtlogin mode we we disable the
# full-screen toggling and release cursor hotkeys
if [[ "${dtlogin}" == "true" ]]; then
  SPICE_FULLSCREEN=
  SPICE_CURSOR=
  cmdArgs="${cmdArgs} --kiosk --kiosk-quit=on-disconnect"
fi

# GENERATE REMOTE-VIEWER CONNECTION FILE
umask 077
TMPFILE=$(mktemp)
cat >"${TMPFILE}" <<EOL
[virt-viewer]
secure-attention=${SPICE_ATTENTION}
delete-this-file=${SPICE_DELETE}
proxy=${SPICE_PROXY}
type=${SPICE_TYPE}
ca=${SPICE_CA}
toggle-fullscreen=${SPICE_FULLSCREEN}
title=${SPICE_TITLE}
host=${SPICE_HOST}
password=${SPICE_PASSWORD}
host-subject=${SPICE_SUBJECT}
release-cursor=${SPICE_CURSOR}
tls-port=${SPICE_PORT}
EOL

# shellcheck disable=SC2086
${REMOTEVIEWER} ${cmdArgs} "${TMPFILE}" &
res=$?

if [[ ${res} != 0 ]]; then
  echo "ERROR: couldn't start remote-viewer"
fi

exit ${res}
