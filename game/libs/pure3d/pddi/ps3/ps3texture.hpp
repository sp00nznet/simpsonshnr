/*===========================================================================
    ps3texture.hpp

    PS3 PDDI Texture Management

    Copyright (c)1996-2002 Radical Entertainment Ltd
    All rights reserved.
===========================================================================*/

#ifndef _PS3TEXTURE_HPP
#define _PS3TEXTURE_HPP

#include <pddi/pddi.hpp>

class ps3Device;

//-------------------------------------------------------------------
// ps3Texture - PS3 texture using RSX memory
//-------------------------------------------------------------------
class ps3Texture : public pddiTexture
{
public:
    ps3Texture(ps3Device* device, pddiTextureDesc* desc);
    virtual ~ps3Texture();

    // pddiTexture interface
    pddiPixelFormat GetPixelFormat();
    int GetWidth();
    int GetHeight();
    int GetDepth();
    int GetAlphaDepth();
    int GetNumMipMaps();

    // Palette management
    int GetNumPaletteEntries(void);
    void SetPalette(int nEntries, pddiColour* palette);
    int GetPalette(pddiColour* palette);

    // Texture upload
    pddiLockInfo* Lock(int mipLevel, pddiRect* rect = 0);
    void Unlock(int mipLevel);

    // Paging control
    void Prefetch();
    void Discard();
    void SetPriority(int priority);
    int GetPriority();

    // PS3-specific methods
    unsigned int GetRSXTextureHandle() { return mRSXTextureHandle; }
    void* GetTextureData() { return mTextureData; }
    unsigned int GetTextureDataSize() { return mTextureDataSize; }

    // Bind the texture to a texture unit
    void Bind(int unit);

    // Swizzle control (PS3 textures often need swizzling)
    void SetSwizzle(bool enable) { mSwizzled = enable; }
    bool IsSwizzled() const { return mSwizzled; }

private:
    ps3Device* mDevice;

    // Texture properties
    int mWidth;
    int mHeight;
    int mDepth;
    int mBpp;
    int mAlphaDepth;
    int mNumMipMaps;
    pddiPixelFormat mFormat;
    pddiTextureType mType;
    pddiTextureUsageHint mUsageHint;

    // Texture data
    void* mTextureData;
    unsigned int mTextureDataSize;
    unsigned int mRSXTextureHandle;

    // Palette (for indexed textures)
    pddiColour* mPalette;
    int mNumPaletteEntries;

    // Lock info
    pddiLockInfo mLockInfo;
    bool mLocked;

    // State
    int mPriority;
    bool mSwizzled;
    bool mDataReady;       // True after Unlock() - data ready for upload
    bool mUploadComplete;  // True after successful glTexImage2D

    // Helper methods
    void AllocateTextureMemory();
    void FreeTextureMemory();
    unsigned int CalculateMipSize(int level);
    pddiPixelFormat DeterminePixelFormat(int bpp, int alphaDepth);
    void UploadToGL();  // Actually upload texture to GL (must be called from render thread)
};

#endif /* _PS3TEXTURE_HPP */
