//=============================================================================
// Copyright (c) 2002 Radical Games Ltd.  All rights reserved.
//=============================================================================
// PS3 Platform Implementation for Pure3D
//=============================================================================

#include <p3d/platform/ps3/platform.hpp>
#include <p3d/platform/ps3/plat_filemap.hpp>
#include <p3d/context.hpp>
#include <p3d/utility.hpp>
#include <constants/version.hpp>

#include <stdlib.h>
#include <string.h>

//-------------------------------------------------------------------
static tPlatform globalPlatform;

tPlatform::tPlatform() : context(NULL)
{
}

tPlatform::~tPlatform()
{
}

tPlatform* tPlatform::Create()
{
    p3d::platform = &globalPlatform;
    return &globalPlatform;
}

tPlatform* tPlatform::GetPlatform(void)
{
    return &globalPlatform;
}

tContext* tPlatform::CreateContext(tContextInitData* d)
{
    pddiDevice* device = NULL;
    pddiDisplay* display = NULL;

    p3d::UsePermanentMem(true);

    int success = pddiCreate(PDDI_VERSION_MAJOR, PDDI_VERSION_MINOR, &device);
    P3DASSERT(success == PDDI_OK);

    tDebug::CapturePDDIMessages(device);

    pddiDisplayMode mode = d->pal ? PDDI_DISPLAY_FULLSCREEN_PAL : PDDI_DISPLAY_FULLSCREEN;
    display = device->NewDisplay(0);

    display->InitDisplay(d);

    pddiRenderContext* pddiContext = device->NewRenderContext(display);
    P3DASSERT(pddiContext != NULL);

    context = new tContext(device, display, pddiContext);

    context->Setup();

    p3d::context = context;
    p3d::inventory = context->GetInventory();
    p3d::stack = context->GetMatrixStack();
    p3d::loadManager = context->GetLoadManager();
    p3d::pddi = context->GetContext();
    p3d::device = context->GetDevice();
    p3d::display = context->GetDisplay();

    p3d::UsePermanentMem(false);
    return context;
}

tFile* tPlatform::OpenFile(const char* filename)
{
    tPS3FileMap* file = new tPS3FileMap(filename);
    if (!file->IsOpen())
    {
        file->Release();
        return NULL;
    }
    return file;
}
