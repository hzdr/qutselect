/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/

#include <qmessagebox.h>
#include <qtimer.h>
#include <qdatetime.h>
#include <qwidget.h>
#include <qlineedit.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

#include <stdlib.h>
#include <c_ldap.h>

#include <pcsc.h>

static int card_rdy=0;
static QTimer *timer=NULL;
static char TokenID[256];
static char pins[64];

void CARDDLG::PIN_TextChanged( const QString &s )
{
    if (s.length()<5) {
      strcpy(pins,s.ascii());
      if (s.length()==4) {
	// verify PIN
	unsigned short ec;
	int r=PCSC_Verify(pins,&ec);

	if (ec==0x9000) {
	  // if successful:
	  // get data
	  CDLG_S_STATE->setEnabled(true);
	  CDLG_CSTATE->setEnabled(true);
	  CDLG_CSTATE->setText(QString("PIN O.K."));

	  char CDN[4096];
	  
	  if (LDAP_Connected()) {
	    char uname[1024];
	    if (LDAP_GetCardDN(TokenID,CDN)) {
	      if (LDAP_GetCardUser(CDN,uname)) {
		// retreive user-data
		CDLG_CSTATE->setText(QString("got card data"));
		CDLG_S_UNAME->setEnabled(true);
		CDLG_UNAME->setEnabled(true);
		CDLG_UNAME->setText(QString(uname));

		// activate PIN change
		CDLG_S_NEWPIN->setEnabled(true);
		CDLG_NPINS1->setEnabled(true);
		CDLG_NPINS2->setEnabled(true);
		CDLG_B_CHPIN->setEnabled(true);

	      } else {
		CDLG_CSTATE->setText(QString("Could not get Card user"));
	      }
	    } else {
	      CDLG_CSTATE->setText(QString("Could not get Card DN"));
	    }
	  } else {
	    CDLG_CSTATE->setText(QString("Could connect to LDAP"));
	  }
	} else {
	  switch (ec) {
	  case 0x6300:
	    CDLG_CSTATE->setText(QString("Invalid PIN!"));
	    break;
	  case 0x6983:
	    CDLG_CSTATE->setText(QString("*** Card is LOCKED ***" )); 
	    break;
	  default: 
	    CDLG_CSTATE->setText(QString("Other Error!"));
	  }
	}
      }
    }
    
}


void CARDDLG::updateState()
{
    timer->stop();

    if ((!card_rdy) && (PCSC_CardConnect(0))) {
	card_rdy=1;
	strcpy(TokenID,PCSC_CardID());
	CDLG_S_TOKEN->setEnabled(true);
	CDLG_TOKEN->setEnabled(true);
	CDLG_TOKEN->setText(QString(TokenID));
    
	if (PCSC_HasPIN()) {
	  // enable PIN field
	  CDLG_S_PIN->setEnabled(true);
	  CDLG_PINS->setEnabled(true);
	  CDLG_PINS->setText(QString(""));
	  QTimer::singleShot(0, CDLG_PINS, SLOT(setFocus()));
	} else {
	  QMessageBox::critical( NULL, "UT Session", "Invalid Token" );
	}
    }
    
    if ((card_rdy) && (!PCSC_CardConnect(0))) {
	ClearFields();
	strcpy(TokenID,PCSC_CardID());
	card_rdy=0;	
    }
    
    timer->start(200);
}


void CARDDLG::Init()
{
    ClearFields();
    
    card_rdy=0;
    strcpy(TokenID,"");

    if (!timer) {
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(updateState()));
	timer->start(200);
    }
    
}


void CARDDLG::ClearFields()
{
  memset(pins,0,64);

  CDLG_S_TOKEN->setEnabled(false);
  CDLG_TOKEN->setEnabled(true);
  CDLG_TOKEN->setText(QString("Please insert card."));
  
  CDLG_S_STATE->setEnabled(false);
  CDLG_CSTATE->setEnabled(false);
  
  CDLG_S_UNAME->setEnabled(false);
  CDLG_UNAME->setEnabled(false);
  
  CDLG_S_PIN->setEnabled(false);
  CDLG_PINS->setEnabled(false);
  CDLG_PINS->setText(QString(""));
  
  CDLG_S_NEWPIN->setEnabled(false);
  CDLG_NPINS1->setEnabled(false);
  CDLG_NPINS1->setText(QString(""));
  CDLG_NPINS2->setEnabled(false);
  CDLG_NPINS2->setText(QString(""));
  CDLG_B_CHPIN->setEnabled(false);
  
  CDLG_S_PUK->setEnabled(false);
  CDLG_PUKS->setEnabled(false);
  CDLG_PUKS->setText(QString(""));
}


void CARDDLG::changePIN()
{
  if (
      (CDLG_NPINS1->text().length()==4) &&
      (CDLG_NPINS2->text().length()==4) &&
      (CDLG_NPINS1->text()==CDLG_NPINS2->text())
      )
    {
      char npins[64];
      strcpy(npins,CDLG_NPINS1->text().ascii());

      unsigned short ec;

      printf("PIN=%s -> PIN=%s\n",pins,npins);

      if (PCSC_Change(pins,npins,&ec)) {
	if (ec==0x9000) {
	  // if successful:
	  CDLG_CSTATE->setText(QString("PIN changed")); 
	} else {
	  char sec[256];
	  sprintf(sec,"%0x04",ec);
	  CDLG_CSTATE->setText(QString("PIN change failed with ec=")+sec); 
	}
      }
    } 
  else 
    {
      QMessageBox::critical( NULL, "UT Session", "New PIN not of length 4 or do not match." );
    }

}
