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

#ifndef JAR_INPUT_EVENTRELAY_HPP
#define JAR_INPUT_EVENTRELAY_HPP

#include "jar/input/EventListener.hpp"
#include "jar/input/EventSender.hpp"

namespace jar {

/** \brief An event listener that relays its received events to its listeners **/
class JARINPUTAPI EventRelay : public EventListener, public EventSender
{
    public:
        EventRelay();
        virtual ~EventRelay();

        virtual void ReceiveEvent(const Event& e);
    protected:
    private:
};

} // namespace jar

#endif // JAR_INPUT_EVENTRELAY_HPP
