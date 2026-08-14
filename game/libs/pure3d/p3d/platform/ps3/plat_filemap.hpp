//=============================================================================
// Copyright (c) 2002 Radical Games Ltd.  All rights reserved.
//=============================================================================
// PS3 File Map Header
//=============================================================================

#ifndef _PLAT_FILEMAP_PS3_HPP
#define _PLAT_FILEMAP_PS3_HPP

#include <p3d/file.hpp>

class tPS3FileMap : public tFileMem
{
public:
    tPS3FileMap(const char* filename);

    bool IsOpen(void) { return GetMemory() != NULL; }

protected:
    virtual ~tPS3FileMap();

    void Open(const char* filename);
    void Close(void);
};

#endif /* _PLAT_FILEMAP_PS3_HPP */
