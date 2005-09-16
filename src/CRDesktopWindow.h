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

#ifndef CRDESKTOPWINDOW_H
#define CRDESKTOPWINDOW_H

#include <QMainWindow>
#include <QPushButton>

// forward declarations
class QCloseEvent;
class QLabel;
class QComboBox;
class QKeyEvent;
class QRadioButton;
class QSettings;

class CRDesktopWindow : public QMainWindow
{
  Q_OBJECT

  public:
    CRDesktopWindow(bool noUserPosition = false);
		~CRDesktopWindow();

		// set methods
		void setKeepAlive(const bool on)			{ m_bKeepAlive = on; }
		void setFullScreenOnly(const bool on);
		void setQuitText(const QString& str)	{ m_pQuitButton->setText(str); }

		// overloaded methods
		void keyPressEvent(QKeyEvent* e);

	private slots:
		void startButtonPressed(void);

	private:
		QLabel*				m_pLogoLabel;
		QLabel*				m_pServerListLabel;
		QComboBox*		m_pServerListBox;
		QLabel*				m_pScreenResolutionLabel;
		QComboBox*		m_pScreenResolutionBox;
		QLabel*				m_pColorsLabel;
		QRadioButton*	m_p8bitColorsButton;
		QRadioButton* m_p16bitColorsButton;
		QRadioButton* m_p24bitColorsButton;
		QLabel*				m_pKeyboardLabel;
		QRadioButton* m_pGermanKeyboardButton;
		QRadioButton* m_pEnglishKeyboardButton;
		QPushButton*	m_pQuitButton;
		QPushButton*	m_pStartButton;
		QSettings*		m_pSettings;

		bool					m_bKeepAlive;
		bool					m_bNoUserPosition;
};

#endif /* CRDESKTOPWINDOW_H */
