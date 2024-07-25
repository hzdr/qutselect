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

#ifndef _CAPPLICATION_HPP_
#define _CAPPLICATION_HPP_

#include <QApplication>

// forward declarations

class CApplication : public QApplication
{
  Q_OBJECT
  
  public:
    CApplication(int& argc, char** argv);
    ~CApplication();

    bool isInitialized() const { return m_bInitialized; }
    bool wasAborted() const { return m_bAbortFlag; }
    bool hasFailed() const { return m_bFailedFlag; }    
    bool dtLoginMode() const { return m_bDtLoginMode; }
    bool noListDisplay() const { return m_bNoList; }
    bool noUserName() const { return m_bNoUserName; }
    bool keepAlive() const { return m_bKeepAlive; }
    QString customServerListFile() const { return m_sServerListFile; }

  private:
    bool parseCommandLine(int& argc, char** argv);

  private:
    bool m_bInitialized;
    bool m_bAbortFlag;
    bool m_bFailedFlag;
    bool m_bQuietFlag;

    // for our configuration
    bool    m_bDtLoginMode;
    bool    m_bNoList;
    bool    m_bNoUserName;
    bool    m_bKeepAlive;
    QString m_sServerListFile;
};

#endif // _CAPPLICATION_H_
