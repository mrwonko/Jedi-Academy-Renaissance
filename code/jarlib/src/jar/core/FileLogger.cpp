#include "jar/core/FileLogger.hpp"
#include <fstream>
#include <time.h>

namespace jar {

FileLogger::FileLogger(const std::string& filename, const bool timestamps) :
    mFilename(filename),
    mTimestamps(timestamps)
{
    //clear the logfile
    std::ofstream stream(filename.c_str());
    stream.close();
    Write("Logfile opened");
}

FileLogger::~FileLogger()
{
    //dtor
}

void FileLogger::Log(const std::string& message)
{
    Write(message);
}

void FileLogger::Warning(const std::string& message)
{
    Write("WARNING: " + message);
}

void FileLogger::Error(const std::string& message)
{
    Write("ERROR: " + message);
}

void FileLogger::Write(const std::string& message)
{
    std::ofstream file(mFilename.c_str(), std::ios_base::out | std::ios_base::app);
    if(file.fail()) return;
    if(mTimestamps)
    {
        time_t rawtime;
        time(&rawtime);
        tm* timeinfo = localtime(&rawtime); //pointer to a static tm struct - don't delete!

        file<<timeinfo->tm_year+1900<<"-"<<(timeinfo->tm_mon < 11 ? "0" : "")<<timeinfo->tm_mon+1<<"-"<<(timeinfo->tm_mday < 10 ? "0" : "")<<timeinfo->tm_mday<<" "<<timeinfo->tm_hour<<":"<<(timeinfo->tm_min < 10 ? "0" : "")<<timeinfo->tm_min<<":"<<(timeinfo->tm_sec < 10 ? "0" : "")<<timeinfo->tm_sec<<" "<<message<<"\n";
    }
    else
    {
        file<<message<<"\n";
    }
}

} // namespace jar
