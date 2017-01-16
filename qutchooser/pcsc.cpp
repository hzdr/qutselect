
/******************************************************************

	MUSCLE SmartCard Development ( http://www.linuxnet.com )
	    Title  : test.c
	    Package: pcsc lite
            Author : David Corcoran
            Date   : 7/27/99
	    License: Copyright (C) 1999 David Corcoran
	             <corcoran@linuxnet.com>
            Purpose: This is a test program for pcsc-lite.
	            
********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <pcsclite.h>
#include <winscard.h>
#include <reader.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/time.h>
#include <arpa/inet.h>

#include <termios.h>

static struct termios initialAtt,newAtt;

#include <pcsc.h>

static SCARDHANDLE hCard;
static SCARDCONTEXT hContext;
static SCARD_READERSTATE rgReaderStates[1];
static unsigned long dwReaderLen, dwState, dwProt, dwAtrLen;
static unsigned long dwPref, dwReaders;
static char *pcReaders, *mszReaders;
static unsigned char pbAtr[MAX_ATR_SIZE];
static const char *mszGroups;
static long rv;

DWORD verify_ioctl = 0;
DWORD modify_ioctl = 0;

// static int i, p, iReader;
// static int iList[16];

/*
APDU:

select file: 0x00 0xa4 P1 P2 <length_of_fid> <fid>
P1: 
0 0 0 0 0 0 x x 	Selection by file identifier
0 0 0 0 0 0 0 0 	- Select MF, DF or EF (data field=identifier or empty)
0 0 0 0 0 0 0 1 	- Select child DF (data field=DF identifier)
0 0 0 0 0 0 1 0 	- Select EF under current DF (data field=EF identifier)
0 0 0 0 0 0 1 1 	- Select parent DF of the current DF (empty data field)
0 0 0 0 0 1 x x 	Selection by DF name
0 0 0 0 0 1 0 0 	- Direct selection by DF name (data field=DF name)
0 0 0 0 1 x x x 	Selection by path (see 5.1.2)
0 0 0 0 1 0 0 0 	- Select from MF (data field=path without the identifier of the MF)
0 0 0 0 1 0 0 1 	- Select from current DF (data field=path without the identifier of the current DF)
Any other value 	RFU 

P2:
0 0 0 0 -- -- 0 0 	First record
0 0 0 0 -- -- 0 1 	Last record
0 0 0 0 -- -- 1 0 	Next record
0 0 0 0 -- -- 1 1 	Previous record
0 0 0 0 x x -- -- 	File control information option (see 5.1.5)
0 0 0 0 0 0 -- -- 	- Return FCI, optional template
0 0 0 0 0 1 -- -- 	- Return FCP template
0 0 0 0 1 0 -- -- 	- Return FMD template
Any other value 	RFU 

 */

//                               select      MF    1st   2 byte follow  <file#>

/*
read record: 0x00 0xb2 P1 P2 <number of byte to be read>  
P1: 0x00 : current record
    other: record#

P2:
0 0 0 0 0 -- -- -- 	Currently selected EF
x x x x x -- -- -- 	Short EF identifier
1 1 1 1 1 -- -- -- 	RFU
-- -- -- -- -- 1 x x 	Usage of record number in P1
-- -- -- -- -- 1 0 0 	- Read record P1
-- -- -- -- -- 1 0 1 	- Read all records from P1 up to the last
-- -- -- -- -- 1 1 0 	- Read all records from the last up to P1
-- -- -- -- -- 1 1 1 	RFU
-- -- -- -- -- 0 x x 	Usage of record identifier in P1
-- -- -- -- -- 0 0 0 	- Read first occurence
-- -- -- -- -- 0 0 1 	- Read last occurrence
-- -- -- -- -- 0 1 0 	- Read next occurrence
-- -- -- -- -- 0 1 1 	- Read previous occurrence 
 */

//                               read rec    current current/first  8 byte
// static BYTE PAYFLEX_READRECORD { 0x00, 0xb2, 0x00,   0x00,          0x08 };             // to get the ID, read file#0x0002

PCSC_TYPE pcsc_type=PCSC;
static int twn4_port=-1;

static int pcsc_rdy=0;
static int twn4_rdy=0;
static int connected_type=0;
  
unsigned short PCSC_err2str(const char *s,BYTE *sw) {
  unsigned short ec;

  ec=(((unsigned short)sw[0]) << 8) | ((unsigned short)sw[1]);

  if ((ec!=0x0000) && (ec!=0x9000) && (ec!=0x0090)) {
    if (s) if (strlen(s)>0) printf("%s: ",s);
    printf("%02x %02x: ",sw[0],sw[1]);
  }

  if ((sw[0] & 0xf0==0x60)) printf("Transmission protocol related codes\n");
  if ((sw[0]==0x61)) printf("SW2 indicates the number of response bytes still available\n");
  if ((sw[0]==0x62) && (sw[1]==0x00)) printf("No information given\n");
  if ((sw[0]==0x62) && (sw[1]==0x81)) printf("Returned data may be corrupted\n");
  if ((sw[0]==0x62) && (sw[1]==0x82)) printf("The end of the file has been reached before the end of reading\n");
  if ((sw[0]==0x62) && (sw[1]==0x83)) printf("Invalid DF\n");
  if ((sw[0]==0x62) && (sw[1]==0x84)) printf("Selected file is not valid. File descriptor error\n");
  if ((sw[0]==0x63) && (sw[1]==0x00)) printf("Authentification failed. Invalid secret code or forbidden value\n");
  if ((sw[0]==0x63) && (sw[1]==0x81)) printf("File filled up by the last write\n");
  if ((sw[0]==0x63) && (sw[1]&0xf0==0xC0)) printf("Counter provided by 'X' (valued from 0 to 15) (exact meaning depending on the command)\n");
  if ((sw[0]==0x65) && (sw[1]==0x01)) printf("Memory failure. There have been problems in writing or reading the EEPROM.\n");
  if ((sw[0]==0x65) && (sw[1]==0x81)) printf("Write problem / Memory failure / Unknown mode\n");
  if ((sw[0]==0x67)) printf("Error, incorrect parameter P3 (ISO code)\n");
  if ((sw[0]==0x67) && (sw[1]==0x00)) printf("Incorrect length or address range error\n");
  if ((sw[0]==0x68) && (sw[1]==0x00)) printf("The request function is not supported by the card.\n");
  if ((sw[0]==0x68) && (sw[1]==0x81)) printf("Logical channel not supported\n");
  if ((sw[0]==0x68) && (sw[1]==0x82)) printf("Secure messaging not supported\n");
  if ((sw[0]==0x69) && (sw[1]==0x00)) printf("No successful transaction executed during session\n");
  if ((sw[0]==0x69) && (sw[1]==0x81)) printf("Cannot select indicated file, command not compatible with file organization\n");
  if ((sw[0]==0x69) && (sw[1]==0x82)) printf("Access conditions not fulfilled\n");
  if ((sw[0]==0x69) && (sw[1]==0x83)) printf("Secret code locked\n");
  if ((sw[0]==0x69) && (sw[1]==0x84)) printf("Referenced data invalidated\n");
  if ((sw[0]==0x69) && (sw[1]==0x85)) printf("No currently selected EF, no command to monitor / no Transaction Manager File\n");
  if ((sw[0]==0x69) && (sw[1]==0x86)) printf("Command not allowed (no current EF)\n");
  if ((sw[0]==0x69) && (sw[1]==0x87)) printf("Expected SM data objects missing\n");
  if ((sw[0]==0x69) && (sw[1]==0x88)) printf("SM data objects incorrect\n");
  if ((sw[0]==0x6A) && (sw[1]==0x00)) printf("Bytes P1 and/or P2 are incorrect.\n");
  if ((sw[0]==0x6A) && (sw[1]==0x80)) printf("The parameters in the data field are incorrect\n");
  if ((sw[0]==0x6A) && (sw[1]==0x81)) printf("Card is blocked or command not supported\n");
  if ((sw[0]==0x6A) && (sw[1]==0x82)) printf("File not found\n");
  if ((sw[0]==0x6A) && (sw[1]==0x83)) printf("Record not found\n");
  if ((sw[0]==0x6A) && (sw[1]==0x84)) printf("There is insufficient memory space in record or file\n");
  if ((sw[0]==0x6A) && (sw[1]==0x85)) printf("Lc inconsistent with TLV structure\n");
  if ((sw[0]==0x6A) && (sw[1]==0x86)) printf("Incorrect parameters P1-P2\n");
  if ((sw[0]==0x6A) && (sw[1]==0x87)) printf("The P3 value is not consistent with the P1 and P2 values.\n");
  if ((sw[0]==0x6A) && (sw[1]==0x88)) printf("Referenced data not found.\n");
  if ((sw[0]==0x6B) && (sw[1]==0x00)) printf("Incorrect reference; illegal address; Invalid P1 or P2 parameter\n");
  if ((sw[0]==0x6C)) printf("Incorrect P3 length.\n");
  if ((sw[0]==0x6D) && (sw[1]==0x00)) printf("Command not allowed. Invalid instruction byte (INS)\n");
  if ((sw[0]==0x6E) && (sw[1]==0x00)) printf("Incorrect application (CLA parameter of a command)\n");
  if ((sw[0]==0x6F) && (sw[1]==0x00)) printf("Checking error\n");
  if ((sw[0]==0x90) && (sw[1]==0x00)) ;
  if ((sw[0]==0x91) && (sw[1]==0x00)) printf("Purse Balance error cannot perform transaction\n");
  if ((sw[0]==0x91) && (sw[1]==0x02)) printf("Purse Balance error\n");
  if ((sw[0]==0x92)) printf("Memory error\n");
  if ((sw[0]==0x92) && (sw[1]==0x02)) printf("Write problem / Memory failure\n");
  if ((sw[0]==0x92) && (sw[1]==0x40)) printf("Error, memory problem\n");
  if ((sw[0]==0x94)) printf("File error\n");
  if ((sw[0]==0x94) && (sw[1]==0x04)) printf("Purse selection error or invalid purse\n");
  if ((sw[0]==0x94) && (sw[1]==0x06)) printf("Invalid purse detected during the replacement debit step\n");
  if ((sw[0]==0x94) && (sw[1]==0x08)) printf("Key file selection error\n");
  if ((sw[0]==0x98)) printf("Security error\n");
  if ((sw[0]==0x98) && (sw[1]==0x00)) printf("Warning\n");
  if ((sw[0]==0x98) && (sw[1]==0x04)) printf("Access authorization not fulfilled\n");
  if ((sw[0]==0x98) && (sw[1]==0x06)) printf("Access authorization in Debit not fulfilled for the replacement debit step\n");
  if ((sw[0]==0x98) && (sw[1]==0x20)) printf("No temporary transaction key established\n");
  if ((sw[0]==0x98) && (sw[1]==0x34)) printf("Error, Update SSD order sequence not respected\n");
  if ((sw[0]==0x9F)) printf("Success, XX bytes of data available to be read via \"Get_Response\" task.\n");

  return ec;
}

int PCSC_Init(PCSC_TYPE t_) {
  pcsc_type=t_;

  pcReaders=new char[4096];
    
  rv = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext);
  if (rv != SCARD_S_SUCCESS) {
    printf("%s\n", pcsc_stringify_error(rv));
  } else {
    rv = SCardGetStatusChange(hContext, 200, 0, 0);
    if (rv != SCARD_S_SUCCESS) {
      printf("%s\n", pcsc_stringify_error(rv));
      SCardReleaseContext(hContext);
    } else {
      mszGroups = 0;
      rv = SCardListReaders(hContext, mszGroups, 0, &dwReaders);
      if (rv != SCARD_S_SUCCESS) {
	printf("%s\n", pcsc_stringify_error(rv));
	SCardReleaseContext(hContext);
      } else {
	mszReaders = (char *) malloc(sizeof(char) * dwReaders);
	rv = SCardListReaders(hContext, mszGroups, mszReaders, &dwReaders);
	if (rv != SCARD_S_SUCCESS) {
	  SCardReleaseContext(hContext);
	} else {
	  rgReaderStates[0].szReader = &mszReaders[0];
	  rgReaderStates[0].dwCurrentState = SCARD_STATE_EMPTY;
	  
	  pcsc_rdy=1;
	  
	  printf("Found PC/SC reader\n");

	}
      }
    }
  }

  twn4_port=open("/dev/ttyACM0",O_RDWR|O_NOCTTY|O_NDELAY);
  if (twn4_port==-1) {
    twn4_port=-1;
  } else {
    // set up terminal for raw data
    tcgetattr(twn4_port,&initialAtt);		// save this to restore later
    newAtt=initialAtt;
    cfmakeraw(&newAtt);
    int br=B115200;
    cfsetspeed(&newAtt,br);           // set baud
    if (tcsetattr(twn4_port,TCSANOW,&newAtt)){
      printf("Error setting terminal attributes\n");
      close(twn4_port);
      twn4_port=-1;
    } else {
      twn4_rdy=1;
      printf("Found TWN4 reader\n");
    }
  }

  if ((pcsc_rdy) || (twn4_rdy))
    return 1;
  else
    return 0;
}

static char reader_list[1024];

const char* PCSC_Readers() {
  strcpy(reader_list,"");
  if (pcsc_rdy) strcat(reader_list,"PC/SC ");
  if (twn4_rdy) strcat(reader_list,"TWN4 ");

  return reader_list;
}

char twn4_id[1024];

int TWN4_ReadID() {
  memset(twn4_id,0,1024);

  if (twn4_port>0) {
    write(twn4_port,"050010\n\r",8);    // 0008
    fsync(twn4_port);
    int gr=0;
    int cc=0;
    unsigned char c;
    do {
      int ts=read(twn4_port,&c,1);
      if (ts==1) {
	if (c>=0x20) {
	  twn4_id[cc]=c;
	  cc++;
	}
      }
      if (strlen(twn4_id)==4) {
	if (
	    (twn4_id[0]=='0') &&
	    (twn4_id[1]=='0') &&
	    (twn4_id[2]=='0') &&
	    (twn4_id[3]=='0')
	    ) gr=1;
      } else {
	if (strlen(twn4_id)==18) gr=1;
      }
    } while (!gr);

    // printf("ReadMifare: %s\n",twn4_id);
    if (strlen(twn4_id)==18) return 1;
  }

  return 0;
}
int PCSC_SupportDirectPIN() {
  verify_ioctl=0;
  modify_ioctl=0;

  if (connected_type==1) {
    unsigned char bRecvBuffer [4096];
    DWORD length;
    
    rv = SCardControl(hCard, CM_IOCTL_GET_FEATURE_REQUEST, NULL, 0, bRecvBuffer, sizeof(bRecvBuffer), &length);
    if (length % sizeof(PCSC_TLV_STRUCTURE)) {
      printf("Inconsistent result! Bad TLV values!\n");
      return -1;
    } else {
      length /= sizeof(PCSC_TLV_STRUCTURE);
      PCSC_TLV_STRUCTURE *pcsc_tlv = (PCSC_TLV_STRUCTURE *)bRecvBuffer;

      for (DWORD i = 0; i < length; i++) {
	if (pcsc_tlv[i].tag == FEATURE_VERIFY_PIN_DIRECT) {
	  verify_ioctl = ntohl(pcsc_tlv[i].value);
	  printf("Reader supports VERIFY PIN DIRECT (%x)\n",verify_ioctl);
	}
	if (pcsc_tlv[i].tag == FEATURE_MODIFY_PIN_DIRECT) {
	  modify_ioctl = ntohl(pcsc_tlv[i].value);
	  printf("Reader supports MODIFY PIN DIRECT (%x)\n",modify_ioctl);
	}
      }
    }
  }

  if (verify_ioctl)
    return 1;

  return 0;
}

int PCSC_CardConnect(int waitforcard,int ct) {
  if ((pcsc_rdy==1) && ((ct==0) || (ct==1)) && ((connected_type==0) || (connected_type==1))) {
    if (waitforcard==1) {
      //--- Waiting for card insertion
      rv = SCardGetStatusChange(hContext, INFINITE, rgReaderStates, 1);
      if (rv != SCARD_S_SUCCESS) {
	// printf("StatusChange : %s\n",pcsc_stringify_error(rv));
	connected_type=0;
      } else {
	rv = SCardConnect(hContext, &mszReaders[0],SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1,&hCard, &dwPref);
	if (rv != SCARD_S_SUCCESS) {
	  // printf("CardConnect : %s\n", pcsc_stringify_error(rv));
	  connected_type=0;
	} else {
	  connected_type=1;
	  return 1;
	}
      }
    } else {
      //--- Waiting for card insertion
      rv = SCardGetStatusChange(hContext, 250, rgReaderStates, 1); // for 100 ms 
      if (rv != SCARD_S_SUCCESS) {
	// printf("StatusChange.2 : %s\n",pcsc_stringify_error(rv));
	connected_type=0;
      } else {
	rv = SCardConnect(hContext, &mszReaders[0],SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1,&hCard, &dwPref);
	if (rv != SCARD_S_SUCCESS) {
	  // printf(" CardConnect.2 : %s\n", pcsc_stringify_error(rv));
	  connected_type=0;
	} else {
	  connected_type=1;
	  return 1;
	}
      }
    }
  }
  
  if ((twn4_rdy==1) && ((ct==0) || (ct==2)) && ((connected_type==0) || (connected_type==2))) {
    if (twn4_port==-1) {
      connected_type=0;
      return 0;
    } else {
      if (TWN4_ReadID()) {
	// printf("PCSC_CardConnect: detected RFID card\n");
	connected_type=2;
	return 1;
      } else {
	connected_type=0;
      }
    }
  }

  return 0;
}

int PCSC_GetATR(unsigned char *atr) {
  dwReaderLen = 4096;
  dwAtrLen    = MAX_ATR_SIZE;
  rv = SCardStatus(hCard, pcReaders, &dwReaderLen, &dwState, &dwProt,atr,&dwAtrLen);
  if (rv != SCARD_S_SUCCESS) {
    printf("%s\n", pcsc_stringify_error(rv));
    return 0;
  }
  return dwAtrLen;
}

static char pcsc_atr[1024];

const char *PCSC_GetATRStr() {
  strcpy(pcsc_atr,"");
  unsigned char atr[MAX_ATR_SIZE];
  int atrlen=PCSC_GetATR(atr);
  if (atrlen>0) {
    char sx[16];
    for (int i=0;i<atrlen;i++) {
      sprintf(sx,"%02x",atr[i]);
      strcat(pcsc_atr,sx);
    }
  }

  return pcsc_atr;
}

int PCSC_SelectFile(unsigned short fid) {
  SCARD_IO_REQUEST pioRecvPci;
  BYTE apdu[]= { 0x00, 0xa4, 0x00, 0x00, 0x02,   0x00, 0x00 }; // last two byte are file# in MSB
  unsigned long blen=4;
  BYTE buffer[blen];
  
  apdu[5]=(BYTE)((fid >> 8) & 0xff);
  apdu[6]=(BYTE)(fid & 0xff);

  rv = SCardTransmit(hCard, SCARD_PCI_T0, apdu, sizeof(apdu), &pioRecvPci , buffer, &blen);
  if (rv != SCARD_S_SUCCESS) {
    printf("%s\n", pcsc_stringify_error(rv));
    return 0;
  }

  if (blen>0) {
    char s[256];
    sprintf(s,"SelectFile: %04x ",fid);

    if (PCSC_err2str(s,buffer)==0x9000) return 1;
  }

  return 0;
}
		    
// static BYTE PAYFLEX_READRECORD { 0x00, 0xb2, 0x00,   0x00,          0x08 };             // to get the ID, read file#0x0002
int PCSC_ReadRecord(BYTE nreq,BYTE *buffer) {
  SCARD_IO_REQUEST pioRecvPci;

  BYTE apdu[]= { 0x00, 0xb2, 0x00, 0x00, nreq };
  unsigned long blen=nreq+2;
  
  rv = SCardTransmit(hCard, SCARD_PCI_T0, apdu, sizeof(apdu), &pioRecvPci , buffer, &blen);
  if (rv != SCARD_S_SUCCESS) {
    printf("%ld : %s\n", rv , pcsc_stringify_error(rv));
    return 0;
  }

  return blen;
}
		    
int PCSC_WriteRecord(BYTE nreq,BYTE *buffer) {
  SCARD_IO_REQUEST pioRecvPci;

  // BYTE apdu[]= { 0x00, 0xd2, 0x00, 0x02, nreq };
  BYTE *apdu = new BYTE [ 5 + nreq ];

  // for AC nreq+8 if there is an MAC
  // BYTE *apdu = new BYTE [ 5+ nreq + 8 + 2 ];
  unsigned long blen=4;
  
  apdu [ 0]=0x00;
  apdu [ 1]=0xd2;
  apdu [ 2]=0x00;
  apdu [ 3]=0x02;
  apdu [ 4]=nreq;

  for (unsigned int i=0;i<nreq;i++) apdu[5+i]=buffer[i];

  rv = SCardTransmit(hCard, SCARD_PCI_T0, apdu, 5+nreq , &pioRecvPci , buffer, &blen);

  if (rv != SCARD_S_SUCCESS) {
    printf("%ld : %s\n", rv , pcsc_stringify_error(rv));
    delete apdu;
    return 0;
  }

  delete apdu;

  if (blen>0) {
    if (PCSC_err2str("WriteRecord",buffer)==0x9000) return 1;
  }

  return 0;
}
		    
int PCSC_UpdateBinary(unsigned short offset,BYTE nreq,BYTE *buffer) {
  SCARD_IO_REQUEST pioRecvPci;

  // BYTE apdu[]= { 0x00, 0xd2, 0x00, 0x02, nreq };
  BYTE *apdu = new BYTE [ 5 + nreq ];

  // for AC nreq+8 if there is an MAC
  // BYTE *apdu = new BYTE [ 5+ nreq + 8 + 2 ];
  BYTE rbuffer[4];
  unsigned long blen=4;
  
  apdu[ 0]=0x00;
  apdu[ 1]=0xd6;
  apdu[ 2]=(BYTE)((offset >> 8) & 0xff);
  apdu[ 3]=(BYTE)(offset & 0xff);
  apdu[ 4]=nreq;

  for (unsigned int i=0;i<nreq;i++) apdu[5+i]=buffer[i];

  rv = SCardTransmit(hCard, SCARD_PCI_T0, apdu, 5+nreq, &pioRecvPci , rbuffer, &blen);

  if (rv != SCARD_S_SUCCESS) {
    printf("%ld : %s\n", rv , pcsc_stringify_error(rv));
    delete apdu;
    return 0;
  }

  delete apdu;

  if (blen>0) {
    if (PCSC_err2str("UpdateBinary",rbuffer)==0x9000) return 1;
  }

  return blen;
}
		    
int PCSC_ReadBinary(unsigned short offset,BYTE nreq,BYTE *buffer) {
  SCARD_IO_REQUEST pioRecvPci;

  // BYTE apdu[]= { 0x00, 0xd2, 0x00, 0x02, nreq };
  BYTE *apdu = new BYTE [ 5 + nreq ];

  // for AC nreq+8 if there is an MAC
  // BYTE *apdu = new BYTE [ 5+ nreq + 8 + 2 ];
  unsigned long blen=nreq+2;
  
  apdu[ 0]=0x00;
  apdu[ 1]=0xb0;
  apdu[ 2]=(BYTE)((offset >> 8) & 0xff);
  apdu[ 3]=(BYTE)(offset & 0xff);
  apdu[ 4]=nreq;

  for (unsigned int i=0;i<nreq;i++) apdu[5+i]=buffer[i];

  rv = SCardTransmit(hCard, SCARD_PCI_T0, apdu, 5, &pioRecvPci , buffer, &blen);

  delete apdu;

  if (rv != SCARD_S_SUCCESS) {
    printf("ReadBinary: %ld : %s\n", rv , pcsc_stringify_error(rv));
    return 0;
  }

  if (blen>0) {
    if (PCSC_err2str("ReadBinary",buffer+nreq)==0x9000) return 1;
  }

  return 0;
}
		    
int PCSC_HasPIN() {
  if (PCSC_SelectFile(0x7f10))
    return PCSC_SelectFile(0x7f12);
  return 0;
}

int PCSC_Verify_Secure(unsigned short *ec) {
  if (PCSC_SupportDirectPIN()) {
    unsigned char bRecvBuffer [4096];
    unsigned char bSendBuffer [4096];
    DWORD length;
    
    PIN_VERIFY_STRUCTURE *pin_verify;
    
    /* verify PIN */
    pin_verify = (PIN_VERIFY_STRUCTURE *)bSendBuffer;
    
    /* PC/SC v2.02.05 Part 10 PIN verification data structure */
    pin_verify -> bTimerOut = 0x40;                          // < timeout is seconds (00 means use default timeout)
    pin_verify -> bTimerOut2 = 0x40;                         // < timeout in seconds after first key stroke 
    pin_verify -> bmFormatString = 0x82;                     // < formatting options 
    pin_verify -> bmPINBlockString = 0x44;                   /* < bits 7-4 bit size of PIN length in APDU,
								+ * bits 3-0 PIN block size in bytes after
								+ * justification and formatting */
    pin_verify -> bmPINLengthFormat = 0x00;                  /**< bits 7-5 RFU,
								+ * bit 4 set if system units are bytes, clear if
								+ * system units are bits,
								+ * bits 3-0 PIN length position in system units */
    pin_verify -> wPINMaxExtraDigit = HOST_TO_CCID_16(0x0404); /**< 0xXXYY where XX is minimum PIN size in digits,
								  + and YY is maximum PIN size in digits */
    pin_verify -> bEntryValidationCondition = 0x02;	/**< Conditions under which PIN entry should
							   + * be considered complete,
							   + 0x02 : validation key pressed */
    pin_verify -> bEntryValidationCondition = 0x01;     // max pin size reached

    pin_verify -> bNumberMessage = 0x01;                /**< Number of messages to display for PIN verification */
    pin_verify -> wLangId = HOST_TO_CCID_16(0x0904);    /**< Language for messages, here: english */
    pin_verify -> bMsgIndex = 0x00;                     /**< Message index (should be 00) */
    pin_verify -> bTeoPrologue[0] = 0x00;               /**< T=1 block prologue field to use (fill with 00) */
    pin_verify -> bTeoPrologue[1] = 0x00;               
    pin_verify -> bTeoPrologue[2] = 0x00;
    
    /* APDU: 00 20 00 00 08 30 30 30 30 00 00 00 00 */
    int offset = 0;
    pin_verify -> abData[offset++] = 0x00;	/* CLA */
    pin_verify -> abData[offset++] = 0x20;	/* INS: VERIFY */
    pin_verify -> abData[offset++] = 0x00;	/* P1 */
    pin_verify -> abData[offset++] = 0x00;	/* P2 */
    pin_verify -> abData[offset++] = 0x08;	/* Lc: 8 data bytes */
    pin_verify -> abData[offset++] = 0x00;	/* '0' */
    pin_verify -> abData[offset++] = 0x00;	/* '0' */
    pin_verify -> abData[offset++] = 0x00;	/* '0' */
    pin_verify -> abData[offset++] = 0x00;	/* '0' */
    pin_verify -> abData[offset++] = 0x00;	/* '\0' */
    pin_verify -> abData[offset++] = 0x00;	/* '\0' */
    pin_verify -> abData[offset++] = 0x00;	/* '\0' */
    pin_verify -> abData[offset++] = 0x00;	/* '\0' */
    pin_verify -> ulDataLength = HOST_TO_CCID_32(offset);	/* APDU size */
    
    length = sizeof(PIN_VERIFY_STRUCTURE) + offset;	/* because PIN_VERIFY_STRUCTURE contains the first byte of abData[] */
    
    rv = SCardControl(hCard, verify_ioctl , bSendBuffer,length, bRecvBuffer, sizeof(bRecvBuffer), &length);
    
    for (int i=0; i<length; i++) ec[i]=bRecvBuffer[i];

    return 1;

  } else
    return 0;
}

int PCSC_Verify(BYTE *key,unsigned short *ec) {
  SCARD_IO_REQUEST pioRecvPci;
  BYTE   apdu[] = { 0x00, 0x20, 0x00, 0x00, 0x08,        0x47, 0x46, 0x58, 0x49, 0x32, 0x56, 0x78, 0x40 }; // Paylfex TK ?
  //  BYTE   apdu[] = { 0x00, 0x20, 0x00, 0x10, 0x08,    0x2c, 0x15, 0xe5, 0x26, 0xe9, 0x3e, 0x8a, 0x19 }; // Cryptoflex TK?

  if (key) {
    apdu[3]=0x10;
    for (unsigned int i=0;i<8;i++)
      apdu[5+i]=key[i];
  }

  unsigned long blen=4;
  BYTE buffer[blen];

  rv = SCardTransmit(hCard, SCARD_PCI_T0, apdu, 13, &pioRecvPci , buffer, &blen);

  if (rv != SCARD_S_SUCCESS) {
    printf("%s\n", pcsc_stringify_error(rv));
    return 0;
  }

  if (blen>0) {
    *ec=PCSC_err2str("Verify",buffer);
    if (*ec==0x9000) {
      return 1;
    } else {
	return 0;
    }
  }
  
  return 0;
}

int PCSC_Verify(const char *pin,unsigned short *ec) {
  BYTE PIN[8];
  for (unsigned int i=0;i<8;i++) PIN[i]=0;
  PIN[0]=(BYTE)(pin[0]-'0');
  PIN[1]=(BYTE)(pin[1]-'0');
  PIN[2]=(BYTE)(pin[2]-'0');
  PIN[3]=(BYTE)(pin[3]-'0');
  return PCSC_Verify(PIN,ec);
}

int PCSC_Change(BYTE *okey,BYTE *nkey,unsigned short *ec) {
  SCARD_IO_REQUEST pioRecvPci;
  BYTE   apdu[] = { 
    0x00, 0x24,   0x00, 0x10, 0x10,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };

  if ((okey) && (nkey)) {
    for (unsigned int i=0;i<8;i++) {
      apdu[ 5+i]=okey[i];
      apdu[13+i]=nkey[i];
    }

    unsigned long blen=4;
    BYTE buffer[blen];
    
    rv = SCardTransmit(hCard, SCARD_PCI_T0, apdu, 21, &pioRecvPci , buffer, &blen);
    if (rv != SCARD_S_SUCCESS) {
      printf("%s\n", pcsc_stringify_error(rv));
      return 0;
    }
    
    if (blen>0) {
      *ec=PCSC_err2str("Verify",buffer);
      if (*ec==0x9000) {
	return 1;
      } else {
	return 0;
      }
    }

  }

  return 0;
}

int PCSC_Change(const char *opin,const char *npin,unsigned short *ec) {
  BYTE OPIN[8];
  BYTE NPIN[8];
  for (unsigned int i=0;i<8;i++) {
    OPIN[i]=0;
    NPIN[i]=0;
  }
  OPIN[0]=(BYTE)(opin[0]-'0');
  OPIN[1]=(BYTE)(opin[1]-'0');
  OPIN[2]=(BYTE)(opin[2]-'0');
  OPIN[3]=(BYTE)(opin[3]-'0');

  NPIN[0]=(BYTE)(npin[0]-'0');
  NPIN[1]=(BYTE)(npin[1]-'0');
  NPIN[2]=(BYTE)(npin[2]-'0');
  NPIN[3]=(BYTE)(npin[3]-'0');

  return PCSC_Change(OPIN,NPIN,ec);
}

int PCSC_CreateDF(unsigned short fid, unsigned short bs) {
  SCARD_IO_REQUEST pioRecvPci;
  // C. DF 7F10 :  00      E0    00    00    09      02    6F    7F    10     38   02    00    00  00

  //                                        DF        -- BS----   -- FID ---  FT    -- AC ---  -- KN ----
  BYTE   apdu[]= { 0x00, 0xe0, 0x00, 0x00, 0x09,     0x00, 0x00, 0x00, 0x00, 0x38, 0x00,0x00, 0x00, 0x00 };

  //                                      DF AC=PRO
  /*
    BYTE proapdu[]= { 0x00, 0xe0, 0x00, 0x00, 0x11,     0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00 ,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; // MAC, if AC=PRO
  */
  unsigned long blen=4;
  BYTE buffer[blen];
  
  apdu[5]=(BYTE)((bs >> 8) & 0xff);
  apdu[6]=(BYTE)(bs & 0xff);

  apdu[7]=(BYTE)((fid >> 8) & 0xff);
  apdu[8]=(BYTE)(fid & 0xff);


  rv = SCardTransmit(hCard, SCARD_PCI_T0, apdu, sizeof(apdu), &pioRecvPci , buffer, &blen);
  if (rv != SCARD_S_SUCCESS) {
    printf("%s\n", pcsc_stringify_error(rv));
    return 0;
  }

  if (blen>0) {
    if (PCSC_err2str("CreateDF",buffer)==0x9000) return 1;
  }

  return 0;
}
		    
int PCSC_CreateTREF(unsigned short fid, unsigned short bs,BYTE pin) {
  SCARD_IO_REQUEST pioRecvPci;
  // BS: block size
  // FID: file ID
  // FT: FileType
  // AC: Access Condition
  // KN: Key Number
  // RL: record length
  // NWR: # of written records
  //                                        EF       -- BS----   -- FID ---  FT    -- AC ---  -- KN ----  -- RFU ---
  BYTE   apdu[]= { 0x00, 0xe0, 0x00, 0x00, 0x0B,     0x00, 0x00, 0x00, 0x00, 0x01, 0x00,0x00, 0x00, 0x00, 0x00, 0x00 };

  //                                      DF AC=PRO
  //  BYTE proapdu[]= { 0x00, 0xe0, 0x00, 0x00, 0x11,     0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00 ,
  //                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; // MAC, if AC=PRO
  unsigned long blen=4;
  BYTE buffer[blen];
  
  apdu[ 5]=(BYTE)((bs >> 8) & 0xff);
  apdu[ 6]=(BYTE)(bs & 0xff);

  apdu[ 7]=(BYTE)((fid >> 8) & 0xff);
  apdu[ 8]=(BYTE)(fid & 0xff);

  if (pin>0) {
    apdu[10]=0x11;  // read_binary and update_binary protection by PIN
    apdu[11]=0x00;  // no lock_update_binary
    apdu[12]=pin; // pin# for read_binary and update_binary
    apdu[13]=pin; // pin# for lock_update_binary
  }

  rv = SCardTransmit(hCard, SCARD_PCI_T0, apdu, 15, &pioRecvPci , buffer, &blen);
  if (rv != SCARD_S_SUCCESS) {
    printf("%s\n", pcsc_stringify_error(rv));
    return 0;
  }

  if (blen>0) {
    if (PCSC_err2str("CreateTREF",buffer)==0x9000) return 1;
  }

  return 0;
}
		    
int PCSC_CreateLFEF(unsigned short fid, unsigned short bs,BYTE rl) {
  SCARD_IO_REQUEST pioRecvPci;
  // BS: block size
  // FID: file ID
  // FT: FileType
  // AC: Access Condition
  // KN: Key Number
  // RL: record length
  // NWR: # of written records
  //                                        EF       -- BS----   -- FID ---  FT    -- AC ---  -- KN ----  - RL  NWR
  BYTE   apdu[]= { 0x00, 0xe0, 0x00, 0x00, 0x0B,     0x00, 0x00, 0x00, 0x00, 0x02, 0x00,0x00, 0x00, 0x00, 0x00, 0x00 };
  // for a PIN-file this should look like:                                   
  //               0x00, 0xe0, 0x00, 0x00, 0x0b,                             0x22, 0x00,0x00, 0x00, 0x00, 0x0a, 0x00 };

  //                                      DF AC=PRO
  //  BYTE proapdu[]= { 0x00, 0xe0, 0x00, 0x00, 0x11,     0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00 ,
  //                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; // MAC, if AC=PRO
  unsigned long blen=4;
  BYTE buffer[blen];
  
  apdu[ 5]=(BYTE)((bs >> 8) & 0xff);
  apdu[ 6]=(BYTE)(bs & 0xff);

  apdu[ 7]=(BYTE)((fid >> 8) & 0xff);
  apdu[ 8]=(BYTE)(fid & 0xff);

  apdu[14]=rl;


  rv = SCardTransmit(hCard, SCARD_PCI_T0, apdu, sizeof(apdu), &pioRecvPci , buffer, &blen);
  if (rv != SCARD_S_SUCCESS) {
    printf("%s\n", pcsc_stringify_error(rv));
    return 0;
  }

  if (blen>0) {
    if (PCSC_err2str("CreateLFEF",buffer)==0x9000) return 1;
  }

  return 0;
}



int PCSC_ResetPIN(BYTE *oldpin,BYTE *newpin) {
  SCARD_IO_REQUEST pioRecvPci;

  BYTE   apdu[]= { 
    0x00, 0x24, 0x00, 0x10, 0x10, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, // old PIN
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00  // new PIN
  };

  unsigned long blen=4;
  BYTE buffer[blen];

  for (unsigned int i=0;i<8;i++) apdu[ 5+i]=oldpin[7-i];
  for (unsigned int i=0;i<8;i++) apdu[13+i]=newpin[7-i];
  
  rv = SCardTransmit(hCard, SCARD_PCI_T0, apdu, sizeof(apdu), &pioRecvPci , buffer, &blen);
  if (rv != SCARD_S_SUCCESS) {
    printf("%s\n", pcsc_stringify_error(rv));
    return 0;
  }
  
  if (blen>0) {
    if (PCSC_err2str("ResetPIN",buffer)==0x9000) return 1;
  }

  return 0;
}

int PCSC_CreatePIN(unsigned short fid,BYTE *pin,BYTE *puk) {
  if (pin) {
    SCARD_IO_REQUEST pioRecvPci;
    // BS: block size
    // FID: file ID
    // FT: FileType
    // AC: Access Condition
    // KN: Key Number
    // RL: record length
    // NWR: # of written records
    //                                        EF       -- BS----   -- FID ---  FT    -- AC ---  -- KN ----  - RL  NWR
    BYTE   apdu[]= { 0x00, 0xe0, 0x00, 0x00, 0x0B,     0x00, 0x00, 0x00, 0x00, 0x22, 0x00,0x00, 0x00, 0x00, 0x0a, 0x00 };
    //                 00    E0    00    00    0B        00    0A    00    00    22    F0   02    00    01    0A    00
    unsigned long blen=4;
    BYTE buffer[blen];
    
    if (!puk)
      apdu[ 6]=0x0a;
    else
      apdu[ 6]=0x14;
    
    apdu[ 7]=(BYTE)((fid >> 8) & 0xff);
    apdu[ 8]=(BYTE)(fid & 0xff);
    
    rv = SCardTransmit(hCard, SCARD_PCI_T0, apdu, 16, &pioRecvPci , buffer, &blen);
    if (rv != SCARD_S_SUCCESS) {
      printf("%s\n", pcsc_stringify_error(rv));
    return 0;
    }
    
    if (blen>0) {
      if (PCSC_err2str("CreatePIN File",buffer)!=0x9000) return 0;
    }

    if ((buffer[0]==0x90) && (buffer[1]==0x00)) {
      // now write PIN to file
      
      if (PCSC_SelectFile(fid)) {
	BYTE *pbuffer=new BYTE [10];
	
	pbuffer[0]=0x0f; // activated and 15 remaining attempts ?
	// copy pin to buffer
	for (unsigned int i=0;i<8;i++) pbuffer[1+i]=pin[i];
	pbuffer[9]=0x0f; // max 15 attempts
	
	if (PCSC_WriteRecord(10,pbuffer)) {
	
	  if (puk) {
	    pbuffer[0]=0x0f; // activated and 15 remaining attempts ?
	    // copy pin to buffer
	    for (unsigned int i=0;i<8;i++) pbuffer[1+i]=puk[i];
	    pbuffer[9]=0x0f; // max 15 attempts
	    if (PCSC_WriteRecord(10,pbuffer))
	      return 1;
	    else
	      return 0;
	  }
	  return 1;

	}
      }
    }
  }

  return 0; // no PIN given
}
		    
int PCSC_DeleteFile(unsigned short fid) {
  SCARD_IO_REQUEST pioRecvPci;
  BYTE   apdu[]= { 0x00, 0xe4, 0x00, 0x00, 0x02, 0x00, 0x00 };

  unsigned long blen=4;
  BYTE buffer[blen];
  
  apdu[ 5]=(BYTE)((fid >> 8) & 0xff);
  apdu[ 6]=(BYTE)(fid & 0xff);

  rv = SCardTransmit(hCard, SCARD_PCI_T0, apdu, sizeof(apdu), &pioRecvPci , buffer, &blen);
  if (rv != SCARD_S_SUCCESS) {
    printf("%s\n", pcsc_stringify_error(rv));
    return 0;
  }

  if (blen>0) {
    char s[256];
    sprintf(s,"Delete File (0x%04x)",fid);
    if (PCSC_err2str(s,buffer)==0x9000) return 1;
  }

  return 0;
}

int PCSC_CardRemoved() {
  rv = SCardStatus(hCard, pcReaders, &dwReaderLen, &dwState, &dwProt,pbAtr, &dwAtrLen);
  if (rv==SCARD_W_REMOVED_CARD) return 1;
  return 0;
}

int PCSC_CardDisconnect() {
  rv = SCardDisconnect(hCard, SCARD_UNPOWER_CARD);
  if (rv != SCARD_S_SUCCESS) {
    printf("%s\n", pcsc_stringify_error(rv));
    SCardReleaseContext(hContext);
    return 0;
  }

  return 1;
}

int PCSC_Close() {
  rv = SCardReleaseContext(hContext);
  if (rv != SCARD_S_SUCCESS) {
    printf("%s\n", pcsc_stringify_error(rv));
    return 0;
  }
  return 1;
}


static char pfx_id[512];
static char mifare_id[512];

const char* PCSC_PayflexID() {
  strcpy(pfx_id,"");

  if (PCSC_SelectFile(0x3f00)) {
    if (PCSC_SelectFile(0x0002)) {
      BYTE id[4096];
      int idl=PCSC_ReadRecord(8,id);
      if (idl==10) {
	sprintf(pfx_id,"Payflex.");
	for (int i=0;i<8;i++) {
	  char sx[16];
	  sprintf(sx,"%02x",id[i]);
	  strcat(pfx_id,sx);
	}
      }
    }
  }

  return pfx_id;
}

const char* PCSC_MifareID() {
  TWN4_ReadID();

  if (strlen(twn4_id)==18) {
    sprintf(mifare_id,"Mifare.%s",twn4_id);
  } else {
    strcpy(mifare_id,"");
  }

  return mifare_id;
}

const char* PCSC_CardID() {
  if ((pcsc_rdy==1) && (connected_type==1)) {
    return PCSC_PayflexID();
  }

  if ((twn4_rdy==1) && (connected_type==2)) {
    return PCSC_MifareID();
  }

  return "";
}

void PCSC_ClearPrivateKeyBuffer() {
  // memset(pcsc_id_dsa,0,1024);
}

int PCSC_GetPrivateKey(const char *pin,char *pcsc_id_dsa) {
  memset(pcsc_id_dsa,0,1024);

  int haspin=PCSC_HasPIN();

  if (!PCSC_SelectFile(0x7f10)) return 0;
  if (!PCSC_SelectFile(0x7f11)) return 0;

  if ((haspin) && (pin) && (strlen(pin)==4)) {
    BYTE PIN[8];
    for (unsigned int i=0;i<8;i++) PIN[i]=0;

    if (pin) {
      PIN[0]=(BYTE)(pin[0]-'0');
      PIN[1]=(BYTE)(pin[1]-'0');
      PIN[2]=(BYTE)(pin[2]-'0');
      PIN[3]=(BYTE)(pin[3]-'0');
    }

    unsigned short ec;
    int r=-1;
    //if (PCSC_SupportDirectPIN())
    //      r=PCSC_Verify_Secure(ec);
    //    else
    r=PCSC_Verify(PIN,&ec);

    if (ec==0x6300) return -1; // wrong pin, but not locked yet
    if (ec==0x6983) return -2; // locked!
    if (ec!=0x9000) return 0;
  }

  BYTE *data=new BYTE [1024];
  memset(data,0,1024);

  unsigned int ofs=0;
  unsigned short bsize;
  do {
    bsize = PCSC_ReadBinary(ofs,8,data+ofs);

    if (bsize == 0) {
      printf("Error getting PKI data: %d\n",bsize);
      delete data;
      return 0;
    }
    ofs=ofs+8;
  } while (ofs<976);
  
  memcpy(pcsc_id_dsa,data,1024);

  delete data;

  return 1;
}

const char pcsc_atr_payflex[]="3b6900002494010201000101a9";

int PCSC_IsPayflex() {
  if (!strcmp(PCSC_GetATRStr(),pcsc_atr_payflex)) return 1;
  return 0;
}

//	Chipcard from SUN to be used in SunRay's
//	370-4328-01 (31091)

// vermutlich  Payflex 1K

/*
#    MicroPayflex and Payflex - new version
#	The "new" cards encode the last four bytes of the ATR
#	history with the following information:
#
#	    RFU - (reserved for future use)
#	     CM - Certification Mode
#	    SMC - Standard Memory Capacity
#	    CID - Card ID (Type) (this is the Card Type, not the SunRay ID)
#
#	The following table describes all the known values as of 8/23/2000:
#
#	    Card Type       RFU     CM      SMC     CID
#	    -------------------------------------------
#	    MicroPayflex    00      00      00      A9
#	    Payflex 1K      00      00      01      A9
#	    Payflex 2K      00      00      02      A9
#	    Payflex 4K      00      00      04      A9
#	    Payflex 4K SAM  00      00      04      C9
#	    Payflex 8K      00      01      08      A9
#	    Payflex 8K SAM  00      01      08      C9
*/
