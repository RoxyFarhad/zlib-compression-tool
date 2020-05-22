#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include "zlibfunc.h"

static void die(const char* message) {
	perror(message);
	exit(1); 
}

int main(int argc, char **argv) {

	printf("\n ~~~~~~~ Loading Program ~~~~~~~ \n\n"); 

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
	printf("mode: %s\n\n", mode);

	FILE *fIn = fopen(filename, "rb");
	if (fIn == NULL) {
		die(filename);
	}

	// write to a file that has the same name before .ext and add .comp
	char *fileOutName; 
	if(strcmp(mode, "compress") == 0){

		if ((fileOutName = strtok(filename, ".")) == NULL){
			fprintf(stderr, "Invalid file type. Please input a '.' file \n\n");
			exit(1); 
		}
		strcat(fileOutName, ".txt.z");
		compressMain(fIn, fileOutName);
	} 
	else if(strcmp(mode, "compress")) {

		if ((fileOutName = strtok(filename, ".")) == NULL){
			fprintf(stderr, "Invalid file type. Please input a '.'' file \n\n");
			exit(1); 
		}

		strcat(fileOutName, ".txt");
		decompressMain(fIn, fileOutName);
	}
	
	return 0; 
}