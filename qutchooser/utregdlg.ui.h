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
#include <stdlib.h>
#include <string.h>

#include <ldap.h>
#include <c_ldap.h>

#include <qapplication.h>
#include <qmessagebox.h>

static char TokenID[8192];
static char ldapsrv[8192];
static char *path=NULL;

static LDAP *ldap=NULL;

void utregdlg::RegToken()
{
  ldap=ldap_open(ldapsrv,389);

  if (ldap) {
    char dn[8192];
    char uname[8192];
    strcpy(uname,T_UNAME->text().ascii());
    sprintf(dn,"uid=%s,ou=users,ou=FZR-NIS,ou=it,o=fsr,dc=de",uname);
    int br=ldap_simple_bind_s(ldap,dn,T_PASSWD->text().ascii());
    printf("bind result for [%s] is %d\n",dn,br);

    if (br==LDAP_SUCCESS) {
      // register token ...
      if (path) {
	char pwdfname[8192];
	sprintf(pwdfname,"%s/.pwd",path);
	FILE *pwfile=fopen(pwdfname,"r");
	if (pwfile) {
	  char ldappw[512];
	  fscanf(pwfile,"%[^\n]",ldappw);
	  br=ldap_simple_bind_s(ldap,"cn=manager,o=fsr,dc=de",ldappw);  // its in $UTDIR/.pwd
	  printf("bind result for [%s] is %d\n","manager",br);
	  
	  if (br==LDAP_SUCCESS) {
	    sprintf(dn,"cn=%s,ou=uttoken,ou=utdata,o=fsr,dc=de",TokenID);
	    LDAPMod *attrs[5];
	    LDAPMod attribute1, attribute2,attribute3,attribute4;
	    
	    attribute1.mod_type = "objectClass";
	    char *objectClass_values[] = { "account", "radiusprofile", NULL };
	    attribute1.mod_values = objectClass_values;
	    
	    attribute2.mod_type = "cn";
	    char *r1[]={TokenID, NULL };
	    attribute2.mod_values = r1;
	    
	    attribute3.mod_type = "uid";
	    char *r2[]={ uname, NULL };
	    attribute3.mod_values = r2;
	    
	    attribute4.mod_type = "radiusRealm";
	    char tid[8192];
	    sprintf(tid,"%s@Payflex",TokenID);
	    char *r3[]={ tid, NULL };
	    attribute4.mod_values = r3;
	    
	    attrs[0] = &attribute1;
	    attrs[1] = &attribute2;
	    attrs[2] = &attribute3;
	    attrs[3] = &attribute4;
	    attrs[4] = NULL;
	    
	    br=ldap_add_s(ldap,dn,attrs);
	    
	    printf("done with br=%d\n",br);
	    
	    ldap_unbind_s(ldap);
	} else
	    QMessageBox::critical( this, tr("UT Register Card"), tr(QString("Bind to LDAP failed with "+br)) );
	  
	  close();
	} else
	  QMessageBox::critical( this, tr("UT Register Card"), tr(QString("Failed to obtain bind data (2).")) );
      } else
	QMessageBox::critical( this, tr("UT Register Card"), tr(QString("Failed to obtain bind data (1).")) );
    } else {
      QMessageBox::critical( this, tr("UT Register Card"), tr("Authentication failed.") );
    }
  } else {
    QMessageBox::critical( this, tr("UT Register Card"), tr("Could not connect to LDAP server.") );
  }
}


void utregdlg::Cancel()
{
}

void utregdlg::SetToken( const char * token )
{
  strcpy(TokenID,token);
}


void utregdlg::SetLDAP( const char *srv )
{
  strcpy(ldapsrv,srv);
}


void utregdlg::SetPath( const char *p )
{
  if (path) {
    delete path;
    path=NULL;
  }

  path=new char [strlen(p)+1];
  strcpy(path,p);

}
