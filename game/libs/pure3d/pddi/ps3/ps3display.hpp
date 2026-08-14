/*===========================================================================
    ps3display.hpp

    PS3 PDDI Display Management

    Copyright (c)1996-2002 Radical Entertainment Ltd
    All rights reserved.
===========================================================================*/

#ifndef _PS3DISPLAY_HPP
#define _PS3DISPLAY_HPP

#include <pddi/pddi.hpp>
#include <pddi/pddips3.hpp>

class ps3Device;

//-------------------------------------------------------------------
// ps3Display - PS3 display management using RSX
//-------------------------------------------------------------------
class ps3Display : public pddiDisplay
{
public:
    ps3Display(ps3Device* device);
    virtual ~ps3Display();

    // pddiDisplay interface
    bool InitDisplay(int x, int y, int bpp);
    bool InitDisplay(const pddiDisplayInit* initData);

    int GetHeight();
    int GetWidth();
    int GetDepth();
    pddiDisplayMode GetDisplayMode();
    int GetNumColourBuffer();
    unsigned GetBufferMask();

    unsigned GetFreeTextureMem();

    void SwapBuffers(void);

    unsigned Screenshot(pddiColour* buffer, int nBytes);

    bool IsWidescreen(void);

    // PS3-specific methods
    void* GetColorBuffer(int index);
    void* GetDepthBuffer();
    unsigned int GetColorBufferPitch();

    // PSGL context (if using PSGL)
    void* GetPSGLContext() { return mPSGLContext; }

private:
    ps3Device* mDevice;

    int mWidth;
    int mHeight;
    int mBpp;
    bool mVSync;
    bool mWidescreen;

    // Frame buffers
    void* mColorBuffer[2];
    void* mDepthBuffer;
    int mCurrentBuffer;
    unsigned int mColorBufferPitch;
    unsigned int mDepthBufferPitch;

    // PSGL context
    void* mPSGLContext;
    void* mPSGLDevice;

    // GCM context (if using GCM directly)
    void* mGcmContext;

    bool InitPSGL(int width, int height);
    void TerminatePSGL();

    bool InitGCM(int width, int height);
    void TerminateGCM();

    void SetupFrameBuffers(int width, int height);
};

#endif /* _PS3DISPLAY_HPP */
