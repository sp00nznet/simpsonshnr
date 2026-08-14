/*===========================================================================
    ps3display.cpp

    PS3 PDDI Display Implementation

    Copyright (c)1996-2002 Radical Entertainment Ltd
    All rights reserved.
===========================================================================*/

#ifdef RAD_PS3

#include <pddi/ps3/ps3display.hpp>
#include <pddi/ps3/ps3device.hpp>

#include <string.h>
#include <stdio.h>

// PS3 SDK includes
#include <PSGL/psgl.h>
#include <PSGL/psglu.h>
#include <cell/gcm.h>
#include <cell/dbgfont.h>
#include <sysutil/sysutil_sysparam.h>
#include <sys/spu_initialize.h>

//-------------------------------------------------------------------
// ps3Display implementation
//-------------------------------------------------------------------
ps3Display::ps3Display(ps3Device* device) :
    mDevice(device),
    mWidth(1280),
    mHeight(720),
    mBpp(32),
    mVSync(true),
    mWidescreen(true),
    mCurrentBuffer(0),
    mColorBufferPitch(0),
    mDepthBufferPitch(0),
    mPSGLContext(NULL),
    mPSGLDevice(NULL),
    mGcmContext(NULL)
{
    mColorBuffer[0] = NULL;
    mColorBuffer[1] = NULL;
    mDepthBuffer = NULL;
}

ps3Display::~ps3Display()
{
    TerminatePSGL();
}

bool ps3Display::InitDisplay(int x, int y, int bpp)
{
    pddiDisplayInit init;
    init.xsize = x;
    init.ysize = y;
    init.bpp = bpp;
    return InitDisplay(&init);
}

bool ps3Display::InitDisplay(const pddiDisplayInit* initData)
{
    mWidth = initData->xsize;
    mHeight = initData->ysize;
    mBpp = initData->bpp;
    mVSync = initData->lockToVsync;

    // Determine resolution mode
    // Force 480p for now to minimize PSGL memory usage during debugging
    // TODO: Once memory issues are resolved, enable higher resolutions
    mWidth = 720;
    mHeight = 480;
    (void)initData;  // Suppress unused warning

    // Check widescreen setting
    int displayMode = 0;
    cellSysutilGetSystemParamInt(CELL_SYSUTIL_SYSTEMPARAM_ID_GAME_PARENTAL_LEVEL, &displayMode);
    mWidescreen = true; // Default to widescreen for HD

    // Initialize PSGL (OpenGL-like API)
    return InitPSGL(mWidth, mHeight);
}

bool ps3Display::InitPSGL(int width, int height)
{
    // Initialize PSGL
    PSGLinitOptions initOpts;
    memset(&initOpts, 0, sizeof(initOpts));
    // Initialize SPU subsystem before PSGL
    sys_spu_initialize(6, 1);  // 6 SPUs for general use, 1 raw SPU for PSGL

    // CRITICAL: Must include all memory flags or PSGL uses huge defaults!
    initOpts.enable = PSGL_INIT_MAX_SPUS |
                      PSGL_INIT_INITIALIZE_SPUS |
                      PSGL_INIT_HOST_MEMORY_SIZE |
                      PSGL_INIT_FIFO_SIZE;
    initOpts.maxSPUs = 1;
    initOpts.initializeSPUs = GL_TRUE;
    initOpts.persistentMemorySize = 0;  // Use RSX memory, not host
    initOpts.transientMemorySize = 0;   // Use RSX memory, not host
    initOpts.errorConsole = 0;
    initOpts.fifoSize = 256 * 1024;     // 256KB command buffer (reduced from 1MB)
    initOpts.hostMemorySize = 8 * 1024 * 1024;  // 8MB host memory (minimal for PSGL)

    psglInit(&initOpts);

    // Create device
    PSGLdeviceParameters params;
    memset(&params, 0, sizeof(params));
    params.enable = PSGL_DEVICE_PARAMETERS_COLOR_FORMAT |
                    PSGL_DEVICE_PARAMETERS_DEPTH_FORMAT |
                    PSGL_DEVICE_PARAMETERS_MULTISAMPLING_MODE |
                    PSGL_DEVICE_PARAMETERS_WIDTH_HEIGHT;
    params.colorFormat = GL_ARGB_SCE;
    params.depthFormat = GL_DEPTH_COMPONENT24;
    params.multisamplingMode = GL_MULTISAMPLING_NONE_SCE;
    params.width = width;
    params.height = height;

    mPSGLDevice = psglCreateDeviceExtended(&params);
    if (!mPSGLDevice)
    {
        return false;
    }

    // Get actual dimensions
    GLuint actualWidth, actualHeight;
    psglGetDeviceDimensions((PSGLdevice*)mPSGLDevice, &actualWidth, &actualHeight);
    mWidth = (int)actualWidth;
    mHeight = (int)actualHeight;

    // Create and set context
    mPSGLContext = psglCreateContext();
    if (!mPSGLContext)
    {
        psglDestroyDevice((PSGLdevice*)mPSGLDevice);
        mPSGLDevice = NULL;
        return false;
    }

    psglMakeCurrent((PSGLcontext*)mPSGLContext, (PSGLdevice*)mPSGLDevice);
    psglResetCurrentContext();

    // Set up default OpenGL state
    glViewport(0, 0, mWidth, mHeight);
    glScissor(0, 0, mWidth, mHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepthf(1.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Enable vsync if requested
    // Note: PSGL uses vsync by default with psglSwap()
    // Disable vsync by using glEnable(GL_VSYNC_SCE) / glDisable(GL_VSYNC_SCE)
    if (!mVSync)
    {
        // Disable vsync if not requested
        // glDisable(GL_VSYNC_SCE); // May not be available in all SDK versions
    }

    return true;
}

void ps3Display::TerminatePSGL()
{
    if (mPSGLContext)
    {
        psglDestroyContext((PSGLcontext*)mPSGLContext);
        mPSGLContext = NULL;
    }

    if (mPSGLDevice)
    {
        psglDestroyDevice((PSGLdevice*)mPSGLDevice);
        mPSGLDevice = NULL;
    }

    psglExit();
}

bool ps3Display::InitGCM(int width, int height)
{
    // GCM initialization (alternative to PSGL)
    // This would be used for more direct RSX control
    return false; // Not implemented - using PSGL instead
}

void ps3Display::TerminateGCM()
{
    // GCM termination
}

void ps3Display::SetupFrameBuffers(int width, int height)
{
    mColorBufferPitch = width * 4; // ARGB8888
    mDepthBufferPitch = width * 4; // 24-bit depth with 8-bit stencil
}

int ps3Display::GetHeight()
{
    return mHeight;
}

int ps3Display::GetWidth()
{
    return mWidth;
}

int ps3Display::GetDepth()
{
    return mBpp;
}

pddiDisplayMode ps3Display::GetDisplayMode()
{
    return PDDI_DISPLAY_FULLSCREEN;
}

int ps3Display::GetNumColourBuffer()
{
    return 2; // Double buffered
}

unsigned ps3Display::GetBufferMask()
{
    return PDDI_BUFFER_COLOUR | PDDI_BUFFER_DEPTH | PDDI_BUFFER_STENCIL;
}

unsigned ps3Display::GetFreeTextureMem()
{
    return mDevice->GetFreeRSXMemory();
}

void ps3Display::SwapBuffers(void)
{
    if (mPSGLDevice)
    {
        psglSwap();
    }

    mCurrentBuffer = 1 - mCurrentBuffer;
}

unsigned ps3Display::Screenshot(pddiColour* buffer, int nBytes)
{
    if (!buffer || nBytes < (int)(mWidth * mHeight * sizeof(pddiColour)))
    {
        return 0;
    }

    // Read pixels from front buffer
    glReadPixels(0, 0, mWidth, mHeight, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

    return mWidth * mHeight * sizeof(pddiColour);
}

bool ps3Display::IsWidescreen(void)
{
    return mWidescreen;
}

void* ps3Display::GetColorBuffer(int index)
{
    if (index >= 0 && index < 2)
    {
        return mColorBuffer[index];
    }
    return NULL;
}

void* ps3Display::GetDepthBuffer()
{
    return mDepthBuffer;
}

unsigned int ps3Display::GetColorBufferPitch()
{
    return mColorBufferPitch;
}

#endif // RAD_PS3
