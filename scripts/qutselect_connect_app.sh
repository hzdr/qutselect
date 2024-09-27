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
#username="${9}"
app="${10}"

TMPDIR=/tmp

if [[ "${app}" == "zoom" ]]; then

  if [[ ! -d /opt/zoom ]]; then
    yad --center --text="Downloading zoom.pkg...." --title "Zoom Installation" --no-buttons &
    yad_pid=$!
    /usr/bin/wget -q "${BASE_PATH}/pkgs/zoom.pkg" -O "${TMPDIR}/zoom.pkg"
    kill -9 ${yad_pid}

    yad --center --text="Installing zoom.pkg...." --title "Zoom Installation" --no-buttons &
    yad_pid=$!
    tar -C / -xf "${TMPDIR}/zoom.pkg"
    rm -f "${TMPDIR}/zoom.pkg"
    kill -9 ${yad_pid}
  fi

  if [[ ! -x /opt/zoom/ZoomLauncher ]]; then
    yad --center --text="ERROR: Installation of Zoom failed"
  else
    yad --center --text="Starting zoom..." --no-buttons &
    yad_pid=$!

    # remove all previous data
    rm -rf ${HOME}/.zoom ${HOME}/.config/zoom*

    /opt/zoom/ZoomLauncher &
    kill -9 ${yad_pid}
  fi

elif [[ "${app}" == "chrome" ]] || [[ "${app}" == "bbb" ]]; then

  if [[ ! -d /opt/chrome ]]; then
    yad --center --text="Downloading chrome.pkg...." --title "Chrome Installation" --no-buttons &
    yad_pid=$!
    /usr/bin/wget -q "${BASE_PATH}/pkgs/chrome.pkg" -O "${TMPDIR}/chrome.pkg"
    kill -9 ${yad_pid}

    yad --center --text="Installing chrome.pkg...." --title "Chrome Installation" --no-buttons &
    yad_pid=$!
    tar -C / -xf "${TMPDIR}/chrome.pkg"
    rm -f "${TMPDIR}/chrome.pkg"
    kill -9 ${yad_pid}
  fi

  if [[ ! -x /opt/chrome/chrome ]]; then
    yad --center --text="ERROR: Installation of Chrome failed"
  else
    yad --center --text="Starting chrome..." --no-buttons &
    yad_pid=$!
    # remove all previous data
    rm -rf ${HOME}/.config/chrome

    # start chromium in kiosk mode
    if [[ "${app}" == "bbb" ]]; then
      /opt/chrome/chrome --app=https://bbb.hzdr.de --start-fullscreen --kiosk --test-type --noerrdialogs --no-first-run --disable-translate --disk-cache-dir=/dev/null --no-sandbox --disable-extensions &
    else
      /opt/chrome/chrome --start-maximized --test-type --noerrdialogs --no-first-run --disable-translate --disk-cache-dir=/dev/null --no-sandbox --disable-extensions &
    fi
    kill -9 ${yad_pid}
  fi

elif [[ "${app}" == "firefox" ]]; then

  # start the firefox browser
  if ! /bin/firefox-startup.sh; then
    exit 2
  fi
fi

exit 0
