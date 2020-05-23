#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "zlib.h"

#define COMP_MODE 0
#define DECOMP_MODE 1  

static void die(const char* message) {
	perror(message);
	exit(1); 
}


/* static void zerr(int ret)
* Writes the error codes associated with the zlib functions
*/

static void zerr(int ret) {

	fprintf(stderr, "~~~~~~~~ ERROR PROCESSING FILE ~~~~~~~\n");
	fprintf(stderr, "zpipe: ");

	if(ret == Z_ERRNO) {
		if (ferror(stdin)) 
			fprintf(stderr, "Error reading stdin\n");
		if (ferror(stdout))
			fprintf(stderr, "Error writing stdout\n");
	}
	else if(ret == Z_STREAM_ERROR)
		fprintf(stderr, "Invalid Compresion level supplied.\n"); 
	else if(ret == Z_DATA_ERROR)
		fprintf(stderr, "The deflate data is invalid.\n");
	else if(ret == Z_VERSION_ERROR) 
		fprintf(stderr, "The version of zlib.h and linked library do not match. \n");
	else if(ret == Z_MEM_ERROR)
		fprintf(stderr, "Memory could be allocated for the process. \n");
	fprintf(stderr, "\n");
	exit(1); 

}

/*
* static int readBytes (FILE* input)
* Reads the number of bytes in the file that will then be allocated to in/out buffers
*/

static int readBytes (FILE* input) {

	int bytes = 0; 
	for(bytes = 0; getc(input) != EOF; ++bytes); 

	if (bytes == 0) {
		fprintf(stderr, "Read 0 bytes from the file. Please try again with another file.\n");
		exit(1); 
	}

	return bytes; 
}

/*
* static int init (z_stream *strm, int mode) 
* initialises the zstream - mode = 0: init for compresssion, mode = 1: init for decompression
* if the strm is not ready, it will call the error function and the program quits. 
*/

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

/* int compressMain(FILE* input, char* fileOutput)
* Main function for compression
*/

int compressMain(FILE* input, char* fileOutput) {

	clock_t start, end; 
	double cpu_time_used; 
	start = clock(); 

	z_stream strm; 
	int ret = init(&strm, 0); 

	int bytes = readBytes(input);
	printf("Size of File: %d Bytes\n\n", bytes); 

	fseek(input, 0L, SEEK_SET);

	unsigned char* in = malloc(1 * bytes); 
	unsigned char* out = malloc(1 * bytes); 

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

	float newBytes = 0;  // the # of bytes in compressed file

	// writing the bytes from input to the output using the inflate function 
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
		newBytes += rem; 

	} while (strm.avail_out == 0); 

	(void)deflateEnd(&strm); 

	end = clock(); 
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC; 

	printf("~~~~~~~~~ Finished File Compression! ~~~~~~~ \n\n");
	printf("Time taken: %f ms\n", cpu_time_used*1000);
	printf("Stored in: %s\n", fileOutput); 

	printf("Percentage change in file size: %f \n\n", (newBytes - bytes)/bytes * 100);
	free(in); 
	free(out); 

	return 0; 	

}

/* int decompressMain(FILE* input, char* fileOutput)
* Main function for decompresseion (v similar to compression except for inflate instead of deflate)
*/

int decompressMain(FILE* input, char *fileOutput){

	clock_t start, end; 
	double cpu_time_used; 
	start = clock(); 

	z_stream strm; 
	int ret = init(&strm, 1); 

	int bytes = readBytes(input);
	printf("Size of File: %d Bytes\n\n", bytes); 

	fseek(input, 0L, SEEK_SET);

	unsigned char* in = malloc(1 * bytes); 
	unsigned char* out = malloc(1 * bytes); 

	strm.avail_in = fread(in, sizeof(char), bytes, input); 

	if (ferror(input)) {
		(void)inflateEnd(&strm); 
		zerr(Z_ERRNO); 
	} else if (strm.avail_in != bytes) {
		fprintf(stderr, "\nDid not correctly read the number of bytes. Please re-run the program. \n");
		exit(1); 
	}
	
	strm.next_in = in; 

	FILE *fOut = fopen(fileOutput, "wb+"); 
	if (fOut == NULL) {
		die(fileOutput);
	}

	float newBytes = 0; // the # of bytes in decompressed file

	// writing the bytes from input to the output using the inflate function 
	do {

		strm.avail_out = bytes; 
		strm.next_out = out; 
		ret = inflate(&strm, Z_FINISH); 
		if(ret == Z_NEED_DICT || ret == Z_DATA_ERROR){ zerr(Z_DATA_ERROR); }
		else if(ret == Z_MEM_ERROR){ zerr(Z_MEM_ERROR); }

		int rem = bytes - strm.avail_out; 
		if(fwrite(out, sizeof(char), rem, fOut) != rem || ferror(fOut)) {
			(void)inflateEnd(&strm); 
			zerr(Z_ERRNO); 
		}	
		newBytes += rem; 

	} while (strm.avail_out == 0); 

	(void)inflateEnd(&strm); 

	end = clock(); 
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC; 

	printf("~~~~~~~~~ Finished File Decompression! ~~~~~~~\n\n");
	printf("Time taken: %f ms\n", cpu_time_used*1000);
	printf("Stored in: %s\n", fileOutput); 

	printf("Percentage change in file size: %f \n\n", (newBytes - bytes)/bytes * 100);

	free(in); 
	free(out); 

	return 0; 

}	



