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

#ifndef JAR_CORE_COUTLOGGER_H
#define JAR_CORE_COUTLOGGER_H

#include <jar/Library.hpp>

#include "Logger.hpp"

namespace jar
{
    /**
        \brief A Logger that outputs to the console using std::cout, appending a newline.

        On windows, warnings will be yellow and errors red.
        \headerfile <jar/core/CoutLogger.hpp>
    **/
    class JARAPI CoutLogger : public Logger
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
