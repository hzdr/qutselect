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
class CApplication;
class QCloseEvent;
class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QComboBox;
class QKeyEvent;
class QRadioButton;
class QSettings;
class QPushButton;
class QTreeWidget;
class QTreeWidgetItem;
class QFileSystemWatcher;
class QStackedLayout;
class QTimer;
class QHBoxLayout;

class CMainWindow : public QMainWindow
{
  Q_OBJECT

  public:
    enum ServerType { SRSS=0, TLINC, RDP, XDM, VNC };
    enum LayoutType { DefaultLayout=0, UserPassLayout, PassLayout };

    CMainWindow(CApplication* app);
		~CMainWindow();

		// set methods
		void setKeepAlive(const bool on)			{ m_bKeepAlive = on; }
		void setFullScreenOnly(const bool on);
		void setQuitText(const QString& str)	{ m_pQuitButton->setText(str); }

		// overloaded methods
		void keyPressEvent(QKeyEvent* e);
    void closeEvent(QCloseEvent* e);

	private:
		void loadServerList();
    void loadMotdText();
		enum ServerType matchServerType(const QString& string);
    void changeLayout(enum LayoutType type);
    bool passwordDialog(const QString& serverName, QString& username, QString& password);

	private slots:
		void connectButtonPressed(void);
		void currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
		void itemDoubleClicked(QTreeWidgetItem* item, int column);
		void serverTypeChanged(int index);
		void serverListChanged(const QString& path);
		void serverComboBoxChanged(int index);
    void startConnection(void);
    void pwButtonCancelClicked(void);
    void pwButtonLoginClicked(void);
    void passwordTimedOut(void);

	private:
		QLabel*				      m_pLogoLabel;
		QLabel*				      m_pServerListLabel;
		QComboBox*		      m_pServerListBox;
		QLabel*				      m_pScreenResolutionLabel;
		QComboBox*		      m_pScreenResolutionBox;
    QHBoxLayout*        m_pScreenResolutionLayout;
		QLabel*				      m_pColorsLabel;
		QRadioButton*	      m_p8bitColorsButton;
		QRadioButton*       m_p16bitColorsButton;
		QRadioButton*       m_p24bitColorsButton;
    QHBoxLayout*        m_pColorsButtonLayout;
		QLabel*				      m_pKeyboardLabel;
		QRadioButton*       m_pGermanKeyboardButton;
		QRadioButton*       m_pEnglishKeyboardButton;
    QHBoxLayout*        m_pKeyboardButtonLayout;
		QPushButton*	      m_pQuitButton;
    QWidget*            m_pSpaceWidget;
		QPushButton*	      m_pStartButton;
		QSettings*		      m_pSettings;
    QTreeWidget*        m_pServerTreeWidget;
    QLineEdit*          m_pServerLineEdit;
		QComboBox*					m_pServerTypeComboBox;
		QFileSystemWatcher* m_pServerListWatcher;
    QStackedLayout*     m_pStackedLayout;
    QLabel*             m_pPasswordLayoutLabel;
    QLabel*             m_pUsernameLabel;
    QLineEdit*          m_pUsernameLineEdit;
    QLineEdit*          m_pPasswordLineEdit;
    QDialogButtonBox*   m_pPasswordButtonBox;
    QTimer*             m_pPasswordEnterTimer;
    QWidget*            m_pOptionsWidget;
    QWidget*            m_pMotdWidget;
    QLabel*             m_pMotdLabel;

		bool m_bKeepAlive;
		bool m_bDtLoginMode;
		bool m_bNoSRSS;
		bool m_bNoList;
		QString m_sServerListFile;

    // stored result before starting a connection
    QString m_sServerType;
    QString m_sResolution;
    int m_iColorDepth;
    QString m_sKeyLayout;
    QString m_sDomain;
    QString m_sUsername;
    QString m_sPassword;
    QString m_sStartupScript;
    QString m_sServerName;
};

#endif /* CMAINWINDOW_H */
