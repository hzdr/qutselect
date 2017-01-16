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
#include <sys/types.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>   //ifreq

#include <string.h>
#include <stdlib.h>

#include <pcsc.h>

#include <hlist.h>

#include <utregdlg.h>
#include <pindlg.h>
#include <carddlg.h>

#include <c_ldap.h>

#include <unistd.h>
#include <sys/stat.h>

static int use_pcsc=0;
static int card_rdy=0;
static int new_card_inserted=0;
static int no_km_remove=0;

//--- some hard-coded constants --------------------------------

const char * ldapsrv[]={"ldap.fz-rossendorf.de",NULL};
const char * keyfile  = "/tmp/user.key";
const char * logfile  = "/home/tsuser/chooser.log";

//---------------------------------------------------------------

static QTimer *timer;
static QTimer *timer2;

static int hlist_n=0;
static hlist_struct *hlist=NULL;

static char TokenID[8192];
static char LastTokenID[8192];
static int  token_valid=0;

static pid_t sessionid=0;

static char hostname[512];
static char groupname[512];
static char serial[512];

static PINDLG *pdlg=NULL;
static CARDDLG *cdlg=NULL;

static char *path=NULL;

void log(const char* lm)
{
    FILE *f=fopen(logfile,"a");
    if (f) {
	fprintf(f,lm);
	fclose(f);
    }
}

void log(const char* lf,int ld)
{
    FILE *f=fopen(logfile,"a");
    if (f) {
	fprintf(f,lf,ld);
	fclose(f);
    }
}

void log(const char* lf,const char * ld)
{
    FILE *f=fopen(logfile,"a");
    if (f) {
	fprintf(f,lf,ld);
	fclose(f);
    }
}

int Form1::LoadPrivateKey( const char * tokenid )
{
    char sc_pk[1024];
    if (PCSC_HasPIN()) {
	if (!pdlg) pdlg=new PINDLG();
	
	
	
	int sc_authok=0;
	
	do {
	    pdlg->Init();
	    int dr=pdlg->exec();
	    
	    if (dr) {
		char sc_pin[16];
		pdlg->GetPIN(sc_pin);
		
		int r=-1;
		//		if (PCSC_SupportDirectPIN())
		//		    r=PCSC_GetPrivateKey(NULL,sc_pk);
		//		else
		r=PCSC_GetPrivateKey(sc_pin,sc_pk);
		
		if (r==1) {
		    log("trying to create user.key : ");
		    FILE *f=fopen(keyfile,"w");
		    if (f) {
			log("success\n");
			fprintf(f,"%s",sc_pk);
			fclose(f);
			chmod(keyfile, 0600 );
			memset(sc_pk,0,1024);
			return 1;
		    } else {
			log("fail\n");
			QMessageBox::critical( NULL, "UT Session", "Could not create key-file!" );
			return 0;
		    }
		} else {
		    switch (r) {
		    case -1: QMessageBox::critical( NULL, "UT Session", "Invalid PIN!" ); break;
		    case -2: QMessageBox::critical( NULL, "UT Session", "Invalid PIN!\n*** Card is LOCKED ***" ); break;
		    default: QMessageBox::critical( NULL, "UT Session", "Other Error!" );
		    }
		}
	    } else {
		return 0;
	    }
	} while (sc_authok==0);
	
	return 0;
	
    } else {
	// QMessageBox::critical( NULL, "UT Session", "Invalid Token" );
	return -1;
    }
    
    return 0;
}

void Form1::StartWindows()
{
  char sfn[8192];

  if (strlen(groupname)>0) {
    if (LDAP_GetButtonHost(groupname,1,sfn)) {
      if (strlen(sfn)==0) strcpy(sfn,"xats");
    } else
      strcpy(sfn,"xats");
  } else
    strcpy(sfn,"xats");

  StartWindows(sfn);
}

void Form1::StartLinux()
{
  char sfn[8192];

  if (strlen(groupname)>0) {
    if (LDAP_GetButtonHost(groupname,2,sfn)) {
      if (strlen(sfn)==0) strcpy(sfn,"lts1");
    } else
      strcpy(sfn,"lts1");
  } else
    strcpy(sfn,"lts1");

  StartTLINC(sfn);
}

void Form1::StartFirefox()
{
    if (sessionid!=0) {
	int pstate;
	if (wait(&pstate)==sessionid) {
	    log("child session %d was terminated\n",sessionid);
	    sessionid=0;
	}
    }
    
    if (sessionid==0) {
	sessionid=fork();
	
	if (sessionid==0) {
	    char* const argv[]={"utrun.sh","2",TokenID,NULL};
	    char cmd[8192];
	    sprintf(cmd,"%s/utrun.sh",path);
	    execv(cmd,argv);
	    exit(0);
	} else {
	    if (sessionid<0) {
		QMessageBox::critical( NULL, "UT Session", "Could not start utsession." );
	    } else {
		char cmd[8192];
		sprintf(cmd,"%s/utupdate.sh 2 %s \"$HOSTNAME\"",path,TokenID);
		int r=system(cmd);
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
	int r=readlink("/proc/self/exe",buffer,8192);
	unsigned int i=strlen(buffer)-1;
	while ((buffer[i]!='/') && (i>0)) i--;
	
	path=new char [i+1];
	memset(path,0,i+1);
	strncpy(path,buffer,i);
    }
    
    for (int i=1;i<argc;i++)
	if (!strcmp(argv[i],"-normk")) no_km_remove=1;
    
    strcpy(hostname,"");
    strcpy(groupname,"");
    strcpy(serial,"");
    
    strcpy(TokenID,"");
    strcpy(LastTokenID,"");
    
    if (!PCSC_Init()) {
	QMessageBox::critical( NULL, "UT Session", "Could not initialize card reader." );
    } else {
	use_pcsc=1;
    }
    
    Form1 *dlg=new Form1();
    
    dlg->GetSerial();
    dlg->GetGroup();
    dlg->SetHostname();
    dlg->reloadHList();
    
    dlg->exec();
    
    return 0;
}


void Form1::SetHostname()
{
    if (getenv("DISPLAY")) {
	char dpy[1024];
	strcpy(dpy,getenv("DISPLAY"));
    }
    
    gethostname(hostname,512);
    
    char hname[1024];
    sprintf(hname,"<p align=\"center\">Welcome to UT: %s@%s</p>",hostname,groupname);
    HOSTNAME->setText(QString(hname));
    
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateState()));
    timer->start(200);
    
    timer2 = new QTimer(this);
    connect(timer2, SIGNAL(timeout()), this, SLOT(reloadHList()));
    timer2->start(30*1000);  // 30 s
    
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
	    
	    if (new_card_inserted) log("new card inserted: %s\n",TokenID);
	    
	    if (strlen(TokenID)>0) {
		TOKEN->setText("<p align=\"right\">"+QString(TokenID)+"</p>");
		strcpy(LastTokenID,TokenID);
		
		if ((LDAP_Connected()) && (new_card_inserted)) {
		    
		    new_card_inserted=0;
		    
		    char CDN[4096];
		    char SDN[4096];
		    
		    // check, if card is registered with ldap
		    if (LDAP_GetCardDN(TokenID,CDN)) {
			token_valid=1;
			
			// now check, if there is a session for this token ...
			if (LDAP_GetSessionDN(TokenID,SDN)) {
			    int tr=LoadPrivateKey(TokenID);
			    if ((tr==1) || (tr==-1)) {
				if (sessionid==0) {
				    sessionid=fork();
				    
				    if (sessionid==0) {
					// we need to get the token ...
					
					if (tr==1) {
					    char* const argv[]={"utrun.sh","reconnect",TokenID,NULL};
					    char cmd[8192];
					    sprintf(cmd,"%s/utrun.sh",path);
					    execv(cmd,argv);
					} else {
					    char* const argv[]={"utrun.sh","reconnect",TokenID,"password",NULL};
					    char cmd[8192];
					    sprintf(cmd,"%s/utrun.sh",path);
					    execv(cmd,argv);
					}
					exit(0);
				    } else {
					if (sessionid<0) {
					    QMessageBox::critical( NULL, "UT Session", "Could not start utsession." );
					}
				    }
				}
			    }
			} else {
			    log("  no session\n");
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
		TOKEN->setText(QString(""));
		if (card_rdy) {
		    if (sessionid>0) {
			log("kill.1: %d",sessionid);
			log(" %d\n",kill(sessionid,SIGTERM));
			sessionid=0;
		    }
		    card_rdy=0;
		}
		// remove local copy of key in any case
		if (!no_km_remove) unlink(keyfile);
		sprintf(TokenID,"pseudo.%s",hostname);
	    }
	    
	} else {
	    TOKEN->setText(QString(""));
	    if (card_rdy) {
		if (sessionid>0) {
		    log("kill.2: %d",sessionid);
		    log(" %d\n",kill(sessionid,SIGTERM));
		    sessionid=0;
		    new_card_inserted=0;
		    
		    if (strlen(LastTokenID)>0) {
			char cmd[8192];
			sprintf(cmd,"%s/utupdate.sh 255 %s",path,LastTokenID);
			int r=system(cmd);
			strcpy(LastTokenID,"");
		    }
		    
		}
		card_rdy=0;
		
		// remove local copy of key in any case
		if (!no_km_remove) unlink(keyfile);
		
	    }
	    sprintf(TokenID,"pseudo.%s",hostname);
	}
    }
    
    timer->start(200);
}


void Form1::OpenInfo()
{
    QMessageBox::information( NULL, "HZDR UT About",
			      QString("HZDR UT Client Version 1.23\n")+
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


void Form1::reloadHList()
{
  if (strlen(groupname)>0) {
    hlist_struct *hl1=NULL;
    int hl_n=LDAP_LoadServerList(groupname,&hl1);
  
    if (hl_n==0) {
      FILE *f=NULL;
      
      char sfn[]="/home/tsuser/.qutselect/qutselect.slist";
      
      f=fopen(sfn,"r");
      
      if (f) {
	hlist_struct *hl2=NULL;
	int r;
	char c;
	char buffer[8192];
	char h_dpy[64];
	char h_host[64];
	char h_domain[64];
	char h_type[64];
	char h_os[64];
	char h_descr[64];
	char h_pwprmt[64];
	char h_script[64];
	// # DISPLAYNAME   HOSTNAME     DOMAIN   TYPE    OS             DESCRIPTION            PWPRMPT SCRIPT
	do {
	  // XATS        ; xats     ; FZR   ; RDP   ; Windows      ; Zentraler Windows-Terminalserver      ; TRUE ; scripts/qutselect_connect_rdp.sh
	  r=fscanf(f,"%[^\n]%c",buffer,&c);
	  if ((r!=EOF) && (r==2) && (strlen(buffer)>0)) {
	    if (buffer[0]!='#') {
	      char *ptr=strtok(buffer,";");
	      if (ptr) {
		strcpy(h_dpy,ptr); // DISPLAYNAME
		ptr=strtok(NULL,";");
		if (ptr) {
		  strcpy(h_host,ptr); // HOSTNAME
		  ptr=strtok(NULL,";");
		  if (ptr) {
		    strcpy(h_domain,ptr); // DOMAIN
		    ptr=strtok(NULL,";");
		    if (ptr) {
		      strcpy(h_type,ptr);
		      ptr=strtok(NULL,";");
		      if (ptr) {
			strcpy(h_os,ptr);
			ptr=strtok(NULL,";");
			if (ptr) {
			  strcpy(h_descr,ptr);
			  ptr=strtok(NULL,";");
			  if (ptr) {
			    strcpy(h_pwprmt,ptr);
			    ptr=strtok(NULL,";");
			    if (ptr) {
			      strcpy(h_script,ptr);
			      hl2=hl1;
			      hl1=new hlist_struct [hl_n+1];
			      for (int i=0;i<hl_n;i++) hl1[i]=hl2[i];
			      delete hl2;					    
			      strcpy(hl1[hl_n].hname,h_host);
			      sprintf(hl1[hl_n].dpytext,"%s",h_descr);
			      strcpy(hl1[hl_n].proto,h_type);
			      hl_n++;
			    }
			  }
			}
		      }
		    }
		  }
		}
	      }
	    }
	  }
	} while ((r!=EOF) && (r==2));
	fclose(f);
      }
    }

    if ((hl_n>0) && (hl1)) {
      delete hlist;
      hlist_n=hl_n;
      hlist=hl1;
	HLIST->clear();
	for (int i=0;i<hlist_n;i++)
	  HLIST->insertItem(QString(hlist[i].hname)+QString(":")+QString(hlist[i].dpytext));
    }
    
  }
}


void Form1::StartWindows( char *hname )
{
    if (sessionid!=0) {
	int pstate;
	if (wait(&pstate)==sessionid) {
	    log("child session %d was terminated\n",sessionid);
	    sessionid=0;
	}
    }
    
    if (sessionid==0) {
	sessionid=fork();
	
	if (sessionid==0) {
	    char* const argv[]={"utrun.sh","1",TokenID,hname,NULL};
	    char cmd[8192];
	    sprintf(cmd,"%s/utrun.sh",path);
	    execv(cmd,argv);
	    exit(0);
	} else {
	    if (sessionid<0) {
		QMessageBox::critical( NULL, "UT Session", "Could not start utsession." );
	    } else {
		if (token_valid) {
		    char cmd[8192];
		    sprintf(cmd,"%s/utupdate.sh 1 %s %s",path,TokenID,hname);
		    int r=system(cmd);
		}
	    }
	}
    }
}

void Form1::DirectConnect( QListBoxItem *citem )
{
  int i=HLIST->index(citem);
  if ((0<=i) && (i<hlist_n)) {
    if (!strcmp(hlist[i].proto,"RDP")) {
      StartWindows(hlist[i].hname);
    }
    if (!strcmp(hlist[i].proto,"TLINC")) {
      StartTLINC(hlist[i].hname);
    }
  }
}

void Form1::StartTLINC( char *hname )
{
    if (sessionid!=0) {
	int pstate;
	if (wait(&pstate)==sessionid) {
	    log("child session %d was terminated\n",sessionid);
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
		  char* const argv[]={"utrun.sh","0",TokenID,hname,NULL};
		  char cmd[8192];
		  sprintf(cmd,"%s/utrun.sh",path);
		  execv(cmd,argv);
		  exit(0);
		} else {
		  char* const argv[]={"utrun.sh","0",TokenID,hname,"password",NULL};
		  char cmd[8192];
		  sprintf(cmd,"%s/utrun.sh",path);
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
			  sprintf(cmd,"%s/utupdate.sh 0 %s %s",path,TokenID,hname);
			else
			  sprintf(cmd,"%s/utupdate.sh 0 %s %s password",path,TokenID,hname);
			int r=system(cmd);
			
			// shall we wait for session to terminate here?
		    }
		}
	    }
	}
    }
}


const char* Form1::GetSerial()
{
    if (!LDAP_Connect(ldapsrv[0])) {
	QMessageBox::critical( NULL, "UT Session", "Could not connect to LDAP." );
    }
    
    if (strlen(serial)==0) {
	struct ifaddrs *ifap;
	if (!getifaddrs(&ifap)) {
	    struct ifaddrs *ifa=ifap;
	    while (ifa) {
	      int family=ifa->ifa_addr->sa_family;
	      if (strcmp(ifa->ifa_name,"lo")) {
		if (family==AF_INET) {
		  struct ifreq ifr;
		  memset(&ifr, 0, sizeof(ifr));
		  int fd = socket(AF_INET, SOCK_DGRAM, 0);
		  ifr.ifr_addr.sa_family = AF_INET;
		  strncpy(ifr.ifr_name , ifa->ifa_name , IFNAMSIZ-1);
		  unsigned char *mac = NULL;
		  if (0 == ioctl(fd, SIOCGIFHWADDR, &ifr)) {
		    mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
		    printf("%8s, MAC : %02x:%02x:%02x:%02x:%02x:%02x\n" , ifa->ifa_name,mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		    sprintf(serial,"%02x:%02x:%02x:%02x:%02x:%02x" , mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		  } else
		    printf("STDNET: %8s can not get MAC\n",ifa->ifa_name);
		}
	      }
	      ifa=ifa->ifa_next;
	    }
	} else {
	    QMessageBox::critical( NULL, "UT Session", "Could not get serial number (MAC) of device." );
	}
    }
}


const char* Form1::GetGroup()
{
  strcpy(groupname,"main");
  if (strlen(serial)>0) {
    if (LDAP_Connected()) {
      char gn[512];
      if (LDAP_GetGroup(serial,gn)) {
	strcpy(groupname,gn);
      } else
	QMessageBox::critical( NULL, "UT Session", "Could not get group." );
    } else
      QMessageBox::critical( NULL, "UT Session", "Could not get group (not connected to LDAP)." );
  }
}
