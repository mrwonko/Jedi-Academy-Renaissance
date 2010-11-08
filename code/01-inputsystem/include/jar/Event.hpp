#ifndef JAR_EVENT_HPP
#define JAR_EVENT_HPP

#include "jar/Time.hpp"
#include "jar/KeyCodes.hpp"

class lua_State;

namespace jar
{

/**
    \brief Events that happen.

    Events include such things as key presses, mouse moves and the window closing.
**/
struct Event
{
    /// Different types of events
    enum EventType
    {
        Closed, ///< The window close button has been clicked
        KeyPressed,
        KeyReleased,
        MouseMoved,
        MouseButtonPressed,
        MouseButtonReleased,
        MouseWheelMoved,
        JoyAxisMoved,
        JoyButtonPressed,
        JoyButtonReleased,
        //Resized, ///< The window has been resized
        LostFocus, ///< The window lost focus
        GainedFocus, ///< The window gained focus
    };
    ///the type of this event
    EventType Type;

    /** \brief Info for Event of EvenType KeyPressed or KeyReleased
    **/
    struct KeyEvent
    {
        Key::Code Code;
    };

    /** \brief Info for Event of EventType MouseMoved
    **/
    struct MouseMovedEvent
    {
        int X; ///< X-Position, from left side
        int Y; ///< Y-Position, from top
    };

    /** \brief Info for Event of EventType MouseWheelMoved
    **/
    struct MouseWheelEvent
    {
        int Delta;
    };

    /** \brief Info for Event of EventType MouseButtonPressed or MouseButtonReleased
    **/
    struct MouseButtonEvent
    {
        unsigned int Button; ///< The button index, 0 = LMB, 1 = RMB, 2 = MMB etc.
    };

    /** \brief Info for Event of EventType JoyButtonPressed or JoyButtonReleased
    **/
    struct JoyButtonEvent
    {
        unsigned int JoyIndex; ///< The joystick index
        unsigned int Button; ///< the button index
    };

    /** \brief Info for Event of EventType JoyAxisMoved
    **/
    struct JoyAxisEvent
    {
        unsigned int JoyIndex; ///< The joystick index
        unsigned int Axis; ///< The axis index
        float Position; ///< The Axis's position, in [-1; 1]
    };

    /// Information related to the event, depends on EventType.
    union
    {
        KeyEvent Key;
        MouseMovedEvent MouseMove;
        MouseButtonEvent MouseButton;
        MouseWheelEvent MouseWheel;
        JoyButtonEvent JoyButton;
        JoyAxisEvent JoyAxis;
    };

    /// Time when this event occured
    TimeType Timestamp;

    /** \brief Binds Events to Lua via luabind **/
    static void Luabind(lua_State* L);
};

} // namespace jar

#endif // JAR_EVENT_HPP
