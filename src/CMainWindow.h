/* vim:set ts=2 nowrap: ****************************************************

 qutselect - A simple Qt4 based GUI frontend for SRSS (utselect)
 Copyright (C) 2009 by Jens Langner <Jens.Langner@light-speed.de>

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

#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QMap>

// forward declarations
class QCloseEvent;
class QLabel;
class QLineEdit;
class QComboBox;
class QKeyEvent;
class QRadioButton;
class QSettings;
class QPushButton;
class QTreeWidget;
class QTreeWidgetItem;

class CMainWindow : public QMainWindow
{
  Q_OBJECT

  public:
    enum ServerType { SRSS=0, RDP, VNC };

    CMainWindow(bool dtLoginMode = false);
		~CMainWindow();

		// set methods
		void setKeepAlive(const bool on)			{ m_bKeepAlive = on; }
		void setFullScreenOnly(const bool on);
		void setQuitText(const QString& str)	{ m_pQuitButton->setText(str); }

		// overloaded methods
		void keyPressEvent(QKeyEvent* e);

	private:
		void loadServerList();
		enum ServerType matchServerType(const QString& string);

	private slots:
		void startButtonPressed(void);
		void currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
		void itemDoubleClicked(QTreeWidgetItem* item, int column);
		void serverTypeChanged(int index);

	private:
		QLabel*				      m_pLogoLabel;
		QLabel*				      m_pServerListLabel;
		QComboBox*		      m_pServerListBox;
		QLabel*				      m_pScreenResolutionLabel;
		QComboBox*		      m_pScreenResolutionBox;
		QLabel*				      m_pColorsLabel;
		QRadioButton*	      m_p8bitColorsButton;
		QRadioButton*       m_p16bitColorsButton;
		QRadioButton*       m_p24bitColorsButton;
		QLabel*				      m_pKeyboardLabel;
		QRadioButton*       m_pGermanKeyboardButton;
		QRadioButton*       m_pEnglishKeyboardButton;
		QPushButton*	      m_pQuitButton;
		QPushButton*	      m_pStartButton;
		QSettings*		      m_pSettings;
    QTreeWidget*        m_pServerTreeWidget;
    QLineEdit*          m_pServerLineEdit;
		QComboBox*					m_pServerTypeComboBox;

		bool m_bKeepAlive;
		bool m_bDtLoginMode;
};

#endif /* CMAINWINDOW_H */
