//=============================================================================
// Copyright (c) 2002 Radical Games Ltd.  All rights reserved.
//=============================================================================
// File: stacktrace.cpp (PS3)
// PS3 Stack Trace Implementation - Stub
//=============================================================================

#include "pch.hpp"

#ifdef RAD_PS3

//=============================================================================
// radStackTracePs3Get
// Stub implementation - returns 0 entries
// Full implementation would use PPU stack frame information
//=============================================================================

extern "C" void radStackTracePs3Get(unsigned int* results, int max, void* stackPointer, void* returnAddress)
{
    (void)stackPointer;
    (void)returnAddress;

    // Clear results - no stack trace available in stub
    for (int i = 0; i < max; i++)
    {
        results[i] = 0;
    }
}

// Wrapper for consistent naming
extern "C" void radStackTraceGet(unsigned int* results, int max, void* stackPointer, void* returnAddress)
{
    radStackTracePs3Get(results, max, stackPointer, returnAddress);
}

#endif // RAD_PS3
