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

#ifndef JAR_INPUT_EVENTQUEUE_HPP
#define JAR_INPUT_EVENTQUEUE_HPP

#include <SFML/System/Mutex.hpp>
#include <queue>
#include "jar/input/EventListener.hpp"
#include "jar/input/API.hpp"

namespace jar
{

/** \brief A thread-safe Queue of Events **/
class JARINPUTAPI EventQueue : public EventListener
{
    public:
        EventQueue();
        virtual ~EventQueue();

        /** \brief Retreives an event, if there is any.
         *
         * \param out_event The event will be put here, if there is one.
         * \return Whether there was any event left. If yes, the first is now in event.
         *
         */
        const bool GetEvent(Event& out_event);
        void ReceiveEvent(const Event& event);
    private:
        std::queue<Event> mEventQueue;
        sf::Mutex mQueueMutex;
};

} // namespace jar

#endif // JAR_INPUT_EVENTQUEUE_HPP
