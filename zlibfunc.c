#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "zlib.h"

#define COMP_MODE 0
#define DECOMP_MODE 1  

static void die(const char* message) {
	perror(message);
	exit(1); 
}


static void zerr(int ret) {

	fprintf(stderr, "\n\n ~~~~~~~~ ERROR PROCESSING FILE ~~~~~~~ \n\n");
	fprintf(stderr, "zpipe: ");
	switch(ret) {

		case Z_ERRNO:
			if (ferror(stdin)) 
				fprintf(stderr, "Error reading stdin\n");
			if (ferror(stdout))
				fprintf(stderr, "Error writing stdout\n");
		case Z_STREAM_ERROR:
			fprintf(stderr, "Invalid Compresion level supplied.\n"); 
		case Z_DATA_ERROR:
			fprintf(stderr, "The deflate data is invalid.\n");
		case Z_VERSION_ERROR:
			fprintf(stderr, "The version of zlib.h and linked library do not match. \n");
		case Z_MEM_ERROR:
			fprintf(stderr, "Memory could be allocated for the process. \n");
	}

	exit(1); 

}

static int readBytes (FILE* input) {
/* This reads the number of bytes in the file that will then be allocated to CHUNK
 *
 */

	int bytes = 0; 
	for(bytes = 0; getc(input) != EOF; ++bytes); 

	if (bytes == 0) {
		fprintf(stderr, "Read 0 bytes from the file. Cannot compress an empty file."
			"Please try again with another file.\n");
		exit(1); 
	}

	printf(" ~~~~~~ Size of File: %d Bytes ~~~~~~~ \n\n", bytes); 
	return bytes; 
}


static int init (z_stream *strm, int mode) {

	int ret; 

	strm->zalloc = Z_NULL; 
	strm->zfree = Z_NULL; 
	strm->opaque = Z_NULL; 

	if(mode == COMP_MODE) {

		ret = deflateInit(strm, Z_DEFAULT_COMPRESSION); 

	} else if(mode == DECOMP_MODE) {

		strm->avail_in = 0; 
		strm->next_in = Z_NULL; 
		ret = inflateInit(strm); 
	}

	if(ret != Z_OK) {
		zerr(ret); 
	} 

	return ret; 

}


int compressMain(FILE* input, char* fileOutput) {
	
	int bytes = readBytes(input); 
	fseek(input, 0L, SEEK_SET);

	z_stream strm; 
	int ret = init(&strm, 0); 

	unsigned char in[bytes], out[bytes]; 

	strm.avail_in = fread(in, sizeof(char), bytes, input); 

	if (ferror(input)) {
		(void)deflateEnd(&strm); 
		zerr(Z_ERRNO); 
	} else if (strm.avail_in != bytes) {
		fprintf(stderr, "\nDid not correctly read the number of bytes. Please re-run the program. \n");
		exit(1); 
	}

	strm.next_in = in; 

	// at this point have correctly read all the bytes -- ready to write to file
	FILE *fOut = fopen(fileOutput, "wb"); 
	if (fOut == NULL) {
		die(fileOutput);
	}

	do {

		strm.avail_out = bytes; 
		strm.next_out = out; 
		ret = deflate(&strm, Z_FINISH); 
		assert(ret != Z_STREAM_ERROR); 
		int rem = bytes - strm.avail_out; 
		if(fwrite(out, sizeof(char), rem, fOut) != rem || ferror(fOut)) {
			(void)deflateEnd(&strm); 
			zerr(Z_ERRNO); 
		}

	} while (strm.avail_out == 0); 

	deflateEnd(&strm); 

	printf("~~~~~~~~~ Finished File Compression! ~~~~~~~ \n\n");
	printf("It is stored in: %s\n\n", fileOutput); 

	return 0; 	

}


int decompressMain(FILE* input, char *fileOutput){

	int bytes = readBytes(input);
	fseek(input, 0L, SEEK_SET);

	z_stream strm; 
	int ret = init(&strm, 1); 
	unsigned char in[bytes], out[bytes]; 

	strm.avail_in = fread(in, sizeof(char), bytes, input); 

	if (ferror(input)) {
		(void)inflateEnd(&strm); 
		zerr(Z_ERRNO); 
	} else if (strm.avail_in != bytes) {
		fprintf(stderr, "\nDid not correctly read the number of bytes. Please re-run the program. \n");
		exit(1); 
	}
	
	strm.next_in = in; 

	FILE *fOut = fopen(fileOutput, "wb"); 
	if (fOut == NULL) {
		die(fileOutput);
	}

	do {

		strm.avail_out = bytes; 
		strm.next_out = out; 
		ret = inflate(&strm, Z_FINISH); 
		if(ret == Z_NEED_DICT || ret == Z_DATA_ERROR){ zerr(Z_DATA_ERROR); }
		else if(ret == Z_MEM_ERROR){ zerr(Z_MEM_ERROR); }

		int rem = bytes - strm.avail_out; 
		if(fwrite(out, sizeof(char), rem, fOut) != rem || ferror(fOut)) {
			(void)deflateEnd(&strm); 
			zerr(Z_ERRNO); 
		}	

	} while (strm.avail_out == 0); 

	deflateEnd(&strm); 

	printf("~~~~~~~~~ Finished File Compression! ~~~~~~~\n\n");
	printf("It is stored in: %s\n\n", fileOutput); 

	return 0; 

}	



