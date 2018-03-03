#pragma once

#include "IO.h"

namespace APE
{

/*************************************************************************************
CInputSource - base input format class (allows multiple format support)
*************************************************************************************/
class CInputSource
{
public:
    // construction / destruction
    CInputSource(CIO * pIO, WAVEFORMATEX * pwfeSource, int * pTotalBlocks, int * pHeaderBytes, int * pTerminatingBytes, int * pErrorCode = NULL) { }
    CInputSource(std::string pSourceName, WAVEFORMATEX * pwfeSource, int * pTotalBlocks, int * pHeaderBytes, int * pTerminatingBytes, int * pErrorCode = NULL) { }
    virtual ~CInputSource() = default;

    // get data
    virtual int GetData(unsigned char * pBuffer, int nBlocks, int * pBlocksRetrieved) = 0;

    // get header / terminating data
    virtual int GetHeaderData(unsigned char * pBuffer) = 0;
    virtual int GetTerminatingData(unsigned char * pBuffer) = 0;
};

/*************************************************************************************
CWAVInputSource - wraps working with WAV files (could be extended to any format)
*************************************************************************************/
class CWAVInputSource : public CInputSource
{
public:
    // construction / destruction
    CWAVInputSource(CIO * pIO, WAVEFORMATEX * pwfeSource, int * pTotalBlocks, int * pHeaderBytes, int * pTerminatingBytes, int * pErrorCode = NULL);
    CWAVInputSource(std::string pSourceName, WAVEFORMATEX * pwfeSource, int * pTotalBlocks, int * pHeaderBytes, int * pTerminatingBytes, int * pErrorCode = NULL);
    ~CWAVInputSource() = default;

    // get data
    int GetData(unsigned char * pBuffer, int nBlocks, int * pBlocksRetrieved) override;

    // get header / terminating data
    int GetHeaderData(unsigned char * pBuffer) override;
    int GetTerminatingData(unsigned char * pBuffer) override;

private:
    int AnalyzeSource();

    CSmartPtr<CIO> m_spIO;
    WAVEFORMATEX m_wfeSource;
    int m_nHeaderBytes;
    unsigned int m_nDataBytes;
    int m_nTerminatingBytes;
    unsigned int m_nFileBytes;
    bool m_bIsValid;
};

/*************************************************************************************
Input souce creation
*************************************************************************************/
CInputSource * CreateInputSource(std::string pSourceName, WAVEFORMATEX * pwfeSource, int * pTotalBlocks, int * pHeaderBytes, int * pTerminatingBytes, int * pErrorCode = NULL);

}
