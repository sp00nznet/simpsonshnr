/*===========================================================================
    ps3primstream.hpp

    PS3 PDDI Immediate Mode Primitive Stream

    Copyright (c)1996-2002 Radical Entertainment Ltd
    All rights reserved.
===========================================================================*/

#ifndef _PS3PRIMSTREAM_HPP
#define _PS3PRIMSTREAM_HPP

// Include pdditype.hpp but prevent its PS3-specific include from causing a cycle
// We set a guard flag before including so pddips3.hpp knows not to include us back
#define _PS3PRIMSTREAM_INCLUDED_
#include <pddi/pdditype.hpp>
#include <pddi/pddienum.hpp>
#include <radmath/radmath.hpp>

class ps3Device;
class ps3Context;
class ps3Shader;

// Maximum vertices in immediate mode batch
#define PS3_PRIMSTREAM_MAX_VERTICES 4096

//-------------------------------------------------------------------
// ps3PrimStream - Immediate mode primitive rendering for PS3
// Note: This class IS the pddiPrimStream for PS3 (via #define in pddips3.hpp)
// so it doesn't inherit from anything - it provides the interface directly.
//-------------------------------------------------------------------
class ps3PrimStream
{
public:
    ps3PrimStream(ps3Device* device);
    virtual ~ps3PrimStream();

    // pddiPrimStream interface
    void Coord(float x, float y, float z);
    void Position(float x, float y, float z);
    void Normal(float x, float y, float z);
    // Nothing in the PS3 shader set consumes tangent space -- same as dx8.
    inline void Binormal(float x, float y, float z) { }
    inline void Tangent(float x, float y, float z) { }
    void Colour(pddiColour colour, int channel = 0);
    void UV(float u, float v, int channel = 0);
    void TexCoord1(float s, int channel = 0);
    void TexCoord2(float s, float t, int channel = 0);
    void TexCoord3(float s, float t, float u, int channel = 0);
    void TexCoord4(float s, float t, float u, float v, int channel = 0);
    void Specular(pddiColour colour);
    void Vertex(rmt::Vector* v, pddiColour c);
    void Vertex(rmt::Vector* v, rmt::Vector* n);
    void Vertex(rmt::Vector* v, rmt::Vector2* uv);
    void Vertex(rmt::Vector* v, pddiColour c, rmt::Vector2* uv);
    void Vertex(rmt::Vector* v, rmt::Vector* n, rmt::Vector2* uv);

    // Start/end primitive batching
    void Begin(ps3Context* context, ps3Shader* shader, pddiPrimType primType, unsigned vertexFormat, int maxVerts);
    void End();

    // Flush current batch
    void Flush();

private:
    ps3Device* mDevice;
    ps3Context* mContext;
    ps3Shader* mShader;

    pddiPrimType mPrimType;
    unsigned mVertexFormat;
    unsigned mMaxVerts;

    // Vertex buffer for immediate mode
    struct ImmediateVertex
    {
        float position[3];
        float normal[3];
        unsigned char colour[4];    // RGBA order for OpenGL
        unsigned char specular[4];  // RGBA order for OpenGL
        float texCoord[8][2];  // Up to 8 texture coordinates
    };

    ImmediateVertex* mVertices;
    unsigned mVertexCount;
    unsigned mCurrentVertex;

    // Current vertex state
    float mCurrentPosition[3];
    float mCurrentNormal[3];
    pddiColour mCurrentColour;
    pddiColour mCurrentSpecular;
    float mCurrentTexCoord[8][2];

    bool mActive;

    // Helper methods
    void AllocateBuffers();
    void FreeBuffers();
    void RenderBatch();
    void ResetCurrentVertex();
};

#endif /* _PS3PRIMSTREAM_HPP */
