#include "externalsort.h"

using namespace std;

int main(int argc, char* argv[])
{
//	std::cout << "Hello world" << std::endl;
//	getchar();

	if (argc != 3)
	{
		cerr << "usage: " << argv[0] << " <k for k-way sorting> <input filename>" << endl;
		return -1;
	}

	unsigned k = (unsigned) atoi(argv[1]);

	// possible values of k = 2, 4, 8
	if (k != 2 && k != 4 && k != 8)
	{
		cerr << "possible values for k: 2, 4, 8";
	}

	int inputFile, runFiles[k];
    const int memSize = 16;	// TODO: Make this a #define
    int ret;				// Stores return value of read/write operations

    // Variables for runs
    uint64_t arr[memSize];
    unsigned arrCount;		// Valid entries in the array
    unsigned runCount = 0;	// Total number of runs

    // Open files
	if ((inputFile = open(argv[2], O_RDONLY, S_IRUSR | S_IWUSR)) < 0)
    {
		cerr << "cannot open file '" << argv[2] << "': " << strerror(errno) << endl;
		return -1;
	}

	// Get pointers for all output files
	for (unsigned i = 0; i < k; ++i)
	{
		if((runFiles[i] = open(getFileName(i, k), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)) < 0)
		{
			cerr << "cannot open file '" << "runOutput" << (i % k) << "': " << strerror(errno) << endl;
            return -1;
		}
		// else
		// {
		// 	cout << "created output file " << getFileName(i, k) << endl;
		// }
	}

	// Read and sort input data into runs
    while ((ret = read(inputFile, arr, sizeof(uint64_t) * memSize)) > 0)
    {
    	// Get number of elements read from file
    	arrCount = ret / sizeof(uint64_t);

    	// Sort elements in array
    	sort(arr, &arr[arrCount]);;
        
        cout << "Writing data to output file " << getFileName(runCount, k) << ".\n";

        // Write sorted run to file
        if (pwrite(runFiles[(runCount % k)], &arr[arrCount], sizeof(uint64_t) * arrCount, 
        	sizeof(uint64_t) * memSize * runCount) < 0)
        {
            cout << "error writing to " << "runOutput" << (runCount % k) << "': " << strerror(errno) << endl;
            return -1;
        }

        // DEBUG: Print to console
        for (unsigned i = 0; i < arrCount; ++i)
        {
        	cout << (runCount * memSize + i) << ")\t" << arr[i] << endl;
        }
    	
        runCount++;
    }

    // Check ret for errors while reading
    if (ret < 0)    // Error when reading
    {
        cout << "error reading from " << argv[1] << ": " << strerror(errno) << endl;
        close(inputFile);
        return -1;
    }

	// EOF reached. All blocks read and sorted into runs.
	close(inputFile);




    // Merge sort files
	const unsigned partSize = memSize / k;	// partition size of arr for loading runs
	unsigned arrIndice[k];	// stores starting index of each partition
	int rets[k];			// stores return values for each file read

	for (unsigned i = 0; i < k; ++i)
	{
		arrIndice[i] = i * partSize;

		// reset file pointers
		if(lseek(runFiles[i], 0, SEEK_SET) < 0)
		{
			cout << "error moving offset for runOutput" << (i % k) << ": " << strerror(errno) << endl;
			return -1;
		}

		if ((rets[i] = read(runFiles[i], &arr[arrIndice[i]], sizeof(uint64_t) * partSize)) < 0)
		{
			cout << "error reading from " << "runOutput" << (runCount % k) << "': " << strerror(errno) << endl;
		}
	}

	// Load runs into memory

	// Load runs into memory, sort and write to final output file
	// while(1)
	// {
		// Write to file

		// See if more
	// }

	// Close and delete extra output files created by the sorting phase
	for (unsigned i = 0; i < k; ++i)
	{
		close(runFiles[i]);

		if(unlink(getFileName(i, k)) < 0)
		{
			cerr << "cannot delete file '" << "runOutput" << (i % k) << "': " << strerror(errno) << endl;
            return -1;
		}
	}

	return 0;
}

void externalSort(int fdInput, uint64_t size, int fdOutput, uint64_t memSize)
{

}

// Generates file name for a specific run
const char* getFileName(unsigned mRunCount, unsigned mK)
{
	char numstr[21];
    sprintf(numstr, "%d", mRunCount % mK);

	string fileName = string("runOutput") + string(numstr);

    return fileName.c_str();
}