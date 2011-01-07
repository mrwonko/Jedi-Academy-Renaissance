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

#ifndef JAR_INPUT_WINDOWS_WINJOYSTICKDIRECTINPUT_H
#define JAR_INPUT_WINDOWS_WINJOYSTICKDIRECTINPUT_H

#define WINVER 0x0500 //I need some functions only available from windows 0x0500 onwards
#ifndef  WIN32_LEAN_AND_MEAN      // This cleans out rarely used stuff
#define  WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

//if you change this, also change it in InputImpl.hpp
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <cmath>

#include "jar/input/InputDeviceJoystick.hpp"

namespace jar {
namespace Windows {

/** \brief Gamepad/Joystick with up to 4 DPads, 128 Buttons and 32 Axes. Hope that's enough. ;-)

    \note The buttons 0-15 are reserved for DPad up (0/4/8/12), down (1/5/9/13), left (2/6/10/14), right (3/7/11/15).
**/
class WinJoystickDirectInput : public jar::InputDeviceJoystick, public Updatable
{
    public:
        WinJoystickDirectInput();
        virtual ~WinJoystickDirectInput();

        const bool Init(LPDIRECTINPUT8 directInput, LPCDIDEVICEINSTANCE deviceInfo);

        virtual const bool Deinit();

        virtual void Update(TimeType deltaT);

        void OnFirstWindowCreated(HWND hwnd);

        struct AxisRangeInfo
        {
            int min;
            int max;
        };

        enum Axis
        {
            AxisX,
            AxisY,
            AxisRX,
            AxisRY,
            AxisZ,
            AxisRZ,
            AxisSlider0,
            AxisSlider1,
            AxisVX,
            AxisVY,
            AxisVZ,
            AxisVRX,
            AxisVRY,
            AxisVRZ,
            AxisVSlider0,
            AxisVSlider1,
            AxisAX,
            AxisAY,
            AxisAZ,
            AxisARX,
            AxisARY,
            AxisARZ,
            AxisASlider0,
            AxisASlider1,
            AxisFX,
            AxisFY,
            AxisFZ,
            AxisFRX,
            AxisFRY,
            AxisFRZ,
            AxisFSlider0,
            AxisFSlider1,
            AxisLAST //keep last
        };
    protected:
    private:
        LPDIRECTINPUTDEVICE8 mDevice;
        GUID mGUID;

        // I don't trust these so I check everything in Update() - because how can I tell /which/ axes are available? Are the two available buttons 0 and 1 or 7 and 41?
        unsigned int mNumAxes;
        unsigned int mNumButtons;
        unsigned int mNumPOVs;



        AxisRangeInfo mRangeInfo[AxisLAST];


        DIJOYSTATE2 mLastState;

        void CheckButtonChange(unsigned int buttonIndex, BYTE oldState, BYTE newState);
        void CheckAxisChange(Axis axisIndex, LONG oldState, LONG newState);
        void CheckPOVChange(unsigned int povIndex, DWORD oldState, DWORD newState);

        const bool POVIsUp(int angle) { return angle != -1 && abs(angle) <= 4500; }
        const bool POVIsRight(int angle) { return angle != -1 && abs(angle - 9000) <= 4500; }
        const bool POVIsDown(int angle) { return angle != -1 && abs(angle - 18000) <= 4500; }
        const bool POVIsLeft(int angle) { return angle != -1 && abs(angle - 27000) <= 4500; }

        void UpdateRumblers();
        std::vector<float> mLastRumbleStrengths;
        GUID mRumbleEffectGUID;
        std::vector<LPDIRECTINPUTEFFECT> mRumbleEffects;

        std::vector<DWORD> mFFAxes;

        //whether this is acquired in exclusive mode yet
        bool mIsExclusive;
};

} // namespace Windows
} // namespace jar

#endif // JAR_INPUT_WINDOWS_WINJOYSTICKDIRECTINPUT_H
