//=============================================================================
// Copyright (c) 2002 Radical Games Ltd.  All rights reserved.
//=============================================================================
// File: ps3crashhandler.cpp
// PS3 Crash Handler Implementation
//=============================================================================

#include "pch.hpp"
#include <raddebug.hpp>
#include <radmemory.hpp>

#ifdef RAD_PS3

//=============================================================================
// PS3 Crash Handler - Stub Implementation
//=============================================================================

void radCrashHandlerInitialize(radMemoryAllocator allocator)
{
    (void)allocator;
    // PS3 crash handling - could use sys_ppu_thread_get_stack_information
    // or custom exception handling
}

void radCrashHandlerTerminate(void)
{
    // Cleanup
}

#endif // RAD_PS3
