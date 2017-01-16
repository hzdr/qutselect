#ifndef CFGTOOLS_
#define CFGTOOLS_

#include <stdio.h>
#include <stdlib.h>

extern void ParseCommandline(int argc,char* argv[]);

extern void Path(const char *p);
extern char *Path();

extern void cfg(const char *fname);
extern char *cfg();

extern const char* GetValue(FILE *f,const char *section,const char *arg);
extern const char* GetValue(const char *cfgname,const char *section,const char *arg);
extern const char* GetValue(const char *section,const char *arg);

#define CFGVALUE(X,S,A) strcpy(X,GetValue(S,A));

extern int GetIntValue(const char *section,const char *arg);
extern float GetFloatValue(const char *section,const char *arg);
extern double GetDoubleValue(const char *section,const char *arg);

extern long int FILESIZE(const char *fname);
extern int Readln(FILE *f,char *buffer,int bsize=1024);

#endif
