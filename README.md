[![Current Release](https://img.shields.io/github/release/hzdr/qutselect.svg)](https://github.com/hzdr/qutselect/releases/latest)
[![Downloads](https://img.shields.io/github/downloads/hzdr/qutselect/latest/total.svg)](https://github.com/hzdr/qutselect/releases/latest)
[![CI Build](https://github.com/hzdr/qutselect/workflows/CI%20Build/badge.svg)](https://github.com/hzdr/qutselect/actions)
[![License](https://img.shields.io/github/license/hzdr/qutselect.svg)](https://github.com/hzdr/qutselect/blob/master/LICENSE)

# qutselect
This is a Qt-based terminal server selection user interface which presents a list of terminal servers (RDP, ThinLinc, VNC, etc.) to which users can connect via connecting applications (xfreerdp, rdesktop, vncviewer, tlclient, etc.). While the supported services are encoded in the Qt-application, the calls to the individual applications are performed via external shell scripts which allows to have some kind of flexibility. It can be used in kiosk-type environemnts (e.g. Thinstation) but also be used in interactive sessions within a window manager.

# License
qutselect is hereby released under LGPL license.

# Authors
Jens Maus – <mail@jens-maus.de>
