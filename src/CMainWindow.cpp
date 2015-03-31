/* vim:set ts=2 nowrap: ****************************************************

 qutselect - A simple Qt based GUI frontend for SRSS (utselect)
 Copyright (C) 2009-2013 by Jens Langner <Jens.Langner@light-speed.de>

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

#include "CMainWindow.h"
#include "CApplication.h"
#include "CLoginDialog.h"
#include "CServerTreeWidget.h"

#include <QApplication>
#include <QLabel>
#include <QButtonGroup>
#include <QComboBox>
#include <QDesktopWidget>
#include <QDialogButtonBox>
#include <QDir>
#include <QFile>
#include <QKeyEvent>
#include <QGridLayout>
#include <QMessageBox>
#include <QPainter>
#include <QPoint>
#include <QPushButton>
#include <QRadioButton>
#include <QProcess>
#include <QRegExp>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QPushButton>
#include <QHostInfo>
#include <QStackedLayout>
#include <QTreeWidget>
#include <QLineEdit>
#include <QFileSystemWatcher>
#include <QTimer>
#include <QHeaderView>

#include <iostream>
#include <unistd.h>

#include <rtdebug.h>

// standard width/height
#define WINDOW_WIDTH	500
#define WINDOW_HEIGHT 600

// the default startup script pattern
#define DEFAULT_SCRIPT_PATTERN "scripts/qutselect_connect_%1.sh"

// the serverlist file name
#define DEFAULT_SLIST_FILE "qutselect.slist"
#define DEFAULT_MOTD_FILE  "qutselect.motd"

// the column numbers
enum ColumnNumbers { CN_DISPLAYNAME=0,
                     CN_HOSTNAME,
                     CN_DOMAIN,
										 CN_SERVERTYPE,
										 CN_SERVEROS,
										 CN_DESCRIPTION,
                     CN_PWPROMPT,
										 CN_STARTUPSCRIPT
									 };

#include "config.h"

CMainWindow::CMainWindow(CApplication* app)
	: m_bKeepAlive(app->keepAlive()),
		m_bDtLoginMode(app->dtLoginMode()),
		m_bNoSRSS(app->noSunrayServers()),
		m_bNoList(app->noListDisplay())
{
	ENTER();

  // skip automated names
  if(app->noUserName() == false)
  {
	  QString userName = QString(getenv("USER"));
    if(userName.startsWith("utku") == false && userName != "root")
      m_sUsername = userName;
  }

	// get/identify the default serverlist file
	if(app->customServerListFile().isEmpty() == false)
		m_sServerListFile = app->customServerListFile();
	else
		m_sServerListFile = QDir(QApplication::applicationDirPath()).absoluteFilePath(DEFAULT_SLIST_FILE);

  // create the central widget to which we are going to add everything
  QWidget* centralWidget = new QWidget;
  setCentralWidget(centralWidget);

	// create a QSettings object to receive the users specific settings
	// written the last time the user used that application
	if(m_bDtLoginMode == false)
		m_pSettings = new QSettings("hzdr.de", "qutselect");

	// we put a logo at the top
	m_pLogoLabel = new QLabel();
  QPixmap logo(":/images/banner-en.png");
  QPainter paint;
  paint.begin(&logo);
  paint.setPen(Qt::black);
	char hostName[256];
  gethostname(hostName, 256);
	QFont serverNameFont = paint.font();
	serverNameFont.setBold(true);
  serverNameFont.setPointSize(12);
  paint.setFont(serverNameFont);
  paint.drawText(195,40,150,100, Qt::AlignLeft|Qt::AlignTop, QString("@ ")+hostName);
  paint.end(); 
	m_pLogoLabel->setPixmap(logo);
	//m_pLogoLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	m_pLogoLabel->setAlignment(Qt::AlignCenter);
	
  // create the treewidget we are going to populate
  m_pServerTreeWidget = new CServerTreeWidget();
  m_pServerTreeWidget->setRootIsDecorated(false);
  m_pServerTreeWidget->setAllColumnsShowFocus(true);
	connect(m_pServerTreeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
					this,								 SLOT(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
	connect(m_pServerTreeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
					this,								 SLOT(itemDoubleClicked(QTreeWidgetItem*, int)));
	
	// add the columns with the labels
	QStringList columnNames;
	columnNames << tr("Name");
  columnNames << tr("Servername");
  columnNames << tr("Domain");
  columnNames << tr("Type");
  columnNames << tr("System");
  columnNames << tr("Description");
  columnNames << tr("PW-Prompt");
  columnNames << tr("Script");
	m_pServerTreeWidget->setHeaderLabels(columnNames);
  m_pServerTreeWidget->setColumnHidden(CN_DOMAIN, true);
  m_pServerTreeWidget->setColumnHidden(CN_SERVERTYPE, true);
  m_pServerTreeWidget->setColumnHidden(CN_PWPROMPT, true);
  m_pServerTreeWidget->setColumnHidden(CN_STARTUPSCRIPT, true);
	m_pServerTreeWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  // create the ServerLineEdit
  m_pServerLineEdit = new QLineEdit();

	// create a combobox for the different ServerTypes we have
	m_pServerTypeComboBox = new QComboBox();
	m_pServerTypeComboBox->addItem("Unix (SRSS)");
	m_pServerTypeComboBox->addItem("Unix (TLINC)");
	m_pServerTypeComboBox->addItem("Windows (RDP)");
	m_pServerTypeComboBox->addItem("X11 (XDM)");
	m_pServerTypeComboBox->addItem("VNC");
	m_pServerTypeComboBox->addItem("Application");
	m_pServerTypeComboBox->setCurrentIndex(-1);
	m_pServerTypeComboBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	connect(m_pServerTypeComboBox, SIGNAL(currentIndexChanged(int)),
					this,									 SLOT(serverTypeChanged(int)));

	// the we need a combobox for the different server a user can select
	m_pServerListLabel = new QLabel(tr("Server:"));
	m_pServerListBox = new QComboBox();
	m_pServerListBox->setCurrentIndex(-1);
	m_pServerListBox->setEditable(false);
	connect(m_pServerListBox, SIGNAL(currentIndexChanged(int)),
					this,							SLOT(serverComboBoxChanged(int)));

	// combine the LineEdit or ServerListBox and the TypeCombobox
	QHBoxLayout* serverLineLayout = new QHBoxLayout();

	if(m_bNoList == true)
		serverLineLayout->addWidget(m_pServerListBox);
	else
		serverLineLayout->addWidget(m_pServerLineEdit);

	serverLineLayout->addWidget(m_pServerTypeComboBox);	

  // create the serverListLayout
	QVBoxLayout* serverListLayout = new QVBoxLayout();
  serverListLayout->addWidget(m_pServerTreeWidget);
	if(m_bNoList == false)
		serverListLayout->addLayout(serverLineLayout);
	
	// selection of the screen depth
	m_pScreenResolutionLabel = new QLabel(tr("Resolution:"));

  // lets find out the aspect ratio and min/max size for width/height of
  // the screen
	QDesktopWidget* desktopWidget = QApplication::desktop();
	QRect screenSize = desktopWidget->screenGeometry(desktopWidget->primaryScreen());

  m_pScreenResolutionLayout = new QHBoxLayout();
	m_pScreenResolutionBox = new QComboBox();
  m_pScreenResolutionBox->setEditable(true);
  m_pScreenResolutionLayout->addWidget(m_pScreenResolutionBox, 0, Qt::AlignLeft);
  m_pScreenResolutionLayout->addWidget(new QLabel("max: " + QString::number(screenSize.width()) + "x" + QString::number(screenSize.height())));
  m_pScreenResolutionLayout->addStretch(100);

  // now fill the screen resolutionbox with resolutions this screen can handle
  if(screenSize.width() >= 800 && screenSize.height() >= 600)
    m_pScreenResolutionBox->addItem("800x600");

  if(screenSize.width() >= 1024 && screenSize.height() >= 768)
    m_pScreenResolutionBox->addItem("1024x768");

  if(screenSize.width() >= 1152 && screenSize.height() >= 900)
    m_pScreenResolutionBox->addItem("1152x900");

  if(screenSize.width() >= 1280 && screenSize.height() >= 1024)
    m_pScreenResolutionBox->addItem("1280x1024");

  if(screenSize.width() >= 1600 && screenSize.height() >= 900)
    m_pScreenResolutionBox->addItem("1600x900");

  if(screenSize.width() >= 1600 && screenSize.height() >= 1200)
    m_pScreenResolutionBox->addItem("1600x1200");

  if(screenSize.width() >= 1920 && screenSize.height() >= 1080)
    m_pScreenResolutionBox->addItem("1920x1080");

  if(screenSize.width() >= 1920 && screenSize.height() >= 1200)
    m_pScreenResolutionBox->addItem("1920x1200");

  // add 'Desktop' and 'Fullscreen' to the end
	m_pScreenResolutionBox->addItem("Desktop");
	m_pScreenResolutionBox->addItem("Fullscreen");

	// we check the QSettings for "resolution" and see if we
	// can use it or not
  int item = -1;
	if(m_bDtLoginMode == false && m_pSettings->value("resolution").isValid())
	{
		QString resolution = m_pSettings->value("resolution").toString();

    item = m_pScreenResolutionBox->findText(resolution);
    if(item < 0)
      item = m_pScreenResolutionBox->findText("Desktop");
	}
	else
	{
		if(screenSize.width() >= 1920)
    {
      if(screenSize.height() >= 1200)
			  item = m_pScreenResolutionBox->findText("1920x1200");
      else
        item = m_pScreenResolutionBox->findText("1920x1080");
    }
		else if(screenSize.width() >= 1600)
    {
      if(screenSize.height() >= 1200)
        item = m_pScreenResolutionBox->findText("1600x1200");
      else
        item = m_pScreenResolutionBox->findText("1600x1080");
    }
		else if(screenSize.width() >= 1280)
      item = m_pScreenResolutionBox->findText("1280x1024");
    else if(screenSize.width() >= 1152)
      item = m_pScreenResolutionBox->findText("1152x900");
		else if(screenSize.width() > 1024)
      item = m_pScreenResolutionBox->findText("1024x768");
		else
      item = m_pScreenResolutionBox->findText("800x600");

    if(item < 0)
      item =  m_pScreenResolutionBox->findText("Fullscreen");
	}

  // set item as current index
  m_pScreenResolutionBox->setCurrentIndex(item);

	// color depth selection
	m_pColorsLabel = new QLabel(tr("Colors:"));
	m_p8bitColorsButton = new QRadioButton(tr("8bit (256)"));
	m_p16bitColorsButton = new QRadioButton(tr("16bit (65535)"));
	m_p24bitColorsButton = new QRadioButton(tr("24bit (Millions)"));
	QButtonGroup* colorsButtonGroup = new QButtonGroup();
	colorsButtonGroup->addButton(m_p8bitColorsButton);
	colorsButtonGroup->addButton(m_p16bitColorsButton);
	colorsButtonGroup->addButton(m_p24bitColorsButton);
	colorsButtonGroup->setExclusive(true);
	m_pColorsButtonLayout = new QHBoxLayout();
	m_pColorsButtonLayout->setMargin(0);
	m_pColorsButtonLayout->addWidget(m_p8bitColorsButton);
	m_pColorsButtonLayout->addWidget(m_p16bitColorsButton);
	m_pColorsButtonLayout->addWidget(m_p24bitColorsButton);
	m_pColorsButtonLayout->addStretch(1);

	// now we check the QSettings for the last selected color depth
	if(m_bDtLoginMode == false)
	{
		int depth = m_pSettings->value("colordepth", 16).toInt();
		switch(depth)
		{
			case 8:
				m_p8bitColorsButton->setChecked(true);
			break;

			case 24:
				m_p24bitColorsButton->setChecked(true);
			break;

			default:
				m_p16bitColorsButton->setChecked(true);
			break;
		}
	}
	else
		m_p16bitColorsButton->setChecked(true);

	// keyboard layout selection radiobuttons
	m_pKeyboardLabel = new QLabel(tr("Keyboard:"));
	m_pGermanKeyboardButton = new QRadioButton(tr("German"));
	m_pEnglishKeyboardButton = new QRadioButton(tr("English"));
	QButtonGroup* keyboardGroup = new QButtonGroup();
	keyboardGroup->addButton(m_pGermanKeyboardButton);
	keyboardGroup->addButton(m_pEnglishKeyboardButton);
	keyboardGroup->setExclusive(true);
	m_pKeyboardButtonLayout = new QHBoxLayout();
	m_pKeyboardButtonLayout->setMargin(0);
	m_pKeyboardButtonLayout->addWidget(m_pGermanKeyboardButton);
	m_pKeyboardButtonLayout->addWidget(m_pEnglishKeyboardButton);
	m_pKeyboardButtonLayout->addStretch(1);

	// select the german keyboard as default
	m_pGermanKeyboardButton->setChecked(true);

	// check the QSettings for the last used keyboard layout
	if(m_bDtLoginMode == true)
	{
		// in dtlogin mode we identify the keyboard vis QApplication::keyboardInputLocale()
		QLocale keyboardLocale = QApplication::inputMethod()->locale();

		D("keyboardLocalName: %s", keyboardLocale.name().toLatin1().constData());
	}
	else
	{
		QString keyboard = m_pSettings->value("keyboard", "de").toString();
		if(keyboard.toLower() == "en")
			m_pEnglishKeyboardButton->setChecked(true);
	}

	// put a frame right before our buttons
	QFrame* buttonFrame = new QFrame();
	buttonFrame->setFrameStyle(QFrame::HLine | QFrame::Raised);

	// our quit and start buttons
	m_pQuitButton = new QPushButton(tr("Quit"));
	m_pStartButton = new QPushButton(tr("Connect"));
	m_pStartButton->setDefault(true);
	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(m_pQuitButton);
	buttonLayout->setStretchFactor(m_pQuitButton, 2);
  m_pSpaceWidget = new QWidget;
  buttonLayout->addWidget(m_pSpaceWidget);
	buttonLayout->setStretchFactor(m_pSpaceWidget, 1);
	buttonLayout->addWidget(m_pStartButton);
	buttonLayout->setStretchFactor(m_pStartButton, 2);
	connect(m_pQuitButton, SIGNAL(clicked()),
					this,					 SLOT(close()));
	connect(m_pStartButton,SIGNAL(clicked()),
					this,					 SLOT(connectButtonPressed()));
	
  QWidget* defaultWidget = new QWidget;
	QGridLayout* layout = new QGridLayout;

	if(m_bNoList == false)
		layout->addLayout(serverListLayout, 			  0, 0, 1, 2);
	else
  {
    layout->addWidget(m_pServerListLabel,       0, 0);
		layout->addLayout(serverLineLayout,					0, 1);
  }

  m_pOptionsWidget = new QWidget;
  QGridLayout* optionsLayout = new QGridLayout;
  optionsLayout->setSpacing(5);
  optionsLayout->setContentsMargins(0,0,0,0);
	optionsLayout->addWidget(m_pScreenResolutionLabel,		0, 0);
	optionsLayout->addLayout(m_pScreenResolutionLayout,	  0, 1);
	optionsLayout->addWidget(m_pColorsLabel,							1, 0);
	optionsLayout->addLayout(m_pColorsButtonLayout,			  1, 1);
	optionsLayout->addWidget(m_pKeyboardLabel,						2, 0);
	optionsLayout->addLayout(m_pKeyboardButtonLayout,		  2, 1);
  m_pOptionsWidget->setLayout(optionsLayout);
  m_pOptionsWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

  // create the MOTD widget
  m_pMotdWidget = new QWidget;
  QVBoxLayout* motdLayout = new QVBoxLayout;
  motdLayout->setSpacing(5);
  motdLayout->setContentsMargins(0,0,0,0);
  QFrame* motdFrame = new QFrame;
  motdFrame->setFrameStyle(QFrame::HLine | QFrame::Raised);
  motdLayout->addWidget(motdFrame);
  m_pMotdLabel = new QLabel;
  motdLayout->addWidget(m_pMotdLabel);
	motdLayout->addWidget(buttonFrame);
  m_pMotdWidget->setLayout(motdLayout);
  m_pMotdWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

  layout->addWidget(m_pOptionsWidget,           1, 0, 1, 2);
  layout->addWidget(m_pMotdWidget,              2, 0, 1, 2);
	layout->addLayout(buttonLayout,								3, 0, 1, 2);
  defaultWidget->setLayout(layout);

  QWidget* passwordWidget = new QWidget;
  QGridLayout* pwLayout = new QGridLayout;

  m_pPasswordLayoutLabel = new QLabel;
	QFont pwlabelFont = m_pPasswordLayoutLabel->font();
	pwlabelFont.setPointSize(14);
  m_pPasswordLayoutLabel->setFont(pwlabelFont);
  m_pPasswordLayoutLabel->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);

  pwLayout->addWidget(m_pPasswordLayoutLabel, 0, 0, 1, 2);
  m_pUsernameLabel = new QLabel(tr("Username:"));
  pwLayout->addWidget(m_pUsernameLabel, 1, 0);
  m_pUsernameLineEdit = new QLineEdit;
  pwLayout->addWidget(m_pUsernameLineEdit, 1, 1);
  pwLayout->addWidget(new QLabel(tr("Password:")), 2, 0);
  m_pPasswordLineEdit = new QLineEdit;
  m_pPasswordLineEdit->setEchoMode(QLineEdit::Password);
  pwLayout->addWidget(m_pPasswordLineEdit, 2, 1);
  pwLayout->addWidget(new QLabel(), 3, 1);
  QFrame* pwButtonFrame = new QFrame();
  pwButtonFrame->setFrameStyle(QFrame::HLine | QFrame::Raised);
  pwLayout->addWidget(pwButtonFrame, 4, 0, 1, 2);
  m_pPasswordButtonBox = new QDialogButtonBox;
  m_pPasswordButtonBox->addButton(QDialogButtonBox::Ok);
  m_pPasswordButtonBox->addButton(QDialogButtonBox::Cancel);
  m_pPasswordButtonBox->button(QDialogButtonBox::Ok)->setText(tr("Login"));
  m_pPasswordButtonBox->button(QDialogButtonBox::Cancel)->setText(tr("Abort"));

  // connects slots
  connect(m_pPasswordButtonBox->button(QDialogButtonBox::Cancel),
          SIGNAL(clicked()),
          this,
          SLOT(pwButtonCancelClicked()));
 
  connect(m_pPasswordButtonBox->button(QDialogButtonBox::Ok),
          SIGNAL(clicked()),
          this,
          SLOT(pwButtonLoginClicked()));

  m_pPasswordEnterTimer = new QTimer(this);
  m_pPasswordEnterTimer->setSingleShot(true);
  connect(m_pPasswordEnterTimer, SIGNAL(timeout()),
          this,                  SLOT(passwordTimedOut()));
          
  pwLayout->addWidget(m_pPasswordButtonBox, 5, 0, 1, 2);
  passwordWidget->setLayout(pwLayout);

  m_pStackedLayout = new QStackedLayout;
  m_pStackedLayout->addWidget(defaultWidget);
  m_pStackedLayout->addWidget(passwordWidget);

  QVBoxLayout* centralLayout = new QVBoxLayout;
  centralLayout->addWidget(m_pLogoLabel);
  centralLayout->addLayout(m_pStackedLayout);

	centralWidget->setLayout(centralLayout);

	// create a FileSystemWatcher to monitor the serverlist file and report 
	// any changes to it
	m_pServerListWatcher = new QFileSystemWatcher();

	if(QFileInfo(m_sServerListFile).exists())
		m_pServerListWatcher->addPath(m_sServerListFile);

	connect(m_pServerListWatcher, SIGNAL(fileChanged(const QString&)),
					this,									SLOT(serverListChanged(const QString&)));

	// now we try to open the serverlist file and add the items to our comobox
	loadServerList();
  loadMotdText();

	// check if the QSettings contains any info about the last position
	if(m_bDtLoginMode == true)
	{
		// make sure to also change some settings according to
		// the dtlogin mode
		setKeepAlive(true);
		setFullScreenOnly(true);

    // hide the Quit Button in DtLogin Mode as ESC does the same
    m_pQuitButton->setVisible(false);
    m_pSpaceWidget->setVisible(false);
  }
	else
	{
    QPoint position = m_pSettings->value("position", QPoint(10, 10)).toPoint();

    if(position.x() < 0)
      position.setX(10);

    if(position.y() < 0)
      position.setY(10);

		move(position);
		resize(m_pSettings->value("size", QSize(WINDOW_WIDTH, WINDOW_HEIGHT)).toSize());
	}
	
	setWindowTitle("qutselect v" + QString(PROJECT_VERSION) + " - (c) 2005-2015 hzdr.de");

	LEAVE();
}

CMainWindow::~CMainWindow()
{
	ENTER();

	delete m_pServerListWatcher;
	delete m_pSettings;
	
	LEAVE();
}

void CMainWindow::serverComboBoxChanged(int index)
{
	ENTER();

	QString serverName = m_pServerListBox->itemText(index);
  if(serverName.isEmpty() == false)
  {
    serverName = serverName.split("\\s+", QString::SkipEmptyParts).at(0);
	  QList<QTreeWidgetItem*> items = m_pServerTreeWidget->findItems(serverName, Qt::MatchStartsWith, CN_HOSTNAME);
    if(items.isEmpty() == false)
		  m_pServerTreeWidget->setCurrentItem(items.first());
  }

	LEAVE();
}

void CMainWindow::serverListChanged(const QString& path)
{
	ENTER();

	D("FileSystemWatcher triggered: '%s'", path.toLatin1().constData());
	
	if(path == m_sServerListFile)
	{
		// make sure to wait some seconds before continuing
		if(QFileInfo(m_sServerListFile).exists() == false)
			sleep(2);

		if(QFileInfo(m_sServerListFile).exists())
		{
			// found server list file changed, so go and reload it
			D("server list file '%s' changed. reloading...", m_sServerListFile.toLatin1().constData());
			loadServerList();

			// refresh the FileSystemWatcher
			m_pServerListWatcher->removePath(m_sServerListFile);
			m_pServerListWatcher->addPath(m_sServerListFile);
		}
		else
			W("server list file '%s' does not exist anymore...", m_sServerListFile.toLatin1().constData());
	}

	LEAVE();
}

void CMainWindow::serverTypeChanged(int id)
{
	ENTER();
  enum CMainWindow::ServerType index = static_cast<CMainWindow::ServerType>(id);

	D("serverTypeChanged to '%d'", index);

	// we disable everything if this is a SRSS
	switch(index)
	{
		case SRSS:
		{
			m_pScreenResolutionBox->setEnabled(false);
			m_p8bitColorsButton->setEnabled(false);
			m_p16bitColorsButton->setEnabled(false);
			m_p24bitColorsButton->setEnabled(false);
			m_pGermanKeyboardButton->setEnabled(false);
			m_pEnglishKeyboardButton->setEnabled(false);
      m_pServerLineEdit->setEnabled(true);
      m_pStartButton->setText(tr("Connect"));
		}
		break;

    case TLINC:
    {
  	  m_pScreenResolutionBox->setEnabled(true);
			m_p8bitColorsButton->setEnabled(false);
			m_p16bitColorsButton->setEnabled(false);
			m_p24bitColorsButton->setEnabled(false);
			m_pGermanKeyboardButton->setEnabled(false);
			m_pEnglishKeyboardButton->setEnabled(false);
      m_pServerLineEdit->setEnabled(true);
      m_pStartButton->setText(tr("Connect"));
    }
    break;

		case RDP:
		{
			m_pScreenResolutionBox->setEnabled(true);
			m_p8bitColorsButton->setEnabled(true);
			m_p16bitColorsButton->setEnabled(true);
			m_p24bitColorsButton->setEnabled(true);
			m_pGermanKeyboardButton->setEnabled(true);
			m_pEnglishKeyboardButton->setEnabled(true);
      m_pServerLineEdit->setEnabled(true);
      m_pStartButton->setText(tr("Connect"));
		}
		break;

		case XDM:
		{
			m_pScreenResolutionBox->setEnabled(true);
			m_p8bitColorsButton->setEnabled(true);
			m_p16bitColorsButton->setEnabled(true);
			m_p24bitColorsButton->setEnabled(true);
			m_pGermanKeyboardButton->setEnabled(false);
			m_pEnglishKeyboardButton->setEnabled(false);
      m_pServerLineEdit->setEnabled(true);
      m_pStartButton->setText(tr("Connect"));
		}
		break;

		case VNC:
		{
			// disable the keyboard control only for VNC servers
			m_pScreenResolutionBox->setEnabled(false);
			m_p8bitColorsButton->setEnabled(true);
			m_p16bitColorsButton->setEnabled(true);
			m_p24bitColorsButton->setEnabled(true);
			m_pGermanKeyboardButton->setEnabled(false);
			m_pEnglishKeyboardButton->setEnabled(false);
      m_pServerLineEdit->setEnabled(true);
      m_pStartButton->setText(tr("Connect"));
		}
		break;

    case APP:
		{
			m_pScreenResolutionBox->setEnabled(false);
			m_p8bitColorsButton->setEnabled(false);
			m_p16bitColorsButton->setEnabled(false);
			m_p24bitColorsButton->setEnabled(false);
			m_pGermanKeyboardButton->setEnabled(false);
			m_pEnglishKeyboardButton->setEnabled(false);
      m_pStartButton->setText(tr("Run Application"));
		}
		break;
	}

	LEAVE();
}

void CMainWindow::currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem*)
{
	ENTER();

	if(current != NULL)
	{
		D("currentItemChanged to '%s'", current->text(CN_HOSTNAME).toLatin1().constData());

    if(current->text(0).isEmpty() == false && current->text(0).startsWith("===") == false)
    {
		  // update the lineedit with the text of the first column
		  m_pServerLineEdit->setText(current->text(CN_HOSTNAME));

		  // make sure the combobox is in sync with the lineedit
		  m_pServerListBox->setCurrentIndex(m_pServerListBox->findText(current->text(CN_HOSTNAME), Qt::MatchStartsWith));

		  // now we have to check which server type the currently selected server
		  // is and then disable some GUI components of qutselect
		  QString serverType = current->text(CN_SERVERTYPE);

		  // sync the ServerType combobox as well
		  m_pServerTypeComboBox->setCurrentIndex(m_pServerTypeComboBox->findText(serverType, Qt::MatchContains));
    }
	}
	else
	{
		// clear everything
		m_pServerLineEdit->clear();
	}

	LEAVE();
}

void CMainWindow::itemDoubleClicked(QTreeWidgetItem* item, int)
{
	ENTER();

	D("Server '%s' doubleclicked", item->text(CN_HOSTNAME).toLatin1().constData());

	// a doubleclick is like pressing the "connect" button
	connectButtonPressed();

	LEAVE();
}

void CMainWindow::setFullScreenOnly(const bool on)
{
	ENTER();

	if(on)
	{
		m_pScreenResolutionBox->setCurrentIndex(m_pScreenResolutionBox->findText("Fullscreen"));
		m_pScreenResolutionBox->setEnabled(false);

		// hide some components competely
		m_pOptionsWidget->setVisible(false);
	}
	else
	{
		m_pScreenResolutionBox->setEnabled(true);

		// hide some components competely
		m_pOptionsWidget->setVisible(true);
	}

	LEAVE();
}

void CMainWindow::loadMotdText(void)
{
  ENTER();

 	QFile motdFile(QDir(QApplication::applicationDirPath()).absoluteFilePath(DEFAULT_MOTD_FILE));
	if(motdFile.open(QFile::ReadOnly))
	{
		QTextStream in(&motdFile);
    QString allText = in.readAll();

    if(allText.isEmpty() == false)
    {
      m_pMotdLabel->setText(allText);
      m_pMotdWidget->setVisible(true);
    }
    else
      m_pMotdWidget->setVisible(false);
  }
  else
    m_pMotdWidget->setVisible(false);

  LEAVE();
}

void CMainWindow::connectButtonPressed(void)
{
	ENTER();

	// save the current position and size of the GUI
	if(m_bDtLoginMode == false)
	{
		m_pSettings->setValue("position", pos());
		m_pSettings->setValue("size", size());
	}

	// get the currently selected server name
	QString serverName;
	if(m_bNoList == true)
		serverName = m_pServerListBox->currentText().section(" ", 0, 0).toLower();
	else
		serverName = m_pServerLineEdit->text().section(" ", 0, 0).toLower();

	if(m_bDtLoginMode == false)
		m_pSettings->setValue("serverused", serverName);

	// get the currently selected resolution
	QString resolution = m_pScreenResolutionBox->currentText().section(" ", 0, 0).toLower();
	if(m_bDtLoginMode == false && 
		 m_pScreenResolutionBox->isEnabled() && m_pScreenResolutionBox->isVisible())
	{
		m_pSettings->setValue("resolution", resolution);
	}

	// if the resolution was set to "Desktop" we have to identify the maximum
	// desktop width here and supply it accordingly.
	if(resolution == "desktop")
	{
		QDesktopWidget* desktopWidget = QApplication::desktop();
		QRect screenSize = desktopWidget->availableGeometry(desktopWidget->primaryScreen());

    // create the resolution string but substract 50 pixel beause the desktop size is always
    // calculated WITH the windows bar in GNOME :(
		resolution = QString().sprintf("%dx%d", screenSize.width()-8, screenSize.height()-28);

		D("Desktop size of '%s' selected", resolution.toLatin1().constData());
	}

	// get the keyboard layout the user wants to have
	QString keyLayout = m_pGermanKeyboardButton->isChecked() ? "de" : "en";
	if(m_bDtLoginMode == false)
		m_pSettings->setValue("keyboard", keyLayout);

	// get the color depth
	short colorDepth;
	if(m_p8bitColorsButton->isChecked())
		colorDepth = 8;
	else if(m_p16bitColorsButton->isChecked())
		colorDepth = 16;
	else
		colorDepth = 24;

	if(m_bDtLoginMode == false)
		m_pSettings->setValue("colordepth", colorDepth);

	// now we get the serverType of the current selection
	QString serverType;
	switch(m_pServerTypeComboBox->currentIndex())
	{
		case SRSS:
			serverType = "SRSS";
		break;

		case TLINC:
			serverType = "TLINC";
		break;

		case RDP:
			serverType = "RDP";
		break;

		case XDM:
			serverType = "XDM";
		break;

		case VNC:
			serverType = "VNC";
		break;

		case APP:
			serverType = "APP";
		break;
	}

  // if we are going to switch to the same SRSS we are already on we go
  // and close qutselect completely so that the real login of that
  // server shows up instead
  if(m_bDtLoginMode == true && m_pServerTypeComboBox->currentIndex() == SRSS)
  {
    char hostname[256];
    QString currentHost;

    if(gethostname(hostname, 256) == 0)
    {
	    currentHost = QString(hostname).toLower();

	    D("got hostname: '%s'", currentHost.toLatin1().constData());

      if(serverName == currentHost)
      {
        close();

        LEAVE();
        return;
      }
    }
  }

	// find the selected server in the tree widget to retrieve some more
  // information (pw prompt, domain, script, etc.)
	// startup script name
	QString startupScript;
  QString domain;
  bool pwprompt = false;
	QList<QTreeWidgetItem*> items = m_pServerTreeWidget->findItems(serverName, Qt::MatchStartsWith, CN_HOSTNAME);
	if(items.isEmpty() == false && items.first()->text(CN_SERVERTYPE) == serverType)
	{
		startupScript = items.first()->text(CN_STARTUPSCRIPT);
    domain = items.first()->text(CN_DOMAIN);
    pwprompt = items.first()->text(CN_PWPROMPT) == "TRUE" ? true : false;
	}
	else
	{
		// if this didn't work out we use the pattern to create a generic script
		startupScript = QString(DEFAULT_SCRIPT_PATTERN).arg(serverType.toLower());

    // now we find out if we will show a password prompt or not by using the default values
    // for certain protocols
    if(serverType == "RDP" || serverType == "VNC" || serverType == "TLINC")
      pwprompt = true;
	}

  // set most of the connection relevant parameters now
  m_sServerType = serverType;
  m_sResolution = resolution;
  m_iColorDepth = colorDepth;
  m_sKeyLayout = keyLayout;
  m_sDomain = domain;
  m_sStartupScript = startupScript;
  m_sServerName = serverName;

  // if we should ask for a password we go and present a password prompt
  QString password;
  if(pwprompt == true)
  {
    // if a password prompt is required we don't start
    // the connection right away but wait until the user
    // entered username/password
    if(serverType == "VNC")
    {
      changeLayout(PassLayout);

      LEAVE();
      return;
    }
    else
    {
      // if the servertype is RDP and the domain is FZR we check if qutselect
      // is running in a ThinLinc session and if so we don't switch to the
      // username/password prompt call startConnection() right away
      if(serverType != "RDP" || m_sDomain != "FZR" || qgetenv("TLSESSIONDATA").isEmpty())
      {
        changeLayout(UserPassLayout);

        LEAVE();
        return;
      }
    }
  }

  // set the password and start the connection
  m_sPassword = password;

  // start the connection NOW
  startConnection();

  LEAVE();
  return;
}

void CMainWindow::startConnection(void)
{
  ENTER();

	// now we construct the commandline arguments
	QStringList cmdArgs;

	// 1.Option: the 'pid' of this application
	cmdArgs << QString::number(QApplication::applicationPid());

	// 2.Option: supply the servertype (SRSS, RDP, etc)
	cmdArgs << m_sServerType;

	// 3.Option: say "true" if dtLoginMode is enabled
	cmdArgs << QString(m_bDtLoginMode == true ? "true" : "false");

	// 4.Option: the resolution (either "fullscreen" or "WxH"
	cmdArgs << m_sResolution.toLower();

	// 5.Option: the selected color depth
	cmdArgs << QString::number(m_iColorDepth);

	// 6.Option: the current color depth of the screen
	cmdArgs << QString::number(QPixmap().depth());

	// 7.Option: the selected keyboard (e.g. 'de' or 'en')
	cmdArgs << m_sKeyLayout.toLower();

  // 8.Option: the domain (e.g. FZR, UKD88)
  cmdArgs << (m_sDomain.isEmpty() ? "NULL" : m_sDomain);

  // 9.Option: the username
  cmdArgs << (m_sUsername.isEmpty() ? "NULL" : m_sUsername);

	// 10.Option: the servername we connect to
	cmdArgs << m_sServerName;
	
	// now we can create a QProcess object and execute the
	// startup script
	D("executing: %s %s", m_sStartupScript.toLatin1().constData(), cmdArgs.join(" ").toLatin1().constData());

	// start it now with the working directory pointing at the
  // directory where the app resists
  QProcess script;
  script.setWorkingDirectory(QApplication::applicationDirPath());
  script.start(m_sStartupScript, cmdArgs);
  if(script.waitForStarted() == true)
  {
    // now send the password via stdin
    if(m_sPassword.isEmpty() == false)
      script.write(m_sPassword.toLatin1().constData());
    else
      script.write("NULL");

    script.closeWriteChannel();
  
		// sync the QSettings only in non dtlogin mode
		if(m_bDtLoginMode == false)
			m_pSettings->sync();

    // wait until the script finished
    if(script.waitForFinished() == false)
      std::cout << "ERROR: Failed to wait for script being finished" << std::endl;

    // check the exit code
    if(script.exitCode() != 0)
    {
      std::cout << "ERROR: startup script '" << m_sStartupScript.toLatin1().constData() << "' returned an error code " << script.exitCode() << std::endl;

      QMessageBox::warning(this, tr("Could not start application"),
                                 tr("An Error occurred during startup. This could be caused because the application is not yet available. Please retry later."));
    }

    // output the standard output to cout
    QString standardOut = QString(script.readAllStandardOutput());
    if(standardOut.isEmpty() == false)
      std::cout << standardOut.toLatin1().constData() << std::endl;

    QString standardErr = QString(script.readAllStandardError());
    if(standardErr.isEmpty() == false)
      std::cerr << standardErr.toLatin1().constData() << std::endl;

		if(m_bKeepAlive == false)
			close();
  }
  else
  {
    std::cout << "ERROR: Failed to execute startup script " << m_sStartupScript.toLatin1().constData() << std::endl;
    QApplication::beep();
  }

  // and make sure we are showing the right
  // default layout
  changeLayout(CMainWindow::DefaultLayout);

	LEAVE();
}

void CMainWindow::keyPressEvent(QKeyEvent* e)
{
	ENTER();

  D("key %d pressed", e->key());

	// we check wheter the user has pressed ESC or RETURN
	switch(e->key())
	{
		case Qt::Key_Escape:
		{
      // depending on the layout we perform differently
      if(m_pStackedLayout->currentIndex() != CMainWindow::DefaultLayout)
        changeLayout(CMainWindow::DefaultLayout);
      else
			  close();

			e->accept();

			LEAVE();
			return;
		}
		break;

		case Qt::Key_Return:
		case Qt::Key_Enter:
		{
      // depending on the layout we perform differently
      if(m_pStackedLayout->currentIndex() != CMainWindow::DefaultLayout)
      {
        // if the username line edit is focused we move
        // on to the password line edit instead of starting the connection
        if(m_pUsernameLineEdit->hasFocus())
          m_pPasswordLineEdit->setFocus(Qt::OtherFocusReason);
        else
          pwButtonLoginClicked();
      }
      else
			  connectButtonPressed();

			e->accept();

			LEAVE();
			return;
		}
		break;
	}

  // activate the window which otherwise causes problems
  // if no window manager is running while qutselect is executed.
  if(m_bDtLoginMode)
    activateWindow();

	// unknown key pressed
	e->ignore();

	LEAVE();
}

void CMainWindow::closeEvent(QCloseEvent* e)
{
  ENTER();

  // save the current position and size of the GUI
  if(m_bDtLoginMode == false)
  {
    m_pSettings->setValue("position", pos());
    m_pSettings->setValue("size", size());
    m_pSettings->sync();
  }

  // call super method
  QMainWindow::closeEvent(e);

  LEAVE();
}

void CMainWindow::showEvent(QShowEvent* e)
{
  ENTER();

  // call super method
  QMainWindow::showEvent(e);

  // if in DtLogin mode we center the window on the current screen
  if(m_bDtLoginMode == true)
  {
    // make sure the sizes (width/height) are correct before continuing
    adjustSize();

		// now we make sure we centre the new window on the current
		// primary screen
		QDesktopWidget* desktopWidget = QApplication::desktop();
		QRect screenSize = desktopWidget->screenGeometry(desktopWidget->primaryScreen());
    move(QPoint((screenSize.width() - width())/2, (screenSize.height() - height())/2));
  }

  LEAVE();
}

void CMainWindow::loadServerList()
{
	ENTER();

	// we have to clear all things we are going to populate here
	m_pServerTreeWidget->clear();
	m_pServerListBox->clear();

	QFile serverListFile(m_sServerListFile);
	if(serverListFile.open(QFile::ReadOnly))
	{
		QTextStream in(&serverListFile);
		QRegExp regexp("^(.*);(.*);(.*);(.*);(.*);(.*);(.*);(.*)");
		QString curLine;

    // parse through the file now and add things to the ServerList and ComboBox
		while((curLine = in.readLine()).isNull() == false)
		{
			// skip any comment line starting with '#'
			if(curLine.at(0) != '#' && curLine.at(0) != '=' && regexp.indexIn(curLine) > -1)
			{
        QString displayname = regexp.cap(CN_DISPLAYNAME+1).simplified();
				QString hostname = regexp.cap(CN_HOSTNAME+1).simplified().toLower();
        QString domain = regexp.cap(CN_DOMAIN+1).simplified();
        QString serverType = regexp.cap(CN_SERVERTYPE+1).simplified();
        QString osType = regexp.cap(CN_SERVEROS+1).simplified();
				QString description = regexp.cap(CN_DESCRIPTION+1).simplified();
        QString pwprompt = regexp.cap(CN_PWPROMPT+1).simplified();
				QString script = regexp.cap(CN_STARTUPSCRIPT+1).simplified();

				// if m_bNoSRSS we filter out any SRSS in our list
				if(m_bNoSRSS == false || serverType != "SRSS")
				{
					// add the server to our listview
					QStringList columnList;
          columnList << displayname;
					columnList << hostname;
          columnList << domain;
					columnList << serverType;
					columnList << osType;
					columnList << description;
          columnList << pwprompt;
					columnList << script;

					// create a new QTreeWidget and set the font for the first column to bold
					QTreeWidgetItem* item = new QTreeWidgetItem(columnList);
					QFont displayNameFont = item->font(CN_DISPLAYNAME);
					displayNameFont.setBold(true);
	        if(m_bDtLoginMode == true)
					  displayNameFont.setPointSize(18);
          else
					  displayNameFont.setPointSize(12);
					item->setFont(CN_DISPLAYNAME, displayNameFont);

					// add an icon depending on the OS type
					QIcon serverIcon;
					if(osType.contains("linux", Qt::CaseInsensitive))
						serverIcon = QIcon(":/images/linux-logo.png");
					else if(osType.contains("solaris", Qt::CaseInsensitive))
						serverIcon = QIcon(":/images/solaris-logo.png");
					else if(osType.contains("windows", Qt::CaseInsensitive))
						serverIcon = QIcon(":/images/windows-logo.png");
					else if(osType.contains("macos", Qt::CaseInsensitive))
						serverIcon = QIcon(":/images/macos-logo.png");
					else if(osType.contains("firefox", Qt::CaseInsensitive))
						serverIcon = QIcon(":/images/firefox-logo.png");

					// add the icon to our items
					item->setIcon(0, serverIcon);
					m_pServerListBox->addItem(serverIcon, hostname + " - " + description);

					m_pServerTreeWidget->addTopLevelItem(item);
				}
			}
      else if(curLine.startsWith("==="))
      {
        QTreeWidgetItem* item = new QTreeWidgetItem();
        item->setText(0, "==========");
        item->setFlags(Qt::NoItemFlags);
        m_pServerTreeWidget->addTopLevelItem(item);

        for(int i=0; i < m_pServerTreeWidget->columnCount(); i++)
        {
          QFrame* hbarFrame = new QFrame;
	        hbarFrame->setFrameStyle(QFrame::HLine | QFrame::Raised);
          hbarFrame->setAutoFillBackground(true);
          m_pServerTreeWidget->setItemWidget(item, i, hbarFrame);
        }
      }
		}
		
		// close the file
		serverListFile.close();

		// resize all columns to its content
		m_pServerTreeWidget->resizeColumnToContents(CN_DISPLAYNAME);
    m_pServerTreeWidget->header()->setSectionResizeMode(CN_DISPLAYNAME, QHeaderView::ResizeToContents);
		m_pServerTreeWidget->resizeColumnToContents(CN_HOSTNAME);
    m_pServerTreeWidget->header()->setSectionResizeMode(CN_HOSTNAME, QHeaderView::ResizeToContents);
		m_pServerTreeWidget->resizeColumnToContents(CN_SERVEROS);
    m_pServerTreeWidget->header()->setSectionResizeMode(CN_SERVEROS, QHeaderView::ResizeToContents);
		m_pServerTreeWidget->resizeColumnToContents(CN_DESCRIPTION);
    m_pServerTreeWidget->header()->setSectionResizeMode(CN_DESCRIPTION, QHeaderView::ResizeToContents);
    m_pServerTreeWidget->updateGeometry();

    QString selectServerName;
		bool serverFound = false;
		if(m_bDtLoginMode == true)
		{
			// set the current item in the ServerTreeWidget to the hostname of the server
      // this qutselect runs on
		  char hostName[256];
      if(gethostname(hostName, 256) == 0)
			  selectServerName = QString(hostName).toLower();

			D("got hostname: '%s'", selectServerName.toLatin1().constData());
		}
		else
		{
			// now we check the QSettings of the user and which server he last used
			if(m_pSettings->value("serverused").isValid())
			{
				selectServerName = m_pSettings->value("serverused").toString().toLower();

				D("read serverused from QSettings: '%s'", selectServerName.toLatin1().constData());
      }
    }

		// now we iterate through our combobox items and check if there is 
		// one with the last server used hostname
		for(int i=0; i < m_pServerListBox->count(); i++)
		{
			if(m_pServerListBox->itemText(i).section(" ", 0, 0).toLower() == selectServerName)
			{
				D("setting ServerListComboBox to %d item", i);

        m_pServerTreeWidget->setCurrentItem(m_pServerTreeWidget->topLevelItem(i));

				m_pServerListBox->setCurrentIndex(-1);
				m_pServerListBox->setCurrentIndex(i);
				serverFound = true;
				break;
			}
		}
		

		if(serverFound == false)
      m_pServerTreeWidget->setCurrentItem(m_pServerTreeWidget->topLevelItem(0));
	}
	else
	{
		W("couldn't open server list file: '%s'", m_sServerListFile.toLatin1().constData());
		m_pServerListBox->setEditable(true);
		m_pServerTypeComboBox->setCurrentIndex(RDP);
	}

	LEAVE();
}

void CMainWindow::changeLayout(enum LayoutType type)
{
  ENTER();

  // now we change the GUI layout accordingt to the supplied type
  switch(type)
  {
    case CMainWindow::DefaultLayout:
    {
      // lets clear the password right away!
      m_sPassword.clear();
      m_pPasswordLineEdit->clear();

      m_pStackedLayout->setCurrentIndex(CMainWindow::DefaultLayout);     
      m_pStartButton->setFocus(Qt::OtherFocusReason);

      m_pPasswordEnterTimer->stop();
    }
    break;

    case CMainWindow::UserPassLayout:
    {
      m_pPasswordLayoutLabel->setText(tr("Please enter login data for server <b>%1</b>:").arg(m_sServerName));
      m_pPasswordButtonBox->button(QDialogButtonBox::Ok)->setText(tr("Login"));
      m_pPasswordButtonBox->button(QDialogButtonBox::Cancel)->setText(tr("Abort"));
      m_pStackedLayout->setCurrentIndex(CMainWindow::UserPassLayout);
      m_pUsernameLabel->setVisible(true);
      m_pUsernameLineEdit->setVisible(true);

      if(m_sUsername.isEmpty() == false)
      {
        m_pUsernameLineEdit->setText(m_sUsername);
        m_pPasswordLineEdit->setFocus(Qt::OtherFocusReason);
      }
      else
        m_pUsernameLineEdit->setFocus(Qt::OtherFocusReason);

      // now we start a QTimer() which resets the layout to the default one and removes
      // the password stuff after 30 seconds for security reasons.
      m_pPasswordEnterTimer->start(30000);
    }
    break;

    case CMainWindow::PassLayout:
    {
      m_pPasswordLayoutLabel->setText(tr("Please enter password for server <b>%1</b>:").arg(m_sServerName));
      m_pPasswordButtonBox->button(QDialogButtonBox::Ok)->setText(tr("Login"));
      m_pPasswordButtonBox->button(QDialogButtonBox::Cancel)->setText(tr("Abort"));
      m_pStackedLayout->setCurrentIndex(CMainWindow::UserPassLayout);
      m_pUsernameLabel->setVisible(false);
      m_pUsernameLineEdit->setVisible(false);
      m_pPasswordLineEdit->setFocus(Qt::OtherFocusReason);

      // now we start a QTimer() which resets the layout to the default one and removes
      // the password stuff after 30 seconds for security reasons.
      m_pPasswordEnterTimer->start(30000);
    }
    break;
 
  }

  LEAVE();
}

bool CMainWindow::passwordDialog(const QString& serverName, QString& username, QString& password)
{
  bool result = false;
  ENTER();

  // open a login dialog
  CLoginDialog* loginDialog = new CLoginDialog(this, username, serverName);
  if(loginDialog->exec() == QDialog::Accepted)
  {
    username = loginDialog->username();
    password = loginDialog->password();

    result = true;
  }

  delete loginDialog;

  RETURN(result);
  return result;
}

void CMainWindow::pwButtonLoginClicked(void)
{
  ENTER();

  // set username/password and assume all
  // other things have been set correctly
  // already.
  m_sUsername = m_pUsernameLineEdit->text();
  m_sPassword = m_pPasswordLineEdit->text();

  // start the connection NOW
  startConnection();

  LEAVE();
}

void CMainWindow::pwButtonCancelClicked(void)
{
  ENTER();

  m_pPasswordLineEdit->clear();

  // make sure to return to the default layout
  changeLayout(CMainWindow::DefaultLayout);

  LEAVE();
}

void CMainWindow::passwordTimedOut(void)
{
  ENTER();

  // reset the layout to the default one and clear the password stuff
  changeLayout(CMainWindow::DefaultLayout);

  LEAVE();
}

