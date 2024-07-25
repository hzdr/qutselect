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

// own includes
#include "CApplication.h"
#include "CMainWindow.h"

// Qt includes
#include <QtCore>
#include <QFileInfo>

// ANSI includes
#include <iostream>

#include "config.h"

#include <rtdebug.h>

using namespace std;

CApplication::CApplication(int& argc, char** argv)
  : QApplication(argc, argv),
    m_bInitialized(false),
    m_bAbortFlag(false),
    m_bFailedFlag(false),
    m_bQuietFlag(false),
    m_bDtLoginMode(false),
    m_bNoList(false),
    m_bNoUserName(false),
    m_bKeepAlive(false)
{
  ENTER();

  // set same general information about our organizational structure and
  // the name of the application
  setApplicationName("qutselect");
  setOrganizationName("Helmholtz-Zentrum Dresden-Rossendorf");
  setOrganizationDomain("hzdr.de");
    
  // we parse the CommandLine for specified options for the
  // daemon process.
  if((m_bInitialized = parseCommandLine(argc, argv)) == true)
  {
    // now we instanciate our main CMainWindow class
    CMainWindow* mainWin = new CMainWindow(this);

    // show the mainwindow now
    mainWin->show();

    // activate the window which otherwise causes problems
    // if no window manager is running while qutselect is executed.
    if(m_bDtLoginMode == true)
      mainWin->activateWindow();

    D("app size: %d x %d", mainWin->width(), mainWin->height());  
  }

  LEAVE();
}

CApplication::~CApplication()
{
  ENTER();

  LEAVE();
}

bool CApplication::parseCommandLine(int& argc, char** argv)
{
  ENTER();
  bool result = false;

  SHOWVALUE(argc);

  // put all arguments in a temporary MultiHash
  QHash<QString, QString> args;

  // and all potential input filenames into a QStringList
  QStringList inputFileNames; 

  // if the user has specified some commandline options
  // lets process and parse them.
  for(int i=1; i < argc; i++)
  {
    QString option(argv[i]);

    if(option[0] == '-')
    {
      QString nextOption(argv[i+1]);

      if(i+1 < argc && (nextOption[0] != '-' || nextOption[0].isLetter() == false))
        args.insert(option, nextOption);
      else
        args.insert(option, "");
    }
    else 
      inputFileNames << argv[i];
  }

  // now we check/process the different options according to their
  // priority
  if(args.contains("-h") == false &&
     args.contains("-v") == false)
  {
    // we are optimistic so we go and assume everything will go fine
    // if one option says there is something wrong we will set the result back to false
    result = true;

    if(args.contains("-dtlogin"))
      m_bDtLoginMode = true;

    if(args.contains("-nolist"))
      m_bNoList = true;

    if(args.contains("-nouser"))
      m_bNoUserName = true;

    if(args.contains("-keep"))
      m_bKeepAlive = true;

    if(inputFileNames.isEmpty() == false)
      m_sServerListFile = inputFileNames[0];
  }

  // output some general program information
  if(result == false)
  {
    cout << "qutselect " << PROJECT_VERSION << " - a simple Qt based GUI frontend for terminal clients" << endl
         << "(" __DATE__ ")  Copyright (c) 2005-2024 Jens Maus, hzdr.de" << endl << endl;
  }

  // in case "-v" is specified we output some version
  // information
  if(args.contains("-v"))
  {
    cout << "Detailed compilation information:" << endl << endl

         // Compiler information
         << "  "
         #if defined(__GNUC__)
         << "GCC " << __GNUC__ << "." << __GNUC_MINOR__ <<  "." << __GNUC_PATCHLEVEL__ << " "
         #else
         #warning unknown compiler suite
         << "unknown compiler "
         #endif
         #if defined(__SPARC__)
         << "[sparc]"
         #elif defined(__POWERPC__)
         << "[ppc]"
         #elif defined(__i386__)
         << "[x86]"
         #elif defined(__x86_64__)
         << "[x86_64]"
         #else
         #warning Unknown CPU model
         << "[Unknown]"
         #endif
         << endl << endl

         // Qt version information
         << "  Qt " << QString::number((QT_VERSION & 0xff0000)>>16).toLatin1().constData() << "."
                    << QString::number((QT_VERSION & 0x00ff00)>>8).toLatin1().constData() << "." 
                    << QString::number((QT_VERSION & 0x0000ff)).toLatin1().constData()
                    << " (" << qVersion() << ")" << endl;
  }
  else if(result == false) // output usage information on the console.
  {
    cout << "Usage: " << argv[0] << " <options> <slistfile>" << endl
         << "Options:" << endl
         << "  -dtlogin   : start qutselect in dtlogin mode (e.g. kiosk, etc)" << endl
         << "  -nolist    : display no list of servers but only a combobox" << endl
         << "  -nouser    : do not take username from USER env variable" << endl
         << "  -keep      : do not quit after establishing the connection" << endl
         << "  -q         : keep quiet as much as possible." << endl
         << "  -v         : drop some more detailed version information." << endl
         << "  -h         : this help page." << endl
         << endl;

    m_bQuietFlag = true;
  }

  RETURN(result);
  return result;
}
