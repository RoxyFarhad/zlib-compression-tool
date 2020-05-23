#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include "zlibfunc.h"

static void die(const char* message) {
	perror(message);
	exit(1); 
}

int main(int argc, char **argv) {

	printf("\n~~~~~~~ Loading Program ~~~~~~~\n\n"); 

	if(argc != 3){
		fprintf(stderr, "usage: main <filename> <mode> \nMode can be either 'compress' or 'decompress'\n\n"); 
		exit(1); 
	}

	char *filename = argv[1]; 
	char mode[strlen(argv[2]) + 1]; 
	strcpy(mode, argv[2]); 

	if(strcmp(mode, "compress") != 0 && strcmp(mode, "decompress") != 0){
		fprintf(stderr, "Wrong Mode entered. Please enter 'compress' or 'decompress'\n\n"); 
		exit(1); 
	}
	printf("filename: %s\n", filename); 
	printf("mode: %s\n", mode);

	FILE *fIn = fopen(filename, "rb");
	if (fIn == NULL) {
		die(filename);
	} 

	// check its not a directory 
	int status; 
	struct stat st_buf; 
	status = stat (filename, &st_buf); 
	if (status != 0){
		fprintf(stderr, "Error: %d\n", errno);
	}
	if (S_ISDIR (st_buf.st_mode)) {
        fprintf (stderr, "%s is a directory. This program is not equipped for directories. \n", filename);
        fprintf(stderr, "usage: main <filename> <mode>\n\n"); 
        exit(1); 
    }

	// write to a file that has the same name before .ext and add .comp
	char *fileOutName; 
	char *ext; 
	if(strcmp(mode, "compress") == 0){

		// generates the output file name from the ext and the filename before the ext
		char tempFile[strlen(filename)];
		strcpy(tempFile, filename); 

		if ((ext = strstr(filename, ".")) == NULL || (fileOutName = strtok(tempFile, ".")) == NULL){
			fprintf(stderr, "Invalid file type. Please input a '.' file \n\n");
			exit(1); 
		}

		char outFileName[strlen(fileOutName) + strlen(ext) + 2]; 
		sprintf(outFileName, "%s%s.z", fileOutName, ext); 

		compressMain(fIn, outFileName);
	} 
	else if(strcmp(mode, "decompress") == 0) {

		// generates the output file name from the ext and the filename before the ext
		char tempFile[strlen(filename)];
		strcpy(tempFile, filename); 

		if ((ext = strstr(filename, ".")) == NULL || (fileOutName = strtok(tempFile, ".")) == NULL){
			fprintf(stderr, "Invalid file type. Please input a '.' file \n\n");
			exit(1); 
		}
		ext = strtok(ext, "."); 
		char outFileName[strlen(fileOutName) + strlen(ext)]; 
		sprintf(outFileName, "%s.%s", fileOutName, ext); 
		decompressMain(fIn, outFileName);
	}

	return 0; 
}