/*
===========================================================================
Copyright (C) 2010 Willi Schinmeyer

This file is part of the Jedi Academy: Renaissance source code.

Jedi Academy: Renaissance source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Jedi Academy: Renaissance source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Jedi Academy: Renaissance source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

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
