#ifndef JAR_CORE_LUALOGGER_HPP
#define JAR_CORE_LUALOGGER_HPP

#include "jar/core/Logger.hpp"
#include <luabind/wrapper_base.hpp>

namespace jar {


/** \brief Wrapper for the Logger so you can inherit from it in Lua

    To inherit using lua, you'd do something like this:
    \code
class "LuaLogger" (jar.Logger)

function LuaLogger:__init()
	jar.Logger.__init(self)
end

function LuaLogger:Log(message)
	print("Log: " .. message)
end

function LuaLogger:Warning(message)
	print("Warning: " .. message)
end

function LuaLogger:Error(message)
	print("Error: " .. message)
end
    \endcode
**/
class LuaLogger : public Logger, public luabind::wrap_base
{
    public:
        LuaLogger();
        virtual ~LuaLogger();

        virtual void Log(const std::string& message);
        virtual void Warning(const std::string& message);
        virtual void Error(const std::string& message);

        static void WrapLog(Logger* logger, const std::string& message);
        static void WrapWarning(Logger* logger, const std::string& message);
        static void WrapError(Logger* logger, const std::string& message);

    protected:
    private:
};

} // namespace jar

#endif // JAR_CORE_LUALOGGER_HPP
