/*===========================================================================
    ps3device.hpp

    PS3 PDDI Device Factory

    Copyright (c)1996-2002 Radical Entertainment Ltd
    All rights reserved.
===========================================================================*/

#ifndef _PS3DEVICE_HPP
#define _PS3DEVICE_HPP

#include <pddi/pddi.hpp>
#include <pddi/pddips3.hpp>

class ps3Display;
class ps3Context;

//-------------------------------------------------------------------
// ps3Device - Factory class for PS3 PDDI objects
//-------------------------------------------------------------------
class ps3Device : public pddiDevice
{
public:
    ps3Device();
    virtual ~ps3Device();

    // pddiDevice interface
    void GetLibraryInfo(pddiLibInfo* info);
    void SetCurrentContext(pddiRenderContext* context);
    pddiRenderContext* GetCurrentContext();

    pddiDisplay* NewDisplay(int id);
    pddiRenderContext* NewRenderContext(pddiDisplay* display);
    pddiTexture* NewTexture(pddiTextureDesc* desc);
    pddiPrimBuffer* NewPrimBuffer(pddiPrimBufferDesc* desc);
    pddiShader* NewShader(const char* name, const char* aux = NULL);

    void AddCustomShader(const char* name, const char* aux);

    // PS3-specific methods
    static ps3Device* GetInstance() { return sInstance; }

    // RSX memory management
    void* AllocateRSXMemory(unsigned int size, unsigned int alignment = 16);
    void FreeRSXMemory(void* ptr);
    unsigned int GetFreeRSXMemory();
    unsigned int GetTotalRSXMemory();
    void* GetRSXMemoryBase() { return mRSXMemoryBase; }

    // Thread safety for GL operations
    void SetRenderThreadID();
    bool IsRenderThread();

private:
    static ps3Device* sInstance;

    ps3Display* mDisplay;
    ps3Context* mContext;

    // RSX memory pool
    void* mRSXMemoryBase;
    unsigned int mRSXMemorySize;
    unsigned int mRSXMemoryUsed;

    // Render thread ID for GL thread safety
    unsigned long long mRenderThreadID;
};

#endif /* _PS3DEVICE_HPP */
