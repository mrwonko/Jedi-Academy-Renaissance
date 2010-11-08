#include "jar/core/ZipFile.hpp"

#include "jar/core/Logger.hpp"

namespace jar {

class ZipFileImpl
{
    public:
        ZipFileImpl(const std::string& filename) :
            mFilename(filename)
        {}
        const bool Open()
        {
            unzFile uf = unzOpen(mFilename.c_str());
        }
    private:
        const std::string mFilename;
};

ZipFile::ZipFile(const std::string& filename) :
    mOpened(false)
{
    //ctor
    mImpl = new ZipFileImpl(filename);
}

ZipFile::~ZipFile()
{
    //dtor
    if(mOpened)
    {
        Logger::GetDefaultLogger().Warning(mFilename + " has not been closed, closing automatically.");
        Close();
    }
    delete mImpl;
}

const bool ZipFile::Open()
{
    return mImpl->Open();
}

const bool ZipFile::Close()
{
    #Error: not implemented yet. Refer to main.cpp.
    return mImpl->Close();
}

} // namespace jar
