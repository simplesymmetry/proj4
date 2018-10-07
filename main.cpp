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

int main(int argc, char *argv[]) {
    size_t bufferSize = 1024;
    int mmapMode;

    if (argc == 4){
        mmapMode = 1;
    }

    if (argc == 5){
        bufferSize = (size_t) argv[3];
        if (bufferSize >= 8912){
            std::cout << "Buffer size too large, defaulting to 1024 bytes.\n" << std::endl;
            bufferSize = 1024;
        }
        mmapMode = 1;
    }
    char buffer[bufferSize];

    ssize_t result = 0;
    char * filename = argv[1];
    std::string searchString = argv[2];
    ssize_t fileSize;
    struct stat sb = {};
    int count;
    char *pch;

    int fd;
    ssize_t bytesRead;

    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << std::endl;
    }

    if (fstat(fd, &sb) < 0){
        std::cerr << "Could not stat" << std::endl;
    }
    else if ((fd = open(filename, O_RDONLY)) < 0) {
        std::cerr << "Cannot open: " << filename << std::endl;
        exit(1);
    }

    int counter = 0;
    int pos = 0;
    int i = 0;
    while ((bytesRead = read(fd, buffer, bufferSize)) > 0)
    {
        for (pos = 0; pos < bufferSize; pos++) {
            if (searchString[i] == buffer[pos]) {
                i++;
                   if (i == searchString.length()){
                        counter++;
                        i = 0;
                   }
            }
        }
    }

    fileSize = stat(argv[1], &sb);
    result = counter;

    if (mmapMode) {
        if (fstat(fd, &sb) < 0) {
            std::cerr << "Could not stat." <<  std::endl;
            exit(1);
        } else if ((pch = (char *) mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0)) == (char *) -1) {
            std::cerr << "Could not map file to memory." << std::endl;
            exit(1);
        }
        while ((pch = (char *) mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0)) == (char *) -1){
            for (pos = 0; pos < bufferSize; pos++) {
                if (searchString[i] == buffer[pos]) {
                    i++;
                    if (i == searchString.length()){
                        counter++;
                        i = 0;
                    }
                }
            }
        }

        if (munmap(pch, sb.st_size) < 0) {
            std::cerr << "Could not unmap memory" << std::endl;
            exit(1);
        }
    }

    std::cout << "Occurrences of the string \"" << searchString << "\" in " << filename << " is " << result << "." << std::endl;
    std::cout << "Size of the file is " << sb.st_size << " bytes. " << std::endl;

    if (fd > 0) {
        close(fd);
    }
}