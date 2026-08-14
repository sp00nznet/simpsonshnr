//=============================================================================
// Copyright (c) 2002 Radical Games Ltd.  All rights reserved.
//=============================================================================
// File: ps3controller.cpp
// PS3 Controller Implementation - Stub
//=============================================================================

#include "pch.hpp"
#include <radcontroller.hpp>
#include <raddebug.hpp>

#ifdef RAD_PS3

//=============================================================================
// PS3 Controller System Implementation
// Minimal stub implementation for initial port
// Full implementation would use libpad
//=============================================================================

static bool g_PS3ControllerInitialized = false;

void radControllerSystemCreate(radMemoryAllocator allocator)
{
    (void)allocator;

    // Stub - just mark as initialized
    g_PS3ControllerInitialized = true;
}

void radControllerSystemTerminate(void)
{
    g_PS3ControllerInitialized = false;
}

IRadControllerSystem* radControllerSystemGet(void)
{
    // Return NULL for stub - full implementation needed
    return NULL;
}

#endif // RAD_PS3
