#ifndef _PCSC_
#define _PCSC_

#include <pcsclite.h>
#include <winscard.h>
#include <reader.h>
#include <stdlib.h>

typedef enum { PCSC, TWN4 } PCSC_TYPE;

extern int PCSC_Init(PCSC_TYPE t_=PCSC);
extern int PCSC_CardConnect(int waitforcard=1,int ct=0);
extern int PCSC_GetATR(unsigned char *atr);
extern const char *PCSC_GetATRStr();

extern int PCSC_HasPIN();

extern int PCSC_Verify(const char *pin,unsigned short *ec=NULL);
extern int PCSC_Verify(BYTE *key,unsigned short *ec=NULL);

extern int PCSC_Change(BYTE *okey,BYTE *nkey,unsigned short *ec);
extern int PCSC_Change(const char *opin,const char *npin,unsigned short *ec);

extern int PCSC_SelectFile(unsigned short fid);

extern int PCSC_CreateDF(unsigned short fid, unsigned short bs);
extern int PCSC_CreatePIN(unsigned short fid,BYTE *pin,BYTE *puk);
extern int PCSC_CreateTREF(unsigned short fid, unsigned short bs,BYTE pin=0);

extern int PCSC_UpdateBinary(unsigned short offset,BYTE nreq,BYTE *buffer);

extern int PCSC_ReadBinary(unsigned short offset,BYTE nreq,BYTE *buffer);
extern int PCSC_ReadRecord(BYTE nreq,BYTE *buffer);

extern int PCSC_DeleteFile(unsigned short fid);

extern int PCSC_CardRemoved();
extern int PCSC_CardDisconnect();
extern int PCSC_Close();

extern int PCSC_IsPayflex();
extern const char* PCSC_PayflexID();
extern const char* PCSC_MifareID();
extern const char* PCSC_CardID();

extern void PCSC_ClearPrivateKeyBuffer();
extern int PCSC_GetPrivateKey(const char *pin,char *pcsc_id_dsa);

extern const char* PCSC_Readers();

#endif
