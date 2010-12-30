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

#ifndef JAR_INPUT_EVENTSENDER_HPP
#define JAR_INPUT_EVENTSENDER_HPP

#include <set>
#include "jar/input/API.hpp"

namespace jar
{

class Event;
class EventListener;
/** \brief Class for sending Events. Adds timestamps.
**/
class JARINPUTAPI EventSender
{
    public:
        /** \brief Destructor **/
        virtual ~EventSender();

        /** \brief Registers a listener that will receive sent events
            \param listener The listener **/
        void RegisterListener(EventListener* listener);

        /** \brief Unregisters a listener

            \param listener the EventListener to unregister
            \return Whether such a listener existed
        **/
        const bool UnregisterListener(EventListener* listener);

    protected:
        /** \brief Constructor **/
        EventSender();

        /** \brief Sends an event to all registered listeners.
            \param event the Event to send **/
        void SendEvent(Event& event) const;

    private:
        std::set<EventListener*> mListeners; ///< the registered listeners
};

} // namespace jar

#endif // JAR_INPUT_EVENTSENDER_HPP
