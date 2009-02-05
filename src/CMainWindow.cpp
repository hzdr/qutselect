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

#include "CMainWindow.h"

#include <QApplication>
#include <QLabel>
#include <QButtonGroup>
#include <QComboBox>
#include <QDesktopWidget>
#include <QDir>
#include <QFile>
#include <QKeyEvent>
#include <QGridLayout>
#include <QMessageBox>
#include <QPoint>
#include <QPushButton>
#include <QRadioButton>
#include <QProcess>
#include <QRegExp>
#include <QSettings>
#include <QSound>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QPushButton>
#include <QHostInfo>
#include <QTreeWidget>
#include <QLineEdit>

#include <iostream>

#include <rtdebug.h>

// standard width/height
#define WINDOW_WIDTH	450
#define WINDOW_HEIGHT 600

// the default startup script pattern
#define DEFAULT_SCRIPT_PATTERN "/usr/local/petlib/qutselect_connect_%1.sh"

// the column numbers
enum ColumnNumbers { CN_SERVERNAME=0,
										 CN_SERVERTYPE,
										 CN_SERVEROS,
										 CN_DESCRIPTION,
										 CN_STARTUPSCRIPT
									 };

#include "config.h"

CMainWindow::CMainWindow(bool dtLoginMode)
	: m_bKeepAlive(false),
		m_bDtLoginMode(dtLoginMode)
{
	ENTER();

  // create the central widget to which we are going to add everything
  QWidget* centralWidget = new QWidget;
  setCentralWidget(centralWidget);

	// create a QSettings object to receive the users specific settings
	// written the last time the user used that application
	m_pSettings = new QSettings("fzd.de", "qutselect");

	// we put a logo at the top
	m_pLogoLabel = new QLabel();
	m_pLogoLabel->setPixmap(QPixmap(":/images/banner-en.png"));
	m_pLogoLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	m_pLogoLabel->setAlignment(Qt::AlignCenter);
	
  // create the treewidget we are going to populate
  m_pServerTreeWidget = new QTreeWidget();
  m_pServerTreeWidget->setRootIsDecorated(false);
  m_pServerTreeWidget->setAllColumnsShowFocus(true);
	connect(m_pServerTreeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
					this,								 SLOT(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
	connect(m_pServerTreeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
					this,								 SLOT(itemDoubleClicked(QTreeWidgetItem*, int)));
	
	// add the columns with the labels
	QStringList columnNames;
	columnNames << tr("Server") << tr("Server") << tr("System") << tr("Description") << tr("Script");
	m_pServerTreeWidget->setHeaderLabels(columnNames);
  m_pServerTreeWidget->setColumnHidden(CN_SERVERTYPE, true);
  m_pServerTreeWidget->setColumnHidden(CN_STARTUPSCRIPT, true);

  // create the ServerLineEdit
  m_pServerLineEdit = new QLineEdit();

	// create a combobox for the different ServerTypes we have
	m_pServerTypeComboBox = new QComboBox();
	m_pServerTypeComboBox->addItem("Unix (SRSS)");
	m_pServerTypeComboBox->addItem("Windows (RDP)");
	m_pServerTypeComboBox->addItem("VNC");
	connect(m_pServerTypeComboBox, SIGNAL(currentIndexChanged(int)),
					this,									 SLOT(serverTypeChanged(int)));

	// combine the LineEdit and the Combobox
	QHBoxLayout* serverLineLayout = new QHBoxLayout();
	serverLineLayout->addWidget(m_pServerLineEdit);
	serverLineLayout->addWidget(m_pServerTypeComboBox);

  // create the serverListLayout
	QVBoxLayout* serverListLayout = new QVBoxLayout();
  serverListLayout->addWidget(m_pServerTreeWidget);
  serverListLayout->addLayout(serverLineLayout);

	// the we need a combobox for the different server a user can select
	m_pServerListLabel = new QLabel(tr("Server:"));
	m_pServerListBox = new QComboBox();

	// selection of the screen depth
	m_pScreenResolutionLabel = new QLabel(tr("Resolution:"));
	m_pScreenResolutionBox = new QComboBox();
	m_pScreenResolutionBox->addItem("800x600");
	m_pScreenResolutionBox->addItem("1024x768");
	m_pScreenResolutionBox->addItem("1152x900");
	m_pScreenResolutionBox->addItem("1280x1024");
	m_pScreenResolutionBox->addItem("1600x1200");
	m_pScreenResolutionBox->addItem("Fullscreen");

	// we check the QSettings for "resolution" and see if we
	// can use it or not
	if(m_bDtLoginMode == false && m_pSettings->value("resolution").isValid())
	{
		QString resolution = m_pSettings->value("resolution").toString();

		if(resolution.toLower() == "fullscreen")
			m_pScreenResolutionBox->setCurrentIndex(5);
		else
		{
			int width = resolution.section("x", 0, 0).toInt();

			if(width >= 1600)
				m_pScreenResolutionBox->setCurrentIndex(4);
			else if(width >= 1280)
				m_pScreenResolutionBox->setCurrentIndex(3);
			else if(width >= 1152)
				m_pScreenResolutionBox->setCurrentIndex(2);
			else if(width >= 1024)
				m_pScreenResolutionBox->setCurrentIndex(1);
			else
				m_pScreenResolutionBox->setCurrentIndex(0);
		}
	}
	else
	{
		QDesktopWidget* desktopWidget = QApplication::desktop();
		QRect screenSize = desktopWidget->screenGeometry(desktopWidget->primaryScreen());
		
		if(screenSize.width() > 1600)
			m_pScreenResolutionBox->setCurrentIndex(4);
		else if(screenSize.width() > 1280)
			m_pScreenResolutionBox->setCurrentIndex(3);
		else if(screenSize.width() > 1152)
			m_pScreenResolutionBox->setCurrentIndex(2);
		else if(screenSize.width() > 1024)
			m_pScreenResolutionBox->setCurrentIndex(1);
		else
			m_pScreenResolutionBox->setCurrentIndex(0);
	}

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
	QHBoxLayout* colorsButtonLayout = new QHBoxLayout();
	colorsButtonLayout->setMargin(0);
	colorsButtonLayout->addWidget(m_p8bitColorsButton);
	colorsButtonLayout->addWidget(m_p16bitColorsButton);
	colorsButtonLayout->addWidget(m_p24bitColorsButton);
	colorsButtonLayout->addStretch(1);

	// now we check the QSettings for the last selected color depth
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

	// keyboard layout selection radiobuttons
	m_pKeyboardLabel = new QLabel(tr("Keyboard:"));
	m_pGermanKeyboardButton = new QRadioButton(tr("German"));
	m_pEnglishKeyboardButton = new QRadioButton(tr("English"));
	QButtonGroup* keyboardGroup = new QButtonGroup();
	keyboardGroup->addButton(m_pGermanKeyboardButton);
	keyboardGroup->addButton(m_pEnglishKeyboardButton);
	keyboardGroup->setExclusive(true);
	QHBoxLayout* keyboardButtonLayout = new QHBoxLayout();
	keyboardButtonLayout->setMargin(0);
	keyboardButtonLayout->addWidget(m_pGermanKeyboardButton);
	keyboardButtonLayout->addWidget(m_pEnglishKeyboardButton);
	keyboardButtonLayout->addStretch(1);

	// select the german keyboard as default
	m_pGermanKeyboardButton->setChecked(true);

	// check the QSettings for the last used keyboard layout
	if(m_bDtLoginMode)
	{
		// in dtlogin mode we identify the keyboard vis QApplication::keyboardInputLocale()
		QLocale keyboardLocale = QApplication::keyboardInputLocale();

		D("keyboardLocalName: %s", keyboardLocale.name().toAscii().constData());
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
	buttonLayout->addStretch(1);
	buttonLayout->addWidget(m_pStartButton);
	buttonLayout->setStretchFactor(m_pStartButton, 2);
	connect(m_pQuitButton, SIGNAL(clicked()),
					this,					 SLOT(close()));
	connect(m_pStartButton,SIGNAL(clicked()),
					this,					 SLOT(startButtonPressed()));
	
	QGridLayout* layout = new QGridLayout;
	layout->addWidget(m_pLogoLabel,								0, 0, 1, 2);
	layout->addWidget(m_pServerListLabel,					1, 0);

	if(m_bDtLoginMode)
		layout->addLayout(serverListLayout, 			  1, 1);
	else
		layout->addWidget(m_pServerListBox,					1, 1);

	layout->addWidget(m_pScreenResolutionLabel,		2, 0);
	layout->addWidget(m_pScreenResolutionBox,			2, 1);
	layout->addWidget(m_pColorsLabel,							3, 0);
	layout->addLayout(colorsButtonLayout,					3, 1);
	layout->addWidget(m_pKeyboardLabel,						4, 0);
	layout->addLayout(keyboardButtonLayout,				4, 1);
	layout->addWidget(buttonFrame,								5, 0, 1, 2);
	layout->addLayout(buttonLayout,								6, 0, 1, 2);
	centralWidget->setLayout(layout);

	// now we try to open the serverlist file and add the items to our comobox
	loadServerList();

	// check if the QSettings contains any info about the last position
	if(m_bDtLoginMode)
	{
		// set the current item in the ServerTreeWidget
		m_pServerTreeWidget->setCurrentItem(m_pServerTreeWidget->topLevelItem(0));

		// make sure to also change some settings according to
		// the dtlogin mode
		setKeepAlive(true);
		setFullScreenOnly(true);
		setQuitText(tr("Logout"));		

		// now we make sure we centre the new window on the current
		// primary screen
		QDesktopWidget* desktopWidget = QApplication::desktop();
		QRect screenSize = desktopWidget->screenGeometry(desktopWidget->primaryScreen());

		// set the geometry of the current widget
		setGeometry((screenSize.width() - WINDOW_WIDTH)/2, (screenSize.height() - WINDOW_HEIGHT)/2,
								WINDOW_WIDTH, WINDOW_HEIGHT);
	}
	else
	{
		// now we check the QSettings of the user and which server he last used
		if(m_pSettings->value("serverused").isValid())
		{
			QString lastServerUsed = m_pSettings->value("serverused").toString().toLower();

			// now we iterate through our combobox items and check if there is 
			// one with the last server used hostname
			for(int i=0; i < m_pServerListBox->count(); i++)
			{
				if(m_pServerListBox->itemText(i).section(" ", 0, 0).toLower() == lastServerUsed)
				{
					m_pServerListBox->setCurrentIndex(i);
					break;
				}
			}
		}

		move(m_pSettings->value("position", QPoint(10, 10)).toPoint());
	}
	
	setWindowTitle("qutselect v" PACKAGE_VERSION " - (c) 2005-2009 fzd.de");

	LEAVE();
}

CMainWindow::~CMainWindow()
{
	ENTER();

	delete m_pSettings;
	
	LEAVE();
}

void CMainWindow::serverTypeChanged(int index)
{
	ENTER();

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
		}
		break;
	}

	LEAVE();
}

void CMainWindow::currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
	ENTER();

	// update the lineedit with the text of the first column
	m_pServerLineEdit->setText(current->text(CN_SERVERNAME));

	// make sure the combobox is in sync with the lineedit
	m_pServerListBox->setCurrentIndex(m_pServerListBox->findText(current->text(CN_SERVERNAME), Qt::MatchStartsWith));

	// now we have to check which server type the currently selected server
	// is and then disable some GUI components of qutselect
	QString serverType = current->text(1);

	// sync the ServerType combobox as well
	m_pServerTypeComboBox->setCurrentIndex(m_pServerTypeComboBox->findText(serverType, Qt::MatchContains));

	LEAVE();
}

void CMainWindow::itemDoubleClicked(QTreeWidgetItem* item, int column)
{
	ENTER();

	D("Server '%s' doubleclicked", item->text(CN_SERVERNAME).toAscii().constData());

	// a doubleclick is like pressing the "connect" button
	startButtonPressed();

	LEAVE();
}

void CMainWindow::setFullScreenOnly(const bool on)
{
	ENTER();

	if(on)
	{
		m_pScreenResolutionBox->setCurrentIndex(5); // full screen
		m_pScreenResolutionBox->setEnabled(false);

		// hide some components competely
		m_pScreenResolutionLabel->setVisible(false);
		m_pScreenResolutionBox->setVisible(false);
	}
	else
	{
		m_pScreenResolutionBox->setEnabled(true);

		// hide some components competely
		m_pScreenResolutionLabel->setVisible(true);
		m_pScreenResolutionBox->setVisible(true);
	}

	LEAVE();
}

void CMainWindow::startButtonPressed(void)
{
	ENTER();

	// save the current position of the GUI
	m_pSettings->setValue("position", pos());

	// get the currently selected server name
	QString serverName = m_pServerLineEdit->text().section(" ", 0, 0).toLower();
	m_pSettings->setValue("serverused", serverName);
	
	// get the currently selected resolution
	QString resolution = m_pScreenResolutionBox->currentText().section(" ", 0, 0).toLower();
	if(m_pScreenResolutionBox->isEnabled() && m_pScreenResolutionBox->isVisible())
		m_pSettings->setValue("resolution", resolution);

	// get the keyboard layout the user wants to have
	QString keyLayout = m_pGermanKeyboardButton->isChecked() ? "de" : "en";
	m_pSettings->setValue("keyboard", keyLayout);

	// get the color depth
	short colorDepth;
	if(m_p8bitColorsButton->isChecked())
		colorDepth = 8;
	else if(m_p16bitColorsButton->isChecked())
		colorDepth = 16;
	else
		colorDepth = 24;

	m_pSettings->setValue("colordepth", colorDepth);

	// sync the QSettings only in non dtlogin mode
	if(m_bDtLoginMode == false)
		m_pSettings->sync();
	else
		m_pSettings->clear();

	// now we get the serverType of the current selection
	QString serverType;
	switch(m_pServerTypeComboBox->currentIndex())
	{
		case SRSS:
			serverType = "SRSS";
		break;

		case RDP:
			serverType = "RDP";
		break;

		case VNC:
			serverType = "VNC";
		break;
	}

	// find the selected server in the tree widget to retrieve the
	// startup script name
	QString startupScript;
	QList<QTreeWidgetItem*> items = m_pServerTreeWidget->findItems(serverName, Qt::MatchStartsWith);
	if(items.isEmpty() == false)
		startupScript = items.first()->text(CN_STARTUPSCRIPT);
	else
	{
		// if this didn't work out we use the pattern to create a generic script
		startupScript = QString(DEFAULT_SCRIPT_PATTERN).arg(serverType.toLower());
	}

	// now we construct the commandline arguments
	QStringList cmdArgs;

	// 1.Option: the 'pid' of this application
	cmdArgs << QString::number(QApplication::applicationPid());

	// 2.Option: supply the servertype (SRSS, RDP, etc)
	cmdArgs << serverType;

	// 3.Option: say "true" if dtLoginMode is enabled
	cmdArgs << QString(m_bDtLoginMode == true ? "true" : "false");

	// 4.Option: the resolution (either "fullscreen" or "WxH"
	cmdArgs << resolution.toLower();

	// 5.Option: the selected color depth
	cmdArgs << QString::number(colorDepth);

	// 6.Option: the current color depth of the screen
	cmdArgs << QString::number(QPixmap().depth());

	// 7.Option: the selected keyboard (e.g. 'de' or 'en')
	cmdArgs << keyLayout.toLower();

	// 8.Option: the servername we connect to
	cmdArgs << serverName;
	
	// now we can create a QProcess object and execute the
	// startup script
	D("executing: %s %s", startupScript.toAscii().constData(), cmdArgs.join(" ").toAscii().constData());

	// start it now
	bool started = QProcess::startDetached(startupScript, cmdArgs);

	// depending on the keepalive state we either close the GUI immediately or keep it open
	if(started == true)
	{
		if(m_bKeepAlive == false)
			close();
	}
	else
		std::cout << "ERROR: Failed to execute startup script " << startupScript.toAscii().constData() << std::endl;

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
			close();
			e->accept();

			LEAVE();
			return;
		}
		break;

		case Qt::Key_Return:
		case Qt::Key_Enter:
		{
			startButtonPressed();
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

void CMainWindow::loadServerList()
{
	ENTER();

	QFile serverListFile(QDir(QApplication::instance()->applicationDirPath()).absoluteFilePath("qutselect.slist"));
	if(serverListFile.open(QFile::ReadOnly))
	{
		QTextStream in(&serverListFile);
		QRegExp regexp("^(.*);(.*);(.*);(.*);(.*)");
		QString curLine;

    // parse through the file now and add things to the ServerList and ComboBox
		while((curLine = in.readLine()).isNull() == false)
		{
			// skip any comment line starting with '#'
			if(curLine.at(0) != '#' && curLine.at(0) != '-' && regexp.indexIn(curLine) > -1)
			{
				QString hostname = regexp.cap(CN_SERVERNAME+1).simplified().toLower();
        QString serverType = regexp.cap(CN_SERVERTYPE+1).simplified();
        QString osType = regexp.cap(CN_SERVEROS+1).simplified();
				QString description = regexp.cap(CN_DESCRIPTION+1).simplified();
				QString script = regexp.cap(CN_STARTUPSCRIPT+1).simplified();

        // add the server to our combobox
				m_pServerListBox->addItem(hostname+" - "+description);

        // add the server to our listview
				QStringList columnList;

				columnList << hostname;
				columnList << serverType;
				columnList << osType;
				columnList << description;
				columnList << script;

				// create a new QTreeWidget and set the font for the first column (servername) to Bold
				QTreeWidgetItem* item = new QTreeWidgetItem(columnList);
				QFont serverNameFont = item->font(CN_SERVERNAME);
				serverNameFont.setBold(true);
        serverNameFont.setPointSize(12);
        //serverNameFont.setCapitalization(QFont::SmallCaps);
				item->setFont(CN_SERVERNAME, serverNameFont);

				m_pServerTreeWidget->addTopLevelItem(item);
			}
		}
		
		serverListFile.close();
	}
	else
		m_pServerListBox->setEditable(true);

	LEAVE();
}
