#ifndef JAR_INPUT_INPUTDEVICEJOYSTICK_HPP
#define JAR_INPUT_INPUTDEVICEJOYSTICK_HPP

#include "jar/input/InputDevice.hpp"
#include "jar/input/API.hpp"

#include <vector>

class lua_State;

namespace jar {

class JARINPUTAPI InputDeviceController : public InputDevice
{
    public:
        /** \brief Destructor **/
        virtual ~InputDeviceController();

        virtual const DeviceType GetDeviceType() const { return DT_Controller; }

        const unsigned int GetNumRumblers() const;

        /** \param value in range [0, 1]
        **/
        void SetRumbleStrength(unsigned int index, float value);
        /** \param value in range [0, 1]
        **/
        void IncreaseRumbleStrength(unsigned int index, float value);
        /** \param value in range [0, 1]
        **/
        void DecreaseRumbleStrength(unsigned int index, float value);

        /** \brief Enable or disable rumbling **/
        void EnableRumble(const bool enable) { mRumbleEnabled = enable; }

        const unsigned int GetIndex() const { return mIndex; }

        /** \note While two Joysticks may have the same name, they'll never have the same UID. **/
        virtual std::string GetUniqueID() const = 0;

        static void Luabind(lua_State* L);

    protected:

        unsigned int mIndex;

        /** \brief Whether rumbling is enabled (default: true) **/
        bool mRumbleEnabled;

        /** \brief makes sure we don't get errors due to accessing invalid stuff **/
        void CheckRumbleStrengths();

        /** Joysticks should check this value during their Update() and use it.
            \note values may exceed 1.0f, keep that in mind! **/
        std::vector<float> mRumbleStrengths;

        /// Amount of rumble motors in this gamepad - should be set during Init() or somewhere else, or defaults to 0.
        unsigned int mNumRumblers;

        /** \brief Constructor **/
        InputDeviceController(unsigned int index);

        /** \brief Sends a JoyButtonPressed Event.
            \param button index of the pressed button **/
        void JoyButtonPressed(unsigned int button);

        /** \brief Sends a MouseButtonReleased Event.
            \param button index of the released button **/
        void JoyButtonReleased(unsigned int button);

        /** \brief Sends a JoyAxisMoved Event. **/
        void JoyAxisMoved(unsigned int axis, float position);
};

} // namespace jar

#endif // JAR_INPUTDEVICEJOYSTICK_HPP
