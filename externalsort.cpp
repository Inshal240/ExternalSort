#include<iostream>
#include<algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>

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

	int k = atoi(argv[1]);

	// possible values of k = 2, 4, 8
	if (k != 2 && k != 4 && k != 8)
	{
		cerr << "possible values for k: 2, 4, 8";
	}

	int inputFile;

	// Read file
	if ((inputFile = open(argv[2], O_RDONLY, S_IRUSR | S_IWUSR)) < 0)
    {
		cerr << "cannot open file '" << argv[2] << "': " << strerror(errno) << endl;
		return -1;
	}

	// Test value for run size
	unsigned memSize = 64;
	unsigned runCount = 0;
	uint64_t arr[memSize];

	if (read(inputFile, &arr[0], sizeof(uint64_t)) < 0)
    {
        cout << "error reading from " << argv[1] << ": " << strerror(errno) << endl;
        return -1;
    }

    // Peek read for EOF
    while(1)
    {
        // Read data from file and store it into an array
        int arrCount = 0, // keeps count of valid entries in array
            ret;          // used to see if EOF is reached

        for (unsigned i = 0; i < memSize; ++i)
        {
            ret = read(inputFile, &arr[i], sizeof(uint64_t));
            if (ret < 0)    // Error when reading
            {
                cout << "error reading from " << argv[1] << ": " << strerror(errno) << endl;
                //return -1;
            }
            else
            {
                arrCount++;
            }
        }

        cout << "Read " << arrCount << " items from file." << endl;

        for(unsigned i = 0; i < arrCount; ++i)
        {
            cout << "Item " << i << ". " << arr[i] << endl;
        }

        sort(arr, &arr[arrCount]);

        cout << "Sorted run." << endl;

        for(unsigned i = 0; i < arrCount; ++i)
        {
            cout << "Item " << i << ". " << arr[i] << endl;
        }

        int outputFile;
        string fileName("runOutput");

        char numstr[21];
        fileName = fileName + itoa(runCount % k, numstr, 10);

        if ((outputFile = open(fileName.c_str(), O_CREAT | O_APPEND | O_WRONLY, S_IRUSR | S_IWUSR)) < 0)
        {
            cerr << "cannot open file '" << "runOutput" << "': " << strerror(errno) << endl;
            return -1;
        }

        for(unsigned i = 0; i < arrCount; ++i)
        {
            if (write(outputFile, &arr[i], sizeof(uint64_t)) < 0)
            {
                cout << "error writing to " << "runOutput" << ": " << strerror(errno) << endl;
                return -1;
            }
        }

        runCount++;

        // Break if last read was EOF
        //if (ret == 0)
        //{
            //cout << "EOF reached" << endl;
            //break;
        //}

    }
	// Merge sort files


	close(inputFile);

	return 0;
}

void externalSort(int fdInput, uint64_t size, int fdOutput, uint64_t memSize)
{

}