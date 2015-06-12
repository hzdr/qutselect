#ifndef _CHOOSER_LDAP_
#define _CHOOSER_LDAP_

#include <ldap.h>

extern int LDAP_Connect(const char* srv);
extern int LDAP_Connected();

extern int LDAP_GetCardDN(const char *TokenID,char *dn);
extern int LDAP_GetCardUser(const char *dn,char *uname);

extern int LDAP_GetSessionDN(const char *TokenID,char *dn);

#endif
