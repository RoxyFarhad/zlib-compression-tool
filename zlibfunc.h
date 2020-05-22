#ifndef __ZLIBFUNC__
#define __ZLIBFUNC__

#include "zlib.h"

static int readBytes (FILE* input); 
int compressMain(FILE* input, char* fileOutput); 
static int init (z_stream *strm, int mode); 
static void zerr(int ret); 
int decompressMain(FILE *input, char *fileOutput); 

#endif 