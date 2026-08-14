/*===========================================================================
    ps3primstream.cpp

    PS3 PDDI Immediate Mode Primitive Stream Implementation

    Copyright (c)1996-2002 Radical Entertainment Ltd
    All rights reserved.
===========================================================================*/

#ifdef RAD_PS3

#include <pddi/ps3/ps3primstream.hpp>
#include <pddi/ps3/ps3device.hpp>
#include <pddi/ps3/ps3context.hpp>
#include <pddi/ps3/ps3shader.hpp>

#include <string.h>

// PS3 SDK includes
#include <PSGL/psgl.h>

//-------------------------------------------------------------------
// ps3PrimStream implementation
//-------------------------------------------------------------------
ps3PrimStream::ps3PrimStream(ps3Device* device) :
    mDevice(device),
    mContext(NULL),
    mShader(NULL),
    mPrimType(PDDI_PRIM_TRIANGLES),
    mVertexFormat(0),
    mMaxVerts(0),
    mVertices(NULL),
    mVertexCount(0),
    mCurrentVertex(0),
    mActive(false)
{
    ResetCurrentVertex();
    AllocateBuffers();
}

ps3PrimStream::~ps3PrimStream()
{
    FreeBuffers();
}

void ps3PrimStream::Coord(float x, float y, float z)
{
    mCurrentPosition[0] = x;
    mCurrentPosition[1] = y;
    mCurrentPosition[2] = z;

    // Commit the vertex (Coord acts like glVertex3f - it finalizes and stores)
    if (mVertexCount < mMaxVerts && mVertices)
    {
        ImmediateVertex& vert = mVertices[mVertexCount];
        memcpy(vert.position, mCurrentPosition, sizeof(mCurrentPosition));
        memcpy(vert.normal, mCurrentNormal, sizeof(mCurrentNormal));
        vert.colour[0] = mCurrentColour.Red();
        vert.colour[1] = mCurrentColour.Green();
        vert.colour[2] = mCurrentColour.Blue();
        vert.colour[3] = mCurrentColour.Alpha();
        vert.specular[0] = mCurrentSpecular.Red();
        vert.specular[1] = mCurrentSpecular.Green();
        vert.specular[2] = mCurrentSpecular.Blue();
        vert.specular[3] = mCurrentSpecular.Alpha();
        memcpy(vert.texCoord, mCurrentTexCoord, sizeof(mCurrentTexCoord));
        mVertexCount++;
    }

    ResetCurrentVertex();
}

void ps3PrimStream::Position(float x, float y, float z)
{
    mCurrentPosition[0] = x;
    mCurrentPosition[1] = y;
    mCurrentPosition[2] = z;
}

void ps3PrimStream::Normal(float x, float y, float z)
{
    mCurrentNormal[0] = x;
    mCurrentNormal[1] = y;
    mCurrentNormal[2] = z;
}

void ps3PrimStream::Colour(pddiColour colour, int channel)
{
    mCurrentColour = colour;
}

void ps3PrimStream::UV(float u, float v, int channel)
{
    TexCoord2(u, v, channel);
}

void ps3PrimStream::TexCoord1(float s, int channel)
{
    if (channel >= 0 && channel < 8)
    {
        mCurrentTexCoord[channel][0] = s;
        mCurrentTexCoord[channel][1] = 0.0f;
    }
}

void ps3PrimStream::TexCoord2(float s, float t, int channel)
{
    if (channel >= 0 && channel < 8)
    {
        mCurrentTexCoord[channel][0] = s;
        mCurrentTexCoord[channel][1] = t;
    }
}

void ps3PrimStream::TexCoord3(float s, float t, float u, int channel)
{
    if (channel >= 0 && channel < 8)
    {
        mCurrentTexCoord[channel][0] = s;
        mCurrentTexCoord[channel][1] = t;
    }
}

void ps3PrimStream::TexCoord4(float s, float t, float u, float v, int channel)
{
    if (channel >= 0 && channel < 8)
    {
        mCurrentTexCoord[channel][0] = s;
        mCurrentTexCoord[channel][1] = t;
    }
}

void ps3PrimStream::Specular(pddiColour colour)
{
    mCurrentSpecular = colour;
}

void ps3PrimStream::Vertex(rmt::Vector* v, pddiColour c)
{
    Position(v->x, v->y, v->z);
    Colour(c);

    // Store vertex and move to next
    if (mVertexCount < mMaxVerts && mVertices)
    {
        ImmediateVertex& vert = mVertices[mVertexCount];
        memcpy(vert.position, mCurrentPosition, sizeof(mCurrentPosition));
        memcpy(vert.normal, mCurrentNormal, sizeof(mCurrentNormal));
        // Convert BGRA (pddiColour) to RGBA (OpenGL)
        vert.colour[0] = mCurrentColour.Red();
        vert.colour[1] = mCurrentColour.Green();
        vert.colour[2] = mCurrentColour.Blue();
        vert.colour[3] = mCurrentColour.Alpha();
        vert.specular[0] = mCurrentSpecular.Red();
        vert.specular[1] = mCurrentSpecular.Green();
        vert.specular[2] = mCurrentSpecular.Blue();
        vert.specular[3] = mCurrentSpecular.Alpha();
        memcpy(vert.texCoord, mCurrentTexCoord, sizeof(mCurrentTexCoord));
        mVertexCount++;
    }

    ResetCurrentVertex();
}

void ps3PrimStream::Vertex(rmt::Vector* v, rmt::Vector* n)
{
    Position(v->x, v->y, v->z);
    Normal(n->x, n->y, n->z);

    if (mVertexCount < mMaxVerts && mVertices)
    {
        ImmediateVertex& vert = mVertices[mVertexCount];
        memcpy(vert.position, mCurrentPosition, sizeof(mCurrentPosition));
        memcpy(vert.normal, mCurrentNormal, sizeof(mCurrentNormal));
        // Convert BGRA (pddiColour) to RGBA (OpenGL)
        vert.colour[0] = mCurrentColour.Red();
        vert.colour[1] = mCurrentColour.Green();
        vert.colour[2] = mCurrentColour.Blue();
        vert.colour[3] = mCurrentColour.Alpha();
        vert.specular[0] = mCurrentSpecular.Red();
        vert.specular[1] = mCurrentSpecular.Green();
        vert.specular[2] = mCurrentSpecular.Blue();
        vert.specular[3] = mCurrentSpecular.Alpha();
        memcpy(vert.texCoord, mCurrentTexCoord, sizeof(mCurrentTexCoord));
        mVertexCount++;
    }

    ResetCurrentVertex();
}

void ps3PrimStream::Vertex(rmt::Vector* v, rmt::Vector2* uv)
{
    Position(v->x, v->y, v->z);
    TexCoord2(uv->u, uv->v);

    if (mVertexCount < mMaxVerts && mVertices)
    {
        ImmediateVertex& vert = mVertices[mVertexCount];
        memcpy(vert.position, mCurrentPosition, sizeof(mCurrentPosition));
        memcpy(vert.normal, mCurrentNormal, sizeof(mCurrentNormal));
        // Convert BGRA (pddiColour) to RGBA (OpenGL)
        vert.colour[0] = mCurrentColour.Red();
        vert.colour[1] = mCurrentColour.Green();
        vert.colour[2] = mCurrentColour.Blue();
        vert.colour[3] = mCurrentColour.Alpha();
        vert.specular[0] = mCurrentSpecular.Red();
        vert.specular[1] = mCurrentSpecular.Green();
        vert.specular[2] = mCurrentSpecular.Blue();
        vert.specular[3] = mCurrentSpecular.Alpha();
        memcpy(vert.texCoord, mCurrentTexCoord, sizeof(mCurrentTexCoord));
        mVertexCount++;
    }

    ResetCurrentVertex();
}

void ps3PrimStream::Vertex(rmt::Vector* v, pddiColour c, rmt::Vector2* uv)
{
    Position(v->x, v->y, v->z);
    Colour(c);
    TexCoord2(uv->u, uv->v);

    if (mVertexCount < mMaxVerts && mVertices)
    {
        ImmediateVertex& vert = mVertices[mVertexCount];
        memcpy(vert.position, mCurrentPosition, sizeof(mCurrentPosition));
        memcpy(vert.normal, mCurrentNormal, sizeof(mCurrentNormal));
        // Convert BGRA (pddiColour) to RGBA (OpenGL)
        vert.colour[0] = mCurrentColour.Red();
        vert.colour[1] = mCurrentColour.Green();
        vert.colour[2] = mCurrentColour.Blue();
        vert.colour[3] = mCurrentColour.Alpha();
        vert.specular[0] = mCurrentSpecular.Red();
        vert.specular[1] = mCurrentSpecular.Green();
        vert.specular[2] = mCurrentSpecular.Blue();
        vert.specular[3] = mCurrentSpecular.Alpha();
        memcpy(vert.texCoord, mCurrentTexCoord, sizeof(mCurrentTexCoord));
        mVertexCount++;
    }

    ResetCurrentVertex();
}

void ps3PrimStream::Vertex(rmt::Vector* v, rmt::Vector* n, rmt::Vector2* uv)
{
    Position(v->x, v->y, v->z);
    Normal(n->x, n->y, n->z);
    TexCoord2(uv->u, uv->v);

    if (mVertexCount < mMaxVerts && mVertices)
    {
        ImmediateVertex& vert = mVertices[mVertexCount];
        memcpy(vert.position, mCurrentPosition, sizeof(mCurrentPosition));
        memcpy(vert.normal, mCurrentNormal, sizeof(mCurrentNormal));
        // Convert BGRA (pddiColour) to RGBA (OpenGL)
        vert.colour[0] = mCurrentColour.Red();
        vert.colour[1] = mCurrentColour.Green();
        vert.colour[2] = mCurrentColour.Blue();
        vert.colour[3] = mCurrentColour.Alpha();
        vert.specular[0] = mCurrentSpecular.Red();
        vert.specular[1] = mCurrentSpecular.Green();
        vert.specular[2] = mCurrentSpecular.Blue();
        vert.specular[3] = mCurrentSpecular.Alpha();
        memcpy(vert.texCoord, mCurrentTexCoord, sizeof(mCurrentTexCoord));
        mVertexCount++;
    }

    ResetCurrentVertex();
}

void ps3PrimStream::Begin(ps3Context* context, ps3Shader* shader, pddiPrimType primType, unsigned vertexFormat, int maxVerts)
{
    mContext = context;
    mShader = shader;
    mPrimType = primType;
    mVertexFormat = vertexFormat;
    mMaxVerts = (maxVerts < PS3_PRIMSTREAM_MAX_VERTICES) ? maxVerts : PS3_PRIMSTREAM_MAX_VERTICES;
    mVertexCount = 0;
    mActive = true;

    ResetCurrentVertex();
}

void ps3PrimStream::End()
{
    if (!mActive)
        return;

    // Flush any remaining vertices
    if (mVertexCount > 0)
    {
        Flush();
    }

    mActive = false;
    mContext = NULL;
    mShader = NULL;
}

void ps3PrimStream::Flush()
{
    if (mVertexCount == 0 || !mVertices)
        return;

    RenderBatch();
    mVertexCount = 0;
}

void ps3PrimStream::AllocateBuffers()
{
    mVertices = new ImmediateVertex[PS3_PRIMSTREAM_MAX_VERTICES];
    memset(mVertices, 0, sizeof(ImmediateVertex) * PS3_PRIMSTREAM_MAX_VERTICES);
}

void ps3PrimStream::FreeBuffers()
{
    if (mVertices)
    {
        delete[] mVertices;
        mVertices = NULL;
    }
}

void ps3PrimStream::RenderBatch()
{
    if (mVertexCount == 0 || !mVertices)
        return;

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

    // Use vertex arrays for PSGL (no immediate mode support)
    // Set up vertex pointer - position is at offset 0 in ImmediateVertex
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof(ImmediateVertex), &mVertices[0].position[0]);

    // Set up normal pointer
    if (mVertexFormat & PDDI_V_NORMAL)
    {
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, sizeof(ImmediateVertex), &mVertices[0].normal[0]);
    }
    else
    {
        glDisableClientState(GL_NORMAL_ARRAY);
    }

    // Set up color pointer - colors stored as pddiColour (4 bytes BGRA)
    if (mVertexFormat & PDDI_V_COLOUR)
    {
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImmediateVertex), &mVertices[0].colour[0]);
    }
    else
    {
        glDisableClientState(GL_COLOR_ARRAY);
    }

    // Set up texture coordinate pointers
    int numUVs = (mVertexFormat & PDDI_V_UVMASK) >> PDDI_V_UVSHIFT;
    for (int t = 0; t < numUVs && t < 8; t++)
    {
        glClientActiveTexture(GL_TEXTURE0 + t);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(ImmediateVertex), &mVertices[0].texCoord[t][0]);
    }
    // Disable unused texture coord arrays
    for (int t = numUVs; t < 8; t++)
    {
        glClientActiveTexture(GL_TEXTURE0 + t);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    // Draw the primitives
    glDrawArrays(glPrimType, 0, mVertexCount);

    // Clean up
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    for (int t = 0; t < 8; t++)
    {
        glClientActiveTexture(GL_TEXTURE0 + t);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    glClientActiveTexture(GL_TEXTURE0);
}

void ps3PrimStream::ResetCurrentVertex()
{
    mCurrentPosition[0] = 0.0f;
    mCurrentPosition[1] = 0.0f;
    mCurrentPosition[2] = 0.0f;
    mCurrentNormal[0] = 0.0f;
    mCurrentNormal[1] = 0.0f;
    mCurrentNormal[2] = 1.0f;
    mCurrentColour = pddiColour(255, 255, 255, 255);
    mCurrentSpecular = pddiColour(0, 0, 0, 0);
    memset(mCurrentTexCoord, 0, sizeof(mCurrentTexCoord));
}

#endif // RAD_PS3
