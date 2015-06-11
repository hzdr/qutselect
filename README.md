[![Build Status](https://travis-ci.org/hzdr/qutselect.svg?branch=master)](https://travis-ci.org/hzdr/qutselect)

# qutselect
This is a Qt-based terminal server selection user interface which presents a list of terminal servers (RDP, ThinLinc, VNC, etc.) to which users can connect via connecting applications (xfreerdp, rdesktop, vncviewer, tlclient, etc.). While the supported services are encoded in the Qt-application, the calls to the individual applications are performed via external shell scripts which allows to have some kind of flexibility. It can be used in kiosk-type environemnts (e.g. Thinstation) but also be used in interactive sessions within a window manager.

# License
qutselect is hereby released under LGPL license.

# Authors
Jens Maus – <mail@jens-maus.de>
