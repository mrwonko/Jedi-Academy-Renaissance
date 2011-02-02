#ifndef JAR_CORE_COUTANDFILELOGGER_HPP
#define JAR_CORE_COUTANDFILELOGGER_HPP

#include "jar/core/Logger.hpp"

namespace jar {

class CoutLogger;
class FileLogger;

class JARCOREAPI CoutAndFileLogger : public jar::Logger
{
    public:
        CoutAndFileLogger(const std::string& filename, const bool timestamps = true);
        virtual ~CoutAndFileLogger();

        virtual void Log(const std::string& message);
        virtual void Warning(const std::string& message);
        virtual void Error(const std::string& message);
    protected:
    private:
        //if these were no pointers, they'd be allocated before the CoutAndFileLogger and could thus accidently become the DefaultLogger
        CoutLogger* mCoutLogger;
        FileLogger* mFileLogger;
};

} // namespace jar

#endif // JAR_CORE_COUTANDFILELOGGER_HPP
