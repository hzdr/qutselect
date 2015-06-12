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

static char PIN[5];
	
void PINDLG::Init()
{
    PINS->setText(QString(""));
    for (unsigned int i=0;i<5;i++) PIN[i]=0;
}


void PINDLG::GetPIN( char *pin )
{
    strcpy(pin,PIN);
    Init();
}


void PINDLG::TextChanged(const QString &s )
{
    if (s.length()<5) {
	strcpy(PIN,s.ascii());
	if (s.length()==4) accept();
    }
}
