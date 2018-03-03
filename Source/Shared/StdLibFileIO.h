#ifdef IO_USE_STD_LIB_FILE_IO

#pragma once

#include "IO.h"
#include <boost/filesystem.hpp>

namespace APE
{

class CStdLibFileIO : public CIO
{
public:
    // construction / destruction
    CStdLibFileIO() = default;
    ~CStdLibFileIO() { Close(); }

    // open / close
    int Open(std::string pName, bool bOpenReadOnly = false) override;
    int Close() override;

    // read / write
    int Read(void * pBuffer, unsigned int nBytesToRead, unsigned int * pBytesRead) override;
    int Write(const void * pBuffer, unsigned int nBytesToWrite, unsigned int * pBytesWritten) override;

    // seek
    int Seek(intn nDistance, unsigned int nMoveMode) override;

    // other functions
    int SetEOF() override;

    // creation / destruction
    int Create(std::string pName) override;
    int Delete() override;

    // attributes
    int GetPosition() override;
    unsigned int GetSize() override;
    std::string GetName() override;
    int GetHandle();

private:

    boost::filesystem::path m_cFileName;
    bool m_bReadOnly {false};
    FILE * m_pFile {nullptr};
};

}

#endif // #ifdef IO_USE_STD_LIB_FILE_IO
