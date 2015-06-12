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
#include <qlistbox.h>
#include <stdlib.h>

int nsys=0;
char dpy[64][128];
char dns[64][128];

void Form1::loadserverlist( char *fname )
{
	FILE* f=fopen(fname,"r");
	if (f) {
		SRVLIST->clear();
		nsys=0;
		int r;
		do {
			char buffer[2048];
			r=fscanf(f,"%[^\n]",buffer);
			if (r!=EOF) {
				memset(dns[nsys],0,128);
				memset(dpy[nsys],0,128);
				char *t1=strstr(buffer,",");
				// scan hostname
				strncpy(dns[nsys],buffer,t1-buffer);
				// scan displaytext
				strcpy(dpy[nsys],t1+1);
				
				SRVLIST->insertItem (QString(dpy[nsys]));
				char c;
				r=fscanf(f,"%c",&c);
				nsys++;
			}
		} while (r!=EOF);
		fclose(f);
	}
}


void Form1::connectserver()
{
	if (strlen(host->text().ascii())==0) {
		int i=SRVLIST->currentItem();
		// printf("%s",SRVLIST->text(i).ascii());
		printf("%s",dns[i]);
	} else {
		printf("%s",host->text().ascii());
	}
}

int main(int argc,char* argv[])
{
     QApplication app( argc, argv );
     Form1 *dlg=new Form1();
     if (argc==2) dlg->loadserverlist(argv[1]);
     dlg->exec();

     return 0;
 }


void Form1::openhost()
{
    printf("%s",host->text().ascii());
}
