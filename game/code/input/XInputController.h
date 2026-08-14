//=============================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// File:        XInputController.h
//
// Description: XInput-based controller support for modern Xbox controllers
//              on Windows. Works alongside the existing DirectInput system.
//
// History:     Created for Windows 10/11 Xbox controller support
//
//=============================================================================

#ifndef XINPUTCONTROLLER_H
#define XINPUTCONTROLLER_H

#ifdef RAD_WIN32

#include <windows.h>
#include <xinput.h>
#include <input/virtualinputs.hpp>

//=============================================================================
// XInput button and axis mappings
//=============================================================================

// XInput axis indices (matching gamepad layout)
enum eXInputAxis
{
    XINPUT_AXIS_LEFT_X = 0,
    XINPUT_AXIS_LEFT_Y,
    XINPUT_AXIS_RIGHT_X,
    XINPUT_AXIS_RIGHT_Y,
    XINPUT_AXIS_LEFT_TRIGGER,
    XINPUT_AXIS_RIGHT_TRIGGER,
    NUM_XINPUT_AXES
};

//=============================================================================
// Class: XInputController
//=============================================================================
//
// Description: Provides XInput-based controller support. This is a standalone
//              polling-based controller that can be checked each frame.
//              XInput is preferred for Xbox controllers as it provides better
//              support than DirectInput.
//
//=============================================================================

class XInputController
{
public:
    XInputController();
    ~XInputController();

    // Initialize/shutdown
    static void Initialize();
    static void Shutdown();
    static bool IsAvailable();

    // Check if XInput controller is connected at the given port (0-3)
    static bool IsConnected(int port);

    // Poll the controller state
    bool Poll(int port);

    // Get button states (true = pressed)
    bool IsButtonPressed(WORD button) const;
    bool WasButtonPressed(WORD button) const;  // Just pressed this frame
    bool WasButtonReleased(WORD button) const; // Just released this frame

    // Get axis values normalized to -1.0 to 1.0 (sticks) or 0.0 to 1.0 (triggers)
    float GetLeftStickX() const;
    float GetLeftStickY() const;
    float GetRightStickX() const;
    float GetRightStickY() const;
    float GetLeftTrigger() const;
    float GetRightTrigger() const;

    // D-Pad convenience functions
    bool IsDPadUp() const;
    bool IsDPadDown() const;
    bool IsDPadLeft() const;
    bool IsDPadRight() const;

    // Vibration/rumble support
    void SetVibration(float leftMotor, float rightMotor);
    void StopVibration();

    // Get raw state for advanced use
    const XINPUT_STATE& GetState() const { return m_state; }
    const XINPUT_STATE& GetPrevState() const { return m_prevState; }

    // Deadzone configuration
    static void SetStickDeadzone(float deadzone);
    static void SetTriggerDeadzone(float deadzone);
    static float GetStickDeadzone() { return s_stickDeadzone; }
    static float GetTriggerDeadzone() { return s_triggerDeadzone; }

    // Map XInput to the game's virtual button system
    int MapToVirtualButton(WORD xinputButton) const;

private:
    float ApplyDeadzone(float value, float deadzone) const;
    float NormalizeStick(SHORT value) const;
    float NormalizeTrigger(BYTE value) const;

    XINPUT_STATE m_state;
    XINPUT_STATE m_prevState;
    int m_port;
    bool m_connected;

    static bool s_initialized;
    static float s_stickDeadzone;
    static float s_triggerDeadzone;
};

//=============================================================================
// XInput button constants (from XInput.h, for reference)
//=============================================================================
// XINPUT_GAMEPAD_DPAD_UP        0x0001
// XINPUT_GAMEPAD_DPAD_DOWN      0x0002
// XINPUT_GAMEPAD_DPAD_LEFT      0x0004
// XINPUT_GAMEPAD_DPAD_RIGHT     0x0008
// XINPUT_GAMEPAD_START          0x0010
// XINPUT_GAMEPAD_BACK           0x0020
// XINPUT_GAMEPAD_LEFT_THUMB     0x0040
// XINPUT_GAMEPAD_RIGHT_THUMB    0x0080
// XINPUT_GAMEPAD_LEFT_SHOULDER  0x0100
// XINPUT_GAMEPAD_RIGHT_SHOULDER 0x0200
// XINPUT_GAMEPAD_A              0x1000
// XINPUT_GAMEPAD_B              0x2000
// XINPUT_GAMEPAD_X              0x4000
// XINPUT_GAMEPAD_Y              0x8000

#endif // RAD_WIN32

#endif // XINPUTCONTROLLER_H
