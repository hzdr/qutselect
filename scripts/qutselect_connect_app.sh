#!/bin/sh
# shellcheck shell=sh disable=SC2030,SC2031
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
if [ "$#" -lt 10 ]; then
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
MACHINE=$(uname -m)

# Browser used for BigBlueButton sessions: "chrome" or "firefox".
# The environment may override this value without modifying the script.
BBB_BROWSER="${BBB_BROWSER:-firefox}"

if [ "${app}" = "bbb" ] && \
   [ "${BBB_BROWSER}" != "chrome" ] && [ "${BBB_BROWSER}" != "firefox" ]; then
  printf "ERROR: invalid BBB_BROWSER '%s' (expected chrome or firefox)\n" "${BBB_BROWSER}"
  exit 2
fi

# font descriptions
YAD_FONT_DESC="${YAD_FONT_DESC:-DejaVu Sans 18}"
OSD_FONT_DESC="${OSD_FONT_DESC:--*-fixed-bold-r-*-*-30-*-*-*-*-*-*-*}"

# osd_cat usage helpers
osd_pid=

osd_splash() {
  osd_msg="$1"
  osd_color="${2:-orange}"
  osd_time="${3:-0}"
  if [ -x /usr/bin/osd_cat ]; then
    export LANG=en_US.UTF-8
    echo "${osd_msg}" | /usr/bin/osd_cat -A center -p top -f "${OSD_FONT_DESC}" -c "${osd_color}" -s 5 -d "${osd_time}" &
    osd_pid=$!
  else
    osd_pid=
  fi
}

osd_close() {
  if [ -n "${osd_pid}" ]; then
    kill "${osd_pid}" 2>/dev/null
    wait "${osd_pid}" 2>/dev/null
    osd_pid=
  fi
}
trap 'osd_close' EXIT INT TERM

res=2
if [ "${app}" = "zoom" ]; then

  if [ ! -d /opt/zoom ]; then
    osd_splash "Downloading zoom.pkg..."
    /usr/bin/wget -q "${BASE_PATH}/pkgs/zoom-${MACHINE}.pkg" -O "${TMPDIR}/zoom.pkg"
    osd_close

    osd_splash "Installing zoom.pkg..."
    tar -C / -xf "${TMPDIR}/zoom.pkg"
    rm -f "${TMPDIR}/zoom.pkg"
    osd_close
  fi

  if [ ! -x /opt/zoom/ZoomLauncher ]; then
    osd_splash "ERROR: Installation of Zoom failed" "red" 3
    wait "${osd_pid}" 2>/dev/null
    exit 1
  else
    osd_splash "Starting zoom..."

    # make sure to kill any old zoom
    pkill zoom

    # remove all previous data
    rm -rf "${HOME}/.zoom" "${HOME}/.config/zoom.conf" "${HOME}/.config/zoomus.conf"

    # try to avoid that zoom stays open upon closing the window
    {
      echo "[General]"
      echo "forceEnableTrayIcon=false"
      echo "showSystemTitlebar=true"
      echo "sso_domain=hzdr-de.zoom.us"
      echo "useSystemTheme=true"
      echo "timeFormat12HoursEnable=false"
      echo "bForceMaximizeWM=true"
    } >"${HOME}/.config/zoomus.conf"

    # add manual proxy settings in case HTTP_PROXY is set
    if [ -n "${HTTP_PROXY}" ] || [ -n "${HTTPS_PROXY}" ]; then
      http_host=$(echo "${HTTP_PROXY}" | cut -d/ -f3 | cut -d: -f1)
      http_port=$(echo "${HTTP_PROXY}" | cut -d: -f3)
      https_host=$(echo "${HTTPS_PROXY}" | cut -d/ -f3 | cut -d: -f1)
      https_port=$(echo "${HTTPS_PROXY}" | cut -d: -f3)
      {
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
      } >>"${HOME}/.config/zoomus.conf"
    fi

    USER="Enter your name here" /opt/zoom/ZoomLauncher >"/tmp/zoom-${USER}-$$.log" 2>&1 &
    res=$?

    sleep 4
    osd_close
  fi

elif [ "${app}" = "chrome" ] || [ "${app}:${BBB_BROWSER}" = "bbb:chrome" ]; then

  if [ ! -d /opt/chrome ]; then
    osd_splash "Downloading chrome.pkg..."
    /usr/bin/wget -q "${BASE_PATH}/pkgs/chrome-${MACHINE}.pkg" -O "${TMPDIR}/chrome.pkg"
    osd_close

    osd_splash "Installing chrome.pkg..."
    tar -C / -xf "${TMPDIR}/chrome.pkg"
    rm -f "${TMPDIR}/chrome.pkg"
    osd_close
  fi

  if [ ! -x /opt/chrome/chrome ]; then
    osd_splash "ERROR: Installation of Chrome failed" "red" 3
    wait "${osd_pid}" 2>/dev/null
    exit 1
  else
    osd_splash "Starting chrome..."

    # remove all previous data
    rm -rf "${HOME}/.config/chrome" "${HOME}/.config/chromium"

    if [ "${resolution}" = "fullscreen" ]; then
      CMDOPT="--start-fullscreen"
    else
      CMDOPT="--start-maximized"
    fi

    # add some options for BBB mode
    if [ "${app}" = "bbb" ]; then
      (
        # when we start a BBB session we have to ask for a
        # potential room identifier
        BBB_BASE="https://bbb.hzdr.de/"
        bbb_url="${BBB_BASE}"
        room=""

        while true; do
          bbb_url="${BBB_BASE}"
          room="$(
            yad --entry \
                --title="https://bbb.hzdr.de/b/xxx-xxx-xxx-xxx" \
                --entry-text="${room}" \
                --text="<span font_desc=\"${YAD_FONT_DESC}\"><b>Please enter BBB Room-ID:</b></span>\n\nFormat: xxx-xxx-xxx-xxx (part after /b/ in link)" \
                --window-type=dialog --on-top --sticky --center --fixed \
                --button="OK":0 --button="No Room ID":1 --button="Cancel":2
          )"
          rc=$?

          if [ "$rc" -eq 2 ] || [ "$rc" -eq 252 ]; then
            exit 0
          elif [ "$rc" -eq 1 ] || [ -z "${room}" ]; then
            room=""
            break
          fi

          # Optional: remote whitespaces and if someone added the url, extract it
          room="$(printf '%s' "${room}" | tr -d '[:space:]')"
          room="${room#"${BBB_BASE}"b/}"
          #room=$(echo "${room}" | LC_ALL=C tr '[:upper:]' '[:lower:]')

          # now try to access the supplied URL and continue only if a 200
          code=0
          if [ -n "${room}" ]; then
            bbb_url="${BBB_BASE}b/${room}"
            code="$(curl -sS -o /dev/null -L \
                         --connect-timeout 5 --max-time 15 \
                         -w '%{http_code}' \
                         "${bbb_url}"
                   )" || code=600

            # http-error: 4xx/5xx
            if [ "${code}" -lt 400 ]; then
              break
            fi
          fi

          yad --error \
              --title="Invalid Room-ID (${code}: ${bbb_url})" \
              --text="<span font_desc=\"${YAD_FONT_DESC}\"><b>The entered Room-ID was invalid.</b></span>\n\nExpected: xxx-xxx-xxx-xxx (each 3 alphanumerical characters)." \
              --window-type=dialog --on-top --center --fixed \
              --button="Retry":0
        done

        CMDOPT="${CMDOPT} --app=${bbb_url}"
        if [ "${resolution}" = "fullscreen" ]; then
          CMDOPT="${CMDOPT} --kiosk"
        fi

        # shellcheck disable=SC2086
        /opt/chrome/chrome ${CMDOPT} --test-type --noerrdialogs --no-first-run --disable-translate --disk-cache-dir=/dev/null --disable-extensions >"/tmp/chrome-${USER}-$$.log" 2>&1 &
        osd_close
      ) </dev/null >/dev/null 2>&1 &

      # parent should return immediately
      sleep 4
      exit 0
    else
      /opt/chrome/chrome ${CMDOPT} --test-type --noerrdialogs --no-first-run --disable-translate --disk-cache-dir=/dev/null --disable-extensions >"/tmp/chrome-${USER}-$$.log" 2>&1 &
      res=$?

      sleep 4
      osd_close
    fi
  fi

elif [ "${app}" = "firefox" ] || [ "${app}:${BBB_BROWSER}" = "bbb:firefox" ]; then

  # Download the unmodified current Mozilla tar.xz archive.
  # The archive already contains the top-level directory "firefox".
  if [ ! -x /opt/firefox/firefox ]; then

    osd_splash "Downloading firefox.pkg..."
    if ! /usr/bin/wget -q "${BASE_PATH}/pkgs/firefox-${MACHINE}.pkg" -O "${TMPDIR}/firefox.pkg"; then
      osd_close
      rm -f "${TMPDIR}/firefox.pkg"
      osd_splash "ERROR: Download of firefox.pkg failed" "red" 3
      wait "${osd_pid}" 2>/dev/null
      exit 1
    fi
    osd_close

    osd_splash "Installing firefox.pkg..."
    if ! tar -C /opt -xf "${TMPDIR}/firefox.pkg"; then
      osd_close
      rm -f "${TMPDIR}/firefox.pkg"
      osd_splash "ERROR: Installation of firefox.pkg" "red" 3
      wait "${osd_pid}" 2>/dev/null
      exit 1
    fi
    rm -f "${TMPDIR}/firefox.pkg"
    osd_close
  fi

  if [ ! -x /opt/firefox/firefox ]; then
    osd_splash "ERROR: Installation of firefox.pkg failed" "red" 3
    wait "${osd_pid}" 2>/dev/null
    exit 1
  else
    osd_splash "Starting firefox..."

    # Always use a clean, explicitly selected profile. On thinRoot this is
    # already volatile because HOME resides in RAM.
    FIREFOX_PROFILE="${HOME}/.mozilla/firefox/thinroot"
    rm -rf "${FIREFOX_PROFILE}"
    mkdir -p "${FIREFOX_PROFILE}"

    # Apply kiosk, cache and process preferences to the fresh profile.
    {
      # Disable the HTTP disk cache completely.
      echo 'user_pref("browser.cache.disk.enable", false);'
      echo 'user_pref("browser.cache.disk.smart_size.enabled", false);'
      echo 'user_pref("browser.cache.disk.capacity", 0);'

      # Retain a small and explicitly limited memory cache.
      echo 'user_pref("browser.cache.memory.enable", true);'
      echo 'user_pref("browser.cache.memory.capacity", 32768);'
      echo 'user_pref("browser.cache.memory.max_entry_size", 4096);'

      # Always run in private mode.
      echo 'user_pref("browser.privatebrowsing.autostart", true);'

      # Keep video/audio caching in memory and limit its size.
      echo 'user_pref("browser.privatebrowsing.forceMediaMemoryCache", true);'
      echo 'user_pref("media.memory_cache_max_size", 4096);'
      echo 'user_pref("media.memory_caches_combined_limit_kb", 65536);'
      echo 'user_pref("media.memory_caches_combined_limit_pc_sysmem", 2);'
      echo 'user_pref("media.cache_readahead_limit", 15);'
      echo 'user_pref("media.cache_resume_threshold", 5);'

      # Reduce the decoded image surface cache.
      echo 'user_pref("image.mem.surfacecache.max_size_kb", 262144);'
      echo 'user_pref("image.mem.surfacecache.size_factor", 8);'

      # Do not restore previous sessions.
      echo 'user_pref("browser.sessionstore.resume_from_crash", false);'
      echo 'user_pref("browser.sessionstore.max_tabs_undo", 0);'
      echo 'user_pref("browser.sessionstore.max_windows_undo", 0);'

      # Suppress first-run/default-browser UI and unnecessary features.
      echo 'user_pref("browser.shell.checkDefaultBrowser", false);'
      echo 'user_pref("browser.aboutwelcome.enabled", false);'
      echo 'user_pref("browser.translations.enable", false);'
      echo 'user_pref("xpinstall.enabled", false);'

      # Reduce content processes and disable process preallocation.
      echo 'user_pref("dom.ipc.processCount", 2);'
      echo 'user_pref("dom.ipc.processPrelaunch.enabled", false);'
      echo 'user_pref("dom.ipc.processPrelaunch.fission.number", 0);'
    } >"${FIREFOX_PROFILE}/user.js"

    if [ ! -e /opt/firefox/distribution/policies.json ]; then
      mkdir -p /opt/firefox/distribution
      {
        echo '{'
        echo '  "policies": {'
        echo '    "OverrideFirstRunPage": "",'
        echo '    "OverridePostUpdatePage": "",'
        echo '    "SkipTermsOfUse": true,'
        echo '    "DontCheckDefaultBrowser": true'
        echo '  }'
        echo '}'
      } > /opt/firefox/distribution/policies.json
    fi

    if [ "${app}" = "bbb" ]; then
      (
        # Ask for an optional BBB room identifier just like the Chrome path.
        BBB_BASE="https://bbb.hzdr.de/"
        bbb_url="${BBB_BASE}"
        room=""

        while true; do
          bbb_url="${BBB_BASE}"
          room="$(
            yad --entry \
                --title="https://bbb.hzdr.de/b/xxx-xxx-xxx-xxx" \
                --entry-text="${room}" \
                --text="<span font_desc=\"${YAD_FONT_DESC}\"><b>Please enter BBB Room-ID:</b></span>\n\nFormat: xxx-xxx-xxx-xxx (part after /b/ in link)" \
                --window-type=dialog --on-top --sticky --center --fixed \
                --button="OK":0 --button="No Room ID":1 --button="Cancel":2
          )"
          rc=$?

          if [ "$rc" -eq 2 ] || [ "$rc" -eq 252 ]; then
            exit 0
          elif [ "$rc" -eq 1 ] || [ -z "${room}" ]; then
            room=""
            break
          fi

          # Remove whitespace and an optionally supplied complete BBB URL.
          room="$(printf '%s' "${room}" | tr -d '[:space:]')"
          room="${room#"${BBB_BASE}"b/}"

          code=0
          if [ -n "${room}" ]; then
            bbb_url="${BBB_BASE}b/${room}"
            code="$(curl -sS -o /dev/null -L \
                         --connect-timeout 5 --max-time 15 \
                         -w '%{http_code}' \
                         "${bbb_url}"
                   )" || code=600

            if [ "${code}" -lt 400 ]; then
              break
            fi
          fi

          yad --error \
              --title="Invalid Room-ID (${code}: ${bbb_url})" \
              --text="<span font_desc=\"${YAD_FONT_DESC}\"><b>The entered Room-ID was invalid.</b></span>\n\nExpected: xxx-xxx-xxx-xxx (each 3 alphanumerical characters)." \
              --window-type=dialog --on-top --center --fixed \
              --button="Retry":0
        done

        FIREFOX_CMDOPT=""
        if [ "${resolution}" = "fullscreen" ]; then
          # BBB uses the locked-down Firefox kiosk mode in fullscreen mode.
          FIREFOX_CMDOPT="--kiosk"
        fi

        # shellcheck disable=SC2086
        MOZ_CRASHREPORTER_DISABLE=1 \
          /opt/firefox/firefox ${FIREFOX_CMDOPT} \
          --no-remote \
          --profile "${FIREFOX_PROFILE}" \
          --private-window "${bbb_url}" \
          >"/tmp/firefox-${USER}-$$.log" 2>&1 &

        osd_close
      ) </dev/null >/dev/null 2>&1 &

      # The parent should return immediately while the room dialog and Firefox
      # continue in the background.
      sleep 4
      exit 0
    else
      # A normal Firefox session uses reversible F11 fullscreen, not kiosk
      # mode. Firefox has no native --start-fullscreen command-line option.
      MOZ_CRASHREPORTER_DISABLE=1 \
        /opt/firefox/firefox \
        --no-remote \
        --profile "${FIREFOX_PROFILE}" \
        --private-window "https://google.de/" \
        >"/tmp/firefox-${USER}-$$.log" 2>&1 &

      res=$?
      firefox_pid=$!

      if [ "${res}" -eq 0 ] && [ "${resolution}" = "fullscreen" ]; then
        (
          firefox_window=""

          if command -v xdotool >/dev/null 2>&1; then
            # Wait up to approximately 20 seconds for Firefox's X11 window.
            i=0
            while [ "${i}" -lt 20 ]; do
              firefox_window="$(
                xdotool search --onlyvisible --pid "${firefox_pid}" 2>/dev/null |
                  head -n 1
              )"

              # The launcher may have handed off to firefox-bin and changed
              # the PID associated with the window. Fall back to WM_CLASS.
              if [ -z "${firefox_window}" ]; then
                firefox_window="$(
                  xdotool search --onlyvisible --class firefox 2>/dev/null |
                    tail -n 1
                )"
              fi

              if [ -n "${firefox_window}" ]; then
                break
              fi

              sleep 1
              i=$((i + 1))
            done

            if [ -n "${firefox_window}" ]; then
              xdotool windowactivate --sync "${firefox_window}"
              xdotool key --window "${firefox_window}" F11
            fi
          else
            printf "WARNING: xdotool not found; Firefox cannot start in reversible fullscreen mode\n" \
              >>"/tmp/firefox-${USER}-$$.log"
          fi
        ) &
      fi

      sleep 4
      osd_close
    fi
  fi
fi

exit ${res}
