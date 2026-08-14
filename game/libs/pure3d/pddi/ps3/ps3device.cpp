/*===========================================================================
    ps3device.cpp

    PS3 PDDI Device Factory Implementation

    Copyright (c)1996-2002 Radical Entertainment Ltd
    All rights reserved.
===========================================================================*/

#ifdef RAD_PS3

#include <pddi/ps3/ps3device.hpp>
#include <pddi/ps3/ps3display.hpp>
#include <pddi/ps3/ps3context.hpp>
#include <pddi/ps3/ps3texture.hpp>
#include <pddi/ps3/ps3prim.hpp>
#include <pddi/ps3/ps3shader.hpp>

#include <string.h>
#include <stdio.h>

// PS3 SDK includes
#include <cell/gcm.h>
#include <sys/memory.h>
#include <sys/ppu_thread.h>

// RSX memory configuration
// NOTE: RSX has its own 256MB video memory accessed via GCM/PSGL, NOT from main RAM
// Staging buffer for CPU-side texture uploads - needs to be large enough to hold
// all textures loaded during bootup since our bump allocator doesn't free
#define RSX_STAGING_SIZE (32 * 1024 * 1024)  // 32MB staging buffer for CPU-side texture data

//-------------------------------------------------------------------
// Static instance
//-------------------------------------------------------------------
ps3Device* ps3Device::sInstance = NULL;

//-------------------------------------------------------------------
// Entry point function (C linkage for external use)
//-------------------------------------------------------------------
extern "C" int pddiCreate(int versionMajor, int versionMinor, pddiDevice** dev)
{
    if (versionMajor != PDDI_VERSION_MAJOR)
    {
        return PDDI_VERSION_ERROR;
    }

    *dev = new ps3Device();
    return PDDI_OK;
}

// Legacy function name for compatibility
pddiDevice* pddiCreatePS3Device(int versionMajor, int versionMinor)
{
    if (versionMajor != PDDI_VERSION_MAJOR)
    {
        return NULL;
    }

    return new ps3Device();
}

//-------------------------------------------------------------------
// ps3Device implementation
//-------------------------------------------------------------------
ps3Device::ps3Device() :
    mDisplay(NULL),
    mContext(NULL),
    mRSXMemoryBase(NULL),
    mRSXMemorySize(0),
    mRSXMemoryUsed(0),
    mRenderThreadID(0)
{
    sInstance = this;

    // Store the current thread as the render thread
    // This should be called from the main thread during initialization
    SetRenderThreadID();

    // Initialize staging buffer for CPU-side texture work
    // RSX video memory is accessed via PSGL/GCM, not from main RAM
    // We only allocate a small staging buffer for CPU->GPU transfers
    mRSXMemorySize = RSX_STAGING_SIZE;
    mRSXMemoryUsed = 0;

    // Allocate a small staging buffer - RSX local memory is managed by PSGL
    mRSXMemoryBase = malloc(mRSXMemorySize);
    if (!mRSXMemoryBase)
    {
        // Staging buffer allocation failed - log and continue without it
        mRSXMemorySize = 0;
    }
}

ps3Device::~ps3Device()
{
    if (mContext)
    {
        delete mContext;
        mContext = NULL;
    }

    if (mDisplay)
    {
        delete mDisplay;
        mDisplay = NULL;
    }

    if (mRSXMemoryBase)
    {
        free(mRSXMemoryBase);
        mRSXMemoryBase = NULL;
    }

    sInstance = NULL;
}

void ps3Device::GetLibraryInfo(pddiLibInfo* info)
{
    info->versionMajor = PDDI_VERSION_MAJOR;
    info->versionMinor = PDDI_VERSION_MINOR;
    info->versionBuild = 0;
    info->libID = PDDI_LIBID_PS3;
    strcpy(info->description, "PS3 PSGL/GCM PDDI Library");
}

void ps3Device::SetCurrentContext(pddiRenderContext* context)
{
    mContext = static_cast<ps3Context*>(context);
}

pddiRenderContext* ps3Device::GetCurrentContext()
{
    return mContext;
}

pddiDisplay* ps3Device::NewDisplay(int id)
{
    if (mDisplay == NULL)
    {
        mDisplay = new ps3Display(this);
    }
    return mDisplay;
}

pddiRenderContext* ps3Device::NewRenderContext(pddiDisplay* display)
{
    ps3Display* ps3Disp = static_cast<ps3Display*>(display);

    if (mContext == NULL)
    {
        mContext = new ps3Context(this, ps3Disp);
    }
    return mContext;
}

pddiTexture* ps3Device::NewTexture(pddiTextureDesc* desc)
{
    ps3Texture* tex = new ps3Texture(this, desc);
    return tex;
}

pddiPrimBuffer* ps3Device::NewPrimBuffer(pddiPrimBufferDesc* desc)
{
    return new ps3PrimBuffer(this, desc);
}

pddiShader* ps3Device::NewShader(const char* name, const char* aux)
{
    // Create appropriate shader based on name
    if (name == NULL || strcmp(name, "simple") == 0)
    {
        return new ps3SimpleShader(this);
    }
    else if (strcmp(name, "error") == 0)
    {
        return new ps3ErrorShader(this);
    }
    // Add more shader types as needed
    // For now, return a simple shader as fallback
    return new ps3SimpleShader(this);
}

void ps3Device::AddCustomShader(const char* name, const char* aux)
{
    // Custom shader registration - implement as needed
}

void* ps3Device::AllocateRSXMemory(unsigned int size, unsigned int alignment)
{
    // Simple bump allocator for RSX memory
    // Real implementation would use proper memory management with GCM

    // Align the current position
    unsigned int alignedPos = (mRSXMemoryUsed + alignment - 1) & ~(alignment - 1);

    if (alignedPos + size > mRSXMemorySize)
    {
        printf("[RSX] OUT OF STAGING MEMORY: need %u bytes, used %u/%u\n",
               size, mRSXMemoryUsed, mRSXMemorySize);
        return NULL;
    }

    void* ptr = (unsigned char*)mRSXMemoryBase + alignedPos;
    mRSXMemoryUsed = alignedPos + size;

    return ptr;
}

void ps3Device::FreeRSXMemory(void* ptr)
{
    // Simple allocator doesn't support individual frees
    // Real implementation would use proper memory management
}

unsigned int ps3Device::GetFreeRSXMemory()
{
    return mRSXMemorySize - mRSXMemoryUsed;
}

unsigned int ps3Device::GetTotalRSXMemory()
{
    return mRSXMemorySize;
}

void ps3Device::SetRenderThreadID()
{
    sys_ppu_thread_t threadId;
    sys_ppu_thread_get_id(&threadId);
    mRenderThreadID = threadId;
    // Render thread ID stored for thread-safety checks in texture binding
}

bool ps3Device::IsRenderThread()
{
    sys_ppu_thread_t threadId;
    sys_ppu_thread_get_id(&threadId);
    return (threadId == mRenderThreadID);
}

#endif // RAD_PS3
