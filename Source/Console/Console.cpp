/***************************************************************************************
MAC Console Frontend (MAC.exe)

Pretty simple and straightforward console front end.  If somebody ever wants to add
more functionality like tagging, auto-verify, etc., that'd be excellent.

Copyrighted (c) 2000 - 2018 Matthew T. Ashland.  All Rights Reserved.
***************************************************************************************/
#include "All.h"
#include "GlobalFunctions.h"
#include "MACLib.h"
#include "CharacterHelper.h"
#include <iostream>
#include <boost/filesystem.hpp>

using namespace std;
using namespace APE;

// defines
#define COMPRESS_MODE		0
#define DECOMPRESS_MODE		1
#define VERIFY_MODE			2
#define CONVERT_MODE		3
#define UNDEFINED_MODE		-1

// global variables
TICK_COUNT_TYPE g_nInitialTickCount = 0;

/***************************************************************************************
Displays the proper usage for MAC.exe
***************************************************************************************/
void DisplayProperUsage()
{
	cerr << "Proper Usage: [EXE] [Input File] [Output File] [Mode]\n\n"

		    "Modes: \n"
		    "    Compress (fast): '-c1000'\n"
		    "    Compress (normal): '-c2000'\n"
		    "    Compress (high): '-c3000'\n"
		    "    Compress (extra high): '-c4000'\n"
		    "    Compress (insane): '-c5000'\n"
		    "    Decompress: '-d'\n"
		    "    Verify: '-v'\n"
		    "    Convert: '-nXXXX'\n\n"

		    "Examples:\n"
		    "    Compress: mac.exe \"Metallica - One.wav\" \"Metallica - One.ape\" -c2000\n"
		    "    Decompress: mac.exe \"Metallica - One.ape\" \"Metallica - One.wav\" -d\n"
		    "    Verify: mac.exe \"Metallica - One.ape\" -v\n"
		    "    (note: int filenames must be put inside of quotations)" << endl;
}

/***************************************************************************************
Progress callback
***************************************************************************************/
void CALLBACK ProgressCallback(int nPercentageDone)
{
    // get the current tick count
	TICK_COUNT_TYPE  nTickCount;
	TICK_COUNT_READ(nTickCount);

	// calculate the progress
	double dProgress = nPercentageDone / 1.e5;											// [0...1]
	double dElapsed = (double) (nTickCount - g_nInitialTickCount) / TICK_COUNT_FREQ;	// seconds
	double dRemaining = dElapsed * ((1.0 / dProgress) - 1.0);							// seconds

	// output the progress
	fprintf(stderr, _T("Progress: %.1f%% (%.1f seconds remaining, %.1f seconds total)          \r"),
		dProgress * 100, dRemaining, dElapsed);
}

/***************************************************************************************
Main (the main function)
***************************************************************************************/
int main(int argc, char * argv[])
{
	// variable declares
	std::string spInputFilename, spOutputFilename;
	int nRetVal = ERROR_UNDEFINED;
	int nMode = UNDEFINED_MODE;
	int nCompressionLevel;
	int nPercentageDone;

	// output the header
	cerr << CONSOLE_NAME;

	// make sure there are at least four arguments (could be more for EAC compatibility)
	if (argc < 3)
	{
		DisplayProperUsage();
		exit(-1);
	}

	// store the filenames
	spInputFilename = argv[1];
	spOutputFilename = argv[2];

	// verify that the input file exists
	if (!boost::filesystem::exists(spInputFilename)) // @todo `-` and `/dev/stdin` checks
	{
		cerr << "Input File Not Found...\n" << endl;
		exit(-1);
	}

	// if the output file equals '-v', then use this as the next argument
	std::string cMode = argv[2];

	if (cMode != "-v"s)
	{
		// verify is the only mode that doesn't use at least the third argument
		if (argc < 4)
		{
			DisplayProperUsage();
			exit(-1);
		}

		// check for and skip if necessary the -b XXXXXX arguments (3,4)
		cMode = argv[3];
	}

	// get the mode
	nMode = UNDEFINED_MODE;
	if (cMode == "-c"s) // @todo case-insensitive compare
		nMode = COMPRESS_MODE;
	else if (cMode == "-d"s) // @todo case-insensitive compare
		nMode = DECOMPRESS_MODE;
	else if (cMode == "-v"s) // @todo case-insensitive compare
		nMode = VERIFY_MODE;
	else if (cMode == "-n"s) // @todo case-insensitive compare
		nMode = CONVERT_MODE;

	// error check the mode
	if (nMode == UNDEFINED_MODE)
	{
		DisplayProperUsage();
		exit(-1);
	}

	// get and error check the compression level
	if (nMode == COMPRESS_MODE || nMode == CONVERT_MODE)
	{
		nCompressionLevel = atoi(&cMode[2]);
		if (nCompressionLevel != 1000 && nCompressionLevel != 2000 &&
			nCompressionLevel != 3000 && nCompressionLevel != 4000 &&
			nCompressionLevel != 5000)
		{
			DisplayProperUsage();
			exit(-1);
		}
	}

	// set the initial tick count
	TICK_COUNT_READ(g_nInitialTickCount);

	// process
	int nKillFlag = 0;
	if (nMode == COMPRESS_MODE)
	{
		std::string cCompressionLevel;
		if (nCompressionLevel == COMPRESSION_LEVEL_FAST) { cCompressionLevel = "fast"; }
		if (nCompressionLevel == COMPRESSION_LEVEL_NORMAL) { cCompressionLevel = "normal"; }
		if (nCompressionLevel == COMPRESSION_LEVEL_HIGH) { cCompressionLevel = "high"; }
		if (nCompressionLevel == COMPRESSION_LEVEL_EXTRA_HIGH) { cCompressionLevel = "extra high"; }
		if (nCompressionLevel == COMPRESSION_LEVEL_INSANE) { cCompressionLevel = "insane"; }

		cerr << "Compressing (" << cCompressionLevel << ")..." << endl;
		nRetVal = CompressFile(spInputFilename, spOutputFilename, nCompressionLevel, &nPercentageDone, ProgressCallback, &nKillFlag);
	}
	else if (nMode == DECOMPRESS_MODE)
	{
		cerr << "Decompressing..." << endl;
		nRetVal = DecompressFile(spInputFilename, spOutputFilename, &nPercentageDone, ProgressCallback, &nKillFlag);
	}
	else if (nMode == VERIFY_MODE)
	{
		cerr << "Verifying..." << endl;
		nRetVal = VerifyFile(spInputFilename, &nPercentageDone, ProgressCallback, &nKillFlag);
	}
	else if (nMode == CONVERT_MODE)
	{
		cerr << "Converting..." << endl;
		nRetVal = ConvertFile(spInputFilename, spOutputFilename, nCompressionLevel, &nPercentageDone, ProgressCallback, &nKillFlag);
	}

	if (nRetVal == ERROR_SUCCESS)
		cerr << "\nSuccess..." << endl;
	else
		cerr << "\nnError: " << nRetVal << endl;

	return nRetVal;
}
