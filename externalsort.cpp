#include "externalsort.h"

// #define DEBUG
#define MEM_SIZE 4096

using namespace std;

int main(int argc, char* argv[])
{
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

	int inputFile, outputFile;
    //const int memSize = 4096;	// TODO: Make this a #define

    // Open input file
	if ((inputFile = open(argv[2], O_RDONLY, S_IRUSR | S_IWUSR)) < 0)
    {
		cerr << "cannot open file '" << argv[2] << "': " << strerror(errno) << endl;
		return -1;
	}

	// check file size
	unsigned fileSize = 0;

	if ((fileSize = lseek(inputFile, 0, SEEK_END)) < 0)
	{
		cerr << "error moving offset for output file: " << strerror(errno) << endl;
		return -1;
	}

	#ifdef DEBUG
	cout << "input file size is " << fileSize << " bytes." << endl;
	#endif

	// restore input file offset
	if (lseek(inputFile, 0, SEEK_SET) < 0)
	{
		cerr << "error moving offset for output file: " << strerror(errno) << endl;
		return -1;
	}

	// prevent execution if file size is not divisible by 8
	assert((fileSize % sizeof(uint64_t)) == 0);

	// Delete output file if it exists
	if (unlink("outputFile") < 0 && errno != ENOENT)
	{
		cerr << "error deleting existing output file: " << strerror(errno) << endl;
		return -1;
	}

    // Open output file
	if ((outputFile = open("outputFile", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)) < 0)
	{
		cerr << "cannot open file outputFile: " << strerror(errno) << endl;
        return -1;
	}

    int ret = externalSort(inputFile, outputFile, MEM_SIZE, k);

    if (lseek(outputFile, 0, SEEK_SET) < 0)
    {
    	cerr << "error moving offset for output file: " << strerror(errno) << endl;
		return -1;
    }

    // Test the output
    if(ret > -1)
    	testExternalSort(outputFile);

	// close files
	close(inputFile);
	close(outputFile);

	return ret;
}

int externalSort(int inputFile, int outputFile, uint64_t memSize, unsigned k)
{
	// Make sure memory can be evenly divided by k
	assert(memSize > k && (memSize % k) == 0);

	int runFiles[k];		// Pointers to temp run files
    int ret;				// Stores return value of read/write operations

    // Variables for indivdiual runs
    uint64_t arr[memSize];
    unsigned arrCount;		// Valid entries in the array
    unsigned runCount = 0;	// Total number of runs

	// Get pointers for all run output files
	for (unsigned i = 0; i < k; ++i)
	{
		if((runFiles[i] = open(getFileName(i, k), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)) < 0)
		{
			cerr << "cannot open file '" << "runOutput" << (i % k) << "': " << strerror(errno) << endl;
            return -1;
		}
		#ifdef DEBUG
		else
		{
			cout << "created output file " << getFileName(i, k) << endl;
		}
		#endif
	}

	// Read and sort input data into runs
    while ((ret = read(inputFile, arr, sizeof(uint64_t) * memSize)) > 0)
    {
    	// Get number of elements read from file
    	arrCount = ret / sizeof(uint64_t);

    	// Sort elements in array
    	sort(arr, &arr[arrCount]);;
        
        #ifdef DEBUG
        cout << "Writing data to output file " << getFileName(runCount, k) << ".\n";
        #endif

        // Write sorted run to file
        if (pwrite(runFiles[(runCount % k)], arr, sizeof(uint64_t) * arrCount, 
        	sizeof(uint64_t) * memSize * (runCount / 2)) < 0)
        {
            cout << "error writing to " << "runOutput" << (runCount % k) << "': " << strerror(errno) << endl;
            return -1;
        }

		#ifdef DEBUG
        for (unsigned i = 0; i < arrCount; ++i)
        {
        	cout << (runCount * memSize + i) << ")\t" << arr[i] << endl;
        }
        #endif
    	
        runCount++;
    } // end of run sorting while loop

    // Check ret for errors while reading
    if (ret < 0)    // Error when reading
    {
        cout << "error reading from input file: " << strerror(errno) << endl;
        return -1;
    }

	// EOF reached. All blocks read and sorted into runs.
	if (runCount == 0)
	{
		cout << "no data read from file. possible error." << endl;
		return -1;
	}

    // Merge sort files
	const unsigned partSize = memSize / k;	// partition size of arr for loading runs
	unsigned arrPartIndice[k];	// stores starting index of each partition
	unsigned arrAccIndice[k];	// stores index of each partition used to access elements
	int rets[k];				// stores return values for each file read

	#ifdef DEBUG
	cout << "merge sort phase" << endl;
	cout << "partSize = " << partSize << endl;
	#endif

	// Initialize all the variables
	for (unsigned i = 0; i < k; ++i)
	{
		arrPartIndice[i] = i * partSize;
		arrAccIndice[i] = arrPartIndice[i];

		// reset file pointers
		if((ret = lseek(runFiles[i], 0, SEEK_SET)) < 0)
		{
			cerr << "error moving offset for runOutput" << (i % k) << ": " << strerror(errno) << endl;
			return -1;
		}
		#ifdef DEBUG
		else
		{
			cout << "File offset set to " << ret << endl;

			if((ret = pread(runFiles[i], arr, sizeof(uint64_t), 0)) < 0)
			{
				cout << "error reading from 'runOutput" << i << "': " << strerror(errno) << endl;
				return -1;
			}
			else
			{
				cout << "First element of runOutput" << i << " is " << arr[0]
				<< " with return value " << ret << endl;
			}
		}
		#endif

		// load elements from file into its partition
		if ((rets[i] = read(runFiles[i], &arr[arrPartIndice[i]], sizeof(uint64_t) * partSize)) < 0)
		{
			cout << "error reading from 'runOutput" << i << "': " << strerror(errno) << endl;
			return -1;
		}
		#ifdef DEBUG
		else
		{
			cout << "read " << rets[i] / sizeof(uint64_t) << " elements from runFile" << i << endl;
			cout << "arrAccIndice = " << arrAccIndice[i] << endl;
		}
		#endif
	}

	bool runsRemaining = true;
	
	#ifdef DEBUG
	unsigned elementCounter = 0;
	#endif

	// Merge loop
	while(runsRemaining)
	{
		// Find minimum
		uint64_t minimum = UINTMAX_MAX;	// stores minimum element
		unsigned indexOfMin = -1;		// stores index of which arr it was found in

		for (unsigned i = 0; i < k; ++i)
		{
			// rets is used to make sure there entries in the partition
			// arrAccIndice[i] < (rets[i] / sizeof(uint64_t) is used to ensure
			// arrAccIndice[i] is accessing a valid index
			if (rets[i] > 0 && (arrAccIndice[i] - arrPartIndice[i]) < (rets[i] / sizeof(uint64_t))
				&& arr[arrAccIndice[i]] <= minimum)
			{
				minimum = arr[arrAccIndice[i]];
				indexOfMin = i;
			}
		}

		// write minimum to file
		if(write(outputFile, &arr[arrAccIndice[indexOfMin]], sizeof(uint64_t)) < 0)
		{
			cout << "error writing to final output file: " << strerror(errno) << endl;
        	return -1;
		}
		else
		{
			#ifdef DEBUG
			cout << elementCounter++ << ")\t" << minimum << endl;
			#endif

			// increment index to next element
			arrAccIndice[indexOfMin]++;
		}

		// check for empty partitions
		runsRemaining = false;

		for (unsigned i = 0; i < k; ++i)
		{
			#ifdef DEBUG
			cout << "Checking load condition: " << arrAccIndice[i] << " - " << arrPartIndice[i]
			<< " >= " << (rets[i] / sizeof(uint64_t)) << endl;
			#endif

			// if partition is empty...
			if ((arrAccIndice[i] - arrPartIndice[i]) >= (rets[i] / sizeof(uint64_t)))
			{
				#ifdef DEBUG
				cout << "Loading new data for partition " << i << endl;
				#endif

				// ... load new elements into the partition
				if ((rets[i] = read(runFiles[i], &arr[arrPartIndice[i]], sizeof(uint64_t) * partSize)) < 0)
				{
					cout << "error reading from '" << "runOutput" << i << "': " << strerror(errno) << endl;
					return -1;
				}

				// if items were read...
				if (rets[i] != 0)
				{
					// ... reset access indice to start of partition
					arrAccIndice[i] = arrPartIndice[i];
				}
				#ifdef DEBUG
				else
				{
					cout << "no new data found for partition " << i << endl;
				}
				#endif
			}

			// if elements are present in the partition...
			if (rets[i] > 0)
			{
				// ... continue the merge sort loop
				runsRemaining = true;
			}
		}

	} // end of while(runsRemaining)

	// Close and delete extra output files created by the sorting phase
	for (unsigned i = 0; i < k; ++i)
	{
		close(runFiles[i]);

		if(unlink(getFileName(i, k)) < 0)
		{
			cerr << "cannot delete file '" << "runOutput" << i << "': " << strerror(errno) << endl;
            return -1;
		}
	}

	return 0;
}

// Generates file name for a specific run
const char* getFileName(unsigned mRunCount, unsigned mK)
{
	char numstr[21];
    sprintf(numstr, "%d", mRunCount % mK);

	string fileName = string("runOutput") + string(numstr);

    return fileName.c_str();
}

// Tests output file of external sort for ascending order
int testExternalSort(int fileToTest)
{
	int ret;
	uint64_t prev, current;

	if((ret = read(fileToTest, &prev, sizeof(uint64_t))) < 0)
	{
		cerr << "failed to read test file: " << strerror(errno) << endl;
		return -1;
	}

	while((ret = read(fileToTest, &current, sizeof(uint64_t))) < 0)
	{
		if (current < prev)
		{
			cout << "File not sorted in ascending order." << endl;
			return -1;
		}

		prev = current;
	}

	if(ret < 0)
	{
		cout << "failed to read test file: " << strerror(errno) << endl;
	}

	cout << "Items in correct ascending order." << endl;

	return 0;
}