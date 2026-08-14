/*===========================================================================
    ps3texture.cpp

    PS3 PDDI Texture Implementation

    Copyright (c)1996-2002 Radical Entertainment Ltd
    All rights reserved.
===========================================================================*/

#ifdef RAD_PS3

#include <pddi/ps3/ps3texture.hpp>
#include <pddi/ps3/ps3device.hpp>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Global flag used by game code to control frame sync checks
bool gIgnoreLastFrameSyncCheck = false;

// PS3 SDK includes
#include <PSGL/psgl.h>

//-------------------------------------------------------------------
// ps3Texture implementation
//-------------------------------------------------------------------
ps3Texture::ps3Texture(ps3Device* device, pddiTextureDesc* desc) :
    mDevice(device),
    mWidth(0),
    mHeight(0),
    mDepth(0),
    mBpp(0),
    mAlphaDepth(0),
    mNumMipMaps(0),
    mFormat(PDDI_PIXEL_UNKNOWN),
    mType(PDDI_TEXTYPE_RGB),
    mUsageHint(PDDI_USAGE_STATIC),
    mTextureData(NULL),
    mTextureDataSize(0),
    mRSXTextureHandle(0),
    mPalette(NULL),
    mNumPaletteEntries(0),
    mLocked(false),
    mPriority(0),
    mSwizzled(true),
    mDataReady(false),
    mUploadComplete(false)
{
    if (desc)
    {
        mWidth = desc->GetSizeX();
        mHeight = desc->GetSizeY();
        mDepth = desc->GetSizeZ();
        mBpp = desc->GetBitDepth();
        mAlphaDepth = desc->GetAlphaDepth();
        mNumMipMaps = desc->GetMipMapCount();
        mType = desc->GetType();
        mUsageHint = desc->GetUsage();
        mFormat = DeterminePixelFormat(mBpp, mAlphaDepth);

        AllocateTextureMemory();
    }

    memset(&mLockInfo, 0, sizeof(mLockInfo));
}

ps3Texture::~ps3Texture()
{
    FreeTextureMemory();

    if (mPalette)
    {
        delete[] mPalette;
        mPalette = NULL;
    }
}

pddiPixelFormat ps3Texture::GetPixelFormat()
{
    return mFormat;
}

int ps3Texture::GetWidth()
{
    return mWidth;
}

int ps3Texture::GetHeight()
{
    return mHeight;
}

int ps3Texture::GetDepth()
{
    return mBpp;
}

int ps3Texture::GetAlphaDepth()
{
    return mAlphaDepth;
}

int ps3Texture::GetNumMipMaps()
{
    return mNumMipMaps;
}

int ps3Texture::GetNumPaletteEntries(void)
{
    return mNumPaletteEntries;
}

void ps3Texture::SetPalette(int nEntries, pddiColour* palette)
{
    if (mPalette)
    {
        delete[] mPalette;
    }

    mNumPaletteEntries = nEntries;
    mPalette = new pddiColour[nEntries];
    memcpy(mPalette, palette, nEntries * sizeof(pddiColour));
}

int ps3Texture::GetPalette(pddiColour* palette)
{
    if (palette && mPalette)
    {
        memcpy(palette, mPalette, mNumPaletteEntries * sizeof(pddiColour));
    }
    return mNumPaletteEntries;
}

pddiLockInfo* ps3Texture::Lock(int mipLevel, pddiRect* rect)
{
    if (mLocked)
    {
        return NULL;
    }

    mLocked = true;

    // Calculate mip level dimensions
    int mipWidth = mWidth >> mipLevel;
    int mipHeight = mHeight >> mipLevel;
    if (mipWidth < 1) mipWidth = 1;
    if (mipHeight < 1) mipHeight = 1;

    // Calculate offset to mip level
    unsigned int offset = 0;
    for (int i = 0; i < mipLevel; i++)
    {
        offset += CalculateMipSize(i);
    }

    mLockInfo.width = mipWidth;
    mLockInfo.height = mipHeight;
    mLockInfo.depth = mBpp;
    mLockInfo.format = mFormat;
    mLockInfo.native = false;
    // Handle sub-byte pixel depths (4-bit = 0.5 bytes per pixel)
    mLockInfo.pitch = (mipWidth * mBpp + 7) / 8;
    mLockInfo.bits = (unsigned char*)mTextureData + offset;
    mLockInfo.palette = mPalette;

    // Set up shift/mask for pixel format
    switch (mFormat)
    {
    case PDDI_PIXEL_ARGB8888:
        mLockInfo.rgbaLShift[0] = 16; mLockInfo.rgbaRShift[0] = 0;  mLockInfo.rgbaMask[0] = 0xFF;
        mLockInfo.rgbaLShift[1] = 8;  mLockInfo.rgbaRShift[1] = 0;  mLockInfo.rgbaMask[1] = 0xFF;
        mLockInfo.rgbaLShift[2] = 0;  mLockInfo.rgbaRShift[2] = 0;  mLockInfo.rgbaMask[2] = 0xFF;
        mLockInfo.rgbaLShift[3] = 24; mLockInfo.rgbaRShift[3] = 0;  mLockInfo.rgbaMask[3] = 0xFF;
        break;
    case PDDI_PIXEL_RGB888:
        mLockInfo.rgbaLShift[0] = 16; mLockInfo.rgbaRShift[0] = 0;  mLockInfo.rgbaMask[0] = 0xFF;
        mLockInfo.rgbaLShift[1] = 8;  mLockInfo.rgbaRShift[1] = 0;  mLockInfo.rgbaMask[1] = 0xFF;
        mLockInfo.rgbaLShift[2] = 0;  mLockInfo.rgbaRShift[2] = 0;  mLockInfo.rgbaMask[2] = 0xFF;
        mLockInfo.rgbaLShift[3] = 0;  mLockInfo.rgbaRShift[3] = 0;  mLockInfo.rgbaMask[3] = 0;
        break;
    case PDDI_PIXEL_RGB565:
        mLockInfo.rgbaLShift[0] = 11; mLockInfo.rgbaRShift[0] = 3;  mLockInfo.rgbaMask[0] = 0x1F;
        mLockInfo.rgbaLShift[1] = 5;  mLockInfo.rgbaRShift[1] = 2;  mLockInfo.rgbaMask[1] = 0x3F;
        mLockInfo.rgbaLShift[2] = 0;  mLockInfo.rgbaRShift[2] = 3;  mLockInfo.rgbaMask[2] = 0x1F;
        mLockInfo.rgbaLShift[3] = 0;  mLockInfo.rgbaRShift[3] = 0;  mLockInfo.rgbaMask[3] = 0;
        break;
    default:
        break;
    }

    return &mLockInfo;
}

// Helper to ensure texture is complete with a placeholder if upload fails
static void EnsureTextureComplete(GLuint handle)
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        // Clear any additional errors
        while (glGetError() != GL_NO_ERROR) {}

        // Upload a 1x1 placeholder to ensure completeness
        glBindTexture(GL_TEXTURE_2D, handle);
        unsigned char placeholder[4] = {255, 0, 255, 255}; // Magenta
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, placeholder);
    }

    // Always set parameters to ensure completeness
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void ps3Texture::Unlock(int mipLevel)
{
    if (!mLocked)
        return;

    mLocked = false;

    // IMPORTANT: Do NOT call OpenGL functions here!
    // Unlock() is called from the loading thread, but OpenGL contexts are
    // thread-specific. All GL calls must happen on the main/render thread.
    //
    // The actual GL texture upload is deferred to Bind() or UploadToGL().
    // The texture data is already stored in mTextureData.

    // Just mark that data is ready for upload
    mDataReady = true;
    mUploadComplete = false;
}

void ps3Texture::UploadToGL()
{
    // This should only be called from the render thread
    if (!mDataReady || mUploadComplete)
        return;

    // Clear any pending GL errors
    while (glGetError() != GL_NO_ERROR) {}

    // Create GL texture if needed
    if (mRSXTextureHandle == 0)
    {
        glGenTextures(1, &mRSXTextureHandle);
    }

    glBindTexture(GL_TEXTURE_2D, mRSXTextureHandle);

    // If no data, create a 1x1 placeholder to avoid incomplete texture
    if (mTextureData == NULL)
    {
        unsigned char placeholder[4] = {255, 0, 255, 255}; // Magenta
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, placeholder);
        EnsureTextureComplete(mRSXTextureHandle);
        mUploadComplete = true;
        return;
    }

    // Upload the texture (mip level 0 only for now)
    int mipLevel = 0;
    int mipWidth = mWidth >> mipLevel;
    int mipHeight = mHeight >> mipLevel;
    if (mipWidth < 1) mipWidth = 1;
    if (mipHeight < 1) mipHeight = 1;

    // PSGL (OpenGL ES based) may not support all desktop GL formats
    // Always convert to RGBA which is universally supported
    // For ARGB8888: swizzle BGRA -> RGBA
    // For RGB888: expand to RGBA
    // For RGB565/ARGB4444: convert to RGBA8

    unsigned char* convertedData = NULL;
    int numPixels = mipWidth * mipHeight;

    switch (mFormat)
    {
    case PDDI_PIXEL_ARGB8888:
        // pddiColour stores as 0xAARRGGBB. On big-endian PS3, bytes in memory are A,R,G,B.
        // OpenGL GL_RGBA + GL_UNSIGNED_BYTE expects bytes R,G,B,A.
        convertedData = (unsigned char*)malloc(numPixels * 4);
        if (!convertedData) {
            glTexImage2D(GL_TEXTURE_2D, mipLevel, GL_RGBA,
                         mipWidth, mipHeight, 0,
                         GL_RGBA, GL_UNSIGNED_BYTE, mTextureData);
            break;
        }
        {
            unsigned char* src = (unsigned char*)mTextureData;
            unsigned char* dst = convertedData;
            for (int i = 0; i < numPixels; i++)
            {
                // Big-endian ARGB bytes: [A,R,G,B] -> RGBA bytes: [R,G,B,A]
                dst[0] = src[1]; // R
                dst[1] = src[2]; // G
                dst[2] = src[3]; // B
                dst[3] = src[0]; // A
                src += 4;
                dst += 4;
            }
        }
        glTexImage2D(GL_TEXTURE_2D, mipLevel, GL_RGBA,
                     mipWidth, mipHeight, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, convertedData);
        break;

    case PDDI_PIXEL_RGB888:
        // On big-endian PS3, 24-bit RGB bytes are already R,G,B. Just expand to RGBA.
        convertedData = (unsigned char*)malloc(numPixels * 4);
        if (convertedData) {
            unsigned char* src = (unsigned char*)mTextureData;
            unsigned char* dst = convertedData;
            for (int i = 0; i < numPixels; i++)
            {
                dst[0] = src[0]; // R
                dst[1] = src[1]; // G
                dst[2] = src[2]; // B
                dst[3] = 255;    // A
                src += 3;
                dst += 4;
            }
            glTexImage2D(GL_TEXTURE_2D, mipLevel, GL_RGBA,
                         mipWidth, mipHeight, 0,
                         GL_RGBA, GL_UNSIGNED_BYTE, convertedData);
        }
        break;

    case PDDI_PIXEL_RGB565:
        // Convert RGB565 to RGBA8
        convertedData = (unsigned char*)malloc(numPixels * 4);
        if (convertedData) {
            unsigned short* src = (unsigned short*)mTextureData;
            unsigned char* dst = convertedData;
            for (int i = 0; i < numPixels; i++)
            {
                unsigned short pixel = *src++;
                // RGB565: RRRRR GGGGGG BBBBB
                dst[0] = ((pixel >> 11) & 0x1F) * 255 / 31; // R
                dst[1] = ((pixel >> 5) & 0x3F) * 255 / 63;  // G
                dst[2] = (pixel & 0x1F) * 255 / 31;         // B
                dst[3] = 255;                               // A
                dst += 4;
            }
            glTexImage2D(GL_TEXTURE_2D, mipLevel, GL_RGBA,
                         mipWidth, mipHeight, 0,
                         GL_RGBA, GL_UNSIGNED_BYTE, convertedData);
        }
        break;

    case PDDI_PIXEL_ARGB4444:
        // Convert ARGB4444 to RGBA8
        convertedData = (unsigned char*)malloc(numPixels * 4);
        if (convertedData) {
            unsigned short* src = (unsigned short*)mTextureData;
            unsigned char* dst = convertedData;
            for (int i = 0; i < numPixels; i++)
            {
                unsigned short pixel = *src++;
                // ARGB4444: AAAA RRRR GGGG BBBB (may be BGRA4444 on some platforms)
                dst[0] = ((pixel >> 8) & 0xF) * 17;  // R (multiply by 17 to expand 4-bit to 8-bit)
                dst[1] = ((pixel >> 4) & 0xF) * 17;  // G
                dst[2] = (pixel & 0xF) * 17;         // B
                dst[3] = ((pixel >> 12) & 0xF) * 17; // A
                dst += 4;
            }
            glTexImage2D(GL_TEXTURE_2D, mipLevel, GL_RGBA,
                         mipWidth, mipHeight, 0,
                         GL_RGBA, GL_UNSIGNED_BYTE, convertedData);
        }
        break;

    case PDDI_PIXEL_PAL8:
    case PDDI_PIXEL_PAL4:
        // Palettized textures - need to expand using palette
        if (mPalette)
        {
            convertedData = (unsigned char*)malloc(numPixels * 4);
            if (convertedData) {
                unsigned char* dst = convertedData;

                if (mFormat == PDDI_PIXEL_PAL8)
                {
                    unsigned char* src = (unsigned char*)mTextureData;
                    for (int i = 0; i < numPixels; i++)
                    {
                        pddiColour c = mPalette[*src++];
                        dst[0] = c.Red();
                        dst[1] = c.Green();
                        dst[2] = c.Blue();
                        dst[3] = c.Alpha();
                        dst += 4;
                    }
                }
                else // PAL4
                {
                    unsigned char* src = (unsigned char*)mTextureData;
                    for (int i = 0; i < numPixels; i += 2)
                    {
                        unsigned char packed = *src++;

                        // First pixel (low nibble)
                        pddiColour c1 = mPalette[packed & 0xF];
                        dst[0] = c1.Red();
                        dst[1] = c1.Green();
                        dst[2] = c1.Blue();
                        dst[3] = c1.Alpha();
                        dst += 4;

                        // Second pixel (high nibble)
                        if (i + 1 < numPixels)
                        {
                            pddiColour c2 = mPalette[(packed >> 4) & 0xF];
                            dst[0] = c2.Red();
                            dst[1] = c2.Green();
                            dst[2] = c2.Blue();
                            dst[3] = c2.Alpha();
                            dst += 4;
                        }
                    }
                }
                glTexImage2D(GL_TEXTURE_2D, mipLevel, GL_RGBA,
                             mipWidth, mipHeight, 0,
                             GL_RGBA, GL_UNSIGNED_BYTE, convertedData);
            }
        }
        else
        {
            // No palette - create placeholder
            unsigned char placeholder[4] = {255, 0, 255, 255};
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, placeholder);
        }
        break;

    default:
        // Unknown format - create placeholder
        unsigned char placeholder[4] = {255, 0, 255, 255};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, placeholder);
        break;
    }

    // Clean up conversion buffer
    if (convertedData)
    {
        free(convertedData);
    }

    // Ensure texture is complete even if upload had issues
    EnsureTextureComplete(mRSXTextureHandle);

    // Mark texture as successfully uploaded
    mUploadComplete = true;
}

void ps3Texture::Prefetch()
{
    // Texture prefetching hint - not needed for immediate upload
}

void ps3Texture::Discard()
{
    // Discard texture from GPU cache if needed
}

void ps3Texture::SetPriority(int priority)
{
    mPriority = priority;
}

int ps3Texture::GetPriority()
{
    return mPriority;
}

void ps3Texture::Bind(int unit)
{
    // IMPORTANT: Only do GL operations if we're on the render thread!
    // Bind() can be called from the loading thread during asset loading,
    // but GL contexts are thread-specific.
    if (!mDevice->IsRenderThread())
    {
        // Not on render thread - skip all GL operations
        // The texture will be uploaded on the first Bind() from render thread
        return;
    }

    // Clear any pending GL errors before binding
    while (glGetError() != GL_NO_ERROR) {}

    // If data is ready but not uploaded, do the upload now (on render thread)
    if (mDataReady && !mUploadComplete)
    {
        UploadToGL();
    }

    // Create texture if it doesn't exist yet OR if upload never completed
    if (mRSXTextureHandle == 0 || !mUploadComplete)
    {
        if (mRSXTextureHandle == 0)
        {
            glGenTextures(1, &mRSXTextureHandle);
        }
        glBindTexture(GL_TEXTURE_2D, mRSXTextureHandle);
        // Initialize with 1x1 magenta pixel to avoid incomplete texture
        unsigned char placeholder[4] = {255, 0, 255, 255};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, placeholder);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // Mark as complete now that we have a placeholder
        mUploadComplete = true;
        // Clear any errors from placeholder creation
        while (glGetError() != GL_NO_ERROR) {}
    }

    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, mRSXTextureHandle);
    glEnable(GL_TEXTURE_2D);

    // Clear any errors from binding
    while (glGetError() != GL_NO_ERROR) {}
}

void ps3Texture::AllocateTextureMemory()
{
    // Calculate total size including mipmaps
    mTextureDataSize = 0;
    int numLevels = mNumMipMaps > 0 ? mNumMipMaps : 1;

    for (int i = 0; i < numLevels; i++)
    {
        mTextureDataSize += CalculateMipSize(i);
    }

    // Allocate from RSX memory
    mTextureData = mDevice->AllocateRSXMemory(mTextureDataSize, 128);

    if (mTextureData)
    {
        memset(mTextureData, 0, mTextureDataSize);
    }
    else
    {
        // Fall back to a minimal 1x1 allocation so we don't crash
        // This texture will display as a solid color but won't crash
        mTextureDataSize = 4; // 1 RGBA pixel
        mTextureData = malloc(4);
        if (mTextureData)
        {
            // Initialize to magenta (missing texture color)
            unsigned char* p = (unsigned char*)mTextureData;
            p[0] = 255; p[1] = 0; p[2] = 255; p[3] = 255;
        }
        // Override dimensions so Lock() calculates correct values
        mWidth = 1;
        mHeight = 1;
        mBpp = 32;
        mNumMipMaps = 0;
    }
}

void ps3Texture::FreeTextureMemory()
{
    if (mRSXTextureHandle)
    {
        glDeleteTextures(1, &mRSXTextureHandle);
        mRSXTextureHandle = 0;
    }

    if (mTextureData)
    {
        // Check if this is our fallback 1x1 texture (allocated with malloc)
        // RSX memory is in the device's managed block, fallback is from heap
        unsigned char* rsxBase = (unsigned char*)mDevice->GetRSXMemoryBase();
        unsigned char* texPtr = (unsigned char*)mTextureData;
        unsigned int rsxSize = mDevice->GetTotalRSXMemory();

        if (texPtr >= rsxBase && texPtr < rsxBase + rsxSize)
        {
            // RSX memory - use device free (which is a no-op for bump allocator)
            mDevice->FreeRSXMemory(mTextureData);
        }
        else
        {
            // Fallback malloc memory
            free(mTextureData);
        }
        mTextureData = NULL;
    }
}

unsigned int ps3Texture::CalculateMipSize(int level)
{
    int mipWidth = mWidth >> level;
    int mipHeight = mHeight >> level;
    if (mipWidth < 1) mipWidth = 1;
    if (mipHeight < 1) mipHeight = 1;

    // Handle sub-byte pixel depths (4-bit = 0.5 bytes per pixel)
    return (mipWidth * mipHeight * mBpp + 7) / 8;
}

pddiPixelFormat ps3Texture::DeterminePixelFormat(int bpp, int alphaDepth)
{
    switch (bpp)
    {
    case 32:
        return (alphaDepth > 0) ? PDDI_PIXEL_ARGB8888 : PDDI_PIXEL_RGB888;
    case 24:
        return PDDI_PIXEL_RGB888;
    case 16:
        return (alphaDepth > 0) ? PDDI_PIXEL_ARGB4444 : PDDI_PIXEL_RGB565;
    case 8:
        return PDDI_PIXEL_PAL8;
    case 4:
        return PDDI_PIXEL_PAL4;
    default:
        return PDDI_PIXEL_UNKNOWN;
    }
}

#endif // RAD_PS3
