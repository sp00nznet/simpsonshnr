/*===========================================================================
    ps3prim.hpp

    PS3 PDDI Primitive Buffer

    Copyright (c)1996-2002 Radical Entertainment Ltd
    All rights reserved.
===========================================================================*/

#ifndef _PS3PRIM_HPP
#define _PS3PRIM_HPP

#include <pddi/pddi.hpp>

class ps3Device;
class ps3Context;

//-------------------------------------------------------------------
// ps3PrimBufferStream - Stream interface for filling primitive buffers
//-------------------------------------------------------------------
class ps3PrimBufferStream : public pddiPrimBufferStream
{
public:
    ps3PrimBufferStream();
    virtual ~ps3PrimBufferStream();

    void Position(float x, float y, float z);
    void Normal(float x, float y, float z);
    void Binormal(float x, float y, float z);
    void Tangent(float x, float y, float z);
    void Colour(pddiColour colour, int channel = 0);
    void TexCoord1(float s, int channel = 0);
    void TexCoord2(float s, float t, int channel = 0);
    void TexCoord3(float s, float t, float u, int channel = 0);
    void TexCoord4(float s, float t, float u, float v, int channel = 0);

    void Specular(pddiColour colour);
    void SkinIndices(unsigned a, unsigned b = 0, unsigned c = 0, unsigned d = 0);
    void SkinWeights(float a, float b = 0.0f, float c = 0.0f);

    void Vertex(rmt::Vector* v, pddiColour c);
    void Vertex(rmt::Vector* v, rmt::Vector* n);
    void Vertex(rmt::Vector* v, rmt::Vector2* uv);
    void Vertex(rmt::Vector* v, pddiColour c, rmt::Vector2* uv);
    void Vertex(rmt::Vector* v, rmt::Vector* n, rmt::Vector2* uv);

    void Next();

    // Setup for streaming
    void Begin(void* vertexBuffer, unsigned vertexFormat, unsigned stride);
    void End();

private:
    void* mVertexBuffer;
    void* mCurrentVertex;
    unsigned mVertexFormat;
    unsigned mStride;
    unsigned mVertexCount;

    // Current vertex components
    float mPosition[3];
    float mNormal[3];
    float mBinormal[3];
    float mTangent[3];
    pddiColour mColour[2];
    float mTexCoords[8][4];
    pddiColour mSpecular;
    unsigned mSkinIndices[4];
    float mSkinWeights[3];
};

//-------------------------------------------------------------------
// ps3PrimBuffer - PS3 primitive buffer using RSX memory
//-------------------------------------------------------------------
class ps3PrimBuffer : public pddiPrimBuffer
{
public:
    ps3PrimBuffer(ps3Device* device, pddiPrimBufferDesc* desc);
    virtual ~ps3PrimBuffer();

    // pddiPrimBuffer interface
    pddiPrimBufferStream* Lock();
    void Unlock(pddiPrimBufferStream* stream);
    void Finalize();

    unsigned char* LockIndexBuffer();
    void UnlockIndexBuffer(int count);

    void SetIndices(unsigned short* indices, int count);

    bool CheckMemImageVersion(int version);
    void* LockMemImage(unsigned memImageLength);
    void UnlockMemImage();
    void SetMemImageParam(unsigned param, unsigned value);

    void SetUsedSize(int size);

    // PS3-specific methods
    void Display(ps3Context* context);

    pddiPrimType GetPrimType() const { return mPrimType; }
    unsigned GetVertexFormat() const { return mVertexFormat; }
    unsigned GetVertexCount() const { return mVertexCount; }
    unsigned GetIndexCount() const { return mIndexCount; }

    void* GetVertexBuffer() { return mVertexBuffer; }
    void* GetIndexBuffer() { return mIndexBuffer; }
    unsigned GetVertexStride() const { return mStride; }

private:
    ps3Device* mDevice;

    // Primitive info
    pddiPrimType mPrimType;
    unsigned mVertexFormat;
    unsigned mVertexCount;
    unsigned mIndexCount;
    unsigned mStride;
    bool mMemoryImaged;
    bool mDeformed;
    unsigned mMatrixCount;

    // Buffers
    void* mVertexBuffer;
    void* mIndexBuffer;
    unsigned mVertexBufferSize;
    unsigned mIndexBufferSize;

    // RSX handles
    unsigned int mVertexArrayOffset;
    unsigned int mIndexArrayOffset;

    // Lock state
    bool mLocked;
    bool mIndexLocked;
    ps3PrimBufferStream mStream;

    // Memory image
    void* mMemImageData;
    unsigned mMemImageLength;

    // Used for dynamic buffers
    int mUsedVertexCount;

    // Helper methods
    void AllocateBuffers();
    void FreeBuffers();
    unsigned CalculateStride(unsigned vertexFormat);
    void UploadToRSX();
};

#endif /* _PS3PRIM_HPP */
