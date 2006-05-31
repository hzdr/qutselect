/* vim:set ts=2 nowrap: ****************************************************

 qRDesktop - A simple Qt4 based GUI frontend for rdesktop
 Copyright (C) 2005 by Jens Langner <Jens.Langner@light-speed.de>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 $Id$

**************************************************************************/

#include <QApplication>
#include <QTranslator>
#include <QLocale>

#include "CRDesktopWindow.h"

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
	Q_INIT_RESOURCE(qrdesktop);

	// let us generate the console application object now.
  QApplication app(argc, argv);

	W("active language: %d (%s)", QLocale::system().language(), QLocale::system().name().toAscii().constData());

	// we now load & initialize eventually existing
	// translation files for the system's default
	QTranslator qtTranslator;
	if(qtTranslator.load(":/lang/qt_de"))
		D("successfully loaded 'qt_de' translation file.");
	else if(qtTranslator.load(":/lang/qt_"+QLocale::system().name()))
		D("successfully loaded 'qt_%s' translation file.", QLocale::system().name().toAscii().constData());
	else
		E("couldn't load any Qt translation file.");

	// install the translator
	app.installTranslator(&qtTranslator);

	QTranslator myTranslator;
	if(myTranslator.load(":/lang/qrdesktop_de"))
		D("successfully loaded 'qrdesktop_de' translation file.");
	else if(myTranslator.load(":/lang/qrdesktop_"+QLocale::system().name()))
		D("successfully loaded 'qrdesktop_%s' translation file.", QLocale::system().name().toAscii().constData());
	else
		E("couldn't load any qrdesktop translation file.");

	// install the translator
	app.installTranslator(&myTranslator);	

	// now we check wheter the user requests some options
	// to be enabled
	bool dtLoginCall = false;
	if(argc > 1)
	{
		if(QString(argv[1]).toLower() == "-dtlogin")
			dtLoginCall = true;
	}

	// now we instanciate our main CRDesktopWindow class
	CRDesktopWindow* mainWin = new CRDesktopWindow(dtLoginCall);
	if(dtLoginCall)
	{
		mainWin->setKeepAlive(true);
		mainWin->setFullScreenOnly(true);
		mainWin->setQuitText(QObject::tr("Logout"));
	}

	// show the mainwindow now
	mainWin->show();

	// now we do execute our application
	returnCode = app.exec();

	RETURN(returnCode);

	#if defined(DEBUG)
	CRTDebug::destroy();
	#endif

  return returnCode;
}
