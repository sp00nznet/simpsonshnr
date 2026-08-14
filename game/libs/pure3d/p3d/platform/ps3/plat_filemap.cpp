//=============================================================================
// Copyright (c) 2002 Radical Games Ltd.  All rights reserved.
//=============================================================================
// PS3 File Map Implementation
//=============================================================================

#include <p3d/platform/ps3/plat_filemap.hpp>
#include <p3d/utility.hpp>
#include <radload/radload.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

tPS3FileMap::tPS3FileMap(const char* filename)
{
    Open(filename);
    SetFilename(filename);
}

tPS3FileMap::~tPS3FileMap()
{
    Close();
}

void tPS3FileMap::Open(const char* filename)
{
    unsigned char* memory = NULL;
    unsigned int length = 0;

    FILE* fp = fopen(filename, "rb");
    if (!fp)
    {
        memory = 0;
        return;
    }

    // Get file size
    fseek(fp, 0, SEEK_END);
    length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Allocate memory and read file
    // Align to 16-byte boundary like PS2 does
    memory = new unsigned char[(length + 15) & ~0xf];
    size_t bytesRead = fread(memory, 1, (length + 15) & ~0xf, fp);
    if (bytesRead != length)
    {
        // Warning: read mismatch
    }

    fclose(fp);

    // Create the data stream (true = delete memory when done)
    dataStream = new radLoadDataStream(memory, length, true);
}

void tPS3FileMap::Close()
{
    // dataStream will handle cleanup
}
