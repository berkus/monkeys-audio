#include "All.h"
#include "APELink.h"
#include "CharacterHelper.h"
#include IO_HEADER_FILE
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace APE
{

#define APE_LINK_HEADER                 "[Monkey's Audio Image Link File]"
#define APE_LINK_IMAGE_FILE_TAG         "Image File"
#define APE_LINK_START_BLOCK_TAG        "Start Block"
#define APE_LINK_FINISH_BLOCK_TAG       "Finish Block"

CAPELink::CAPELink(std::string pFilename)
{
    // open the file
    IO_CLASS_NAME ioLinkFile;
    if (ioLinkFile.Open(pFilename) == ERROR_SUCCESS)
    {
        // create a buffer
        CSmartPtr<char> spBuffer(new char [1024], true);

        // fill the buffer from the file and null terminate it
        unsigned int nBytesRead = 0;
        ioLinkFile.Read(spBuffer.GetPtr(), 1023, &nBytesRead);
        spBuffer[nBytesRead] = 0;

        // call the other constructor (uses a buffer instead of opening the file)
        ParseData(spBuffer, pFilename);
    }
}

CAPELink::CAPELink(const char * pData, std::string pFilename)
{
    ParseData(pData, pFilename);
}

void CAPELink::ParseData(const char * pData, std::string pFilename)
{
    if (pData != NULL)
    {
        // parse out the information
        boost::property_tree::ptree pt;
        std::istringstream iss(pData);
        boost::property_tree::ini_parser::read_ini(iss, pt);

        // @todo more sanitization of input here please!

        std::string pImageFile = pt.get<std::string>(APE_LINK_HEADER "." APE_LINK_IMAGE_FILE_TAG, "");
        // get the start and finish blocks
        m_nStartBlock = pt.get<int>(APE_LINK_HEADER "." APE_LINK_START_BLOCK_TAG, 0);
        m_nFinishBlock = pt.get<int>(APE_LINK_HEADER "." APE_LINK_FINISH_BLOCK_TAG, 0);

        // get the path
        namespace fs = boost::filesystem;

        fs::path baseFile(pFilename);
        fs::path imageFile(pImageFile);

        // process the path
        m_cImageFilename = fs::canonical(baseFile, imageFile).native();

        // this is a valid link file
        m_bIsLinkFile = true;
    }
}

int CAPELink::GetStartBlock()
{
    return m_nStartBlock;
}

int CAPELink::GetFinishBlock()
{
    return m_nFinishBlock;
}

std::string CAPELink::GetImageFilename()
{
    return m_cImageFilename;
}

bool CAPELink::GetIsLinkFile()
{
    return m_bIsLinkFile;
}

}
