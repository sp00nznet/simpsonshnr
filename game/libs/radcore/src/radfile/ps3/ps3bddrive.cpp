//=============================================================================
// Copyright (c) 2002 Radical Games Ltd.  All rights reserved.
//=============================================================================


//=============================================================================
//
// File:        ps3bddrive.cpp
//
// Subsystem:   Radical File System
//
// Description: This file contains the implementation of the PS3 Blu-ray drive.
//              This is a FILE-BASED implementation for RPCS3 compatibility.
//              Files are accessed directly via cellFs* APIs.
//
// Revisions:   Rewritten for file-based access (RPCS3 compatible)
//
//=============================================================================

//=============================================================================
// Include Files
//=============================================================================

#ifdef RAD_PS3

#include <cell/cell_fs.h>
#include <sys/paths.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <radplatform.hpp>
#include <radstring.hpp>
#include <radtime.hpp>
#include "ps3bddrive.hpp"
#include <sys/tty.h>

static void _BDPrint(const char* a, const char* b)
{
    char buf[600];
    int i = 0;
    while (a[i] && i < 100) { buf[i] = a[i]; ++i; }
    int j = 0;
    while (b && b[j] && i < 590) { buf[i++] = b[j++]; }
    buf[i++] = '\n';
    unsigned int written;
    sys_tty_write(0, buf, i, &written);
}
static void _BDPrintI(const char* a, int n)
{
    char buf[80]; int i = 0;
    while (a[i] && i < 60) { buf[i] = a[i]; i++; }
    if (n < 0) { buf[i++] = '-'; n = -n; }
    char num[12]; int k = 0;
    if (n == 0) { num[k++] = '0'; }
    else { while (n) { num[k++] = '0' + (n % 10); n /= 10; } }
    while (k > 0) { buf[i++] = num[--k]; }
    buf[i++] = '\n'; buf[i] = 0;
    unsigned int written; sys_tty_write(0, buf, i, &written);
}

//=============================================================================
// Local Definitions
//=============================================================================

//
// Base path for game data files
// Using /app_home/ for RPCS3 - files are in USRDIR
//
static const char* PS3_BASE_PATH = "/app_home/";

//
// Maximum number of simultaneously open files
//
#define MAX_OPEN_FILES 64

//
// File handle structure - maps radFileHandle to actual file info
//
struct OpenFileInfo
{
    int fd;                     // Cell FS file descriptor (-1 if unused)
    unsigned int size;          // File size
    unsigned int position;      // Current read position
    char filename[256];         // Full file path for debugging
};

static OpenFileInfo s_OpenFiles[MAX_OPEN_FILES];
static bool s_Initialized = false;

//=============================================================================
// Helper Functions
//=============================================================================

//
// Convert game path (backslashes) to PS3 path (forward slashes)
//
static void ConvertPath(const char* gamePath, char* ps3Path, unsigned int maxLen)
{
    // Start with base path
    strncpy(ps3Path, PS3_BASE_PATH, maxLen - 1);
    ps3Path[maxLen - 1] = '\0';

    unsigned int baseLen = strlen(ps3Path);
    unsigned int srcIdx = 0;
    unsigned int dstIdx = baseLen;

    // Skip leading backslash if present
    if (gamePath[0] == '\\' || gamePath[0] == '/')
    {
        srcIdx = 1;
    }

    // Copy and convert path
    while (gamePath[srcIdx] != '\0' && dstIdx < maxLen - 1)
    {
        if (gamePath[srcIdx] == '\\')
        {
            ps3Path[dstIdx] = '/';
        }
        else
        {
            ps3Path[dstIdx] = gamePath[srcIdx];
        }
        srcIdx++;
        dstIdx++;
    }
    ps3Path[dstIdx] = '\0';
}

//
// Find a free file handle slot
//
static int FindFreeSlot()
{
    for (int i = 0; i < MAX_OPEN_FILES; i++)
    {
        if (s_OpenFiles[i].fd < 0)
        {
            return i;
        }
    }
    return -1;
}

//=============================================================================
// Public Functions
//=============================================================================

//=============================================================================
// Function:    radPs3BdDriveFactory
//=============================================================================

void radPs3BdDriveFactory
(
    radDrive**         ppDrive,
    const char*        pDriveName,
    radMemoryAllocator alloc
)
{
    *ppDrive = new( alloc ) radPs3BdDrive( alloc );
    rAssert( *ppDrive != NULL );
}

//=============================================================================
// Public Member Functions
//=============================================================================

radPs3BdDrive::radPs3BdDrive( radMemoryAllocator alloc )
    :
    radDrive( ),
    m_OpenFiles( 0 ),
    m_pMutex( NULL )
{
    // Initialize open file slots
    if (!s_Initialized)
    {
        for (int i = 0; i < MAX_OPEN_FILES; i++)
        {
            s_OpenFiles[i].fd = -1;
            s_OpenFiles[i].size = 0;
            s_OpenFiles[i].position = 0;
            s_OpenFiles[i].filename[0] = '\0';
        }
        s_Initialized = true;
    }

    // Create a mutex for lock/unlock
    radThreadCreateMutex( &m_pMutex, alloc );
    rAssert( m_pMutex != NULL );

    // Create the drive thread
    m_pDriveThread = new( alloc ) radDriveThread( m_pMutex, alloc, 8 * 1024 );
    rAssert( m_pDriveThread != NULL );

    // Media info - always present for file-based access
    m_MediaInfo.m_FreeFiles = 0;
    m_MediaInfo.m_FreeSpace = 0;
    m_MediaInfo.m_SectorSize = PS3_BD_SECTOR_SIZE;
    m_MediaInfo.m_MediaState = IRadDrive::MediaInfo::MediaPresent;
    strcpy(m_MediaInfo.m_VolumeName, "SIMPSONS");

    // Set up aligned buffer for buffered reads
    rAssert( radMemorySpace_OptimalAlignment <= PS3_BD_DRIVE_ALIGNMENT );
    rAssert( radFileOptimalMemoryAlignment <= PS3_BD_DRIVE_ALIGNMENT );
    m_SectorBuffer = (unsigned char*) ::radMemoryRoundUp( (uintptr_t) m_SectorBufferSpace, PS3_BD_DRIVE_ALIGNMENT );

}

radPs3BdDrive::~radPs3BdDrive( void )
{
    // Close any open files
    for (int i = 0; i < MAX_OPEN_FILES; i++)
    {
        if (s_OpenFiles[i].fd >= 0)
        {
            cellFsClose(s_OpenFiles[i].fd);
            s_OpenFiles[i].fd = -1;
        }
    }

    m_pMutex->Release( );
    m_pDriveThread->Release( );
}

void radPs3BdDrive::Lock( void )
{
    m_pMutex->Lock( );
}

void radPs3BdDrive::Unlock( void )
{
    m_pMutex->Unlock( );
}

unsigned int radPs3BdDrive::GetCapabilities( void )
{
    return ( radDriveRemovable | radDriveEnumerable | radDriveFile );
}

const char* radPs3BdDrive::GetDriveName( void )
{
    return s_PS3BDDriveName;
}

unsigned int radPs3BdDrive::GetReadBufferSectors( void )
{
    return PS3_BD_DRIVE_TRANSFER_BUFFER_SECTORS;
}

radDrive::CompletionStatus radPs3BdDrive::Initialize( void )
{
    // Check if base path exists
    CellFsStat stat;
    if( cellFsStat( PS3_BASE_PATH, &stat ) != CELL_FS_SUCCEEDED )
    {

        m_MediaInfo.m_MediaState = IRadDrive::MediaInfo::MediaNotPresent;
        m_LastError = NoMedia;
        return Error;
    }

    m_MediaInfo.m_MediaState = IRadDrive::MediaInfo::MediaPresent;
    m_LastError = Success;
    return Complete;
}

radDrive::CompletionStatus radPs3BdDrive::OpenFile
(
    const char*        fileName,
    radFileOpenFlags   flags,
    bool               writeAccess,
    radFileHandle*     pHandle,
    unsigned int*      pSize
)
{
    rAssert( writeAccess == false );  // Read-only for BD drive
    rAssert( flags == OpenExisting );

    _BDPrint("[BD] OpenFile in: ", fileName);

    // Convert the path
    char ps3Path[512];
    ConvertPath(fileName, ps3Path, sizeof(ps3Path));

    _BDPrint("[BD] OpenFile path: ", ps3Path);

    // Find a free slot
    int slot = FindFreeSlot();
    if (slot < 0)
    {

        m_LastError = NoFreeSpace;
        return Error;
    }

    // Open the file
    int fd;
    CellFsErrno err = cellFsOpen(ps3Path, CELL_FS_O_RDONLY, &fd, NULL, 0);
    _BDPrintI("[BD] cellFsOpen err=", (int)err);
    if (err != CELL_FS_SUCCEEDED)
    {

        m_LastError = FileNotFound;
        return Error;
    }
    _BDPrintI("[BD] cellFsOpen ok fd=", fd);

    // Get file size
    CellFsStat stat;
    err = cellFsFstat(fd, &stat);
    if (err != CELL_FS_SUCCEEDED)
    {

        cellFsClose(fd);
        m_LastError = HardwareFailure;
        return Error;
    }

    // Store file info
    s_OpenFiles[slot].fd = fd;
    s_OpenFiles[slot].size = (unsigned int)stat.st_size;
    s_OpenFiles[slot].position = 0;
    strncpy(s_OpenFiles[slot].filename, ps3Path, sizeof(s_OpenFiles[slot].filename) - 1);
    s_OpenFiles[slot].filename[sizeof(s_OpenFiles[slot].filename) - 1] = '\0';

    // Return handle and size
    *pHandle = (radFileHandle)slot;
    *pSize = s_OpenFiles[slot].size;

    m_OpenFiles++;
    m_LastError = Success;

    _BDPrintI("[BD] OpenFile DONE size=", (int)*pSize);
    return Complete;
}

radDrive::CompletionStatus radPs3BdDrive::CloseFile( radFileHandle handle, const char* fileName )
{
    int slot = (int)handle;

    if (slot < 0 || slot >= MAX_OPEN_FILES || s_OpenFiles[slot].fd < 0)
    {

        return Complete;
    }

    cellFsClose(s_OpenFiles[slot].fd);
    s_OpenFiles[slot].fd = -1;
    s_OpenFiles[slot].size = 0;
    s_OpenFiles[slot].position = 0;
    s_OpenFiles[slot].filename[0] = '\0';

    m_OpenFiles--;
    return Complete;
}

radDrive::CompletionStatus radPs3BdDrive::ReadAligned
(
    radFileHandle handle,
    const char* fileName,
    unsigned int sector,
    unsigned int numSectors,
    void* pData,
    radMemorySpace pDataSpace
)
{
    rAssertMsg( pDataSpace == radMemorySpace_Local,
                "radFileSystem: radPs3BdDrive: reads only supported for main memory." );

    int slot = (int)handle;

    if (slot < 0 || slot >= MAX_OPEN_FILES || s_OpenFiles[slot].fd < 0)
    {

        m_LastError = HardwareFailure;
        return Error;
    }

    // Calculate byte offset and size
    uint64_t offset = (uint64_t)sector * PS3_BD_SECTOR_SIZE;
    uint64_t bytesToRead = (uint64_t)numSectors * PS3_BD_SECTOR_SIZE;

    // Clamp to file size
    if (offset >= s_OpenFiles[slot].size)
    {
        // Reading past end of file - this is OK, just return zeros
        memset(pData, 0, bytesToRead);
        m_LastError = Success;
        return Complete;
    }

    if (offset + bytesToRead > s_OpenFiles[slot].size)
    {
        bytesToRead = s_OpenFiles[slot].size - offset;
    }

    // Seek to position
    uint64_t pos;
    CellFsErrno err = cellFsLseek(s_OpenFiles[slot].fd, offset, CELL_FS_SEEK_SET, &pos);
    if (err != CELL_FS_SUCCEEDED)
    {

        m_LastError = HardwareFailure;
        return Error;
    }

    // Read data
    uint64_t bytesRead;
    err = cellFsRead(s_OpenFiles[slot].fd, pData, bytesToRead, &bytesRead);
    if (err != CELL_FS_SUCCEEDED)
    {

        m_LastError = HardwareFailure;
        return Error;
    }

    // Zero out any remaining bytes (if we read past EOF)
    if (bytesRead < (uint64_t)numSectors * PS3_BD_SECTOR_SIZE)
    {
        memset((char*)pData + bytesRead, 0, numSectors * PS3_BD_SECTOR_SIZE - bytesRead);
    }

    m_LastError = Success;
    return Complete;
}

radDrive::CompletionStatus radPs3BdDrive::ReadBuffered
(
    radFileHandle handle,
    const char* fileName,
    unsigned int sector,
    unsigned int numSectors,
    unsigned int position,
    unsigned int numBytes,
    void* pData,
    radMemorySpace pDataSpace
)
{
    rAssertMsg( pDataSpace == radMemorySpace_Local,
                "radFileSystem: radPs3BdDrive: reads only supported for main memory." );

    int slot = (int)handle;

    if (slot < 0 || slot >= MAX_OPEN_FILES || s_OpenFiles[slot].fd < 0)
    {

        m_LastError = HardwareFailure;
        return Error;
    }

    // Calculate actual byte offset
    uint64_t offset = (uint64_t)sector * PS3_BD_SECTOR_SIZE + position;

    // Clamp to file size
    if (offset >= s_OpenFiles[slot].size)
    {
        memset(pData, 0, numBytes);
        m_LastError = Success;
        return Complete;
    }

    uint64_t bytesToRead = numBytes;
    if (offset + bytesToRead > s_OpenFiles[slot].size)
    {
        bytesToRead = s_OpenFiles[slot].size - offset;
    }

    // Seek to position
    uint64_t pos;
    CellFsErrno err = cellFsLseek(s_OpenFiles[slot].fd, offset, CELL_FS_SEEK_SET, &pos);
    if (err != CELL_FS_SUCCEEDED)
    {

        m_LastError = HardwareFailure;
        return Error;
    }

    // Read data
    uint64_t bytesRead;
    err = cellFsRead(s_OpenFiles[slot].fd, pData, bytesToRead, &bytesRead);
    if (err != CELL_FS_SUCCEEDED)
    {

        m_LastError = HardwareFailure;
        return Error;
    }

    // Zero out remaining if needed
    if (bytesRead < numBytes)
    {
        memset((char*)pData + bytesRead, 0, numBytes - bytesRead);
    }

    m_LastError = Success;
    return Complete;
}

radDrive::CompletionStatus radPs3BdDrive::FindFirst
(
    const char*                 searchSpec,
    IRadDrive::DirectoryInfo*   pDirectoryInfo,
    radFileDirHandle*           pHandle,
    bool                        firstSearch
)
{
    // Convert path
    char ps3Path[512];
    ConvertPath(searchSpec, ps3Path, sizeof(ps3Path));

    // Extract directory path and pattern
    char dirPath[512];
    char pattern[256];

    // Find last slash
    char* lastSlash = strrchr(ps3Path, '/');
    if (lastSlash != NULL)
    {
        size_t dirLen = lastSlash - ps3Path;
        strncpy(dirPath, ps3Path, dirLen);
        dirPath[dirLen] = '\0';
        strcpy(pattern, lastSlash + 1);
    }
    else
    {
        strcpy(dirPath, PS3_BASE_PATH);
        strcpy(pattern, ps3Path);
    }

    // Open directory
    int dirFd;
    CellFsErrno err = cellFsOpendir(dirPath, &dirFd);
    if (err != CELL_FS_SUCCEEDED)
    {

        m_LastError = FileNotFound;
        return Error;
    }

    // Store handle info
    pHandle->m_Lsn = (unsigned int)dirFd;
    strncpy(pHandle->m_pSpec, pattern, RAD_PS3_SPEC_LEN);
    pHandle->m_pSpec[RAD_PS3_SPEC_LEN] = '\0';

    // Find the first matching entry
    return FindNext(pHandle, pDirectoryInfo);
}

radDrive::CompletionStatus radPs3BdDrive::FindNext
(
    radFileDirHandle*           pHandle,
    IRadDrive::DirectoryInfo*   pDirectoryInfo
)
{
    int dirFd = (int)pHandle->m_Lsn;

    CellFsDirent entry;
    uint64_t nread;

    while (true)
    {
        CellFsErrno err = cellFsReaddir(dirFd, &entry, &nread);
        if (err != CELL_FS_SUCCEEDED || nread == 0)
        {
            // End of directory
            pDirectoryInfo->m_Name[0] = '\0';
            pDirectoryInfo->m_Type = IRadDrive::DirectoryInfo::IsDone;
            m_LastError = Success;
            return Complete;
        }

        // Skip . and ..
        if (strcmp(entry.d_name, ".") == 0 || strcmp(entry.d_name, "..") == 0)
        {
            continue;
        }

        // Check if name matches pattern
        if (radStringMatchesWildCardPattern(entry.d_name, pHandle->m_pSpec))
        {
            strncpy(pDirectoryInfo->m_Name, entry.d_name, radFileFilenameMax);
            pDirectoryInfo->m_Name[radFileFilenameMax - 1] = '\0';

            if (entry.d_type == CELL_FS_TYPE_DIRECTORY)
            {
                pDirectoryInfo->m_Type = IRadDrive::DirectoryInfo::IsDirectory;
            }
            else
            {
                pDirectoryInfo->m_Type = IRadDrive::DirectoryInfo::IsFile;
            }

            m_LastError = Success;
            return Complete;
        }
    }
}

radDrive::CompletionStatus radPs3BdDrive::FindClose( radFileDirHandle* pHandle )
{
    int dirFd = (int)pHandle->m_Lsn;
    if (dirFd >= 0)
    {
        cellFsClosedir(dirFd);
    }
    return Complete;
}

#endif // RAD_PS3
