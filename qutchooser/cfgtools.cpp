#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <ostream>
#include <string.h>
#include <sys/stat.h>

#if defined(_WIN32) || defined(_WIN64)
#else
  #include <unistd.h>
#endif

#include <cfgtools.h>

static char *cfgfile=NULL;
static char *apppath=NULL;

void ParseCommandline(int argc,char* argv[]) {

  char *buffer;
  
  // determine application path
  buffer=new char [strlen(argv[0])+1];

  strcpy(buffer,argv[0]);
  int i=strlen(buffer)-1;
  #ifdef WINDOWS
    while ((i>0) && (buffer[i]!='\\')) i--;
  #else
    while ((i>0) && (buffer[i]!='/')) i--;
  #endif
  buffer[i]=0;
  if (strlen(buffer)==0) {
    #ifdef WINDOWS
      strcpy(buffer,".\\");
    #else
      strcpy(buffer,"./");
    #endif
  }
  Path(buffer);

  for (int i=1;i<argc;i++) {
    if (!strcmp(argv[i],"-c")) {
      if (argc>i+1)
	cfg(argv[i+1]);
    }
  }

}

void Path(const char *p) {
  if (apppath!=NULL) delete apppath;
  apppath=new char [strlen(p)+2];
  strcpy(apppath,p);
  #ifdef WINDOWS
    if (apppath[strlen(p)-1]!='\\') 
      strcat(apppath,"\\");
  #else
    if (apppath[strlen(p)-1]!='/')
      strcat(apppath,"/");
  #endif

}

char *Path() {
  return apppath;
}

void cfg(const char *fname) {

  #ifdef WINDOWS
    if (Path()==NULL) Path(".\\");
  #else
    if (Path()==NULL) Path("./");
  #endif
  if (cfgfile!=NULL) delete cfgfile;
  if (fname[0]=='/') {
    cfgfile=new char [strlen(fname)+1];
    strcpy(cfgfile,fname);
  } else {
    cfgfile=new char [strlen(apppath)+strlen(fname)+1];
    sprintf(cfgfile,"%s%s",Path(),fname);
  }

}

char *cfg() { 
  return cfgfile;
 }

int Readln(FILE *f,char *buffer,int bsize) {
  int result;
  int i=0;
  char c;
  
  do {
    result=fscanf(f,"%c",&c);
    if ((result!=EOF) && (c!='\n')) {
      buffer[i++]=c;
      buffer[i]=0;
    }
  } while ((result!=EOF) && (c!='\n') && (i<bsize));

  return result;
  
}

static char tvalue[8192];

const char* GetValue(FILE *f,const char *section,const char *arg) {
  int result;
  char buffer[2049];
  char earg[2049];
  char *value;
  
  rewind(f);

  //--- search section -----------------------------------------
  sprintf(earg,"[%s]",section);
  do {
    result=Readln(f,buffer,2048);
    if (result==EOF) strcpy(buffer,"");
  } while ((result!=EOF) && (strstr(buffer,earg)!=buffer));

  if (strstr(buffer,earg)!=buffer) return "";

  //--- search entry ------------------------------------------
  sprintf(earg,"%s=",arg);
  do {
    result=Readln(f,buffer,2048);
  } while ((strncmp(buffer,earg,strlen(earg))) && (result!=EOF) && (buffer[0]!='['));

  if (!strncmp(buffer,earg,strlen(earg))) {
    value=strtok(buffer,"=");
    value=strtok(NULL,"=");  

    strcpy(tvalue,value);

    //--- need to take care on CR/LF !!!


    return tvalue;
  }

  return "";
}

const char* GetValue(const char *cfgname,const char *section,const char *arg) {

  FILE *cfg;
  const char *rval;

  cfg=fopen(cfgname,"r");
  if (cfg==NULL) {
    printf("cfgtools.GetValue: Initialisierungsdatei '%s' nicht gefunden.\n",cfgname);
    return "";
  }

  rval=GetValue(cfg,section,arg);
  fclose(cfg);

  return rval;

}

const char* GetValue(const char *section,const char *arg) {
  if (cfgfile!=NULL){
    return GetValue(cfgfile,section,arg);
  }
  return NULL;
}

int GetIntValue(const char *section,const char *arg) {
  char buffer[4096];
  int x;
  if (cfgfile!=NULL) {
    strcpy(buffer,GetValue(cfgfile,section,arg));
    if (strlen(buffer)>0) {
      sscanf(buffer,"%d",&x);
      return x;
    }
  }

  return 0;
}

float GetFloatValue(const char *section,const char *arg) {
  char buffer[4096];
  float x;
  if (cfgfile!=NULL) {
    strcpy(buffer,GetValue(cfgfile,section,arg));
    if (strlen(buffer)>0) {
      sscanf(buffer,"%f",&x);
      return x;
    }
  }

  return 0;
}

double GetDoubleValue(const char *section,const char *arg) {
  char buffer[4096];
  float x;
  if (cfgfile!=NULL) {
    strcpy(buffer,GetValue(cfgfile,section,arg));
    if (strlen(buffer)>0) {
      sscanf(buffer,"%f",&x);
      return (double)x;
    }
  }

  return 0;
}

long int FILESIZE(const char *fname) {
  struct stat status;

  stat(fname,&status);
  return status.st_size;
}

// append DataDir on the left side


static char fpath[8192];

const char* DataFilename(const char* path) {
  strcpy(fpath,GetValue("General","DataDir"));
  strcat(fpath,"/");
  strcat(fpath,path);

  return fpath;
}
