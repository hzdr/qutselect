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

#include "CLoginDialog.h"

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QPushButton>

CLoginDialog::CLoginDialog(QWidget *parent,
                           const QString& username,
                           const QString& serverName)
 : QDialog(parent)
{
  setUpGUI();
  setWindowTitle( tr("Login to") + " " + serverName );
  setModal( true );

  // set the default username
  editUsername->setText(username);
  if(username.isEmpty() == false)
    editPassword->setFocus(Qt::OtherFocusReason);
}
 
void CLoginDialog::setUpGUI()
{
  // set up the layout
  QGridLayout* formGridLayout = new QGridLayout( this );

  // initialize the username combo box so that it is editable
  editUsername = new QLineEdit( this );

  // initialize the password field so that it does not echo
  // characters
  editPassword = new QLineEdit( this );
  editPassword->setEchoMode( QLineEdit::Password );

  // initialize the labels
  labelUsername = new QLabel( this );
  labelPassword = new QLabel( this );
  labelUsername->setText( tr( "Username" ) );
  labelUsername->setBuddy( editUsername );
  labelPassword->setText( tr( "Password" ) );
  labelPassword->setBuddy( editPassword );

  // initialize buttons
  buttons = new QDialogButtonBox( this );
  buttons->addButton( QDialogButtonBox::Ok );
  buttons->addButton( QDialogButtonBox::Cancel );
  buttons->button( QDialogButtonBox::Ok )->setText( tr("Login") );
  buttons->button( QDialogButtonBox::Cancel )->setText( tr("Abort") );

  // connects slots
  connect( buttons->button( QDialogButtonBox::Cancel ),
           SIGNAL(clicked()),
           this,
           SLOT(reject())
           );

  connect( buttons->button( QDialogButtonBox::Ok ),
           SIGNAL(clicked()),
           this,
           SLOT(accept())
         );

  // place components into the dialog
  formGridLayout->addWidget( labelUsername, 0, 0 );
  formGridLayout->addWidget( editUsername, 0, 1 );
  formGridLayout->addWidget( labelPassword, 1, 0 );
  formGridLayout->addWidget( editPassword, 1, 1 );
  formGridLayout->addWidget( buttons, 2, 0, 1, 2 );

  setLayout( formGridLayout );
}
 
QString CLoginDialog::username()
{
  return editUsername->text();
}

QString CLoginDialog::password()
{
  return editPassword->text();
}
