#ifndef JAR_INPUT_INPUTDEVICEMANAGER_HPP
#define JAR_INPUT_INPUTDEVICEMANAGER_HPP

#include "jar/input/EventSender.hpp"
#include "jar/input/EventListener.hpp"
#include "jar/core/Time.hpp"
#include "jar/input/API.hpp"
#include <set>
#include <list>

class lua_State;

namespace jar
{

class InputDevice;
class InputDeviceJoystick;
class JARINPUTAPI InputDeviceManager : public EventListener
{
    public:
        /** \brief Constructor **/
        InputDeviceManager();
        /** \brief Destructor **/
        ~InputDeviceManager();

        /** \brief Initializes the device manager. <b>Does not create any devices!</b>

            \return success
        **/
        const bool Init();

        /** \brief Deinitializes the device manager. If you don't call it, it's called in the destructor.
        **/
        const bool Deinit();

        /** \brief Adds an input device to the manager.

            \param device The device to be added

        **/
        void AddInputDevice(InputDevice* device);

        /** \brief Removes an input device from the manager. <b>Does not delete the device!</b>

            \param device The InputDevice
            \return Whether the device existed

        **/
        const bool RemoveInputDevice(InputDevice* device);

        /** \brief Deinitializes, deletes and removes an InputDevice from the manager.
            \param device The InputDevice
            \return Whether the device existed in the manager. Even if it didn't it still gets deleted.
        **/
        const bool DeleteInputDevice(InputDevice* device);

        /** \brief Retrieve a joystick of a given index
            \return The joystick, or NULL if no such joystick exists. **/
        InputDeviceJoystick* GetJoystick(const unsigned int index) const;

        /** \note For luabind to be able to work with the returned list, it apparently mustn't be an object since Luabind only takes iterator ownership. **/
        std::list<InputDeviceJoystick*>& GetAllJoysticks() const;

        virtual void ReceiveEvent(const Event& event);

        static void Luabind(lua_State* state);
    private:
        std::set<InputDevice*> mDevices;
        bool mInitialized;
};

} // namespace jar

#endif // JAR_INPUT_INPUTDEVICEMANAGER_HPP
