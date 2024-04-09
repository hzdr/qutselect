/* vim:set ts=2 nowrap: ****************************************************

 qutselect - A simple Qt-based GUI frontend for remote terminals
 Copyright (C) 2008-2024 by Jens Maus <mail@jens-maus.de>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 3 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software Foundation,
 Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

**************************************************************************/

#include <QApplication>
#include <QTranslator>
#include <QLocale>

#include "CApplication.h"
#include "CMainWindow.h"

#include "config.h"

#include <iostream>
#include <rtdebug.h>

using namespace std;

int main(int argc, char* argv[])
{
	int returnCode = 1; // 0 is no error

  // You want this, unless you mix streams output with C output.
  // Read  http://gcc.gnu.org/onlinedocs/libstdc++/27_io/howto.html#8 for an explanation.
  std::ios::sync_with_stdio(false);

  // before we start anything serious, we need to initialize our
  // debug class
	#if defined(DEBUG)
	#if defined(ANSI_COLOR)
	CRTDebug::instance()->setHighlighting(true);
	#else
	CRTDebug::instance()->setHighlighting(false);
	#endif
	#endif

	ENTER();

	// lets init the resource (images and so on)
	Q_INIT_RESOURCE(qutselect);

	// force some style settings
  QApplication::setDesktopSettingsAware(false);
  QApplication::setStyle("windows");

	// let us generate the console application object now.
  CApplication app(argc, argv);

	if(app.isInitialized())
	{
		W("active language: %d (%s)", QLocale::system().language(), QLocale::system().name().toLatin1().constData());

		// we now load & initialize eventually existing
		// translation files for the system's default
		QTranslator qtTranslator;
		if(qtTranslator.load(":/lang/qt_de"))
			D("successfully loaded 'qt_de' translation file.");
		else if(qtTranslator.load(":/lang/qt_"+QLocale::system().name()))
			D("successfully loaded 'qt_%s' translation file.", QLocale::system().name().toLatin1().constData());
		else
			E("couldn't load any Qt translation file.");

		// install the translator
		app.installTranslator(&qtTranslator);

		QTranslator myTranslator;
		if(myTranslator.load(":/lang/qutselect_de"))
			D("successfully loaded 'qutselect_de' translation file.");
		else if(myTranslator.load(":/lang/qutselect_"+QLocale::system().name()))
			D("successfully loaded 'qutselect_%s' translation file.", QLocale::system().name().toLatin1().constData());
		else
			E("couldn't load any qutselect translation file.");

		// install the translator
		app.installTranslator(&myTranslator);	

		// now we do execute our application
		returnCode = app.exec();

		// in case the app was aborted we signal a failure to
		// our caller
		if(returnCode == EXIT_SUCCESS && (app.wasAborted() || app.hasFailed()))
			returnCode = EXIT_FAILURE;		
	}

	RETURN(returnCode);

	#if defined(DEBUG)
	CRTDebug::destroy();
	#endif

  return returnCode;
}
