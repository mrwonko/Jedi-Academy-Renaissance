#include "jar/core/CoutAndFileLogger.hpp"
#include "jar/core/CoutLogger.hpp"
#include "jar/core/FileLogger.hpp"
#include <cassert>

namespace jar {

CoutAndFileLogger::CoutAndFileLogger(const std::string& filename, const bool timestamps) :
    mCoutLogger(NULL),
    mFileLogger(NULL)
{
    mCoutLogger = new CoutLogger();
    assert(mCoutLogger);
    mFileLogger = new FileLogger(filename, timestamps);
    assert(mFileLogger);
}

CoutAndFileLogger::~CoutAndFileLogger()
{
    delete mCoutLogger;
    delete mFileLogger;
}

void CoutAndFileLogger::Log(const std::string& message)
{
    mCoutLogger->Log(message);
    mFileLogger->Log(message);
}

void CoutAndFileLogger::Warning(const std::string& message)
{
    mCoutLogger->Warning(message);
    mFileLogger->Warning(message);
}

void CoutAndFileLogger::Error(const std::string& message)
{
    mCoutLogger->Error(message);
    mFileLogger->Error(message);
}

} // namespace jar
