//=============================================================================
// Copyright (c) 2002 Radical Games Ltd.  All rights reserved.
//=============================================================================

#ifndef _PLATFORM_PS3_HPP
#define _PLATFORM_PS3_HPP

#include <p3d/buildconfig.hpp>
#include <p3d/platform/ps3/plat_types.hpp>
#include <pddi/pddi.hpp>

class tContext;
class tFile;

class tContextInitData : public pddiDisplayInit
{
public:
    tContextInitData()
    {
        xsize = 1280;
        ysize = 720;
        bufferMask = PDDI_BUFFER_COLOUR | PDDI_BUFFER_DEPTH;
        pal = false;
        progressive = true;
        lockToVsync = true;
        hdtv = true;
    }
};


class tPlatform
{
public:
    tPlatform();
    ~tPlatform();

    // platform creation/destruction
    static tPlatform* Create();
    static void Destroy(tPlatform*)  { /* */ }
    static tPlatform* GetPlatform(void);

    // context creation/destruction
    tContext* CreateContext(tContextInitData*);
    void DestroyContext(tContext*) { /* */ }

    // active context control
    void SetActiveContext(tContext* c) { context = c; }
    tContext* GetActiveContext(void)   { return context; }

private:
    // files
    tFile* OpenFile(const char* filename);

private:
    tContext* context;
};

#endif /* _PLATFORM_PS3_HPP */
