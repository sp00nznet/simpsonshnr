/*===========================================================================
    ps3context.hpp

    PS3 PDDI Render Context

    Copyright (c)1996-2002 Radical Entertainment Ltd
    All rights reserved.
===========================================================================*/

#ifndef _PS3CONTEXT_HPP
#define _PS3CONTEXT_HPP

#include <pddi/pddi.hpp>
#include <pddi/pddips3.hpp>
#include <pddi/base/basecontext.hpp>

class ps3Device;
class ps3Display;
class ps3Texture;
class ps3Shader;
class ps3PrimStream;

// Maximum number of lights supported
#define PS3_MAX_LIGHTS 8

//-------------------------------------------------------------------
// ps3Context - PS3 render context using PSGL/GCM
//-------------------------------------------------------------------
class ps3Context : public pddiBaseContext
{
public:
    ps3Context(ps3Device* device, ps3Display* display);
    virtual ~ps3Context();

    // Frame synchronization
    void BeginFrame();
    void EndFrame();

    // Buffer clearing
    void SetClearColour(pddiColour colour);
    pddiColour GetClearColour(void);
    void SetClearDepth(float depth);
    float GetClearDepth(void);
    void SetClearStencil(unsigned stencil);
    unsigned GetClearStencil(void);
    void Clear(unsigned bufferMask);

    // Immediate mode prim rendering
    pddiPrimStream* BeginPrims(pddiShader* material, pddiPrimType primType, unsigned vertexType, int vertexCount, unsigned pass = 0);
    void EndPrims(pddiPrimStream* stream);

    // Retained mode prim rendering
    void DrawPrimBuffer(pddiShader* material, pddiPrimBuffer* buffer);

    // Debug text
    void DrawString(const char* s, int x, int y, pddiColour colour = pddiColour(255,255,255));

    // Lighting
    int GetMaxLights();
    void SetAmbientLight(pddiColour col);
    pddiColour GetAmbientLight();
    void SetLight(int handle, pddiLightDesc* desc);
    void EnableLight(int handle, bool active);
    bool IsLightEnabled(int handle);
    void SetLightType(int handle, pddiLightType type);
    pddiLightType GetLightType(int handle);
    void SetLightColour(int handle, pddiColour colour);
    pddiColour GetLightColour(int handle);
    void SetLightPosition(int handle, rmt::Vector* pos);
    void GetLightPosition(int handle, rmt::Vector* pos);
    void SetLightDirection(int handle, rmt::Vector* dir);
    void GetLightDirection(int handle, rmt::Vector* dir);
    void SetLightRange(int handle, float range);
    float GetLightRange(int handle);
    void SetLightAttenuation(int handle, float a, float b, float c);
    void GetLightAttenuation(int handle, float* a, float* b, float* c);
    void SetLightCone(int handle, float phi, float theta, float falloff);
    void GetLightCone(int handle, float* phi, float* theta, float* falloff);

    // Backface culling
    void SetCullMode(pddiCullMode mode);
    pddiCullMode GetCullMode();

    // Colour buffer control
    void SetColourWrite(bool red, bool green, bool blue, bool alpha);
    void GetColourWrite(bool* red, bool* green, bool* blue, bool* alpha);

    // Z-buffer control
    void EnableZBuffer(bool enable);
    bool IsZBufferEnabled();
    void SetZCompare(pddiCompareMode compareMode);
    pddiCompareMode GetZCompare();
    void SetZWrite(bool write);
    bool GetZWrite();
    void SetZBias(float bias);
    float GetZBias();
    void SetZRange(float n, float f);
    void GetZRange(float* n, float* f);

    // Stencil buffer control
    void EnableStencilBuffer(bool enable);
    bool IsStencilBufferEnabled();
    void SetStencilCompare(pddiCompareMode compare);
    pddiCompareMode GetStencilCompare();
    void SetStencilRef(int ref);
    int GetStencilRef();
    void SetStencilMask(unsigned mask);
    unsigned GetStencilMask();
    void SetStencilWriteMask(unsigned mask);
    unsigned GetStencilWriteMask();
    void SetStencilOp(pddiStencilOp failOp, pddiStencilOp zFailOp, pddiStencilOp zPassOp);
    void GetStencilOp(pddiStencilOp* failOp, pddiStencilOp* zFailOp, pddiStencilOp* zPassOp);

    // Polygon fill
    void SetFillMode(pddiFillMode mode);
    pddiFillMode GetFillMode();

    // Fog
    void EnableFog(bool enable);
    bool IsFogEnabled();
    void SetFog(pddiColour colour, float start, float end);
    void GetFog(pddiColour* colour, float* start, float* end);

    // Utility
    int GetIntStat(pddiStatType stat);
    float GetFloatStat(pddiStatType stat);
    int GetMaxTextureDimension(void);
    void EnableStatsOverlay(bool enable);
    bool IsStatsOverlayEnabled();

    // State preservation, restoration
    void PushState(pddiStateMask mask);
    void PopState(pddiStateMask mask);

    // Extensions
    pddiExtension* GetExtension(unsigned extID);
    bool VerifyExtension(unsigned extID);

    // Forces drawing to complete
    void DrawSync();

    // Render to texture
    unsigned SetRenderTarget(pddiTexture* tex);

    // Pure virtual implementations from pddiBaseContext
    void BeginTiming();
    float EndTiming();
    void LoadHardwareMatrix(pddiMatrixType id);
    void SetupHardwareProjection();
    void SetupHardwareLight(int handle);

    // PS3-specific methods
    ps3Display* GetDisplay() { return mDisplay; }
    ps3Device* GetDevice() { return mDevice; }

    // Current shader access
    void SetCurrentShader(ps3Shader* shader) { mCurrentShader = shader; }
    ps3Shader* GetCurrentShader() { return mCurrentShader; }

private:
    ps3Device* mDevice;
    ps3Display* mDisplay;

    // Clear values
    pddiColour mClearColour;
    float mClearDepth;
    unsigned mClearStencil;

    // State
    bool mZBufferEnabled;
    bool mZWriteEnabled;
    pddiCompareMode mZCompare;
    float mZBias;
    float mZNear;
    float mZFar;

    pddiCullMode mCullMode;
    pddiFillMode mFillMode;

    bool mColourWrite[4]; // RGBA

    bool mStencilEnabled;
    pddiCompareMode mStencilCompare;
    int mStencilRef;
    unsigned mStencilMask;
    unsigned mStencilWriteMask;
    pddiStencilOp mStencilFailOp;
    pddiStencilOp mStencilZFailOp;
    pddiStencilOp mStencilZPassOp;

    bool mFogEnabled;
    pddiColour mFogColour;
    float mFogStart;
    float mFogEnd;

    // Lighting
    pddiColour mAmbientLight;
    pddiLightDesc mLights[PS3_MAX_LIGHTS];

    // Statistics
    bool mStatsOverlay;
    unsigned int mTriCount;
    unsigned int mPrimCount;
    unsigned int mVertexCount;
    unsigned int mTextureChanges;
    unsigned int mShaderChanges;
    unsigned int mStateChanges;

    // Current shader
    ps3Shader* mCurrentShader;

    // Immediate mode primitive stream
    ps3PrimStream* mPrimStream;

    // Saved state for push/pop (PSGL doesn't have glPushAttrib/glPopAttrib)
    struct SavedState
    {
        bool zBufferEnabled;
        bool zWriteEnabled;
        pddiCompareMode zCompare;
        pddiCullMode cullMode;
        pddiFillMode fillMode;
        bool stencilEnabled;
        bool fogEnabled;
        bool colourWrite[4];
    };
    SavedState mSavedState;

    void ApplyRenderState();
    void ResetStatistics();
};

#endif /* _PS3CONTEXT_HPP */
