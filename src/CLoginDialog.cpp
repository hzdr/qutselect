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
