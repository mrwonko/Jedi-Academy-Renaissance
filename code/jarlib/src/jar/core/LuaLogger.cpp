#include "jar/core/LuaLogger.hpp"

#include <iostream>

namespace jar {

LuaLogger::LuaLogger()
{
    //ctor
}

LuaLogger::~LuaLogger()
{
    //dtor
}

void LuaLogger::Log(const std::string& message)
{
    //FIXME: this crashes. WTF? (SIGSEGV) Or have I fixed it?
    call<void>("Log", message); //inherited from luabind::wrap_base
}

void LuaLogger::Warning(const std::string& message)
{
    call<void>("Warning", message);
}

void LuaLogger::Error(const std::string& message)
{
    call<void>("Error", message);
}


void LuaLogger::WrapLog(Logger* logger, const std::string& message)
{
    logger->Logger::Log(message);
}

void LuaLogger::WrapWarning(Logger* logger, const std::string& message)
{
    logger->Logger::Warning(message);
}

void LuaLogger::WrapError(Logger* logger, const std::string& message)
{
    logger->Logger::Error(message);
}


} // namespace jar
