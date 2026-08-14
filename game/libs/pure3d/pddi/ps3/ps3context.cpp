/*===========================================================================
    ps3context.cpp

    PS3 PDDI Render Context Implementation

    Copyright (c)1996-2002 Radical Entertainment Ltd
    All rights reserved.
===========================================================================*/

#ifdef RAD_PS3

#include <pddi/ps3/ps3context.hpp>
#include <pddi/ps3/ps3device.hpp>
#include <pddi/ps3/ps3display.hpp>
#include <pddi/ps3/ps3texture.hpp>
#include <pddi/ps3/ps3shader.hpp>
#include <pddi/ps3/ps3prim.hpp>
#include <pddi/ps3/ps3primstream.hpp>

#include <string.h>
#include <stdio.h>
#include <math.h>

// PS3 SDK includes
#include <PSGL/psgl.h>
#include <PSGL/psglu.h>
#include <cell/dbgfont.h>

//-------------------------------------------------------------------
// ps3Context implementation
//-------------------------------------------------------------------
ps3Context::ps3Context(ps3Device* device, ps3Display* display) :
    pddiBaseContext(display, device),
    mDevice(device),
    mDisplay(display),
    mClearColour(0, 0, 0, 255),
    mClearDepth(1.0f),
    mClearStencil(0),
    mZBufferEnabled(true),
    mZWriteEnabled(true),
    mZCompare(PDDI_COMPARE_LESSEQUAL),
    mZBias(0.0f),
    mZNear(0.0f),
    mZFar(1.0f),
    mCullMode(PDDI_CULL_NORMAL),
    mFillMode(PDDI_FILL_SOLID),
    mStencilEnabled(false),
    mStencilCompare(PDDI_COMPARE_ALWAYS),
    mStencilRef(0),
    mStencilMask(0xFFFFFFFF),
    mStencilWriteMask(0xFFFFFFFF),
    mStencilFailOp(PDDI_STENCIL_KEEP),
    mStencilZFailOp(PDDI_STENCIL_KEEP),
    mStencilZPassOp(PDDI_STENCIL_KEEP),
    mFogEnabled(false),
    mFogColour(128, 128, 128),
    mFogStart(0.0f),
    mFogEnd(1.0f),
    mAmbientLight(64, 64, 64),
    mStatsOverlay(false),
    mTriCount(0),
    mPrimCount(0),
    mVertexCount(0),
    mTextureChanges(0),
    mShaderChanges(0),
    mStateChanges(0),
    mCurrentShader(NULL),
    mPrimStream(NULL)
{
    mColourWrite[0] = true;
    mColourWrite[1] = true;
    mColourWrite[2] = true;
    mColourWrite[3] = true;

    // Initialize lights
    for (int i = 0; i < PS3_MAX_LIGHTS; i++)
    {
        mLights[i].Clear();
    }

    // NOTE: Debug font disabled to investigate texture completeness issues
    // The cellDbgFont library creates internal textures that might interfere
    // with game texture validation
    /*
    CellDbgFontConfig cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.bufSize = 4096;
    cfg.screenWidth = display->GetWidth();
    cfg.screenHeight = display->GetHeight();
    cellDbgFontInit(&cfg);
    */

    // Create immediate mode primitive stream
    mPrimStream = new ps3PrimStream(device);

    // Initialize the default texture (texture 0) with a valid white pixel
    // This prevents "Texture 0 is incomplete" warnings when code enables
    // GL_TEXTURE_2D without explicitly binding a texture
    {
        glBindTexture(GL_TEXTURE_2D, 0);
        unsigned char white[4] = {255, 255, 255, 255};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // Clear any errors from texture setup
        while (glGetError() != GL_NO_ERROR) {}
    }

    // Initialize base class state stacks with proper defaults and sync to GL.
    // Every other platform (GameCube, Xbox, DX8, PS2, GL) calls this.
    // Without it, renderState defaults to zero (colorWrite=false = black screen),
    // viewState has no camera/projection, etc.
    DefaultState();
}

ps3Context::~ps3Context()
{
    if (mPrimStream)
    {
        delete mPrimStream;
        mPrimStream = NULL;
    }
    // cellDbgFontExit(); // Disabled along with cellDbgFontInit
}

void ps3Context::BeginFrame()
{
    ResetStatistics();

    // Reset current shader so it gets re-bound on the first draw call each frame.
    // This ensures textures and other shader state are properly applied even if
    // the same shader instance is used across frames.
    mCurrentShader = NULL;
}

void ps3Context::EndFrame()
{
    // Clear any GL errors that accumulated during the frame
    while (glGetError() != GL_NO_ERROR) {}
}

void ps3Context::SetClearColour(pddiColour colour)
{
    mClearColour = colour;
    glClearColor(colour.Red() / 255.0f, colour.Green() / 255.0f,
                 colour.Blue() / 255.0f, colour.Alpha() / 255.0f);
}

pddiColour ps3Context::GetClearColour(void)
{
    return mClearColour;
}

void ps3Context::SetClearDepth(float depth)
{
    mClearDepth = depth;
    glClearDepthf(depth);
}

float ps3Context::GetClearDepth(void)
{
    return mClearDepth;
}

void ps3Context::SetClearStencil(unsigned stencil)
{
    mClearStencil = stencil;
    glClearStencil(stencil);
}

unsigned ps3Context::GetClearStencil(void)
{
    return mClearStencil;
}

void ps3Context::Clear(unsigned bufferMask)
{
    GLbitfield glMask = 0;

    if (bufferMask & PDDI_BUFFER_COLOUR)
        glMask |= GL_COLOR_BUFFER_BIT;
    if (bufferMask & PDDI_BUFFER_DEPTH)
        glMask |= GL_DEPTH_BUFFER_BIT;
    if (bufferMask & PDDI_BUFFER_STENCIL)
        glMask |= GL_STENCIL_BUFFER_BIT;

    glClear(glMask);
}

pddiPrimStream* ps3Context::BeginPrims(pddiShader* material, pddiPrimType primType, unsigned vertexType, int vertexCount, unsigned pass)
{
    if (!mPrimStream)
    {
        return NULL;
    }
    if (!material)
    {
        return NULL;
    }

    ps3Shader* shader = static_cast<ps3Shader*>(material);

    // Bind the shader (full state apply when shader changes)
    if (mCurrentShader != shader)
    {
        shader->Bind(this);
        mCurrentShader = shader;
        mShaderChanges++;
    }
    else
    {
        // Same shader but textures may have changed (e.g. tSprite calls
        // SetTexture before each BeginPrims with the same shader).
        // Re-apply texture state to ensure the correct texture is bound.
        shader->ApplyTextureState(this);
    }

    // Begin the primitive stream
    mPrimStream->Begin(this, shader, primType, vertexType, vertexCount);

    // Track immediate mode rendering
    mPrimCount++;

    return mPrimStream;
}

void ps3Context::EndPrims(pddiPrimStream* stream)
{
    if (stream && stream == mPrimStream)
    {
        mPrimStream->End();
    }
}

void ps3Context::DrawPrimBuffer(pddiShader* material, pddiPrimBuffer* buffer)
{
    if (!material || !buffer)
        return;

    ps3Shader* shader = static_cast<ps3Shader*>(material);
    ps3PrimBuffer* primBuf = static_cast<ps3PrimBuffer*>(buffer);

    // Bind shader
    if (mCurrentShader != shader)
    {
        shader->Bind(this);
        mCurrentShader = shader;
        mShaderChanges++;
    }
    else
    {
        // Same shader but textures may have changed
        shader->ApplyTextureState(this);
    }

    // Draw the primitive buffer
    primBuf->Display(this);

    // Update statistics
    mPrimCount++;
    mVertexCount += primBuf->GetVertexCount();

    // Estimate triangle count based on primitive type
    switch (primBuf->GetPrimType())
    {
    case PDDI_PRIM_TRIANGLES:
        mTriCount += primBuf->GetVertexCount() / 3;
        break;
    case PDDI_PRIM_TRISTRIP:
        mTriCount += primBuf->GetVertexCount() - 2;
        break;
    case PDDI_PRIM_TRIFAN:
        mTriCount += primBuf->GetVertexCount() - 2;
        break;
    default:
        break;
    }
}

void ps3Context::DrawString(const char* s, int x, int y, pddiColour colour)
{
    if (!s)
        return;

    float fx = (float)x / mDisplay->GetWidth();
    float fy = (float)y / mDisplay->GetHeight();

    cellDbgFontPrintf(fx, fy, 1.0f,
                      (colour.Alpha() << 24) | (colour.Red() << 16) |
                      (colour.Green() << 8) | colour.Blue(),
                      "%s", s);
}

int ps3Context::GetMaxLights()
{
    return PS3_MAX_LIGHTS;
}

void ps3Context::SetAmbientLight(pddiColour col)
{
    mAmbientLight = col;
    float ambient[4] = {col.Red() / 255.0f, col.Green() / 255.0f,
                        col.Blue() / 255.0f, col.Alpha() / 255.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
}

pddiColour ps3Context::GetAmbientLight()
{
    return mAmbientLight;
}

void ps3Context::SetLight(int handle, pddiLightDesc* desc)
{
    if (handle >= 0 && handle < PS3_MAX_LIGHTS)
    {
        mLights[handle] = *desc;

        GLenum light = GL_LIGHT0 + handle;

        if (desc->IsEnabled())
        {
            glEnable(light);
        }
        else
        {
            glDisable(light);
        }

        // Set light color
        const pddiColour& col = desc->GetColour();
        float diffuse[4] = {col.Red() / 255.0f, col.Green() / 255.0f,
                            col.Blue() / 255.0f, 1.0f};
        glLightfv(light, GL_DIFFUSE, diffuse);
        glLightfv(light, GL_SPECULAR, diffuse);

        // Set position/direction based on type
        if (desc->GetType() == PDDI_LIGHT_DIRECTIONAL)
        {
            const rmt::Vector& direction = desc->GetDirection();
            float dir[4] = {-direction.x, -direction.y, -direction.z, 0.0f};
            glLightfv(light, GL_POSITION, dir);
        }
        else
        {
            const rmt::Vector& position = desc->GetPosition();
            float pos[4] = {position.x, position.y, position.z, 1.0f};
            glLightfv(light, GL_POSITION, pos);

            // Attenuation
            float attenA, attenB, attenC;
            desc->GetAttenuation(&attenA, &attenB, &attenC);
            glLightf(light, GL_CONSTANT_ATTENUATION, attenA);
            glLightf(light, GL_LINEAR_ATTENUATION, attenB);
            glLightf(light, GL_QUADRATIC_ATTENUATION, attenC);

            if (desc->GetType() == PDDI_LIGHT_SPOT)
            {
                const rmt::Vector& spotDirection = desc->GetDirection();
                float spotDir[3] = {spotDirection.x, spotDirection.y, spotDirection.z};
                glLightfv(light, GL_SPOT_DIRECTION, spotDir);
                float phi, theta, falloff;
                desc->GetCone(&phi, &theta, &falloff);
                glLightf(light, GL_SPOT_CUTOFF, phi * 57.2958f);
                glLightf(light, GL_SPOT_EXPONENT, falloff * 128.0f);
            }
        }
    }
}

void ps3Context::EnableLight(int handle, bool active)
{
    if (handle >= 0 && handle < PS3_MAX_LIGHTS)
    {
        mLights[handle].Enable(active);
        if (active)
            glEnable(GL_LIGHT0 + handle);
        else
            glDisable(GL_LIGHT0 + handle);
    }
}

bool ps3Context::IsLightEnabled(int handle)
{
    if (handle >= 0 && handle < PS3_MAX_LIGHTS)
        return mLights[handle].IsEnabled();
    return false;
}

void ps3Context::SetLightType(int handle, pddiLightType type)
{
    if (handle >= 0 && handle < PS3_MAX_LIGHTS)
    {
        mLights[handle].SetType(type);
    }
}

pddiLightType ps3Context::GetLightType(int handle)
{
    if (handle >= 0 && handle < PS3_MAX_LIGHTS)
        return mLights[handle].GetType();
    return PDDI_LIGHT_DIRECTIONAL;
}

void ps3Context::SetLightColour(int handle, pddiColour colour)
{
    if (handle >= 0 && handle < PS3_MAX_LIGHTS)
    {
        mLights[handle].SetColour(colour);
        float diffuse[4] = {colour.Red() / 255.0f, colour.Green() / 255.0f,
                            colour.Blue() / 255.0f, 1.0f};
        glLightfv(GL_LIGHT0 + handle, GL_DIFFUSE, diffuse);
    }
}

pddiColour ps3Context::GetLightColour(int handle)
{
    if (handle >= 0 && handle < PS3_MAX_LIGHTS)
        return mLights[handle].GetColour();
    return pddiColour(0, 0, 0);
}

void ps3Context::SetLightPosition(int handle, rmt::Vector* pos)
{
    if (handle >= 0 && handle < PS3_MAX_LIGHTS && pos)
    {
        mLights[handle].SetPosition(pos);
        float glPos[4] = {pos->x, pos->y, pos->z, 1.0f};
        glLightfv(GL_LIGHT0 + handle, GL_POSITION, glPos);
    }
}

void ps3Context::GetLightPosition(int handle, rmt::Vector* pos)
{
    if (handle >= 0 && handle < PS3_MAX_LIGHTS && pos)
    {
        *pos = mLights[handle].GetPosition();
    }
}

void ps3Context::SetLightDirection(int handle, rmt::Vector* dir)
{
    if (handle >= 0 && handle < PS3_MAX_LIGHTS && dir)
    {
        mLights[handle].SetDirection(dir);
    }
}

void ps3Context::GetLightDirection(int handle, rmt::Vector* dir)
{
    if (handle >= 0 && handle < PS3_MAX_LIGHTS && dir)
    {
        *dir = mLights[handle].GetDirection();
    }
}

void ps3Context::SetLightRange(int handle, float range)
{
    // OpenGL doesn't have direct light range - use attenuation
}

float ps3Context::GetLightRange(int handle)
{
    return 0.0f;
}

void ps3Context::SetLightAttenuation(int handle, float a, float b, float c)
{
    if (handle >= 0 && handle < PS3_MAX_LIGHTS)
    {
        mLights[handle].SetAttenuation(a, b, c);

        GLenum light = GL_LIGHT0 + handle;
        glLightf(light, GL_CONSTANT_ATTENUATION, a);
        glLightf(light, GL_LINEAR_ATTENUATION, b);
        glLightf(light, GL_QUADRATIC_ATTENUATION, c);
    }
}

void ps3Context::GetLightAttenuation(int handle, float* a, float* b, float* c)
{
    if (handle >= 0 && handle < PS3_MAX_LIGHTS)
    {
        mLights[handle].GetAttenuation(a, b, c);
    }
}

void ps3Context::SetLightCone(int handle, float phi, float theta, float falloff)
{
    if (handle >= 0 && handle < PS3_MAX_LIGHTS)
    {
        mLights[handle].SetCone(phi, theta, falloff);

        GLenum light = GL_LIGHT0 + handle;
        glLightf(light, GL_SPOT_CUTOFF, phi * 57.2958f);
        glLightf(light, GL_SPOT_EXPONENT, falloff * 128.0f);
    }
}

void ps3Context::GetLightCone(int handle, float* phi, float* theta, float* falloff)
{
    if (handle >= 0 && handle < PS3_MAX_LIGHTS)
    {
        mLights[handle].GetCone(phi, theta, falloff);
    }
}

void ps3Context::SetCullMode(pddiCullMode mode)
{
    mCullMode = mode;
    mStateChanges++;

    switch (mode)
    {
    case PDDI_CULL_NONE:
        glDisable(GL_CULL_FACE);
        break;
    case PDDI_CULL_NORMAL:
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
        break;
    case PDDI_CULL_INVERTED:
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CW);
        glCullFace(GL_BACK);
        break;
    }
}

pddiCullMode ps3Context::GetCullMode()
{
    return mCullMode;
}

void ps3Context::SetColourWrite(bool red, bool green, bool blue, bool alpha)
{
    mColourWrite[0] = red;
    mColourWrite[1] = green;
    mColourWrite[2] = blue;
    mColourWrite[3] = alpha;
    mStateChanges++;

    glColorMask(red, green, blue, alpha);
}

void ps3Context::GetColourWrite(bool* red, bool* green, bool* blue, bool* alpha)
{
    if (red) *red = mColourWrite[0];
    if (green) *green = mColourWrite[1];
    if (blue) *blue = mColourWrite[2];
    if (alpha) *alpha = mColourWrite[3];
}

void ps3Context::EnableZBuffer(bool enable)
{
    mZBufferEnabled = enable;
    mStateChanges++;

    if (enable)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}

bool ps3Context::IsZBufferEnabled()
{
    return mZBufferEnabled;
}

static GLenum pddiToGLCompare(pddiCompareMode mode)
{
    switch (mode)
    {
    case PDDI_COMPARE_NONE:       return GL_ALWAYS;
    case PDDI_COMPARE_ALWAYS:     return GL_ALWAYS;
    case PDDI_COMPARE_LESS:       return GL_LESS;
    case PDDI_COMPARE_LESSEQUAL:  return GL_LEQUAL;
    case PDDI_COMPARE_GREATER:    return GL_GREATER;
    case PDDI_COMPARE_GREATEREQUAL: return GL_GEQUAL;
    case PDDI_COMPARE_EQUAL:      return GL_EQUAL;
    case PDDI_COMPARE_NOTEQUAL:   return GL_NOTEQUAL;
    case PDDI_COMPARE_NEVER:      return GL_NEVER;
    default:                      return GL_LEQUAL;
    }
}

void ps3Context::SetZCompare(pddiCompareMode compareMode)
{
    mZCompare = compareMode;
    mStateChanges++;
    glDepthFunc(pddiToGLCompare(compareMode));
}

pddiCompareMode ps3Context::GetZCompare()
{
    return mZCompare;
}

void ps3Context::SetZWrite(bool write)
{
    mZWriteEnabled = write;
    mStateChanges++;
    glDepthMask(write);
}

bool ps3Context::GetZWrite()
{
    return mZWriteEnabled;
}

void ps3Context::SetZBias(float bias)
{
    mZBias = bias;
    if (bias != 0.0f)
    {
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(bias, bias);
    }
    else
    {
        glDisable(GL_POLYGON_OFFSET_FILL);
    }
}

float ps3Context::GetZBias()
{
    return mZBias;
}

void ps3Context::SetZRange(float n, float f)
{
    mZNear = n;
    mZFar = f;
    glDepthRangef(n, f);
}

void ps3Context::GetZRange(float* n, float* f)
{
    if (n) *n = mZNear;
    if (f) *f = mZFar;
}

void ps3Context::EnableStencilBuffer(bool enable)
{
    mStencilEnabled = enable;
    mStateChanges++;

    if (enable)
        glEnable(GL_STENCIL_TEST);
    else
        glDisable(GL_STENCIL_TEST);
}

bool ps3Context::IsStencilBufferEnabled()
{
    return mStencilEnabled;
}

void ps3Context::SetStencilCompare(pddiCompareMode compare)
{
    mStencilCompare = compare;
    glStencilFunc(pddiToGLCompare(compare), mStencilRef, mStencilMask);
}

pddiCompareMode ps3Context::GetStencilCompare()
{
    return mStencilCompare;
}

void ps3Context::SetStencilRef(int ref)
{
    mStencilRef = ref;
    glStencilFunc(pddiToGLCompare(mStencilCompare), ref, mStencilMask);
}

int ps3Context::GetStencilRef()
{
    return mStencilRef;
}

void ps3Context::SetStencilMask(unsigned mask)
{
    mStencilMask = mask;
    glStencilFunc(pddiToGLCompare(mStencilCompare), mStencilRef, mask);
}

unsigned ps3Context::GetStencilMask()
{
    return mStencilMask;
}

void ps3Context::SetStencilWriteMask(unsigned mask)
{
    mStencilWriteMask = mask;
    glStencilMask(mask);
}

unsigned ps3Context::GetStencilWriteMask()
{
    return mStencilWriteMask;
}

static GLenum pddiToGLStencilOp(pddiStencilOp op)
{
    switch (op)
    {
    case PDDI_STENCIL_KEEP:    return GL_KEEP;
    case PDDI_STENCIL_ZERO:    return GL_ZERO;
    case PDDI_STENCIL_REPLACE: return GL_REPLACE;
    case PDDI_STENCIL_INCR:    return GL_INCR;
    case PDDI_STENCIL_DECR:    return GL_DECR;
    case PDDI_STENCIL_INVERT:  return GL_INVERT;
    default:                   return GL_KEEP;
    }
}

void ps3Context::SetStencilOp(pddiStencilOp failOp, pddiStencilOp zFailOp, pddiStencilOp zPassOp)
{
    mStencilFailOp = failOp;
    mStencilZFailOp = zFailOp;
    mStencilZPassOp = zPassOp;
    glStencilOp(pddiToGLStencilOp(failOp), pddiToGLStencilOp(zFailOp), pddiToGLStencilOp(zPassOp));
}

void ps3Context::GetStencilOp(pddiStencilOp* failOp, pddiStencilOp* zFailOp, pddiStencilOp* zPassOp)
{
    if (failOp) *failOp = mStencilFailOp;
    if (zFailOp) *zFailOp = mStencilZFailOp;
    if (zPassOp) *zPassOp = mStencilZPassOp;
}

void ps3Context::SetFillMode(pddiFillMode mode)
{
    mFillMode = mode;
    mStateChanges++;

    switch (mode)
    {
    case PDDI_FILL_SOLID:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    case PDDI_FILL_WIRE:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case PDDI_FILL_POINT:
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        break;
    }
}

pddiFillMode ps3Context::GetFillMode()
{
    return mFillMode;
}

void ps3Context::EnableFog(bool enable)
{
    mFogEnabled = enable;
    mStateChanges++;

    if (enable)
        glEnable(GL_FOG);
    else
        glDisable(GL_FOG);
}

bool ps3Context::IsFogEnabled()
{
    return mFogEnabled;
}

void ps3Context::SetFog(pddiColour colour, float start, float end)
{
    mFogColour = colour;
    mFogStart = start;
    mFogEnd = end;

    float fogColor[4] = {colour.Red() / 255.0f, colour.Green() / 255.0f,
                         colour.Blue() / 255.0f, 1.0f};
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_START, start);
    glFogf(GL_FOG_END, end);
    glFogi(GL_FOG_MODE, GL_LINEAR);
}

void ps3Context::GetFog(pddiColour* colour, float* start, float* end)
{
    if (colour) *colour = mFogColour;
    if (start) *start = mFogStart;
    if (end) *end = mFogEnd;
}

int ps3Context::GetIntStat(pddiStatType stat)
{
    switch (stat)
    {
    case PDDI_STAT_TRIS_DRAWN:
        return mTriCount;
    case PDDI_STAT_PRIM_DRAWN:
        return mPrimCount;
    default:
        return 0;
    }
}

float ps3Context::GetFloatStat(pddiStatType stat)
{
    return (float)GetIntStat(stat);
}

int ps3Context::GetMaxTextureDimension(void)
{
    GLint maxSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxSize);
    return maxSize;
}

void ps3Context::EnableStatsOverlay(bool enable)
{
    mStatsOverlay = enable;
}

bool ps3Context::IsStatsOverlayEnabled()
{
    return mStatsOverlay;
}

void ps3Context::PushState(pddiStateMask mask)
{
    // Delegate to base class which manages proper state stacks
    // (viewStateStack, renderStateStack, lightingStateStack, etc.)
    pddiBaseContext::PushState(mask);
}

void ps3Context::PopState(pddiStateMask mask)
{
    // Delegate to base class which pops state stacks and calls Sync()
    // to restore all GL state via the Set* methods
    pddiBaseContext::PopState(mask);
}

pddiExtension* ps3Context::GetExtension(unsigned extID)
{
    // Return extensions as needed
    return NULL;
}

bool ps3Context::VerifyExtension(unsigned extID)
{
    return false;
}

void ps3Context::DrawSync()
{
    glFinish();
}

unsigned ps3Context::SetRenderTarget(pddiTexture* tex)
{
    // Render to texture - would need FBO support
    // For now, return 0 (render to backbuffer)
    return 0;
}

void ps3Context::ApplyRenderState()
{
    // Apply all current render state to OpenGL
    EnableZBuffer(mZBufferEnabled);
    SetZCompare(mZCompare);
    SetZWrite(mZWriteEnabled);
    SetCullMode(mCullMode);
    SetFillMode(mFillMode);
    SetColourWrite(mColourWrite[0], mColourWrite[1], mColourWrite[2], mColourWrite[3]);
    EnableFog(mFogEnabled);
    EnableStencilBuffer(mStencilEnabled);
}

void ps3Context::ResetStatistics()
{
    mTriCount = 0;
    mPrimCount = 0;
    mVertexCount = 0;
    mTextureChanges = 0;
    mShaderChanges = 0;
    mStateChanges = 0;
}

void ps3Context::BeginTiming()
{
    // Start GPU timing - PSGL doesn't have direct timing queries,
    // so this is a placeholder
}

float ps3Context::EndTiming()
{
    // End GPU timing - return 0 for now
    return 0.0f;
}

void ps3Context::LoadHardwareMatrix(pddiMatrixType id)
{
    // Load matrix from PDDI matrix stack to OpenGL
    // Following the GL backend approach - pass matrix directly, negate Z for coordinate system
    pddiMatrix tmp = *state.matrixStack[id]->Top();

    // Negate Z column for OpenGL coordinate system (right-hand vs left-hand)
    tmp.m[0][2] = -tmp.m[0][2];
    tmp.m[1][2] = -tmp.m[1][2];
    tmp.m[2][2] = -tmp.m[2][2];
    tmp.m[3][2] = -tmp.m[3][2];

    switch (id)
    {
    case PDDI_MATRIX_MODELVIEW:
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf((float*)&tmp);
        break;
    case PDDI_MATRIX_TEXTURE0:
        glActiveTexture(GL_TEXTURE0);
        glMatrixMode(GL_TEXTURE);
        glLoadMatrixf((float*)&tmp);
        break;
    case PDDI_MATRIX_TEXTURE1:
        glActiveTexture(GL_TEXTURE1);
        glMatrixMode(GL_TEXTURE);
        glLoadMatrixf((float*)&tmp);
        break;
    case PDDI_MATRIX_TEXTURE2:
        glActiveTexture(GL_TEXTURE2);
        glMatrixMode(GL_TEXTURE);
        glLoadMatrixf((float*)&tmp);
        break;
    case PDDI_MATRIX_TEXTURE3:
        glActiveTexture(GL_TEXTURE3);
        glMatrixMode(GL_TEXTURE);
        glLoadMatrixf((float*)&tmp);
        break;
    }
}

void ps3Context::SetupHardwareProjection()
{
    // Setup projection matrix based on camera settings
    pddiCamera& camera = state.viewState->camera;
    pddiFloatRect& vw = state.viewState->viewWindow;

    float width = (float)mDisplay->GetWidth();
    float height = (float)mDisplay->GetHeight();

    glMatrixMode(GL_PROJECTION);

    float proj[16];
    memset(proj, 0, sizeof(proj));

    switch (state.viewState->projectionMode)
    {
    case PDDI_PROJECTION_PERSPECTIVE:
        {
            // Calculate frustum parameters
            // Note: camera.fov is already in RADIANS (not degrees)
            // Scrooby uses 1.5708 radians = PI/2 = 90 degrees
            float halfFov = camera.fov * 0.5f;
            float halfY = tanf(halfFov) * camera.nearPlane;
            float halfX = halfY * camera.aspect;

            // Guard against divide-by-zero from uninitialized camera
            if (halfX < 0.0001f) halfX = 0.0001f;
            if (halfY < 0.0001f) halfY = 0.0001f;

            float left = -halfX;
            float right = halfX;
            float bottom = -halfY;
            float top = halfY;
            float nearVal = camera.nearPlane;
            float farVal = camera.farPlane;

            // Guard against near==far
            if (farVal - nearVal < 0.0001f) farVal = nearVal + 0.0001f;

            // Build frustum matrix (column-major for OpenGL)
            proj[0] = (2.0f * nearVal) / (right - left);
            proj[5] = (2.0f * nearVal) / (top - bottom);
            proj[8] = (right + left) / (right - left);
            proj[9] = (top + bottom) / (top - bottom);
            proj[10] = -(farVal + nearVal) / (farVal - nearVal);
            proj[11] = -1.0f;
            proj[14] = -(2.0f * farVal * nearVal) / (farVal - nearVal);

            glLoadMatrixf(proj);

            // Set viewport
            float viewWidth = vw.right - vw.left;
            float viewHeight = vw.bottom - vw.top;
            glViewport((int)(vw.left * width), (int)((1.0f - vw.bottom) * height),
                       (int)(viewWidth * width), (int)(viewHeight * height));
        }
        break;

    case PDDI_PROJECTION_ORTHOGRAPHIC:
        {
            // Guard against zero aspect
            float aspect = camera.aspect;
            if (aspect < 0.0001f) aspect = 1.0f;

            float l = -0.5f;
            float r = 0.5f;
            float b = -0.5f / aspect;
            float t = 0.5f / aspect;
            float nearVal = camera.nearPlane;
            float farVal = camera.farPlane;

            // Guard against near==far
            if (farVal - nearVal < 0.0001f) farVal = nearVal + 0.0001f;

            // Build ortho matrix (column-major for OpenGL)
            proj[0] = 2.0f / (r - l);
            proj[5] = 2.0f / (t - b);
            proj[10] = -2.0f / (farVal - nearVal);
            proj[12] = -(r + l) / (r - l);
            proj[13] = -(t + b) / (t - b);
            proj[14] = -(farVal + nearVal) / (farVal - nearVal);
            proj[15] = 1.0f;

            glLoadMatrixf(proj);

            float viewWidth = vw.right - vw.left;
            float viewHeight = vw.bottom - vw.top;
            glViewport((int)(vw.left * width), (int)((1.0f - vw.bottom) * height),
                       (int)(viewWidth * width), (int)(viewHeight * height));
        }
        break;

    case PDDI_PROJECTION_DEVICE:
        {
            // 2D screen-space projection
            // Guard against zero dimensions
            float l = 0;
            float r = (width > 0.0f) ? width : 640.0f;
            float b = (height > 0.0f) ? height : 480.0f;
            float t = 0;
            float nearVal = camera.nearPlane;
            float farVal = camera.farPlane;

            // Guard against near==far
            if (farVal - nearVal < 0.0001f) farVal = nearVal + 0.0001f;

            // Build ortho matrix (column-major for OpenGL)
            proj[0] = 2.0f / (r - l);
            proj[5] = 2.0f / (t - b);
            proj[10] = -2.0f / (farVal - nearVal);
            proj[12] = -(r + l) / (r - l);
            proj[13] = -(t + b) / (t - b);
            proj[14] = -(farVal + nearVal) / (farVal - nearVal);
            proj[15] = 1.0f;

            glLoadMatrixf(proj);
            glViewport(0, 0, (int)r, (int)b);
        }
        break;
    }
}

void ps3Context::SetupHardwareLight(int handle)
{
    // Setup hardware light from cached description
    if (handle >= 0 && handle < PS3_MAX_LIGHTS)
    {
        pddiLightDesc* desc = &mLights[handle];
        GLenum light = GL_LIGHT0 + handle;

        if (desc->IsEnabled())
        {
            glEnable(light);
        }
        else
        {
            glDisable(light);
            return;
        }

        // Set light color
        const pddiColour& col = desc->GetColour();
        float diffuse[4] = {col.Red() / 255.0f, col.Green() / 255.0f,
                            col.Blue() / 255.0f, 1.0f};
        glLightfv(light, GL_DIFFUSE, diffuse);
        glLightfv(light, GL_SPECULAR, diffuse);

        // Set position/direction based on type
        if (desc->GetType() == PDDI_LIGHT_DIRECTIONAL)
        {
            const rmt::Vector& direction = desc->GetDirection();
            float dir[4] = {-direction.x, -direction.y, -direction.z, 0.0f};
            glLightfv(light, GL_POSITION, dir);
        }
        else
        {
            const rmt::Vector& position = desc->GetPosition();
            float pos[4] = {position.x, position.y, position.z, 1.0f};
            glLightfv(light, GL_POSITION, pos);

            float attenA, attenB, attenC;
            desc->GetAttenuation(&attenA, &attenB, &attenC);
            glLightf(light, GL_CONSTANT_ATTENUATION, attenA);
            glLightf(light, GL_LINEAR_ATTENUATION, attenB);
            glLightf(light, GL_QUADRATIC_ATTENUATION, attenC);

            if (desc->GetType() == PDDI_LIGHT_SPOT)
            {
                const rmt::Vector& spotDirection = desc->GetDirection();
                float spotDir[3] = {spotDirection.x, spotDirection.y, spotDirection.z};
                glLightfv(light, GL_SPOT_DIRECTION, spotDir);
                float phi, theta, falloff;
                desc->GetCone(&phi, &theta, &falloff);
                glLightf(light, GL_SPOT_CUTOFF, phi * 57.2958f);
                glLightf(light, GL_SPOT_EXPONENT, falloff * 128.0f);
            }
        }
    }
}

#endif // RAD_PS3
