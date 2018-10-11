/* Project 4 - CS3013 - WPI
 * A program to implement different I/O methods to compare them.
 * Copyright 2018. Thomas Graham
 */

#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

/**
 * This function is used to map the contents of the file to memory.
 * @param fd the file descriptor
 * @param searchString the search string to be counted
 * @param bufferSize the size of the buffer to use
 * @param filename the name of the file
 * @return the count
 */
int mmapM(int fd, std::string searchString, size_t bufferSize, const char *filename){
    struct stat sb = {};
    char *pch;
    int pos = 0;
    int counter = 0;
    int i = 0;
    ssize_t bytesRead;

    char buffer[bufferSize];
    if ((fd = open(filename, O_RDONLY)) < 0) {
        std::cerr << "Could not open file" << std::endl;
        exit (1);
    }
    if (fstat(fd, &sb) < 0) {
            std::cerr << "Mmap Mode: Could not stat." <<  std::endl;
            exit(1);
        }

    if ((pch = (char *) mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0)) != (char *) -1) {
        while ((bytesRead = read(fd, buffer, bufferSize)) > 0) {
            for (pos = 0; pos < bufferSize; pos++) {
                if (searchString[i] == buffer[pos]) {
                    i++;
                    if (i == searchString.length()) {
                        counter++;
                        i = 0;
                    }
                }
            }
        }
    }

    if (munmap(pch, sb.st_size) < 0) {
        std::cerr << "Mmap Mode: Could not unmap memory" << std::endl;
        exit(1);
    }
    return counter;
}

int main(int argc, char *argv[]) {

    // Setting up variables
    int bufferSize = 1024;
    int mmapMode = 0;
    struct stat sb = {};
    std::string input;

    // Checking the input
    if (argc == 4){
        input = argv[3];
        if (input == "mmap"){
            mmapMode = 1;
        }
        else if (atoi(input.c_str()) != 0){
            bufferSize = atoi(input.c_str());
        }
    }

    if (argc == 5){
        bufferSize = atoi(argv[4]);
        if (bufferSize >= 8912){
            bufferSize = 1024;
        }
        mmapMode = 1;
    }

    // Setting up more variables
    char buffer[bufferSize];
    ssize_t result = 0;
    const char * filename = argv[1];
    std::string searchString = argv[2];
    ssize_t fileSize;
    ssize_t bytesRead;
    int fd, i, counter, pos = 0;

    // Checking for errors
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << std::endl;
        exit(1);
    }
    else if ((fd = open(filename, O_RDONLY)) < 0) {
        std::cerr << "Cannot open: " << filename << std::endl;
        exit(1);
    }

    /* If mmap is toggled then this will call the function and return result */
    if (mmapMode){
        result = mmapM(fd, searchString, bufferSize, filename);
    }
    else {
        while ((bytesRead = read(fd, buffer, bufferSize)) > 0) {
            for (pos = 0; pos < bufferSize; pos++) {
                if (searchString[i] == buffer[pos]) {
                    i++;
                    if (i == searchString.length()) {
                        counter++;
                        i = 0;
                    }
                }
            }
            result = counter;
        }
    }

    fileSize = stat(argv[1], &sb);

    // Setting up output
    std::cout << "File size: " << sb.st_size << " bytes. " << std::endl;
    std::cout << "Occurrences of the string " << searchString << " in " << filename << " is " << result << "." << std::endl;
    std::cout << "Buffer size is " << bufferSize << " bytes. " << std::endl;
    if (mmapMode){
        std::cout << "Mmap Mode is on." << std::endl;
    }

    if (fd > 0) {
        close(fd);
    }
}