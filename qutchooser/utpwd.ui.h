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

#include <stdio.h>
#include <qlineedit.h>
#include <qapplication.h>
#include <stdlib.h>

char puname[1024];
char ppasswd[1024];

void PWDDLG::EchoData()
{
    if (strlen(puname)==0) {
	strcpy(puname,UNAME->text().ascii());
    }
    strcpy(ppasswd,PWD->text().ascii());
}


void PWDDLG::SetUsername( const char *u )
{
    strcpy(puname,u);
    
    if (strlen(puname)>0) {
	UNAME->setText(QString(puname));
	UNAME->setReadOnly(true);
	PWD->setFocus();
    }
}



int main(int argc,char* argv[])
{
    QApplication app( argc, argv );
    
    PWDDLG *dlg=new PWDDLG();
    dlg->SetHostname(argv[1]);
    if (argc>2) {
	dlg->SetUsername(argv[2]);
    } else
	dlg->SetUsername("");
    
    int r=dlg->exec();
    
    // setenv("PWD_UNAME",puname,1);
    // setenv("PWD_PASSWD",ppasswd,1);
 
    printf("%s~%s\n",puname,ppasswd);
    
    return r;
}



void PWDDLG::SetHostname( const char *h )
{
    HNAME->setText(QString(h));
}
