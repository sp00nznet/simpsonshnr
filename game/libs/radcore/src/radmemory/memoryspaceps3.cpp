//=============================================================================
// Copyright (c) 2002 Radical Games Ltd.  All rights reserved.
//=============================================================================
// File: memoryspaceps3.cpp
// PS3 Memory Space Implementation
//=============================================================================

#include "pch.hpp"
#include <radmemory.hpp>
#include <radmemorymonitor.hpp>
#include <stdlib.h>
#include <string.h>

#ifdef RAD_PS3

//=============================================================================
// radMemorySpace - PS3 Implementation using standard malloc/free
// For initial port, we use simple system allocation
//=============================================================================

// PS3 optimal alignment settings
// Using 128-byte alignment for cache line size
unsigned int radMemorySpace_OptimalMultiple = 128;
unsigned int radMemorySpace_OptimalAlignment = 128;

//============================================================================
// struct MemorySpaceAsyncRequest_Copy
//============================================================================

// This is just a placeholder object that always returns true when asked if it
// is done all copies are synchronous on PS3.

struct MemorySpaceAsyncRequest_Copy
    :
    public IRadMemorySpaceCopyRequest
{
    MemorySpaceAsyncRequest_Copy( void ) : m_RefCount( 0 ) {}
    virtual ~MemorySpaceAsyncRequest_Copy( void ) {}

    virtual void AddRef( void ) { m_RefCount++; }
    virtual void Release( void ) { if (m_RefCount > 0) m_RefCount--; }
    virtual bool IsDone( void ) { return true; }

    unsigned int m_RefCount;
};

static MemorySpaceAsyncRequest_Copy g_MemorySpaceAsyncRequest_Copy_Placeholder;

void radMemorySpaceInitialize(void)
{
    // PS3 initialization - use system allocator
}

void radMemorySpaceTerminate(void)
{
    // Cleanup
}

//============================================================================
// radMemorySpaceCopyAsync
//============================================================================

IRadMemorySpaceCopyRequest * radMemorySpaceCopyAsync
(
    void * pDest,
    radMemorySpace spaceDest,
    const void * pSrc,
    radMemorySpace spaceSrc,
    unsigned int bytes
)
{
    rAssert( pDest != NULL );
    rAssert( pSrc != NULL );
    rAssert( bytes > 0 );

    ::memcpy( pDest, pSrc, bytes );

    return & g_MemorySpaceAsyncRequest_Copy_Placeholder;
}

//============================================================================
// radMemorySpaceAlloc - two parameter version
//============================================================================

void* radMemorySpaceAlloc(radMemorySpace space, unsigned int size)
{
    (void)space; // Ignore space for now, use system allocator
    return malloc(size);
}

//============================================================================
// radMemorySpaceAlloc - three parameter version
//============================================================================

void * radMemorySpaceAlloc( radMemorySpace space, radMemoryAllocator allocator, unsigned int numBytes )
{
    (void)space;
    return ::radMemoryAlloc( allocator, numBytes );
}

//============================================================================
// radMemorySpaceAllocAligned
//============================================================================

void * radMemorySpaceAllocAligned
(
    radMemorySpace space,
    radMemoryAllocator allocator,
    unsigned int numBytes,
    unsigned int alignment
)
{
    (void)space;
    return ::radMemoryAllocAligned( allocator, numBytes, alignment );
}

//============================================================================
// radMemorySpaceGetAllocator
//============================================================================

IRadMemoryAllocator * radMemorySpaceGetAllocator( radMemorySpace memSpace, radMemoryAllocator allocator )
{
    (void)memSpace;
    return ::radMemoryGetAllocator( allocator );
}

//============================================================================
// radMemorySpaceFree - two parameter version
//============================================================================

void radMemorySpaceFree(radMemorySpace space, void* pMemory)
{
    (void)space;
    free(pMemory);
}

//============================================================================
// radMemorySpaceFree - three parameter version
//============================================================================

void radMemorySpaceFree
(
    radMemorySpace space,
    radMemoryAllocator allocator,
    void * pMemory
)
{
    (void)space;
    ::radMemoryFree( allocator, pMemory );
}

//============================================================================
// radMemorySpaceFreeAligned
//============================================================================

void radMemorySpaceFreeAligned
(
    radMemorySpace space,
    radMemoryAllocator allocator,
    void * pMemory
)
{
    (void)space;
    ::radMemoryFreeAligned( allocator, pMemory );
}

void* radMemorySpaceRealloc(radMemorySpace space, void* pMemory, unsigned int size)
{
    (void)space;
    return realloc(pMemory, size);
}

unsigned int radMemorySpaceGetFreeMemory(radMemorySpace space)
{
    (void)space;
    // Return a large value - actual implementation would query system
    return 256 * 1024 * 1024; // 256MB
}

#endif // RAD_PS3
