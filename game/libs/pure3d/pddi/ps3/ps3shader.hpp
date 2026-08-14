/*===========================================================================
    ps3shader.hpp

    PS3 PDDI Shader Base Class

    Copyright (c)1996-2002 Radical Entertainment Ltd
    All rights reserved.
===========================================================================*/

#ifndef _PS3SHADER_HPP
#define _PS3SHADER_HPP

#include <pddi/pddi.hpp>
#include <pddi/base/baseshader.hpp>
#include <pddi/base/basetype.hpp>

class ps3Device;
class ps3Context;
class ps3Texture;

//-------------------------------------------------------------------
// ps3Shader - Base shader class for PS3 using Cg
//-------------------------------------------------------------------
class ps3Shader : public pddiBaseShader
{
public:
    ps3Shader(ps3Device* device);
    virtual ~ps3Shader();

    // pddiShader interface
    const char* GetType(void);

    bool SetTexture(unsigned paramName, pddiTexture* tex);
    bool SetInt(unsigned paramName, int value);
    bool SetFloat(unsigned paramName, float value);
    bool SetColour(unsigned paramName, pddiColour colour);
    bool SetVector(unsigned paramName, const rmt::Vector& vec);
    bool SetMatrix(unsigned paramName, const rmt::Matrix& mat);

    // Shader binding
    virtual void Bind(ps3Context* context);
    virtual void Unbind();

    // Pass management
    virtual void SetPass(int pass) { mCurrentPass = pass; }
    virtual int GetPasses() { return 1; }
    virtual void PreRender(unsigned pass = 0) {}
    virtual void PostRender(unsigned pass = 0) {}

    // For multiple texture support
    void SetTexture(ps3Texture* texture, int unit = 0);
    ps3Texture* GetTexture(int unit = 0);

    // Material properties
    void SetDiffuse(pddiColour colour) { mDiffuse = colour; }
    void SetAmbient(pddiColour colour) { mAmbient = colour; }
    void SetSpecular(pddiColour colour) { mSpecular = colour; }
    void SetEmissive(pddiColour colour) { mEmissive = colour; }
    void SetShininess(float shininess) { mShininess = shininess; }

    pddiColour GetDiffuse() const { return mDiffuse; }
    pddiColour GetAmbient() const { return mAmbient; }
    pddiColour GetSpecular() const { return mSpecular; }
    pddiColour GetEmissive() const { return mEmissive; }
    float GetShininess() const { return mShininess; }

    // Alpha/blend state
    void SetAlphaTest(bool enable, pddiCompareMode mode = PDDI_COMPARE_GREATER, float ref = 0.5f);
    void SetAlphaBlend(bool enable, pddiBlendMode src = PDDI_BLEND_ONE, pddiBlendMode dst = PDDI_BLEND_ZERO);
    void SetTwoSided(bool enable) { mTwoSided = enable; }

    bool IsAlphaTestEnabled() const { return mAlphaTestEnabled; }
    bool IsAlphaBlendEnabled() const { return mAlphaBlendEnabled; }
    bool IsTwoSided() const { return mTwoSided; }

protected:
    ps3Device* mDevice;
    int mCurrentPass;

    // Textures (up to 8 texture units)
    ps3Texture* mTextures[8];

    // Material properties
    pddiColour mDiffuse;
    pddiColour mAmbient;
    pddiColour mSpecular;
    pddiColour mEmissive;
    float mShininess;

    // Alpha state
    bool mAlphaTestEnabled;
    pddiCompareMode mAlphaTestMode;
    float mAlphaTestRef;

    bool mAlphaBlendEnabled;
    pddiBlendMode mSrcBlend;
    pddiBlendMode mDstBlend;

    bool mTwoSided;

    // Cg program handles (if using Cg shaders)
    void* mVertexProgram;
    void* mFragmentProgram;

    // Apply state to GL
    void ApplyMaterialState(ps3Context* context);
    void ApplyAlphaState(ps3Context* context);
public:
    void ApplyTextureState(ps3Context* context);
};

//-------------------------------------------------------------------
// ps3SimpleShader - Basic single-texture shader
//-------------------------------------------------------------------
class ps3SimpleShader : public ps3Shader
{
public:
    ps3SimpleShader(ps3Device* device);
    virtual ~ps3SimpleShader();

    const char* GetType(void) { return "simple"; }

    void Bind(ps3Context* context);
    void PreRender(unsigned pass = 0);
    void PostRender(unsigned pass = 0);
};

//-------------------------------------------------------------------
// ps3ErrorShader - Fallback shader for errors
//-------------------------------------------------------------------
class ps3ErrorShader : public ps3Shader
{
public:
    ps3ErrorShader(ps3Device* device);
    virtual ~ps3ErrorShader();

    const char* GetType(void) { return "error"; }

    void Bind(ps3Context* context);
};

#endif /* _PS3SHADER_HPP */
