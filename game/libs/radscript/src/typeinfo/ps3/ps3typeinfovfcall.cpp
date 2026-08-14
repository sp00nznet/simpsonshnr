//=============================================================================
// Copyright (c) 2002 Radical Games Ltd.  All rights reserved.
// PS3 PPU version - based on GameCube PowerPC version
//=============================================================================

// PS3 PPU is PowerPC64, but in 32-bit compatibility mode uses similar ABI to GCN
// Using C implementation for PS3 to avoid assembly compatibility issues

extern "C" int InvokeVf( void * pClass, unsigned int offset, unsigned int * pInts, float * pFloats )
{
    // Get the vtable pointer from the class
    void ** vtable = *((void***)pClass);

    // Get the function pointer at the given offset
    // Offset is in terms of vtable entries, not bytes
    // vtable[0] is usually RTTI info, actual methods start at [1]
    typedef int (*VirtualFunc)( void * pThis,
        unsigned int i0, unsigned int i1, unsigned int i2, unsigned int i3,
        unsigned int i4, unsigned int i5, unsigned int i6,
        float f0, float f1, float f2, float f3,
        float f4, float f5, float f6, float f7 );

    VirtualFunc func = (VirtualFunc)vtable[offset + 1];

    // Call the function with all parameters
    return func( pClass,
        pInts[0], pInts[1], pInts[2], pInts[3], pInts[4], pInts[5], pInts[6],
        pFloats[0], pFloats[1], pFloats[2], pFloats[3],
        pFloats[4], pFloats[5], pFloats[6], pFloats[7] );
}
