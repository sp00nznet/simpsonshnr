//=============================================================================
// Copyright (c) 2002 Radical Games Ltd.  All rights reserved.
//=============================================================================

#ifndef _PDDIPS3_HPP
#define _PDDIPS3_HPP

#include <pddi/pddienum.hpp>

class pddiDevice;
class ps3Context;
class ps3Shader;

// prototype for the initialization function through implicit linking
extern "C" int pddiCreate(int versionMajor, int versionMinor, pddiDevice** dev);

// prototype for the initialization function through LoadLibrary
typedef int (*PDDICREATEPROC)(int, int, pddiDevice**);

// machine dependent types (PS3 Cell/PPU)
typedef long long          PDDI_S64;
typedef unsigned long long PDDI_U64;
typedef int                PDDI_S32;
typedef unsigned           PDDI_U32;
typedef char               PDDI_S8;
typedef unsigned char      PDDI_U8;
typedef short              PDDI_S16;
typedef unsigned short     PDDI_U16;

#define PDDI_INTERFACE virtual
#define PDDI_PURE  =0
#define PDDI_VIRTUAL

// display initialization structure
class pddiDisplayInit
{
public:
    int      xsize;         // x resolution (720 or 1080)
    int      ysize;         // y resolution (480, 720, or 1080)
    int      bpp;           // bits per pixel (32 for ARGB8888)
    unsigned bufferMask;    // auxiliary buffer mask
    bool     pal;           // true for PAL regions
    bool     progressive;   // true for progressive scan
    bool     lockToVsync;   // true if framebuffer swap waits for vsync
    bool     hdtv;          // true for HDTV output
    bool     dtv480;        // 480p mode (640x480 or 720x480)
    bool     dtv720;        // 720p mode (1280x720)
    bool     dtv1080;       // 1080p mode (1920x1080)
};

// PS3 Immediate Mode Primitive Stream
#include <pddi/ps3/ps3primstream.hpp>
#define pddiPrimStream ps3PrimStream

#endif /* _PDDIPS3_HPP */
