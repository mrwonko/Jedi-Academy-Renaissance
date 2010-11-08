#ifndef JAR_INPUTDEVICEMANAGER_HPP
#define JAR_INPUTDEVICEMANAGER_HPP

#include "jar/EventQueue.hpp"
#include "jar/Time.hpp"
#include <set>

namespace jar
{

class InputDevice;
class InputDeviceManager : public EventQueue
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
        void Deinit();

        /** \brief Updates the manager and its managed devices.

            Should be called every frame.

            \param deltaT time since last frame.
        **/
        void Update(TimeType deltaT);

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
            \return Whether the device existed in the manager. Even if it didn't it still got deleted.
        **/
        const bool DeleteInputDevice(InputDevice* device);

    private:
        std::set<InputDevice*> mDevices;
        bool mInitialized;
};

} // namespace jar

#endif // JAR_INPUTDEVICEMANAGER_HPP
