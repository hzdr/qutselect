#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pcsc.h>

int main(int argc, char **argv) {
  if (argc<=1) {
    printf("Usage: prepcard <keyfile> <4-digit user-pin> <8-digit PUK>\n");
    exit(0);
  }

  PCSC_Init();

  PCSC_CardConnect(1);
  
  if (PCSC_IsPayflex()) {
    printf("Payflex.%s\n",PCSC_PayflexID());

    int bsize;

    if (!strcmp(argv[1],"check")) {
      if (PCSC_SelectFile(0x7f10)) {
	if (PCSC_HasPIN()) {
	  // this is a PIN protected card
	  printf("Please enter PIN: ");
	  BYTE pin[8];
	  for (unsigned int i=0;i<8;i++) pin[i]=0x00;
	  unsigned int ipn;
	  scanf("%u",&ipn);
	  pin[0]=(BYTE)((ipn/1000) % 10);
	  pin[1]=(BYTE)((ipn/100)  % 10);
	  pin[2]=(BYTE)((ipn/10)   % 10);
	  pin[3]=(BYTE)((ipn)      % 10);
	  PCSC_SelectFile(0x7f11);
	  PCSC_Verify(pin);
	} else
	  PCSC_SelectFile(0x7f11);

	// read data from pk-file
	
	BYTE *data1=new BYTE [1024];
	memset(data1,0,1024);
      
	unsigned short ofs=0;
	do {
	  bsize=PCSC_ReadBinary(ofs,16,data1+ofs);
	  ofs=ofs+16;
	} while (ofs<976);
	
	printf("%s",data1);
	delete data1;
      } else {
	// does not look like a HZDR card
	printf("*** this card seems not to be formatted correctly ***\n");
      }

      PCSC_CardDisconnect();
      PCSC_Close();
      exit(0);
    }

    // cleanup card:
    printf("--- cleanup card ---\n");
    if (PCSC_SelectFile(0x3f00)) {
      if (PCSC_SelectFile(0x7f10)) {
	if (PCSC_Verify(NULL)) {
	  PCSC_DeleteFile(0x7f11);
	  PCSC_DeleteFile(0x7f12);
	  
	  PCSC_SelectFile(0x3f00);
	  PCSC_DeleteFile(0x7f10);
	}
      }
    }
      
    if (!strcmp(argv[1],"cleanup")) {
      PCSC_CardDisconnect();
      PCSC_Close();
      exit(0);
    }

    if (argc!=4) {
      printf("Usage: prepcard <keyfile> <4-digit user-pin> <8-digit PUK>\n");

      PCSC_CardDisconnect();
      PCSC_Close();

      exit(-1);
    }

    printf("--- create APP dir ---\n");
    PCSC_SelectFile(0x3f00); // select MF
    PCSC_Verify(NULL);
    
    unsigned short kfsize=1024-11-13-20;

    PCSC_CreateDF(0x7f10, (13+20) + (11+980) );  // create DF 0x7f10, holding EF 0x7f11 of 1 kByte and PIN file of 2*10 Byte

    printf("--- create PIN entry ---\n");
    PCSC_SelectFile(0x7f10); // select DF 0x7f10
    PCSC_Verify(NULL);
    
    unsigned long ipn;
    BYTE pin[8],puk[8];

    sscanf(argv[2],"%ul",&ipn);
    pin[0]=(BYTE)((ipn/1000) % 10);
    pin[1]=(BYTE)((ipn/100)  % 10);
    pin[2]=(BYTE)((ipn/10)   % 10);
    pin[3]=(BYTE)((ipn)      % 10);
    pin[4]=0;
    pin[5]=0;
    pin[6]=0;
    pin[7]=0;

    sscanf(argv[3],"%ul",&ipn);
    puk[0]=(BYTE)((ipn/10000000) % 10);
    puk[1]=(BYTE)((ipn/1000000)  % 10);
    puk[2]=(BYTE)((ipn/100000)   % 10);
    puk[3]=(BYTE)((ipn/10000)    % 10);
    puk[4]=(BYTE)((ipn/1000)     % 10);
    puk[5]=(BYTE)((ipn/100)      % 10);
    puk[6]=(BYTE)((ipn/10)       % 10);
    puk[7]=(BYTE)((ipn/1)        % 10);

    PCSC_CreatePIN(0x7f12,pin,puk);

    printf("--- create SSH Key data structure ---\n");
    PCSC_SelectFile(0x3f00); // select MF
    PCSC_SelectFile(0x7f10); // select DF 0x7f10

    printf("--- Create Key-File ---\n");
    PCSC_Verify(NULL);
    PCSC_CreateTREF(0x7f11, 976, 1 );  // allocate 980 Byte , using PIN 1
    
    {
      // load data to pk-file
      BYTE *data0=new BYTE [1024];
      memset(data0,0,1024);
      
      FILE *f=fopen(argv[1],"r");
      fread(data0,672,1,f);
      fclose(f);
      
      printf("--- Verify PIN ---\n");
      PCSC_SelectFile(0x7f10);
      PCSC_SelectFile(0x7f11);
      PCSC_Verify(pin);

      printf("--- writing key data ---\n");
      unsigned int ofs=0;
      do {
	bsize=PCSC_UpdateBinary(ofs,16,(data0+ofs));
	ofs=ofs+16;
      } while (ofs<976);

      printf("--- verify written key data ---\n");
      // read data from pk-file
      PCSC_SelectFile(0x7f10);
      PCSC_SelectFile(0x7f11);
      
      BYTE *data1=new BYTE [1024];
      memset(data1,0,1024);
      
      ofs=0;
      do {
	bsize=PCSC_ReadBinary(ofs,16,data1+ofs);
	ofs=ofs+16;
      } while (ofs<976);
      
      int match=1;
      for (unsigned int i=0;i<976;i++)
	if (data0[i]!=data1[i]) match=0;

      if (!match) {
	printf("*** key data verification failed ***\n");
	printf("*** original:\n");
	printf("%s",data0);
	printf("*** on card:\n");
	printf("%s",data1);
      }

      delete data1;
      delete data0;
    }
  }

  PCSC_CardDisconnect();
  PCSC_Close();
}
