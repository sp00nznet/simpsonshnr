//=============================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// File:        XInputController.cpp
//
// Description: XInput-based controller support implementation
//
//=============================================================================

#include <input/XInputController.h>

#ifdef RAD_WIN32

#include <math.h>

// Link with XInput library
#pragma comment(lib, "xinput.lib")

//=============================================================================
// Static member initialization
//=============================================================================
bool XInputController::s_initialized = false;
float XInputController::s_stickDeadzone = 0.24f;  // Default Xbox deadzone
float XInputController::s_triggerDeadzone = 0.12f;

//=============================================================================
// Constructor/Destructor
//=============================================================================
XInputController::XInputController()
    : m_port(-1)
    , m_connected(false)
{
    memset(&m_state, 0, sizeof(m_state));
    memset(&m_prevState, 0, sizeof(m_prevState));
}

XInputController::~XInputController()
{
    StopVibration();
}

//=============================================================================
// Static initialization
//=============================================================================
void XInputController::Initialize()
{
    if (!s_initialized)
    {
        s_initialized = true;
    }
}

void XInputController::Shutdown()
{
    s_initialized = false;
}

bool XInputController::IsAvailable()
{
    return s_initialized;
}

//=============================================================================
// Connection check
//=============================================================================
bool XInputController::IsConnected(int port)
{
    if (port < 0 || port > 3)
        return false;

    XINPUT_STATE state;
    DWORD result = XInputGetState(port, &state);
    return (result == ERROR_SUCCESS);
}

//=============================================================================
// Polling
//=============================================================================
bool XInputController::Poll(int port)
{
    if (port < 0 || port > 3)
    {
        m_connected = false;
        return false;
    }

    m_port = port;
    m_prevState = m_state;

    DWORD result = XInputGetState(port, &m_state);
    m_connected = (result == ERROR_SUCCESS);

    return m_connected;
}

//=============================================================================
// Button state queries
//=============================================================================
bool XInputController::IsButtonPressed(WORD button) const
{
    return (m_state.Gamepad.wButtons & button) != 0;
}

bool XInputController::WasButtonPressed(WORD button) const
{
    bool wasPressed = (m_prevState.Gamepad.wButtons & button) != 0;
    bool isPressed = (m_state.Gamepad.wButtons & button) != 0;
    return isPressed && !wasPressed;
}

bool XInputController::WasButtonReleased(WORD button) const
{
    bool wasPressed = (m_prevState.Gamepad.wButtons & button) != 0;
    bool isPressed = (m_state.Gamepad.wButtons & button) != 0;
    return !isPressed && wasPressed;
}

//=============================================================================
// Axis values
//=============================================================================
float XInputController::GetLeftStickX() const
{
    return ApplyDeadzone(NormalizeStick(m_state.Gamepad.sThumbLX), s_stickDeadzone);
}

float XInputController::GetLeftStickY() const
{
    return ApplyDeadzone(NormalizeStick(m_state.Gamepad.sThumbLY), s_stickDeadzone);
}

float XInputController::GetRightStickX() const
{
    return ApplyDeadzone(NormalizeStick(m_state.Gamepad.sThumbRX), s_stickDeadzone);
}

float XInputController::GetRightStickY() const
{
    return ApplyDeadzone(NormalizeStick(m_state.Gamepad.sThumbRY), s_stickDeadzone);
}

float XInputController::GetLeftTrigger() const
{
    float value = NormalizeTrigger(m_state.Gamepad.bLeftTrigger);
    return (value > s_triggerDeadzone) ? (value - s_triggerDeadzone) / (1.0f - s_triggerDeadzone) : 0.0f;
}

float XInputController::GetRightTrigger() const
{
    float value = NormalizeTrigger(m_state.Gamepad.bRightTrigger);
    return (value > s_triggerDeadzone) ? (value - s_triggerDeadzone) / (1.0f - s_triggerDeadzone) : 0.0f;
}

//=============================================================================
// D-Pad
//=============================================================================
bool XInputController::IsDPadUp() const
{
    return IsButtonPressed(XINPUT_GAMEPAD_DPAD_UP);
}

bool XInputController::IsDPadDown() const
{
    return IsButtonPressed(XINPUT_GAMEPAD_DPAD_DOWN);
}

bool XInputController::IsDPadLeft() const
{
    return IsButtonPressed(XINPUT_GAMEPAD_DPAD_LEFT);
}

bool XInputController::IsDPadRight() const
{
    return IsButtonPressed(XINPUT_GAMEPAD_DPAD_RIGHT);
}

//=============================================================================
// Vibration
//=============================================================================
void XInputController::SetVibration(float leftMotor, float rightMotor)
{
    if (m_port < 0 || m_port > 3 || !m_connected)
        return;

    XINPUT_VIBRATION vibration;
    vibration.wLeftMotorSpeed = (WORD)(leftMotor * 65535.0f);
    vibration.wRightMotorSpeed = (WORD)(rightMotor * 65535.0f);
    XInputSetState(m_port, &vibration);
}

void XInputController::StopVibration()
{
    if (m_port >= 0 && m_port <= 3)
    {
        XINPUT_VIBRATION vibration = { 0, 0 };
        XInputSetState(m_port, &vibration);
    }
}

//=============================================================================
// Deadzone configuration
//=============================================================================
void XInputController::SetStickDeadzone(float deadzone)
{
    s_stickDeadzone = deadzone;
    if (s_stickDeadzone < 0.0f) s_stickDeadzone = 0.0f;
    if (s_stickDeadzone > 0.9f) s_stickDeadzone = 0.9f;
}

void XInputController::SetTriggerDeadzone(float deadzone)
{
    s_triggerDeadzone = deadzone;
    if (s_triggerDeadzone < 0.0f) s_triggerDeadzone = 0.0f;
    if (s_triggerDeadzone > 0.9f) s_triggerDeadzone = 0.9f;
}

//=============================================================================
// Helper functions
//=============================================================================
float XInputController::ApplyDeadzone(float value, float deadzone) const
{
    float absValue = (value < 0.0f) ? -value : value;

    if (absValue < deadzone)
        return 0.0f;

    // Scale the remaining range to 0-1
    float sign = (value < 0.0f) ? -1.0f : 1.0f;
    return sign * (absValue - deadzone) / (1.0f - deadzone);
}

float XInputController::NormalizeStick(SHORT value) const
{
    // Normalize from -32768..32767 to -1..1
    if (value < 0)
        return (float)value / 32768.0f;
    else
        return (float)value / 32767.0f;
}

float XInputController::NormalizeTrigger(BYTE value) const
{
    // Normalize from 0..255 to 0..1
    return (float)value / 255.0f;
}

//=============================================================================
// Virtual button mapping
//=============================================================================
int XInputController::MapToVirtualButton(WORD xinputButton) const
{
    // Map XInput buttons to the game's virtual button indices
    // These mappings correspond to the InputManager::eButtonMap enum for Win32

    switch (xinputButton)
    {
    case XINPUT_GAMEPAD_A:              return 5;  // Jump (X on PS2)
    case XINPUT_GAMEPAD_B:              return 6;  // Sprint (Circle)
    case XINPUT_GAMEPAD_X:              return 4;  // Attack (Square)
    case XINPUT_GAMEPAD_Y:              return 7;  // DoAction (Triangle)
    case XINPUT_GAMEPAD_LEFT_SHOULDER:  return 18; // CameraZoom (L1)
    case XINPUT_GAMEPAD_RIGHT_SHOULDER: return 13; // HandBrake (R1)
    case XINPUT_GAMEPAD_BACK:           return 21; // feBack (Select)
    case XINPUT_GAMEPAD_START:          return 26; // feSelect (Start)
    case XINPUT_GAMEPAD_LEFT_THUMB:     return 14; // Horn (L3)
    case XINPUT_GAMEPAD_RIGHT_THUMB:    return 15; // ResetCar (R3)
    case XINPUT_GAMEPAD_DPAD_UP:        return 0;  // MoveUp
    case XINPUT_GAMEPAD_DPAD_DOWN:      return 1;  // MoveDown
    case XINPUT_GAMEPAD_DPAD_LEFT:      return 2;  // MoveLeft
    case XINPUT_GAMEPAD_DPAD_RIGHT:     return 3;  // MoveRight
    default:
        return -1;
    }
}

#endif // RAD_WIN32
