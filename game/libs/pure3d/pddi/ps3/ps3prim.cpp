/*===========================================================================
    ps3prim.cpp

    PS3 PDDI Primitive Buffer Implementation

    Copyright (c)1996-2002 Radical Entertainment Ltd
    All rights reserved.
===========================================================================*/

#ifdef RAD_PS3

#include <pddi/ps3/ps3prim.hpp>
#include <pddi/ps3/ps3device.hpp>
#include <pddi/ps3/ps3context.hpp>

#include <string.h>

// PS3 SDK includes
#include <PSGL/psgl.h>

//-------------------------------------------------------------------
// ps3PrimBufferStream implementation
//-------------------------------------------------------------------
ps3PrimBufferStream::ps3PrimBufferStream() :
    mVertexBuffer(NULL),
    mCurrentVertex(NULL),
    mVertexFormat(0),
    mStride(0),
    mVertexCount(0)
{
    memset(mPosition, 0, sizeof(mPosition));
    memset(mNormal, 0, sizeof(mNormal));
    memset(mBinormal, 0, sizeof(mBinormal));
    memset(mTangent, 0, sizeof(mTangent));
    mColour[0] = pddiColour(255, 255, 255, 255);
    mColour[1] = pddiColour(255, 255, 255, 255);
    memset(mTexCoords, 0, sizeof(mTexCoords));
    mSpecular = pddiColour(0, 0, 0, 0);
    memset(mSkinIndices, 0, sizeof(mSkinIndices));
    memset(mSkinWeights, 0, sizeof(mSkinWeights));
}

ps3PrimBufferStream::~ps3PrimBufferStream()
{
}

void ps3PrimBufferStream::Position(float x, float y, float z)
{
    mPosition[0] = x;
    mPosition[1] = y;
    mPosition[2] = z;
}

void ps3PrimBufferStream::Normal(float x, float y, float z)
{
    mNormal[0] = x;
    mNormal[1] = y;
    mNormal[2] = z;
}

void ps3PrimBufferStream::Binormal(float x, float y, float z)
{
    mBinormal[0] = x;
    mBinormal[1] = y;
    mBinormal[2] = z;
}

void ps3PrimBufferStream::Tangent(float x, float y, float z)
{
    mTangent[0] = x;
    mTangent[1] = y;
    mTangent[2] = z;
}

void ps3PrimBufferStream::Colour(pddiColour colour, int channel)
{
    if (channel >= 0 && channel < 2)
    {
        mColour[channel] = colour;
    }
}

void ps3PrimBufferStream::TexCoord1(float s, int channel)
{
    if (channel >= 0 && channel < 8)
    {
        mTexCoords[channel][0] = s;
        mTexCoords[channel][1] = 0.0f;
        mTexCoords[channel][2] = 0.0f;
        mTexCoords[channel][3] = 1.0f;
    }
}

void ps3PrimBufferStream::TexCoord2(float s, float t, int channel)
{
    if (channel >= 0 && channel < 8)
    {
        mTexCoords[channel][0] = s;
        mTexCoords[channel][1] = t;
        mTexCoords[channel][2] = 0.0f;
        mTexCoords[channel][3] = 1.0f;
    }
}

void ps3PrimBufferStream::TexCoord3(float s, float t, float u, int channel)
{
    if (channel >= 0 && channel < 8)
    {
        mTexCoords[channel][0] = s;
        mTexCoords[channel][1] = t;
        mTexCoords[channel][2] = u;
        mTexCoords[channel][3] = 1.0f;
    }
}

void ps3PrimBufferStream::TexCoord4(float s, float t, float u, float v, int channel)
{
    if (channel >= 0 && channel < 8)
    {
        mTexCoords[channel][0] = s;
        mTexCoords[channel][1] = t;
        mTexCoords[channel][2] = u;
        mTexCoords[channel][3] = v;
    }
}

void ps3PrimBufferStream::Specular(pddiColour colour)
{
    mSpecular = colour;
}

void ps3PrimBufferStream::SkinIndices(unsigned a, unsigned b, unsigned c, unsigned d)
{
    mSkinIndices[0] = a;
    mSkinIndices[1] = b;
    mSkinIndices[2] = c;
    mSkinIndices[3] = d;
}

void ps3PrimBufferStream::SkinWeights(float a, float b, float c)
{
    mSkinWeights[0] = a;
    mSkinWeights[1] = b;
    mSkinWeights[2] = c;
}

void ps3PrimBufferStream::Vertex(rmt::Vector* v, pddiColour c)
{
    Position(v->x, v->y, v->z);
    Colour(c);
    Next();
}

void ps3PrimBufferStream::Vertex(rmt::Vector* v, rmt::Vector* n)
{
    Position(v->x, v->y, v->z);
    Normal(n->x, n->y, n->z);
    Next();
}

void ps3PrimBufferStream::Vertex(rmt::Vector* v, rmt::Vector2* uv)
{
    Position(v->x, v->y, v->z);
    TexCoord2(uv->u, uv->v);
    Next();
}

void ps3PrimBufferStream::Vertex(rmt::Vector* v, pddiColour c, rmt::Vector2* uv)
{
    Position(v->x, v->y, v->z);
    Colour(c);
    TexCoord2(uv->u, uv->v);
    Next();
}

void ps3PrimBufferStream::Vertex(rmt::Vector* v, rmt::Vector* n, rmt::Vector2* uv)
{
    Position(v->x, v->y, v->z);
    Normal(n->x, n->y, n->z);
    TexCoord2(uv->u, uv->v);
    Next();
}

void ps3PrimBufferStream::Next()
{
    if (!mCurrentVertex)
        return;

    // Write vertex data based on format
    float* fptr = (float*)mCurrentVertex;
    unsigned char* bptr = (unsigned char*)mCurrentVertex;

    // Position (always present)
    if (mVertexFormat & PDDI_V_POSITION)
    {
        *fptr++ = mPosition[0];
        *fptr++ = mPosition[1];
        *fptr++ = mPosition[2];
    }

    // Normal
    if (mVertexFormat & PDDI_V_NORMAL)
    {
        *fptr++ = mNormal[0];
        *fptr++ = mNormal[1];
        *fptr++ = mNormal[2];
    }

    // Color
    if (mVertexFormat & PDDI_V_COLOUR)
    {
        unsigned int* colPtr = (unsigned int*)fptr;
        // Pack colour as RGBA for OpenGL (big-endian PS3 stores as R,G,B,A in memory)
        *colPtr++ = ((mColour[0].Red() << 24) | (mColour[0].Green() << 16) |
                     (mColour[0].Blue() << 8) | mColour[0].Alpha());
        fptr = (float*)colPtr;
    }

    // UV coordinates
    int numUVs = (mVertexFormat & PDDI_V_UVMASK) >> PDDI_V_UVSHIFT;
    for (int i = 0; i < numUVs; i++)
    {
        *fptr++ = mTexCoords[i][0];
        *fptr++ = mTexCoords[i][1];
    }

    // Move to next vertex
    mCurrentVertex = (unsigned char*)mCurrentVertex + mStride;
    mVertexCount++;
}

void ps3PrimBufferStream::Begin(void* vertexBuffer, unsigned vertexFormat, unsigned stride)
{
    mVertexBuffer = vertexBuffer;
    mCurrentVertex = vertexBuffer;
    mVertexFormat = vertexFormat;
    mStride = stride;
    mVertexCount = 0;
}

void ps3PrimBufferStream::End()
{
    mVertexBuffer = NULL;
    mCurrentVertex = NULL;
}


//-------------------------------------------------------------------
// ps3PrimBuffer implementation
//-------------------------------------------------------------------
ps3PrimBuffer::ps3PrimBuffer(ps3Device* device, pddiPrimBufferDesc* desc) :
    mDevice(device),
    mPrimType(PDDI_PRIM_TRIANGLES),
    mVertexFormat(0),
    mVertexCount(0),
    mIndexCount(0),
    mStride(0),
    mMemoryImaged(false),
    mDeformed(false),
    mMatrixCount(0),
    mVertexBuffer(NULL),
    mIndexBuffer(NULL),
    mVertexBufferSize(0),
    mIndexBufferSize(0),
    mVertexArrayOffset(0),
    mIndexArrayOffset(0),
    mLocked(false),
    mIndexLocked(false),
    mMemImageData(NULL),
    mMemImageLength(0),
    mUsedVertexCount(0)
{
    if (desc)
    {
        mPrimType = desc->GetPrimType();
        mVertexFormat = desc->GetVertexFormat();
        mVertexCount = desc->GetVertexCount();
        mIndexCount = desc->GetIndexCount();
        mMemoryImaged = desc->GetMemoryImaged();
        mDeformed = desc->GetDeformed();
        mMatrixCount = desc->GetMatrixCount();

        mStride = CalculateStride(mVertexFormat);
        AllocateBuffers();
    }
}

ps3PrimBuffer::~ps3PrimBuffer()
{
    FreeBuffers();
}

pddiPrimBufferStream* ps3PrimBuffer::Lock()
{
    if (mLocked || !mVertexBuffer)
        return NULL;

    mLocked = true;
    mStream.Begin(mVertexBuffer, mVertexFormat, mStride);
    return &mStream;
}

void ps3PrimBuffer::Unlock(pddiPrimBufferStream* stream)
{
    if (!mLocked)
        return;

    mStream.End();
    mLocked = false;

    // Upload to GPU
    UploadToRSX();
}

void ps3PrimBuffer::Finalize()
{
    UploadToRSX();
}

unsigned char* ps3PrimBuffer::LockIndexBuffer()
{
    if (mIndexLocked || !mIndexBuffer)
        return NULL;

    mIndexLocked = true;
    return (unsigned char*)mIndexBuffer;
}

void ps3PrimBuffer::UnlockIndexBuffer(int count)
{
    mIndexLocked = false;
    if (count > 0)
    {
        mIndexCount = count;
    }
}

void ps3PrimBuffer::SetIndices(unsigned short* indices, int count)
{
    if (indices && count > 0 && mIndexBuffer)
    {
        memcpy(mIndexBuffer, indices, count * sizeof(unsigned short));
        mIndexCount = count;
    }
}

bool ps3PrimBuffer::CheckMemImageVersion(int version)
{
    // Version check for memory-imaged data
    return true;
}

void* ps3PrimBuffer::LockMemImage(unsigned memImageLength)
{
    mMemImageLength = memImageLength;
    mMemImageData = mDevice->AllocateRSXMemory(memImageLength, 16);
    return mMemImageData;
}

void ps3PrimBuffer::UnlockMemImage()
{
    // Process memory image data if needed
}

void ps3PrimBuffer::SetMemImageParam(unsigned param, unsigned value)
{
    // Set memory image parameters
}

void ps3PrimBuffer::SetUsedSize(int size)
{
    mUsedVertexCount = size;
}

void ps3PrimBuffer::Display(ps3Context* context)
{
    if (!mVertexBuffer || mVertexCount == 0)
        return;

    int vertexCount = mUsedVertexCount > 0 ? mUsedVertexCount : mVertexCount;

    // Set up vertex arrays
    glEnableClientState(GL_VERTEX_ARRAY);

    unsigned char* ptr = (unsigned char*)mVertexBuffer;
    int offset = 0;

    // Position
    if (mVertexFormat & PDDI_V_POSITION)
    {
        glVertexPointer(3, GL_FLOAT, mStride, ptr + offset);
        offset += 3 * sizeof(float);
    }

    // Normal
    if (mVertexFormat & PDDI_V_NORMAL)
    {
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, mStride, ptr + offset);
        offset += 3 * sizeof(float);
    }
    else
    {
        glDisableClientState(GL_NORMAL_ARRAY);
    }

    // Color
    if (mVertexFormat & PDDI_V_COLOUR)
    {
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_UNSIGNED_BYTE, mStride, ptr + offset);
        offset += sizeof(unsigned int);
    }
    else
    {
        glDisableClientState(GL_COLOR_ARRAY);
    }

    // Texture coordinates
    int numUVs = (mVertexFormat & PDDI_V_UVMASK) >> PDDI_V_UVSHIFT;
    for (int i = 0; i < numUVs; i++)
    {
        glClientActiveTexture(GL_TEXTURE0 + i);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, mStride, ptr + offset);
        offset += 2 * sizeof(float);
    }

    // Determine GL primitive type
    GLenum glPrimType = GL_TRIANGLES;
    switch (mPrimType)
    {
    case PDDI_PRIM_TRIANGLES:
        glPrimType = GL_TRIANGLES;
        break;
    case PDDI_PRIM_TRISTRIP:
        glPrimType = GL_TRIANGLE_STRIP;
        break;
    case PDDI_PRIM_TRIFAN:
        glPrimType = GL_TRIANGLE_FAN;
        break;
    case PDDI_PRIM_LINES:
        glPrimType = GL_LINES;
        break;
    case PDDI_PRIM_LINESTRIP:
        glPrimType = GL_LINE_STRIP;
        break;
    case PDDI_PRIM_POINTS:
        glPrimType = GL_POINTS;
        break;
    default:
        break;
    }

    // Draw
    if (mIndexBuffer && mIndexCount > 0)
    {
        glDrawElements(glPrimType, mIndexCount, GL_UNSIGNED_SHORT, mIndexBuffer);
    }
    else
    {
        glDrawArrays(glPrimType, 0, vertexCount);
    }

    // Clean up
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    for (int i = 0; i < numUVs; i++)
    {
        glClientActiveTexture(GL_TEXTURE0 + i);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
}

void ps3PrimBuffer::AllocateBuffers()
{
    // Calculate buffer sizes
    mVertexBufferSize = mVertexCount * mStride;
    mIndexBufferSize = mIndexCount * sizeof(unsigned short);

    // Allocate vertex buffer from RSX memory
    if (mVertexBufferSize > 0)
    {
        mVertexBuffer = mDevice->AllocateRSXMemory(mVertexBufferSize, 16);
        if (mVertexBuffer)
        {
            memset(mVertexBuffer, 0, mVertexBufferSize);
        }
    }

    // Allocate index buffer
    if (mIndexBufferSize > 0)
    {
        mIndexBuffer = mDevice->AllocateRSXMemory(mIndexBufferSize, 16);
        if (mIndexBuffer)
        {
            memset(mIndexBuffer, 0, mIndexBufferSize);
        }
    }
}

void ps3PrimBuffer::FreeBuffers()
{
    if (mVertexBuffer)
    {
        mDevice->FreeRSXMemory(mVertexBuffer);
        mVertexBuffer = NULL;
    }

    if (mIndexBuffer)
    {
        mDevice->FreeRSXMemory(mIndexBuffer);
        mIndexBuffer = NULL;
    }

    if (mMemImageData)
    {
        mDevice->FreeRSXMemory(mMemImageData);
        mMemImageData = NULL;
    }
}

unsigned ps3PrimBuffer::CalculateStride(unsigned vertexFormat)
{
    unsigned stride = 0;

    // Position (always 3 floats)
    if (vertexFormat & PDDI_V_POSITION)
        stride += 3 * sizeof(float);

    // Normal (3 floats)
    if (vertexFormat & PDDI_V_NORMAL)
        stride += 3 * sizeof(float);

    // Colour (1 uint32)
    if (vertexFormat & PDDI_V_COLOUR)
        stride += sizeof(unsigned int);

    // Specular (1 uint32)
    if (vertexFormat & PDDI_V_SPECULAR)
        stride += sizeof(unsigned int);

    // UV coordinates (2 floats each)
    int numUVs = (vertexFormat & PDDI_V_UVMASK) >> PDDI_V_UVSHIFT;
    stride += numUVs * 2 * sizeof(float);

    // Skin indices (4 bytes)
    if (vertexFormat & PDDI_V_INDICES)
        stride += 4;

    // Skin weights (3 floats)
    if (vertexFormat & PDDI_V_WEIGHTS)
        stride += 3 * sizeof(float);

    return stride;
}

void ps3PrimBuffer::UploadToRSX()
{
    // For PSGL, data is already in accessible memory
    // VBO upload would happen here for raw GCM implementation
}

#endif // RAD_PS3
