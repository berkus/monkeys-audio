#pragma once

#include "IO.h"
#include "APEInfo.h"

namespace APE
{

class CAPELink
{
public:
    CAPELink(std::string pFilename);
    CAPELink(const char * pData, std::string pFilename);
    ~CAPELink() = default;

    bool GetIsLinkFile();
    int GetStartBlock();
    int GetFinishBlock();
    std::string GetImageFilename();

protected:
    bool m_bIsLinkFile {false};
    int m_nStartBlock {0};
    int m_nFinishBlock {0};
    std::string m_cImageFilename;

    void ParseData(const char * pData, std::string pFilename);
};

}
