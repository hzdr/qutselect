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

#include <qapplication.h>
#include <qmessagebox.h>
#include <qtimer.h>
#include <qdatetime.h>

#include <qurl.h>
#include <qhttp.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include <stdlib.h>
#include <c_ldap.h>

#include <pcsc.h>
#include <utregdlg.h>
#include <pindlg.h>
#include <carddlg.h>

#include <sys/stat.h>

static int use_pcsc=0;
static int card_rdy=0;
static int new_card_inserted=0;

const char * ldapsrv[]={"ldap1.fz-rossendorf.de",NULL};

static QTimer *timer;

static char TokenID[8192];
static char LastTokenID[8192];
static int  token_valid=0;

static pid_t sessionid=0;

static char hostname[512];
static PINDLG *pdlg=NULL;
static CARDDLG *cdlg=NULL;

static char *path=NULL;

int Form1::LoadPrivateKey( const char * tokenid )
{
  char sc_pk[1024];
  if (PCSC_HasPIN()) {
    if (!pdlg) pdlg=new PINDLG();

    int sc_authok=0;

    do {
      pdlg->Init();
      if (pdlg->exec()) {
	char sc_pin[16];
	pdlg->GetPIN(sc_pin);
	
	printf("Read key from CARD\n");
	
	int r=PCSC_GetPrivateKey(sc_pin,sc_pk);

	if (r==1) {
	  FILE *f=fopen("/tmp/user.key","w");
	  fprintf(f,"%s",sc_pk);
	  fclose(f);
	  chmod("/tmp/user.key", 0600 );
	  memset(sc_pk,0,1024);
printf("saved key to /tmp/user.key\n");
	  return 1;
	} else {
	  switch (r) {
	  case -1: QMessageBox::critical( NULL, "UT Session", "Invalid PIN!" ); break;
	  case -2: QMessageBox::critical( NULL, "UT Session", "Invalid PIN!\n*** Card is LOCKED ***" ); break;
	  default: QMessageBox::critical( NULL, "UT Session", "Other Error!" );
	  }
	}
      } else {
printf("error executing PIN dialog\n");
	return 0;
      }
    } while (sc_authok==0);

printf("failed to unlock Token\n");
    return 0;

  } else {
printf("non PIN Token\n");
    // QMessageBox::critical( NULL, "UT Session", "Invalid Token" );
    return -1;
  }
  
  return 0;
}

void Form1::StartWindows()
{
  if (sessionid!=0) {
    int pstate;
    if (wait(&pstate)==sessionid) {
      printf("child session %d was terminated\n",sessionid);
      sessionid=0;
    }
  }

  if (sessionid==0) {
    sessionid=fork();
  
    if (sessionid==0) {
      char* const argv[]={"utrun.sh","1",TokenID,NULL};
      char cmd[8192];
      sprintf(cmd,"%s/utrun.sh",path);
printf("EXECV: [%s 1 %s]\n",cmd,TokenID);
      execv(cmd,argv);
      exit(0);
    } else {
      if (sessionid<0) {
	QMessageBox::critical( NULL, "UT Session", "Could not start utsession." );
      } else {
	if (token_valid) {
	  char cmd[8192];
	  sprintf(cmd,"%s/utupdate.sh 1 %s xats",path,TokenID);
printf("EXEC: [%s]\n",cmd);
	  system(cmd);
	}
      }
    }
  }
}


void Form1::StartLinux()
{
  if (sessionid!=0) {
    int pstate;
    if (wait(&pstate)==sessionid) {
      printf("child session %d was terminated\n",sessionid);
      sessionid=0;
    }
  }

  if (sessionid==0) {

    // load approriate private key to this terminal
    int r=LoadPrivateKey(TokenID);
    if ((r==1) || (r==-1)) {
      sessionid=fork();
      if (sessionid==0) {
	if (r==1) {
	  char* const argv[]={"utrun.sh","0",TokenID,NULL};
	  char cmd[8192];
	  sprintf(cmd,"%s/utrun.sh",path);
printf("EXECV.1: [%s 0 %s]\n",cmd,TokenID);
	  execv(cmd,argv);
	  exit(0);
	} else {
	  char* const argv[]={"utrun.sh","0",TokenID,"password",NULL};
	  char cmd[8192];
	  sprintf(cmd,"%s/utrun.sh",path);
printf("EXECV.2: [%s 0 %s password]\n",cmd,TokenID);
	  execv(cmd,argv);
	  exit(0);
	}
      } else {
	if (sessionid<0) {
	  QMessageBox::critical( NULL, "UT Session", "Could not start utsession." );
	} else {
	  if (token_valid) {
	    char cmd[8192];
	    if (r==1)
	      sprintf(cmd,"%s/utupdate.sh 0 %s lts1",path,TokenID);   // "lts1" should be read from some config file or ldap ...
	    else
	      sprintf(cmd,"%s/utupdate.sh 0 %s lts1 password",path,TokenID);   // "lts1" should be read from some config file or ldap ...
printf("EXEC: [%s]\n",cmd);
	    system(cmd);
	    
	    // shall we wait for session to terminate here?
	  }
	}
      }
    }
  }
}

void Form1::StartFirefox()
{
  if (sessionid!=0) {
    int pstate;
    if (wait(&pstate)==sessionid) {
      printf("child session %d was terminated\n",sessionid);
      sessionid=0;
    }
  }

  if (sessionid==0) {
    sessionid=fork();
    
    if (sessionid==0) {
      char* const argv[]={"utrun.sh","2",TokenID,NULL};
      char cmd[8192];
      sprintf(cmd,"%s/utrun.sh",path);
printf("EXECV: [%s 2 %s]\n",cmd,TokenID);
      execv(cmd,argv);
      exit(0);
    } else {
      if (sessionid<0) {
	QMessageBox::critical( NULL, "UT Session", "Could not start utsession." );
      } else {
	char cmd[8192];
	sprintf(cmd,"%s/utupdate.sh 2 %s \"$HOSTNAME\"",path,TokenID);
printf("EXEC: [%s]\n",cmd);
	system(cmd);
      }
    }
  }
}

int main(int argc,char* argv[])
{
     QApplication app( argc, argv );

     {
       // get executable path
       char buffer[8192];
       memset(buffer,0,8192);
       readlink("/proc/self/exe",buffer,8192);
       unsigned int i=strlen(buffer)-1;
       while ((buffer[i]!='/') && (i>0)) i--;

       path=new char [i+1];
       memset(path,0,i+1);
       strncpy(path,buffer,i);
     }

     strcpy(hostname,"");

     strcpy(TokenID,"");
     strcpy(LastTokenID,"");

     if (!PCSC_Init()) {
       QMessageBox::critical( NULL, "UT Session", "Could not initialize card reader." );
     } else {
       use_pcsc=1;
     }

     Form1 *dlg=new Form1();
		 
     dlg->SetHostname();
		 
     dlg->exec();
  
     return 0;
 }


void Form1::SetHostname()
{
  if (getenv("DISPLAY")) {
    char dpy[1024];
    strcpy(dpy,getenv("DISPLAY"));
    printf("DISPLAY=%s\n",dpy);
  }
  
  if (getenv("HOSTNAME")) {
    printf("HOSTNAME=%s\n",getenv("HOSTNAME"));
    strcpy(hostname,getenv("HOSTNAME"));
  } else {
    printf("could not set HOSTNAME from env,trying /etc/hostname\n");
    FILE *f=fopen("/etc/hostname","r");
    if (f) {
      fscanf(f,"%s",hostname);
      fclose(f);
    }
  }
  char hname[1024];
  sprintf(hname,"<p align=\"center\">Welcome to %s</p>",hostname);
  HOSTNAME->setText(QString(hname));

  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(updateState()));
  timer->start(200);

  if (!LDAP_Connect(ldapsrv[0])) {
    QMessageBox::critical( NULL, "UT Session", "Could not connect to LDAP." );
  }

}


void Form1::ToFWP()
{
  exit(3);
}


void Form1::updateState()
{
  timer->stop();

  //get current date and time
  QDateTime dateTime = QDateTime::currentDateTime();
  QString dateTimeString = dateTime.toString("dd.MM.yyyy hh:mm:ss");
  TIME->setText("<p align=\"left\">" + dateTimeString + "</p>");

  if (use_pcsc) {
    if ((!card_rdy) && (PCSC_CardConnect(0))) {
      card_rdy=1;
      new_card_inserted=1;
      token_valid=0;
    }

    if (card_rdy) {
      strcpy(TokenID,PCSC_CardID());

      if (new_card_inserted) printf("new card inserted: %s\n",TokenID);

      if (strlen(TokenID)>0) {
	TOKEN->setText("<p align=\"right\">"+QString(TokenID)+"</p>");
	strcpy(LastTokenID,TokenID);

	if ((LDAP_Connected()) && (new_card_inserted)) {

	  new_card_inserted=0;

	  char CDN[4096];
	  char SDN[4096];
	
	  // check, if card is registered with ldap
printf("check, if token is registered\n");
	  if (LDAP_GetCardDN(TokenID,CDN)) {
printf("Token DN=%s\n",CDN);
	    token_valid=1;

	    // now check, if there is a session for this token ...
printf("check if there is an open session\n");
	    if (LDAP_GetSessionDN(TokenID,SDN)) {
printf("Session DN=%s\n",SDN);
              int tr=LoadPrivateKey(TokenID);
	      if ((tr==1) || (tr==-1)) {
		if (sessionid==0) {
printf("spawn session\n");
		  sessionid=fork();
		  
		  if (sessionid==0) {
		    // we need to get the token ...
		    
		    if (tr==1) {
		      char* const argv[]={"utrun.sh","reconnect",TokenID,NULL};
		      char cmd[8192];
		      sprintf(cmd,"%s/utrun.sh",path);
printf("EXECV: [%s reconnect %s]\n",cmd,TokenID);
		      execv(cmd,argv);
		    } else {
		      char* const argv[]={"utrun.sh","reconnect",TokenID,"password",NULL};
		      char cmd[8192];
		      sprintf(cmd,"%s/utrun.sh",path);
printf("EXECV: [%s reconnect %s password]\n",cmd,TokenID);
		      execv(cmd,argv);
		    }
		    exit(0);
		  } else {
		    if (sessionid<0) {
		      QMessageBox::critical( NULL, "UT Session", "Could not start utsession." );
		    }
		  }
		} else
printf("session still running\n");
	      } else {
printf("failed to use token\n");
		sessionid=0;
	      }
	    } else {
	      printf("  no session\n");
	      sessionid=0;
	    }
	  } else {
	    // this is a new card
	    utregdlg *dlg=new utregdlg();
	    dlg->SetToken(TokenID);
	    dlg->SetLDAP(ldapsrv[0]);
	    dlg->SetPath(path);
	    dlg->exec();
	  }
	  
	}
	
      } else {
	TOKEN->setText(QString(hostname));
	if (card_rdy) {
printf("could not detect token, assuming it was removed\n");
	  if (sessionid>0) {
	    printf("kill.1: %d %d\n",sessionid,kill(sessionid,SIGTERM));
	    sessionid=0;
	  }
	  card_rdy=0;
	}
	// remove local copy of key in any case
	unlink("/tmp/user.key");
	sprintf(TokenID,"pseudo.%s",hostname);
      }

    } else {
      TOKEN->setText(QString(hostname));
      if (card_rdy) {
printf("could not detect token, assuming it was removed\n");
	if (sessionid>0) {
	  printf("kill.2: %d %d\n",sessionid,kill(sessionid,SIGTERM));
	  sessionid=0;
	  new_card_inserted=0;

	  if (strlen(LastTokenID)>0) {
	    char cmd[8192];
	    sprintf(cmd,"%s/utupdate.sh 255 %s",path,LastTokenID);
	    system(cmd);
	    strcpy(LastTokenID,"");
	  }

	}
	card_rdy=0;

	// remove local copy of key in any case
	unlink("/tmp/user.key");

      }
      sprintf(TokenID,"pseudo.%s",hostname);
    }
  }

  timer->start(200);
}


void Form1::OpenInfo()
{
    QMessageBox::information( NULL, "HZDR UT About",
			      QString("HZDR UT Client Version 1.7\n")+
			      QString("Detected readers: ")+QString(PCSC_Readers()));
}


void Form1::OpenCardInfo()
{
  timer->stop();
  
  if (!cdlg) cdlg=new CARDDLG();

  cdlg->Init();
  cdlg->exec();

  timer->start(200);
}
