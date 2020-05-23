

Intro
------

This is a simple file compressor / decompressor built on zlib for any type of dot file i.e. it cannot compress directories. 

Once in the directory of the repo, you can run the command main <filename> <mode> (where main in the executable) to start the program. 
Mode must also be specified by "compress" or "decompress". 

One of the limitations of the program is that the maximum file size that can be inputted in the program is 1GB. This is because this is the maximum size of arrays in C. 

File Structure
---------------

1. main.c - this includes the main function, which is the core of the compressor/decompressor and calls functions that are actually used for compression. 
2. zlibfunc.c / zlibfunc.h - the .c file holds all the methods for the actual compression / decompression of the file 
	- compressMain / decompressMain are the driver functions for either compression or decompression of the file. 
3. Makefile - compiles and links all the files together to create the program. 

The way that both the compression and decompression algorithms work are that they first read the whole file to determine the number of bytes in the file. 
This is done so that it can dynamically allocate the exact numner of bytes for the data buffers, and minimises the wasted space. 

The reason that it does this before writing anything to the buffer is that it is more efficient to allocate larger buffer sizes for the input and output. 
That being said, there is a time limitation to this method as it must iterate through the data 2 * N times, which for values approaching the 1GB limit would take a much longer time. 

A way to rememdy this is if there are a set domain of values of file sizes, to pre-determine a chunk that will be a rough average for all values in order to mimimise wasted memory allocation in buffer. 

Testing
--------

There are a few files in the directory that I used to test: hello.txt, cyborg.html (this is a wget of your website), file.txt (a dummy file with 1GB of test)

To test just the compression algorithm: openssl zlib -d < hello.txt.z

To test the decompression algorithm, I created a copy of the original file e.g. file.txt , created a compressed version (file.txt.z) and then decompressed it back to file.txt. 
I then ran the command: 
	cmp --silent <compressed/decompressed file> <_original_file_copy> || echo "files are different"

If there was a print to the screen then the decompressed algorithm was not working correctly. 

