TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_on release

LIBS	+= -lpcsclite -lldap -lssl -lcurl

DEFINES	+= LDAP_DEPRECATED

INCLUDEPATH	+= /usr/include/PCSC

HEADERS	+= pcsc.h \
	utregdlg.ui.h \
	c_ldap.h

SOURCES	+= pcsc.cpp \
	c_ldap.cpp

FORMS	= chooser.ui \
	utregdlg.ui \
	pindlg.ui \
	carddlg.ui

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}


