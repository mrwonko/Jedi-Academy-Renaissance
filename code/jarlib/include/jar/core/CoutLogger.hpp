#ifndef JAR_CORE_COUTLOGGER_H
#define JAR_CORE_COUTLOGGER_H

#include <jar/core/API.hpp>

#include "Logger.hpp"

namespace jar
{
    /**
        \brief A Logger that outputs to the console using std::cout, appending a newline.

        On windows, warnings will be yellow and errors red.
        \headerfile <jar/core/CoutLogger.hpp>
    **/
    class JARCOREAPI CoutLogger : public Logger
    {
        public:
            CoutLogger();
            virtual ~CoutLogger();

            virtual void Log(const std::string& message);
            virtual void Warning(const std::string& message);
            virtual void Error(const std::string& message);
        protected:
        private:
    };

} // namespace jar

#endif // JAR_CORE_COUTLOGGER_H
