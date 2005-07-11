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

#include <QLabel>
#include <QButtonGroup>
#include <QComboBox>
#include <QGridLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QProcess>
#include <QString>

#include <iostream>

CRDesktopWindow::CRDesktopWindow()
{
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

	// selection of the screen depth
	m_pScreenResolutionLabel = new QLabel(tr("Resolution:"));
	m_pScreenResolutionBox = new QComboBox();
	m_pScreenResolutionBox->addItem("800x600");
	m_pScreenResolutionBox->addItem("1024x768");
	m_pScreenResolutionBox->addItem("1152x900");
	m_pScreenResolutionBox->addItem("1280x1024");
	m_pScreenResolutionBox->addItem("Fullscreen");

	// color depth selection
	m_pColorsLabel = new QLabel(tr("Colors:"));
	m_p8bitColorsButton = new QRadioButton(tr("256"));
	m_p16bitColorsButton = new QRadioButton(tr("65535"));
	m_p16bitColorsButton->setChecked(true);
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

	// keyboard layout selection radiobuttons
	m_pKeyboardLabel = new QLabel(tr("Keyboard:"));
	m_pGermanKeyboardButton = new QRadioButton(tr("German"));
	m_pGermanKeyboardButton->setChecked(true);
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

	setWindowTitle(tr("qRDesktop v1.0 - (c) 2005 Jens Langner"));
}

void CRDesktopWindow::startButtonPressed(void)
{
	// get some required data
	QString serverName = m_pServerListBox->currentText().section(" ", 0, 0);
	QString resolution = m_pScreenResolutionBox->currentText().section(" ", 0, 0);

	// form the commandString
	QString commandString;
	commandString = "rdesktop ";
	
	// check the resolution combobox
	if(resolution == "Fullscreen")
		commandString += "-f ";
	else
		commandString += "-g " + resolution + " ";

	// check the keyboard selection
	if(m_pGermanKeyboardButton->isChecked())
		commandString += "-k de ";
	else if(m_pEnglishKeyboardButton->isChecked())
		commandString += "-k en-us ";

	// check color settings
	if(m_p8bitColorsButton->isChecked())
		commandString += "-a 8 ";
	else if(m_p16bitColorsButton->isChecked())
		commandString += "-a 16 ";
	else if(m_p24bitColorsButton->isChecked())
		commandString += "-a 24 ";

	// add some general options
	commandString += "-r sound:local -E -x lan -N -P ";

	// add the serverName
	commandString += serverName;

	// now output the string to the user
	std::cout << "executing: " << commandString.toAscii().constData() << std::endl;
	
	// now we can create a QProcess object and start "rdesktop"
	// accordingly
	QProcess::startDetached(commandString);

	close();
}
