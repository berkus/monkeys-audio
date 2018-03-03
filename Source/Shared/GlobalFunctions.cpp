#include "All.h"
#include "GlobalFunctions.h"
#include "IO.h"
#include "CharacterHelper.h"
#include <string>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

namespace APE
{

int ReadSafe(CIO * pIO, void * pBuffer, int nBytes)
{
    unsigned int nBytesRead = 0;
    int nResult = pIO->Read(pBuffer, nBytes, &nBytesRead);
    if (nResult == ERROR_SUCCESS)
    {
        if (nBytes != int(nBytesRead))
            nResult = ERROR_IO_READ;
    }

    return nResult;
}

intn WriteSafe(CIO * pIO, void * pBuffer, intn nBytes)
{
    unsigned int nBytesWritten = 0;
    intn nResult = pIO->Write(pBuffer, (unsigned int) nBytes, &nBytesWritten);
    if (nResult == ERROR_SUCCESS)
    {
        if (nBytes != int(nBytesWritten))
            nResult = ERROR_IO_WRITE;
    }

    return nResult;
}

bool FileExists(std::string pFilename)
{
    using namespace std::string_literals;

    if (pFilename == "-"s or pFilename == "/dev/stdin"s)
        return true;

    return boost::filesystem::exists(pFilename);
}

void * AllocateAligned(intn nBytes, intn nAlignment)
{
#ifdef _WIN32
    return _aligned_malloc(nBytes, nAlignment);
#else
    void * pMemory = NULL;
    posix_memalign(&pMemory, nAlignment, nBytes);
    return pMemory;
#endif
}

void FreeAligned(void * pMemory)
{
#ifdef _WIN32
    _aligned_free(pMemory);
#else
    free(pMemory);
#endif
}

bool GetMMXAvailable()
{
#if defined(__MMX__)
	return true;
#else
	bool bMMX = false;
#ifdef _MSC_VER
	#define CPU_MMX (1 << 23)

	int cpuInfo[4] = { 0 };
	__cpuid(cpuInfo, 0);

	int nIds = cpuInfo[0];
	if (nIds >= 1)
	{
		__cpuid(cpuInfo, 1);
		if (cpuInfo[3] & CPU_MMX)
			bMMX = true;
	}
#endif
	return bMMX;
#endif
}

bool GetSSEAvailable()
{
#if defined(__SSE2__)
    return true;
#else
    bool bSSE = false;
#ifdef _MSC_VER
    #define CPU_SSE2 (1 << 26)

    int cpuInfo[4] = { 0 };
    __cpuid(cpuInfo, 0);

    int nIds = cpuInfo[0];
    if (nIds >= 1)
    {
        __cpuid(cpuInfo, 1);
        if (cpuInfo[3] & CPU_SSE2)
			bSSE = true;
    }
#endif
    return bSSE;
#endif
}

bool StringIsEqual(const char * pString1, const char * pString2, bool bCaseSensitive, int nCharacters)
{
    if (nCharacters != -1) {
        if (bCaseSensitive)
            return strncmp(pString1, pString2, nCharacters) == 0;
        else
            return strncasecmp(pString1, pString2, nCharacters) == 0;
    } else {
        if (bCaseSensitive)
            return boost::algorithm::equals(pString1, pString2);
        else
            return boost::algorithm::iequals(pString1, pString2);
    }
}

}
