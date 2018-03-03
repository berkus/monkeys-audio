#include "All.h"

#ifdef IO_USE_STD_LIB_FILE_IO

#include "StdLibFileIO.h"
#include "CharacterHelper.h"
#include <string>

namespace APE
{

int CStdLibFileIO::GetHandle()
{
    return fileno(m_pFile);
}

int CStdLibFileIO::Open(std::string pName, bool bOpenReadOnly)
{
    using namespace std::string_literals;

    Close();

    m_bReadOnly = false;

    if (pName == "-"s or pName == "/dev/stdin"s)
    {
        m_pFile = stdin;
        m_bReadOnly = true;                                                     // ReadOnly
    }
    else if (pName == "/dev/stdout"s)
    {
        m_pFile = stdout;
        m_bReadOnly = false;                                                    // WriteOnly
    }
    else
    {
        m_pFile = fopen(pName.c_str(), "r+b");
		if (m_pFile == NULL && (errno == EACCES || errno == EPERM || errno == EROFS))
        {
			// failed asking for read/write mode on open, try read-only
			m_pFile = fopen(pName.c_str(), "rb");
			if (m_pFile)
				m_bReadOnly = true;
		}
    }

    if (!m_pFile)
        return -1;

    m_cFileName = pName;

    return 0;
}

int CStdLibFileIO::Close()
{
    int nResult = -1;

    if (m_pFile != NULL)
    {
        nResult = fclose(m_pFile);
        m_pFile = NULL;
    }

    return nResult;
}

int CStdLibFileIO::Read(void * pBuffer, unsigned int nBytesToRead, unsigned int * pBytesRead)
{
    *pBytesRead = fread(pBuffer, 1, nBytesToRead, m_pFile);
    return ferror(m_pFile) ? ERROR_IO_READ : 0;
}

int CStdLibFileIO::Write(const void * pBuffer, unsigned  int nBytesToWrite, unsigned int * pBytesWritten)
{
    *pBytesWritten = fwrite(pBuffer, 1, nBytesToWrite, m_pFile);

    return (ferror(m_pFile) || (*pBytesWritten != nBytesToWrite)) ? ERROR_IO_WRITE : 0;
}

int CStdLibFileIO::Seek(intn nDistance, unsigned int nMoveMode)
{
    return fseek(m_pFile, nDistance, nMoveMode);
}

int CStdLibFileIO::SetEOF()
{
    return ftruncate(GetHandle(), GetPosition());
}

int CStdLibFileIO::GetPosition()
{
    return ftell(m_pFile);
}

unsigned int CStdLibFileIO::GetSize()
{
    int nCurrentPosition = GetPosition();
    Seek(0, FILE_END);
    int nLength = GetPosition();
    Seek(nCurrentPosition, FILE_BEGIN);
    return nLength;
}

std::string CStdLibFileIO::GetName()
{
    return m_cFileName.native();
}

int CStdLibFileIO::Create(std::string pName)
{
    using namespace std::string_literals;

    Close();

    if (pName == "-"s or pName == "/dev/stdout"s)
    {
        m_pFile = stdout;
        m_bReadOnly = false;                            // WriteOnly
    }
    else
    {
		// NOTE: on Mac OSX (BSD Unix), we MUST have "w+b" if we want to read & write, with other systems "wb" seems to be fine
        m_pFile = fopen(pName.c_str(), "w+b");                  // Read/Write
        m_bReadOnly = false;
    }

    if (!m_pFile)
        return -1;

    m_cFileName = pName;

    return 0;
}

int CStdLibFileIO::Delete()
{
    Close();
    boost::system::error_code ec;
    boost::filesystem::remove(m_cFileName, ec);
    return bool(ec);    // false success, true error
}

}

#endif // #ifdef IO_USE_STD_LIB_FILE_IO
