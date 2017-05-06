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

void externalSort(int fdInput, uint64_t size, int fdOutput, uint64_t memSize);
const char* getFileName(unsigned mRunCount, unsigned mK);