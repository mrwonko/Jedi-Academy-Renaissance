#include "jar/core/Logger.hpp"
#include "jar/core/VoidLogger.hpp"

namespace jar
{
    //Initialize the static Default Logger variable
    Logger* Logger::mDefaultLogger = NULL;
    Logger* Logger::mFallbackVoidLogger = NULL;

    //Constructor
    Logger::Logger() :
        mLoggingLevel(0)
    {
        //Set the default logger to be the first logger ever created - though NOT just the temporary VoidLogger!
        if(mDefaultLogger == NULL && this != mFallbackVoidLogger)
        {
            mDefaultLogger = this;
            //we now have a default logger so delete the fallback logger if it exists.
            delete mFallbackVoidLogger;
            mFallbackVoidLogger = NULL;
        }
    }

    Logger::~Logger()
    {
        if(mDefaultLogger == this)
        {
            mDefaultLogger = NULL;
        }
    }

    void Logger::SetDefaultLogger(Logger& l)
    {
        mDefaultLogger = &l;
    }

    Logger& Logger::GetDefaultLogger()
    {
        if(!mDefaultLogger)
        {
            if(!mFallbackVoidLogger)
            {
                mFallbackVoidLogger = new VoidLogger();
            }
            return *mFallbackVoidLogger;
        }
        return(*mDefaultLogger);
    }

    void Logger::Info(const std::string& message, int minlevel)
    {
        if(mLoggingLevel >= minlevel) Log(message);
    }

    void Logger::SetLoggingLevel(int newlevel)
    {
        mLoggingLevel = newlevel;
    }

    const int Logger::GetLoggingLevel()
    {
        return mLoggingLevel;
    }

    void Logger::IncreaseLoggingLevel(int amount)
    {
        mLoggingLevel += amount;
    }

    void Logger::DecreaseLoggingLevel(int amount)
    {
        mLoggingLevel -= amount;
    }
}
