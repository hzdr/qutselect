#include <c_ldap.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

printf("gu.9\n");
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
