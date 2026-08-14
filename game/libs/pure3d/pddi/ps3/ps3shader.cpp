/*===========================================================================
    ps3shader.cpp

    PS3 PDDI Shader Implementation

    Copyright (c)1996-2002 Radical Entertainment Ltd
    All rights reserved.
===========================================================================*/

#ifdef RAD_PS3

#include <pddi/ps3/ps3shader.hpp>
#include <pddi/ps3/ps3device.hpp>
#include <pddi/ps3/ps3context.hpp>
#include <pddi/ps3/ps3texture.hpp>

#include <string.h>

// PS3 SDK includes
#include <PSGL/psgl.h>

//-------------------------------------------------------------------
// ps3Shader base class implementation
//-------------------------------------------------------------------
ps3Shader::ps3Shader(ps3Device* device) :
    mDevice(device),
    mDiffuse(255, 255, 255, 255),
    mAmbient(64, 64, 64, 255),
    mSpecular(0, 0, 0, 255),
    mEmissive(0, 0, 0, 255),
    mShininess(0.0f),
    mAlphaTestEnabled(false),
    mAlphaTestMode(PDDI_COMPARE_GREATER),
    mAlphaTestRef(0.5f),
    mAlphaBlendEnabled(false),
    mLit(false),
    mSrcBlend(PDDI_BLEND_ONE),
    mDstBlend(PDDI_BLEND_ZERO),
    mTwoSided(false),
    mVertexProgram(NULL),
    mFragmentProgram(NULL)
{
    for (int i = 0; i < 8; i++)
    {
        mTextures[i] = NULL;
    }
}

ps3Shader::~ps3Shader()
{
    // Clean up Cg programs if any
}

const char* ps3Shader::GetType(void)
{
    return "ps3shader";
}

bool ps3Shader::SetTexture(unsigned paramName, pddiTexture* tex)
{
    if (paramName == PDDI_SP_BASETEX)
    {
        mTextures[0] = static_cast<ps3Texture*>(tex);
        return true;
    }
    return false;
}

bool ps3Shader::SetInt(unsigned paramName, int value)
{
    switch (paramName)
    {
    case PDDI_SP_TWOSIDED:
        mTwoSided = (value != 0);
        return true;
    case PDDI_SP_ISLIT:
        // Without this the case fell through to "return false" and GL_LIGHTING
        // was never managed, so unlit 2D geometry inherited whatever lighting
        // state the last 3D draw left behind -- with no lights set up that
        // multiplies every fragment to black.
        mLit = (value != 0);
        return true;
    case PDDI_SP_ALPHATEST:
        mAlphaTestEnabled = (value != 0);
        return true;
    case PDDI_SP_BLENDMODE:
        {
            // Map composite blend mode to src/dst blend factors
            // (matches the GL backend's alphaBlendTable)
            pddiBlendMode mode = (pddiBlendMode)value;
            switch (mode)
            {
            case PDDI_BLEND_NONE:
                mAlphaBlendEnabled = false;
                mSrcBlend = PDDI_BLEND_ONE;
                mDstBlend = PDDI_BLEND_ZERO;
                break;
            case PDDI_BLEND_ALPHA:
                mAlphaBlendEnabled = true;
                mSrcBlend = PDDI_BLEND_SRCALPHA;
                mDstBlend = PDDI_BLEND_INVSRCALPHA;
                break;
            case PDDI_BLEND_ADD:
                mAlphaBlendEnabled = true;
                mSrcBlend = PDDI_BLEND_ONE;
                mDstBlend = PDDI_BLEND_ONE;
                break;
            case PDDI_BLEND_SUBTRACT:
                mAlphaBlendEnabled = false;
                mSrcBlend = PDDI_BLEND_ONE;
                mDstBlend = PDDI_BLEND_ZERO;
                break;
            case PDDI_BLEND_MODULATE:
                mAlphaBlendEnabled = true;
                mSrcBlend = PDDI_BLEND_DESTCOLOUR;
                mDstBlend = PDDI_BLEND_ZERO;
                break;
            case PDDI_BLEND_MODULATE2:
                mAlphaBlendEnabled = true;
                mSrcBlend = PDDI_BLEND_DESTCOLOUR;
                mDstBlend = PDDI_BLEND_SRCCOLOUR;
                break;
            case PDDI_BLEND_ADDMODULATEALPHA:
                mAlphaBlendEnabled = true;
                mSrcBlend = PDDI_BLEND_ONE;
                mDstBlend = PDDI_BLEND_SRCALPHA;
                break;
            case PDDI_BLEND_SUBMODULATEALPHA:
                mAlphaBlendEnabled = true;
                mSrcBlend = PDDI_BLEND_SRCALPHA;
                mDstBlend = PDDI_BLEND_SRCALPHA;
                break;
            default:
                mAlphaBlendEnabled = false;
                mSrcBlend = PDDI_BLEND_ONE;
                mDstBlend = PDDI_BLEND_ZERO;
                break;
            }
        }
        return true;
    default:
        return false;
    }
}

bool ps3Shader::SetFloat(unsigned paramName, float value)
{
    switch (paramName)
    {
    case PDDI_SP_SHININESS:
        mShininess = value;
        return true;
    case PDDI_SP_ALPHACOMPARE_THRESHOLD:
        mAlphaTestRef = value;
        return true;
    default:
        return false;
    }
}

bool ps3Shader::SetColour(unsigned paramName, pddiColour colour)
{
    switch (paramName)
    {
    case PDDI_SP_DIFFUSE:
        mDiffuse = colour;
        return true;
    case PDDI_SP_AMBIENT:
        mAmbient = colour;
        return true;
    case PDDI_SP_SPECULAR:
        mSpecular = colour;
        return true;
    case PDDI_SP_EMISSIVE:
        mEmissive = colour;
        return true;
    default:
        return false;
    }
}

bool ps3Shader::SetVector(unsigned paramName, const rmt::Vector& vec)
{
    return false;
}

bool ps3Shader::SetMatrix(unsigned paramName, const rmt::Matrix& mat)
{
    return false;
}

void ps3Shader::Bind(ps3Context* context)
{
    ApplyMaterialState(context);
    ApplyTextureState(context);
    ApplyAlphaState(context);
}

void ps3Shader::Unbind()
{
    // Restore default state if needed
}

void ps3Shader::SetTexture(ps3Texture* texture, int unit)
{
    if (unit >= 0 && unit < 8)
    {
        mTextures[unit] = texture;
    }
}

ps3Texture* ps3Shader::GetTexture(int unit)
{
    if (unit >= 0 && unit < 8)
    {
        return mTextures[unit];
    }
    return NULL;
}

void ps3Shader::SetAlphaTest(bool enable, pddiCompareMode mode, float ref)
{
    mAlphaTestEnabled = enable;
    mAlphaTestMode = mode;
    mAlphaTestRef = ref;
}

void ps3Shader::SetAlphaBlend(bool enable, pddiBlendMode src, pddiBlendMode dst)
{
    mAlphaBlendEnabled = enable;
    mSrcBlend = src;
    mDstBlend = dst;
}

void ps3Shader::ApplyMaterialState(ps3Context* context)
{
    // Set material properties
    float diffuse[4] = {mDiffuse.Red() / 255.0f, mDiffuse.Green() / 255.0f,
                        mDiffuse.Blue() / 255.0f, mDiffuse.Alpha() / 255.0f};
    float ambient[4] = {mAmbient.Red() / 255.0f, mAmbient.Green() / 255.0f,
                        mAmbient.Blue() / 255.0f, mAmbient.Alpha() / 255.0f};
    float specular[4] = {mSpecular.Red() / 255.0f, mSpecular.Green() / 255.0f,
                         mSpecular.Blue() / 255.0f, mSpecular.Alpha() / 255.0f};
    float emissive[4] = {mEmissive.Red() / 255.0f, mEmissive.Green() / 255.0f,
                         mEmissive.Blue() / 255.0f, mEmissive.Alpha() / 255.0f};

    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissive);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mShininess);

    // Two-sided lighting model - only control GL_LIGHT_MODEL_TWO_SIDE here.
    // Do NOT touch GL_CULL_FACE - that is managed by the context's SetCullMode().
    // The shader was incorrectly re-enabling GL_CULL_FACE, overriding Scrooby's
    // PDDI_CULL_NONE and causing all 2D sprites to be back-face culled (black screen).
    // Match pglContext/glmat.cpp: lighting is enabled per material, not left
    // to whatever the previous draw set.
    if (mLit)
    {
        glEnable(GL_LIGHTING);
    }
    else
    {
        glDisable(GL_LIGHTING);
    }

    if (mTwoSided)
    {
        glDisable(GL_CULL_FACE);
        float twoSideValue = 1.0f;
        glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, &twoSideValue);
    }
    else
    {
        float twoSideValue = 0.0f;
        glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, &twoSideValue);
    }
}

void ps3Shader::ApplyTextureState(ps3Context* context)
{
    // Bind textures to texture units
    for (int i = 0; i < 8; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);

        if (mTextures[i])
        {
            mTextures[i]->Bind(i);
        }
        else
        {
            glDisable(GL_TEXTURE_2D);
        }
    }

    // Reset to texture unit 0
    glActiveTexture(GL_TEXTURE0);
}

static GLenum pddiToGLBlend(pddiBlendMode mode)
{
    switch (mode)
    {
    case PDDI_BLEND_ZERO:         return GL_ZERO;
    case PDDI_BLEND_ONE:          return GL_ONE;
    case PDDI_BLEND_SRCCOLOUR:    return GL_SRC_COLOR;
    case PDDI_BLEND_INVSRCCOLOUR: return GL_ONE_MINUS_SRC_COLOR;
    case PDDI_BLEND_SRCALPHA:     return GL_SRC_ALPHA;
    case PDDI_BLEND_INVSRCALPHA:  return GL_ONE_MINUS_SRC_ALPHA;
    case PDDI_BLEND_DESTALPHA:    return GL_DST_ALPHA;
    case PDDI_BLEND_INVDESTALPHA: return GL_ONE_MINUS_DST_ALPHA;
    case PDDI_BLEND_DESTCOLOUR:   return GL_DST_COLOR;
    case PDDI_BLEND_INVDESTCOLOUR:return GL_ONE_MINUS_DST_COLOR;
    default:                      return GL_ONE;
    }
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
    default:                      return GL_ALWAYS;
    }
}

void ps3Shader::ApplyAlphaState(ps3Context* context)
{
    // Alpha test
    if (mAlphaTestEnabled)
    {
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(pddiToGLCompare(mAlphaTestMode), mAlphaTestRef);
    }
    else
    {
        glDisable(GL_ALPHA_TEST);
    }

    // Alpha blend
    if (mAlphaBlendEnabled)
    {
        glEnable(GL_BLEND);
        glBlendFunc(pddiToGLBlend(mSrcBlend), pddiToGLBlend(mDstBlend));
    }
    else
    {
        glDisable(GL_BLEND);
    }
}


//-------------------------------------------------------------------
// ps3SimpleShader implementation
//-------------------------------------------------------------------
ps3SimpleShader::ps3SimpleShader(ps3Device* device) :
    ps3Shader(device)
{
}

ps3SimpleShader::~ps3SimpleShader()
{
}

void ps3SimpleShader::Bind(ps3Context* context)
{
    ps3Shader::Bind(context);

    // Disable lighting for now - vertex colors will be used directly
    // This avoids the black screen issue when no lights are set up
    glDisable(GL_LIGHTING);

    // Set a default color in case no vertex colors
    glColor4f(mDiffuse.Red() / 255.0f, mDiffuse.Green() / 255.0f,
              mDiffuse.Blue() / 255.0f, mDiffuse.Alpha() / 255.0f);

    // Enable texture if we have one
    if (mTextures[0])
    {
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }
    else
    {
        glDisable(GL_TEXTURE_2D);
    }
}

void ps3SimpleShader::PreRender(unsigned pass)
{
    // Pre-render setup
}

void ps3SimpleShader::PostRender(unsigned pass)
{
    // Post-render cleanup
}


//-------------------------------------------------------------------
// ps3ErrorShader implementation
//-------------------------------------------------------------------
ps3ErrorShader::ps3ErrorShader(ps3Device* device) :
    ps3Shader(device)
{
    // Set to bright magenta to indicate error
    mDiffuse = pddiColour(255, 0, 255, 255);
    mEmissive = pddiColour(255, 0, 255, 255);
}

ps3ErrorShader::~ps3ErrorShader()
{
}

void ps3ErrorShader::Bind(ps3Context* context)
{
    // Simple error shader - bright magenta
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
}

#endif // RAD_PS3
