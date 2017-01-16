#ifndef _CHOOSER_LDAP_
#define _CHOOSER_LDAP_

#include <ldap.h>
#include <hlist.h>

extern int LDAP_Connect(const char* srv);
extern int LDAP_Connected();

extern int LDAP_GetCardDN(const char *TokenID,char *dn);
extern int LDAP_GetCardUser(const char *dn,char *uname);

extern int LDAP_GetGroup(const char *serial,char *groupname);
extern int LDAP_GetButtonHost(const char* groupname,int btn,char *host);

extern int LDAP_GetSessionDN(const char *TokenID,char *dn);

extern int LDAP_LoadServerList(const char *groupname,hlist_struct **l);

#endif
