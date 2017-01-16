#include <c_ldap.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hlist.h>

static LDAP *ldap=NULL;

int LDAP_Connected() {
  if (ldap) return 1;
  return 0;
}

int LDAP_Connect(const char* srv) {
  ldap=ldap_open(srv,389);
  return LDAP_Connected();
}

int LDAP_GetCardDN(const char *TokenID,char *dn) {
  if (ldap) {
    char filter[8192];
    sprintf(filter,"cn=%s",TokenID);
    char *attrs[]={"DN",NULL};
    LDAPMessage *res,*entry;

    if (ldap_search_s(ldap,"ou=uttoken,ou=utdata,o=fsr,dc=de",LDAP_SCOPE_ONELEVEL,filter,attrs,0,&res)==LDAP_SUCCESS) {
      int num_entries = ldap_count_entries(ldap, res);
      if (num_entries==1) {
	entry = ldap_first_entry(ldap, res);
	char *ldn = ldap_get_dn( ldap, entry);
	strcpy(dn,ldn);
	ldap_memfree( ldn ); 
	return 1;
      }
    }
  }

  return 0;
}

int LDAP_GetCardUser(const char *dn,char *uname) {
  if (ldap) {
    char filter[8192];
    sprintf(filter,"cn=*");
    char *attrs[]={"userid",NULL};
    LDAPMessage *res,*entry;

    if (ldap_search_s(ldap,dn,LDAP_SCOPE_BASE,filter,attrs,0,&res)==LDAP_SUCCESS) {
      int num_entries = ldap_count_entries(ldap, res);
      if (num_entries==1) {
	entry = ldap_first_entry(ldap, res);
	// char *luname = ldap_get_dn( ldap, entry);

	char         *a; 
	BerElement   *ber; 
	LDAPMessage *e;
	char         **vals; 

	for ( a = ldap_first_attribute( ldap, res, &ber ); a != NULL; a = ldap_next_attribute( ldap,res, ber ) ) { 
	  if ((vals = ldap_get_values( ldap, res, a)) != NULL ) { 
	    for ( int i = 0; vals[i] != NULL; i++ ) { 
	      printf( "%s: %s\n", a, vals[i] ); 
	      if (!strcmp(a,"uid"))
		strcpy(uname,vals[i]);
	    } 
	    ldap_value_free( vals ); 
	  } 
	  ldap_memfree( a ); 
	}

	// strcpy(uname,luname);
	// ldap_memfree( luname ); 
	return 1;
      }
    }
  }

  return 0;
}

int LDAP_GetGroup(const char *serial,char *groupname) {
  if (ldap) {
    char filter[8192];
    sprintf(filter,"serialNumber=%s",serial);
    char *attrs[]={"ou",NULL};
    LDAPMessage *res,*entry;
    if (ldap_search_s(ldap,"ou=utstations,ou=utdata,o=fsr,dc=de",LDAP_SCOPE_ONELEVEL,filter,attrs,0,&res)==LDAP_SUCCESS) {
      int num_entries = ldap_count_entries(ldap, res);
      if (num_entries==1) {
	entry = ldap_first_entry(ldap, res);

	char         *a; 
	BerElement   *ber; 
	LDAPMessage *e;
	char         **vals; 

	for ( a = ldap_first_attribute( ldap, res, &ber ); a != NULL; a = ldap_next_attribute( ldap,res, ber ) ) { 
	  if ((vals = ldap_get_values( ldap, res, a)) != NULL ) { 
	    for ( int i = 0; vals[i] != NULL; i++ ) { 
	      printf( "%s: %s\n", a, vals[i] ); 
	      if (!strcmp(a,"ou")) strcpy(groupname,vals[i]);
	    } 
	    ldap_value_free( vals ); 
	  } 
	  ldap_memfree( a ); 
	}

	// strcpy(uname,luname);
	// ldap_memfree( luname ); 
	return 1;
      }
    }
  }

  return 0;
}

int LDAP_GetSessionDN(const char *TokenID,char *dn) {
  if (ldap) {
    char filter[8192];
    sprintf(filter,"cn=%s",TokenID);
    char *attrs[]={"DN",NULL};
    LDAPMessage *res,*entry;

    if (ldap_search_s(ldap,"ou=utsession,ou=utdata,o=fsr,dc=de",LDAP_SCOPE_ONELEVEL,filter,attrs,0,&res)==LDAP_SUCCESS) {
      int num_entries = ldap_count_entries(ldap, res);
      if (num_entries==1) {
	entry = ldap_first_entry(ldap, res);
	char *ldn = ldap_get_dn( ldap, entry);
	strcpy(dn,ldn);
	ldap_memfree( ldn ); 
	return 1;
      }
    }
  }

  return 0;
}

/*
typedef struct {
    char hname[64];
    char dpytext[128];
    char proto[32];
} hlist_struct;
*/

int LDAP_GetButtonHost(const char* groupname,int btn,char *host) {
  if ((ldap) && ((btn==1) || (btn==2))) {
    char spath[8192];
    sprintf(spath,"ou=%s,ou=utgroups,ou=utdata,o=fsr,dc=de",groupname);
    char filter[2048];
    if (btn==1) sprintf(filter,"(&(&(objectClass=ipService)(ipServicePort=1))(ipServiceProtocol=RDP))",btn);
    if (btn==2) sprintf(filter,"(&(&(objectClass=ipService)(ipServicePort=2))(ipServiceProtocol=TLINC))",btn);
    char *attrs[]={"cn",NULL};
    LDAPMessage *res,*entry;
      
    if (ldap_search_s(ldap,spath,LDAP_SCOPE_ONELEVEL,filter,attrs,0,&res)==LDAP_SUCCESS) {
      int num_entries = ldap_count_entries(ldap, res);
      if (num_entries==1) {
	entry = ldap_first_entry(ldap, res);

	char         *a; 
	BerElement   *ber; 
	LDAPMessage *e;
	char         **vals; 

	for ( a = ldap_first_attribute( ldap, res, &ber ); a != NULL; a = ldap_next_attribute( ldap,res, ber ) ) { 
	  if ((vals = ldap_get_values( ldap, res, a)) != NULL ) { 
	    for ( int i = 0; vals[i] != NULL; i++ ) { 
	      printf( "%s: %s\n", a, vals[i] ); 
	      if (!strcmp(a,"cn")) strcpy(host,vals[i]);
	    } 
	    ldap_value_free( vals ); 
	  } 
	  ldap_memfree( a ); 
	}

	return 1;
      }
    }
  }

  return 0;
}

int LDAP_LoadServerList(const char *groupname,hlist_struct **l) {
  if (l) {
    *l=NULL;
    if (ldap) {
      char spath[8192];
      sprintf(spath,"ou=%s,ou=utgroups,ou=utdata,o=fsr,dc=de",groupname);
      char *attrs[]={"cn","ipServiceProtocol","description",NULL};
      LDAPMessage *res,*entry;
      
      if (ldap_search_s(ldap,spath,LDAP_SCOPE_ONELEVEL,"(&(objectClass=ipService)(ipServicePort=0))",attrs,0,&res)==LDAP_SUCCESS) {
	int num_entries = ldap_count_entries(ldap, res);
	if (num_entries>0) {

	  *l=new hlist_struct [num_entries];

	  entry = ldap_first_entry(ldap, res);

	  char         *a; 
	  BerElement   *ber; 
	  LDAPMessage *e;
	  char         **vals; 
	  
	  int ecnt=0;

	  while (entry) {
	    for ( a = ldap_first_attribute( ldap, entry, &ber ); a != NULL; a = ldap_next_attribute( ldap,entry, ber ) ) { 
	      if ((vals = ldap_get_values( ldap, entry, a)) != NULL ) { 
		for ( int i = 0; vals[i] != NULL; i++ ) { 
		  if (!strcmp(a,"cn")) strcpy(((*l)[ecnt]).hname,vals[i]);
		  if (!strcmp(a,"description")) strcpy(((*l)[ecnt]).dpytext,vals[i]);
		  if (!strcmp(a,"ipServiceProtocol")) strcpy(((*l)[ecnt]).proto,vals[i]);
		} 
		ldap_value_free( vals ); 
	      } 
	      ldap_memfree( a ); 
	    }
	    
	    ecnt++;
	    entry = ldap_next_entry(ldap, entry);
	  }

	  return num_entries;
	}
      }
    }
  }

  return 0;
}
