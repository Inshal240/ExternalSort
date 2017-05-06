#include <iostream>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>

int externalSort(int fdInput, int fdOutput, uint64_t memSize, unsigned k);
const char* getFileName(unsigned runCount, unsigned k);
int testExternalSort(int fileToTest);