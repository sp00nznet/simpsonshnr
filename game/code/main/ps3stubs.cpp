//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        ps3stubs.cpp
//
// Description: Stub implementations for PS3-specific missing symbols
//
//=============================================================================

#ifdef RAD_PS3

#include <stdlib.h>
#include <string.h>
#include <radcontroller.hpp>

// Stub for std::exception::_Raise()
// This is referenced by some code compiled with exception support,
// but we compile with -fno-exceptions so we just abort if called
namespace std
{
    class exception;
}

extern "C" void _ZNKSt9exception6_RaiseEv(void)
{
    // If exceptions are thrown when compiled with -fno-exceptions,
    // the best we can do is abort
    abort();
}

//=============================================================================
// PS3 Controller Stub Classes
//=============================================================================

// Stub input point - returns 0 for all inputs
class PS3StubInputPoint : public IRadControllerInputPoint
{
public:
    PS3StubInputPoint(const char* name, const char* type) : m_refCount(1)
    {
        strncpy(m_name, name, 31);
        m_name[31] = 0;
        strncpy(m_type, type, 31);
        m_type[31] = 0;
    }

    virtual void AddRef() { m_refCount++; }
    virtual void Release() { if (--m_refCount == 0) delete this; }

    virtual const char* GetName() { return m_name; }
    virtual const char* GetType() { return m_type; }
    virtual void SetTolerance(float percentage) {}
    virtual float GetTolerance() { return 0.1f; }
    virtual void RegisterControllerInputPointCallback(IRadControllerInputPointCallback* pCallback, unsigned int userData) {}
    virtual void UnRegisterControllerInputPointCallback(IRadControllerInputPointCallback* pCallback) {}
    virtual float GetCurrentValue(unsigned int* pTime = NULL) { if (pTime) *pTime = 0; return 0.0f; }
    virtual void SetRange(float min, float max) {}
    virtual void GetRange(float* pMin, float* pMax) { if (pMin) *pMin = -1.0f; if (pMax) *pMax = 1.0f; }

private:
    int m_refCount;
    char m_name[32];
    char m_type[32];
};

// Stub output point - does nothing
class PS3StubOutputPoint : public IRadControllerOutputPoint
{
public:
    PS3StubOutputPoint(const char* name, const char* type) : m_refCount(1), m_gain(0.0f)
    {
        strncpy(m_name, name, 31);
        m_name[31] = 0;
        strncpy(m_type, type, 31);
        m_type[31] = 0;
    }

    virtual void AddRef() { m_refCount++; }
    virtual void Release() { if (--m_refCount == 0) delete this; }

    virtual const char* GetName() { return m_name; }
    virtual const char* GetType() { return m_type; }
    virtual float GetGain() { return m_gain; }
    virtual void SetGain(float value) { m_gain = value; }

private:
    int m_refCount;
    char m_name[32];
    char m_type[32];
    float m_gain;
};

// Stub controller - simulates a DualShock-like controller
class PS3StubController : public IRadController
{
public:
    PS3StubController() : m_refCount(1)
    {
        // Create standard PS3 controller input points
        m_inputPoints[0] = new PS3StubInputPoint("LeftStickX", "XAxis");
        m_inputPoints[1] = new PS3StubInputPoint("LeftStickY", "YAxis");
        m_inputPoints[2] = new PS3StubInputPoint("RightStickX", "XAxis");
        m_inputPoints[3] = new PS3StubInputPoint("RightStickY", "YAxis");
        m_inputPoints[4] = new PS3StubInputPoint("X", "Button");
        m_inputPoints[5] = new PS3StubInputPoint("Circle", "Button");
        m_inputPoints[6] = new PS3StubInputPoint("Square", "Button");
        m_inputPoints[7] = new PS3StubInputPoint("Triangle", "Button");
        m_inputPoints[8] = new PS3StubInputPoint("L1", "Button");
        m_inputPoints[9] = new PS3StubInputPoint("L2", "AnalogButton");
        m_inputPoints[10] = new PS3StubInputPoint("R1", "Button");
        m_inputPoints[11] = new PS3StubInputPoint("R2", "AnalogButton");
        m_inputPoints[12] = new PS3StubInputPoint("Start", "Button");
        m_inputPoints[13] = new PS3StubInputPoint("Select", "Button");
        m_inputPoints[14] = new PS3StubInputPoint("DPadUp", "Button");
        m_inputPoints[15] = new PS3StubInputPoint("DPadDown", "Button");
        m_inputPoints[16] = new PS3StubInputPoint("DPadLeft", "Button");
        m_inputPoints[17] = new PS3StubInputPoint("DPadRight", "Button");
        m_inputPoints[18] = new PS3StubInputPoint("L3", "Button");
        m_inputPoints[19] = new PS3StubInputPoint("R3", "Button");
        m_numInputPoints = 20;

        // Create output points (rumble motors)
        // Include both PS2-style names (SmallMotor/LargeMotor) and generic names (LeftMotor/RightMotor)
        // to ensure compatibility with all code paths
        m_outputPoints[0] = new PS3StubOutputPoint("SmallMotor", "Digital");
        m_outputPoints[1] = new PS3StubOutputPoint("LargeMotor", "Analog");
        m_outputPoints[2] = new PS3StubOutputPoint("LeftMotor", "Digital");
        m_outputPoints[3] = new PS3StubOutputPoint("RightMotor", "Analog");
        m_numOutputPoints = 4;
    }

    virtual ~PS3StubController()
    {
        for (unsigned int i = 0; i < m_numInputPoints; i++)
            m_inputPoints[i]->Release();
        for (unsigned int i = 0; i < m_numOutputPoints; i++)
            m_outputPoints[i]->Release();
    }

    virtual void AddRef() { m_refCount++; }
    virtual void Release() { if (--m_refCount == 0) delete this; }

    virtual bool IsConnected() { return true; }
    virtual const char* GetType() { return "PS3Controller"; }
    virtual const char* GetClassification() { return "Joystick"; }
    // PS3 uses "Channel%d" format (same as GameCube) for controller locations
    virtual const char* GetLocation() { return "Channel0"; }

    virtual unsigned int GetNumberOfInputPointsOfType(const char* pType)
    {
        unsigned int count = 0;
        for (unsigned int i = 0; i < m_numInputPoints; i++)
            if (strcmp(m_inputPoints[i]->GetType(), pType) == 0) count++;
        return count;
    }

    virtual unsigned int GetNumberOfOutputPointsOfType(const char* pType)
    {
        unsigned int count = 0;
        for (unsigned int i = 0; i < m_numOutputPoints; i++)
            if (strcmp(m_outputPoints[i]->GetType(), pType) == 0) count++;
        return count;
    }

    virtual IRadControllerInputPoint* GetInputPointByTypeAndIndex(const char* pType, unsigned int indx)
    {
        unsigned int count = 0;
        for (unsigned int i = 0; i < m_numInputPoints; i++)
        {
            if (strcmp(m_inputPoints[i]->GetType(), pType) == 0)
            {
                if (count == indx) return m_inputPoints[i];
                count++;
            }
        }
        return NULL;
    }

    virtual IRadControllerOutputPoint* GetOutputPointByTypeAndIndex(const char* pType, unsigned int indx)
    {
        unsigned int count = 0;
        for (unsigned int i = 0; i < m_numOutputPoints; i++)
        {
            if (strcmp(m_outputPoints[i]->GetType(), pType) == 0)
            {
                if (count == indx) return m_outputPoints[i];
                count++;
            }
        }
        return NULL;
    }

    virtual IRadControllerInputPoint* GetInputPointByName(const char* pName)
    {
        for (unsigned int i = 0; i < m_numInputPoints; i++)
            if (strcmp(m_inputPoints[i]->GetName(), pName) == 0)
                return m_inputPoints[i];
        return NULL;
    }

    virtual IRadControllerOutputPoint* GetOutputPointByName(const char* pName)
    {
        for (unsigned int i = 0; i < m_numOutputPoints; i++)
            if (strcmp(m_outputPoints[i]->GetName(), pName) == 0)
                return m_outputPoints[i];
        return NULL;
    }

    virtual unsigned int GetNumberOfInputPoints() { return m_numInputPoints; }
    virtual IRadControllerInputPoint* GetInputPointByIndex(unsigned int indx)
    {
        if (indx < m_numInputPoints) return m_inputPoints[indx];
        return NULL;
    }

    virtual unsigned int GetNumberOfOutputPoints() { return m_numOutputPoints; }
    virtual IRadControllerOutputPoint* GetOutputPointByIndex(unsigned int indx)
    {
        if (indx < m_numOutputPoints) return m_outputPoints[indx];
        return NULL;
    }

private:
    int m_refCount;
    PS3StubInputPoint* m_inputPoints[32];
    unsigned int m_numInputPoints;
    PS3StubOutputPoint* m_outputPoints[4];
    unsigned int m_numOutputPoints;
};

// Stub controller system
class PS3StubControllerSystem : public IRadControllerSystem
{
public:
    PS3StubControllerSystem() : m_refCount(1), m_controller(NULL)
    {
        m_controller = new PS3StubController();
    }

    virtual ~PS3StubControllerSystem()
    {
        if (m_controller) m_controller->Release();
    }

    virtual void AddRef() { m_refCount++; }
    virtual void Release() { if (--m_refCount == 0) delete this; }

    virtual unsigned int GetNumberOfControllers() { return 1; }
    virtual IRadController* GetControllerByIndex(unsigned int indx)
    {
        if (indx == 0) return m_controller;
        return NULL;
    }

    virtual IRadController* GetControllerAtLocation(const char* pLocation)
    {
        return m_controller;
    }

    virtual void SetBufferTime(unsigned int milliseconds) {}
    virtual void MapVirtualTime(unsigned int timerManagerMs, unsigned int virtualTicks) {}
    virtual void SetVirtualTime(unsigned int virtualTicks) {}
    virtual void SetCaptureRate(unsigned int ms) {}
    virtual void RegisterConnectionChangeCallback(IRadControllerConnectionChangeCallback* pCallback) {}
    virtual void UnRegisterConnectionChangeCallback(IRadControllerConnectionChangeCallback* pCallback) {}

private:
    int m_refCount;
    PS3StubController* m_controller;
};

// Global controller system instance
static PS3StubControllerSystem* s_pControllerSystem = NULL;
static IRadControllerConnectionChangeCallback* s_pConnectionCallback = NULL;

void radControllerInitialize(IRadControllerConnectionChangeCallback* pCallback, radMemoryAllocator allocator)
{
    s_pConnectionCallback = pCallback;
    if (s_pControllerSystem == NULL)
    {
        s_pControllerSystem = new PS3StubControllerSystem();
    }
}

void radControllerTerminate(void)
{
    if (s_pControllerSystem)
    {
        s_pControllerSystem->Release();
        s_pControllerSystem = NULL;
    }
    s_pConnectionCallback = NULL;
}

void radControllerSystemService(void)
{
    // PS3 controller polling would go here
}

IRadControllerSystem* radControllerSystemGet(void)
{
    return s_pControllerSystem;
}

#endif // RAD_PS3
