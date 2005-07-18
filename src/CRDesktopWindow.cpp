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

#include "CRDesktopWindow.h"

#include <QApplication>
#include <QLabel>
#include <QButtonGroup>
#include <QComboBox>
#include <QDesktopWidget>
#include <QKeyEvent>
#include <QGridLayout>
#include <QPoint>
#include <QPushButton>
#include <QRadioButton>
#include <QProcess>
#include <QSettings>
#include <QSound>
#include <QString>
#include <QStringList>

#include <iostream>

CRDesktopWindow::CRDesktopWindow(bool noUserPosition)
	: m_bKeepAlive(false),
		m_bNoUserPosition(noUserPosition)
{
	// create a QSettings object to receive the users specific settings
	// written the last time the user used that application
	QSettings settings("fz-rossendorf.de", "qrdesktop");

	// we put a logo at the top
	m_pLogoLabel = new QLabel();
	m_pLogoLabel->setPixmap(QPixmap(":/images/banner-en.png"));
	m_pLogoLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
	m_pLogoLabel->setAlignment(Qt::AlignCenter);
	
	// the we need a combobox for the different server a user can select
	m_pServerListLabel = new QLabel(tr("Terminal Server:"));
	m_pServerListBox = new QComboBox();
	m_pServerListBox->addItem("fwbts.ad");
	m_pServerListBox->addItem("fwbts04.ad - Allgemein");
	m_pServerListBox->addItem("fwbts02.ad - Micropet");
	m_pServerListBox->addItem("fwbts03.ad - Micropet");
	m_pServerListBox->addItem("fwbts01.ad - Printer");
	m_pServerListBox->addItem("fwbfs01.ad - (Admin only)");

	// now we check the QSettings of the user and which server he last used
	if(settings.value("serverused").isValid())
	{
		QString lastServerUsed = settings.value("serverused").toString().toLower();

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

	// selection of the screen depth
	m_pScreenResolutionLabel = new QLabel(tr("Resolution:"));
	m_pScreenResolutionBox = new QComboBox();
	m_pScreenResolutionBox->addItem("800x600");
	m_pScreenResolutionBox->addItem("1024x768");
	m_pScreenResolutionBox->addItem("1152x900");
	m_pScreenResolutionBox->addItem("1280x1024");
	m_pScreenResolutionBox->addItem("Fullscreen");

	// we check the QSettings for "resolution" and see if we
	// can use it or not
	if(settings.value("resolution").isValid())
	{
		QString resolution = settings.value("resolution").toString();

		if(resolution.toLower() == "fullscreen")
			m_pScreenResolutionBox->setCurrentIndex(4);
		else
		{
			int width = resolution.section("x", 0, 0).toInt();

			if(width >= 1280)
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
		if(desktopWidget->width() > 1280)
			m_pScreenResolutionBox->setCurrentIndex(3);
		else if(desktopWidget->width() > 1152)
			m_pScreenResolutionBox->setCurrentIndex(2);
		else if(desktopWidget->width() > 1024)
			m_pScreenResolutionBox->setCurrentIndex(1);
		else
			m_pScreenResolutionBox->setCurrentIndex(0);
	}

	// color depth selection
	m_pColorsLabel = new QLabel(tr("Colors:"));
	m_p8bitColorsButton = new QRadioButton(tr("256"));
	m_p16bitColorsButton = new QRadioButton(tr("65535"));
	m_p24bitColorsButton = new QRadioButton(tr("Millions"));
	QButtonGroup* colorsGroup = new QButtonGroup();
	colorsGroup->addButton(m_p8bitColorsButton);
	colorsGroup->addButton(m_p16bitColorsButton);
	colorsGroup->addButton(m_p24bitColorsButton);
	colorsGroup->setExclusive(true);
	QHBoxLayout* colorButtonLayout = new QHBoxLayout();
	colorButtonLayout->setMargin(0);
	colorButtonLayout->addWidget(m_p8bitColorsButton);
	colorButtonLayout->addWidget(m_p16bitColorsButton);
	colorButtonLayout->addWidget(m_p24bitColorsButton);
	colorButtonLayout->addStretch(1);

	// now we check the QSettings for the last selected color depth
	int depth = settings.value("colordepth", 16).toInt();
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

	// check the QSettings for the last used keyboard layout
	QString keyboard = settings.value("keyboard", "de").toString();
	if(keyboard.toLower() == "en-us")
		m_pEnglishKeyboardButton->setChecked(true);
	else
		m_pGermanKeyboardButton->setChecked(true);

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
	layout->addWidget(m_pServerListBox,						1, 1);
	layout->addWidget(m_pScreenResolutionLabel,		2, 0);
	layout->addWidget(m_pScreenResolutionBox,			2, 1);
	layout->addWidget(m_pColorsLabel,							3, 0);
	layout->addLayout(colorButtonLayout,					3, 1);
	layout->addWidget(m_pKeyboardLabel,						4, 0);
	layout->addLayout(keyboardButtonLayout,				4, 1);
	layout->addWidget(buttonFrame,								5, 0, 1, 2);
	layout->addLayout(buttonLayout,								6, 0, 1, 2);
	setLayout(layout);

	// check if the QSettings contains any info about the last position
	if(noUserPosition)
		move(QPoint(10, 10));
	else
		move(settings.value("position", QPoint(10, 10)).toPoint());

	setWindowTitle(tr("qRDesktop v1.4 - (c) 2005 Jens Langner"));
}

void CRDesktopWindow::setFullScreenOnly(const bool on)
{
	if(on)
	{
		m_pScreenResolutionBox->setCurrentIndex(4); // full screen
		m_pScreenResolutionBox->setEnabled(false);
	}
	else
	{
		m_pScreenResolutionBox->setEnabled(true);
	}
}

void CRDesktopWindow::startButtonPressed(void)
{
	// during setup we go and save the current setup of
	// affairs to a QSettings object
	QSettings settings("fz-rossendorf.de", "qrdesktop");

	// save the current position of the GUI
	if(m_bNoUserPosition == false)
		settings.setValue("position", pos());

	// get the currently selected server name
	QString serverName = m_pServerListBox->currentText().section(" ", 0, 0);
	settings.setValue("serverused", serverName.toLower());
	
	// get the currently selected resolution
	QString resolution = m_pScreenResolutionBox->currentText().section(" ", 0, 0);
	settings.setValue("resolution", resolution.toLower());
	
	// lets generate the commandline options stringlist
	QStringList arguments;
	arguments << "rdesktop";

	// check the resolution combobox
	if(resolution == "Fullscreen")
		arguments << "-f";
	else
		arguments << "-g" << resolution;

	// check the keyboard selection
	if(m_pGermanKeyboardButton->isChecked())
	{
		arguments << "-k" << "de";
		settings.setValue("keyboard", "de");
	}
	else if(m_pEnglishKeyboardButton->isChecked())
	{
		arguments << "-k" << "en-us";
		settings.setValue("keyboard", "en-us");
	}

	// now we try to find out how many colors the current PaintDevice supports
	// and if it supports only 8bit color depth, then we have to set the private
	// colormap option for rdesktop
	QPixmap pixmap;
	if(m_p8bitColorsButton->isChecked())
	{
		arguments << "-a" << "8";

		if(pixmap.depth() <= 8)
			arguments << "-C";

		settings.setValue("colordepth", 8);
	}
	else if(m_p16bitColorsButton->isChecked())
	{
		arguments << "-a" << "16";

		if(pixmap.depth() < 16)
			arguments << "-C";	

		settings.setValue("colordepth", 16);
	}
	else if(m_p24bitColorsButton->isChecked())
	{
		arguments << "-a" << "24";

		if(pixmap.depth() < 24)
			arguments << "-C";		

		settings.setValue("colordepth", 24);
	}

	// we add sound redirection
	arguments << "-r" << "sound:local";

	// add some general options
	arguments << "-E";
	arguments << "-x" << "lan";
	arguments << "-N";
	arguments << "-P";
	arguments << "-d" << "FZR";
	
	// add last but not least the final terminal server hostname
	arguments << serverName;

	// now output the string to the user
	QString args = arguments.join(" ");
	std::cout << "executing: 'nice " << args.toAscii().constData() << "'" << std::endl;
	
	// now we can create a QProcess object and start "rdesktop"
	// accordingly in nice mode
	QProcess::startDetached("nice", arguments);

	// depending on the keepalive state we either close the GUI immediately or keep it open
	if(m_bKeepAlive == false)
		close();
}

void CRDesktopWindow::keyPressEvent(QKeyEvent* e)
{
	// we check wheter the user has pressed ESC or RETURN
	switch(e->key())
	{
		case Qt::Key_Escape:
		{
			close();
			e->accept();

			return;
		}
		break;

		case Qt::Key_Return:
		case Qt::Key_Enter:
		{
			startButtonPressed();
			e->accept();

			return;
		}
		break;
	}

	// unknown key pressed
	e->ignore();
}
