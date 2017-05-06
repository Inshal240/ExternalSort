# ExternalSort
> K-way merge sort with k = 2, 4, 8
> Interface changed to "int externalSort(int fdInput, int fdOutput, uint64_t memSize, unsigned k)"
> Function used to test ascending output file is "int testExternalSort(int fileToTest)"
> Output of externalSort is always written to "outputFile"
> externalsort can be run via the command line using the command syntax "./externalsort <k> <input file name>"