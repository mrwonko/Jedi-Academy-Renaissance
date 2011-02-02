#ifndef JAR_CORE_FILELOGGER_HPP
#define JAR_CORE_FILELOGGER_HPP

#include "jar/core/Logger.hpp"

namespace jar {

class JARCOREAPI FileLogger : public jar::Logger
{
    public:
        FileLogger(const std::string& filename, const bool timestamps = true);
        virtual ~FileLogger();

        virtual void Log(const std::string& message);
        virtual void Warning(const std::string& message);
        virtual void Error(const std::string& message);
    protected:
    private:
        void Write(const std::string& message);

        const std::string mFilename;
        const bool mTimestamps;
};

} // namespace jar

#endif // JAR_CORE_FILELOGGER_HPP
