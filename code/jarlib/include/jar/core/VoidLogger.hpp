#ifndef JAR_CORE_VOIDLOGGER_HPP
#define JAR_CORE_VOIDLOGGER_HPP

#include "Logger.hpp"
#include <jar/core/API.hpp>

namespace jar
{
    /**
        \brief A Logger that simply ignores every log.
        \note Used as the default Logger if none exists.
        \headerfile <jar/core/VoidLogger.hpp>
    **/
    class JARCOREAPI VoidLogger : public Logger
    {
        public:
            VoidLogger() {}
            virtual ~VoidLogger() {}
            virtual void Log(const std::string& message){}
            virtual void Warning(const std::string& message){}
            virtual void Error(const std::string& message){}
    };

} // namespace jar

#endif // JAR_CORE_VOIDLOGGER_HPP
