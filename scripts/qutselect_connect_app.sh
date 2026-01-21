#!/bin/bash
# shellcheck shell=dash disable=SC3010,SC3020,SC3057,SC3004,SC3060,SC2030,SC2031
#
# This is a startup script for qutselect which initates a
# third-party application download+install in a thinRoot
# environment where qutselect is used.
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
resolution="${4}"
#colorDepth="${5}"
#curDepth="${6}"
#keyLayout="${7}"
#domain="${8}"
#username="${9}"
app="${10}"

# make sure files are generated for user only
umask 077

TMPDIR=/tmp

# font descriptions
YAD_FONT_DESC="${YAD_FONT_DESC:-DejaVu Sans 18}"
OSD_FONT_DESC="${OSD_FONT_DESC:--*-fixed-bold-r-*-*-30-*-*-*-*-*-*-*}"

# osd_cat usage helpers
osd_pid=

osd_splash() {
  local msg="$1"
  local color="${2:-orange}"
  local time="${3:-0}"
  if [ -x /usr/bin/osd_cat ]; then
    export LANG=en_US.UTF-8
    echo "${msg}" | /usr/bin/osd_cat -A center -p top -f "${OSD_FONT_DESC}" -c "${color}" -s 5 -d "${time}" &
    osd_pid=$!
  else
    osd_pid=
  fi
}

osd_close() {
  if [[ -n "${osd_pid}" ]]; then
    kill "${osd_pid}" 2>/dev/null
    wait "${osd_pid}" 2>/dev/null
    osd_pid=
  fi
}
trap 'osd_close' EXIT INT TERM

res=2
if [[ "${app}" == "zoom" ]]; then

  if [[ ! -d /opt/zoom ]]; then
    osd_splash "Downloading zoom.pkg..."
    /usr/bin/wget -q "${BASE_PATH}/pkgs/zoom.pkg" -O "${TMPDIR}/zoom.pkg"
    osd_close

    osd_splash "Installing zoom.pkg..."
    tar -C / -xf "${TMPDIR}/zoom.pkg"
    rm -f "${TMPDIR}/zoom.pkg"
    osd_close
  fi

  if [[ ! -x /opt/zoom/ZoomLauncher ]]; then
    osd_splash "ERROR: Installation of Zoom failed" "red" 3
    wait "${osd_pid}" 2>/dev/null
    exit 1
  else
    osd_splash "Starting zoom..."

    # remove all previous data
    rm -rf "${HOME}/.zoom" "${HOME}/.config/zoom.conf" "${HOME}/.config/zoomus.conf"

    # add manual proxy settings in case HTTP_PROXY is set
    if [[ -n "${HTTP_PROXY}" ]] || [[ -n "${HTTPS_PROXY}" ]]; then
      http_host=$(echo "${HTTP_PROXY}" | cut -d/ -f3 | cut -d: -f1)
      http_port=$(echo "${HTTP_PROXY}" | cut -d: -f3)
      https_host=$(echo "${HTTPS_PROXY}" | cut -d/ -f3 | cut -d: -f1)
      https_port=$(echo "${HTTPS_PROXY}" | cut -d: -f3)
      {
        echo "[General]"
        echo "cefhttpProxyHost=${http_host}"
        echo "cefhttpProxyPort=${http_port}"
        echo "cefhttpsProxyHost=${https_host}"
        echo "cefhttpsProxyPort=${https_port}"
        echo "cefproxyType=manual"
        echo "httpProxyHost=${http_host}"
        echo "httpProxyPort=${http_port}"
        echo "httpsProxyHost=${https_host}"
        echo "httpsProxyPort=${https_port}"
        echo "proxyType=manual"
      } >"${HOME}/.config/zoomus.conf"
    fi

    USER="Enter your name here" /opt/zoom/ZoomLauncher >"/tmp/zoom-${USER}-$$.log" 2>&1 &
    res=$?

    osd_close
  fi

elif [[ "${app}" == "chrome" ]] || [[ "${app}" == "bbb" ]]; then

  if [[ ! -d /opt/chrome ]]; then
    osd_splash "Downloading chrome.pkg..."
    /usr/bin/wget -q "${BASE_PATH}/pkgs/chrome.pkg" -O "${TMPDIR}/chrome.pkg"
    osd_close

    osd_splash "Installing chrome.pkg..."
    tar -C / -xf "${TMPDIR}/chrome.pkg"
    rm -f "${TMPDIR}/chrome.pkg"
    osd_close
  fi

  if [[ ! -x /opt/chrome/chrome ]]; then
    osd_splash "ERROR: Installation of Chrome failed" "red" 3
    wait "${osd_pid}" 2>/dev/null
    exit 1
  else
    osd_splash "Starting chrome..."

    # remove all previous data
    rm -rf "${HOME}/.config/chrome"

    if [[ "${resolution}" == "fullscreen" ]]; then
      CMDOPT="--start-fullscreen"
    else
      CMDOPT="--start-maximized"
    fi

    # add some options for BBB mode
    if [[ "${app}" == "bbb" ]]; then
      (
        # when we start a BBB session we have to ask for a
        # potential room identifier
        BBB_BASE="https://bbb.hzdr.de/"
        ROOM_RE='^[A-Za-z0-9]{3}(-[A-Za-z0-9]{3}){3}$'   # xxx-xxx-xxx-xxx
        room=""

        while true; do
          room="$(
            yad --entry \
                --title="https://bbb.hzdr.de/b/xxx-xxx-xxx-xxx" \
                --entry-text="${room}" \
                --text="<span font_desc=\"${YAD_FONT_DESC}\"><b>Please enter BBB Room-ID:</b></span>\n\nFormat: xxx-xxx-xxx-xxx (part after /b/ in link)" \
                --window-type=dialog --on-top --sticky --center --fixed \
                --button="OK":0 --button="No Room ID":1 --button="Cancel":2
          )"
          rc=$?

          if [[ $rc -eq 2 ]] || [[ $rc -eq 252 ]]; then
            exit 0
          elif [[ $rc -eq 1 ]] || [[ -z "${room}" ]]; then
            room=""
            break
          fi

          # Optional: remote whitespaces and if someone added the url, extract it
          room="${room//[[:space:]]/}"
          room="${room#"${BBB_BASE}"b/}" # removes "https://bbb.hzdr.de/b/" if there
          room=$(echo "${room}" | LC_ALL=C tr '[:upper:]' '[:lower:]')

          if [[ "$room" =~ $ROOM_RE ]]; then
            break
          fi

          yad --error \
              --title="Invalid Room-ID" \
              --text="<span font_desc=\"${YAD_FONT_DESC}\"><b>The entered Room-ID was invalid.</b></span>\n\nExpected: xxx-xxx-xxx-xxx (each 3 alphanumerical characters)." \
              --window-type=dialog --on-top --center --fixed \
              --button="Retry":0
        done

        if [[ -n "${room}" ]]; then
          bbb_url="${BBB_BASE}b/${room}"
        else
          bbb_url="${BBB_BASE}"
        fi

        CMDOPT="${CMDOPT} --app=${bbb_url}"
        if [[ "${resolution}" == "fullscreen" ]]; then
          CMDOPT="${CMDOPT} --kiosk"
        fi

        # shellcheck disable=SC2086
        /opt/chrome/chrome ${CMDOPT} --test-type --noerrdialogs --no-first-run --disable-translate --disk-cache-dir=/dev/null --disable-extensions >"/tmp/chrome-${USER}-$$.log" 2>&1 &
        osd_close
      ) </dev/null >/dev/null 2>&1 &

      # parent should return immediately
      exit 0
    else
      /opt/chrome/chrome ${CMDOPT} --test-type --noerrdialogs --no-first-run --disable-translate --disk-cache-dir=/dev/null --disable-extensions >"/tmp/chrome-${USER}-$$.log" 2>&1 &
      res=$?
      osd_close
    fi
  fi

elif [[ "${app}" == "firefox" ]]; then

  # start the firefox browser
  if ! /bin/firefox-startup.sh; then
    exit 2
  else
    res=0
  fi
fi

exit ${res}
