/***************************************************************************************
Analyze - Sample 1
Copyright (C) 2000-2001 by Matthew T. Ashland   All Rights Reserved.
Feel free to use this code in any way that you like.

This example opens an APE file and displays some basic information about it. To use it,
just type Sample 1.exe followed by a file name and it'll display information about that
file.

Notes for use in a new project:
	-you need to include "MACLib.lib" in the included libraries list
	-life will be easier if you set the [MAC SDK]\\Shared directory as an include
	directory and an additional library input path in the project settings
	-set the runtime library to "Multithreaded"

WARNING:
	-This class driven system for using Monkey's Audio is still in development, so
	I can't make any guarantees that the classes and libraries won't change before
	everything gets finalized.  Use them at your own risk.
***************************************************************************************/

// includes
#include "All.h"
#include "MACLib.h"
#include "APETag.h"
#include <string>
#include <memory>
#include <iostream>

using namespace std;
using namespace APE;

int main(int argc, char* argv[])
{
	///////////////////////////////////////////////////////////////////////////////
	// error check the command line parameters
	///////////////////////////////////////////////////////////////////////////////
	if (argc != 2)
	{
		cerr << "~~~Improper Usage~~~\n\n"
				"Usage Example: Sample 1.exe 'c:\\1.ape'\n" << endl;
		return -1;
	}

	///////////////////////////////////////////////////////////////////////////////
	// variable declares
	///////////////////////////////////////////////////////////////////////////////
	int					nRetVal = 0;										// generic holder for return values
	char				cTempBuffer[256] = {};	// generic buffer for string stuff
	char *				pFilename = argv[1];								// the file to open

	///////////////////////////////////////////////////////////////////////////////
	// open the file and error check
	///////////////////////////////////////////////////////////////////////////////
	std::unique_ptr<IAPEDecompress> pAPEDecompress(CreateIAPEDecompress(pFilename, &nRetVal));
	if (pAPEDecompress == nullptr)
	{
		printf("Error opening APE file. (error code %d)\r\n\r\n", nRetVal);
		return 0;
	}

	///////////////////////////////////////////////////////////////////////////////
	// display some information about the file
	///////////////////////////////////////////////////////////////////////////////
	printf("Displaying information about '%s':\r\n\r\n", pFilename);

	// file format information
	printf("File Format:\r\n");
	printf("\tVersion: %.2f\r\n", float(pAPEDecompress->GetInfo(APE_INFO_FILE_VERSION)) / float(1000));
	switch (pAPEDecompress->GetInfo(APE_INFO_COMPRESSION_LEVEL))
	{
		case COMPRESSION_LEVEL_FAST: printf("\tCompression level: Fast\r\n\r\n"); break;
		case COMPRESSION_LEVEL_NORMAL: printf("\tCompression level: Normal\r\n\r\n"); break;
		case COMPRESSION_LEVEL_HIGH: printf("\tCompression level: High\r\n\r\n"); break;
		case COMPRESSION_LEVEL_EXTRA_HIGH: printf("\tCompression level: Extra High\r\n\r\n"); break;
		case COMPRESSION_LEVEL_INSANE: cout << "\tCompression level: Insane\n" << endl; break;
	}

	// audio format information
	cout << "Audio Format:\n"
		    "\tSamples per second: " << pAPEDecompress->GetInfo(APE_INFO_SAMPLE_RATE) << "\n"
			"\tBits per sample: " << pAPEDecompress->GetInfo(APE_INFO_BITS_PER_SAMPLE) << "\n"
			"\tNumber of channels: " << pAPEDecompress->GetInfo(APE_INFO_CHANNELS) << "\n"
			"\tPeak level: " << pAPEDecompress->GetInfo(APE_INFO_PEAK_LEVEL) << "\n\n"

	// size and duration information
			"Size and Duration:\n"
			"\tLength of file (s): " << pAPEDecompress->GetInfo(APE_INFO_LENGTH_MS) / 1000 << "\n"
			"\tFile Size (kb): " << pAPEDecompress->GetInfo(APE_INFO_APE_TOTAL_BYTES) / 1024 << "\n\n"

	// tag information
			"Tag Information:\n";

	CAPETag * pAPETag = (CAPETag *) pAPEDecompress->GetInfo(APE_INFO_TAG);
	BOOL bHasID3Tag = pAPETag->GetHasID3Tag();
	BOOL bHasAPETag = pAPETag->GetHasAPETag();

	if (bHasID3Tag || bHasAPETag)
	{
		// iterate through all the tag fields
		int index = 0;
		while (CAPETagField * pTagField = pAPETag->GetTagField(index))
		{
			index += 1;

			// output the tag field properties (don't output huge fields like images, etc.)
			if (pTagField->GetFieldValueSize() > 128)
			{
				cout << "\t" << pTagField->GetFieldName() << ": --- too much data to display ---\n";
			}
			else
			{
				cout << "\t" << pTagField->GetFieldName() << ": " << pTagField->GetFieldValue() << "\n";
			}
		}
	}
	else
	{
		cout << "\tNot tagged\n";
	}

	cout << endl; // flush
}
