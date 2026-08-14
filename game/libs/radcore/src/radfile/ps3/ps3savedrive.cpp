//=============================================================================
// Copyright (c) 2002 Radical Games Ltd.  All rights reserved.
//=============================================================================


//=============================================================================
//
// File:        ps3savedrive.cpp
//
// Subsystem:   Radical File System
//
// Description: This file contains the implementation of the PS3 HDD save drive.
//
// Revisions:   Based on Xbox save drive implementation
//
//=============================================================================

//=============================================================================
// Include Files
//=============================================================================

#ifdef RAD_PS3

#include <cell/cell_fs.h>
#include <sys/paths.h>
#include <sysutil/sysutil_savedata.h>
#include <string.h>
#include <stdlib.h>
#include <radplatform.hpp>
#include <radstring.hpp>
#include "ps3savedrive.hpp"

//=============================================================================
// Local Definitions
//=============================================================================

//
// Game title ID - should be defined by the game build
//
#ifndef PS3_TITLE_ID
#define PS3_TITLE_ID "NPXX00000"
#endif

//=============================================================================
// Public Functions
//=============================================================================

//=============================================================================
// Function:    radPs3SaveDriveFactory
//=============================================================================
// Description: This member is responsible for constructing a radPs3SaveDrive object.
//
// Parameters:  pointer to receive drive object
//              pointer to the drive name
//              allocator
//
// Returns:
//------------------------------------------------------------------------------

void radPs3SaveDriveFactory
(
    radDrive**         ppDrive,
    const char*        pDriveName,
    radMemoryAllocator alloc
)
{
    //
    // Simply construct the drive object.
    //
    *ppDrive = new( alloc ) radPs3SaveDrive( pDriveName, alloc );
    rAssert( *ppDrive != NULL );
}

//=============================================================================
// Public Member Functions
//=============================================================================

radPs3SaveDrive::radPs3SaveDrive( const char* driveSpec, radMemoryAllocator alloc )
    :
    radDrive( ),
    m_SlotNumber( 0 ),
    m_pMutex( NULL )
{
    //
    // Extract slot number from drive spec
    //
    m_SlotNumber = driveSpec[ PS3SAVEDRIVE_SLOT_LOC ] - '0';
    if( m_SlotNumber < 0 || m_SlotNumber > 9 )
    {
        m_SlotNumber = 0;
    }

    //
    // Build the drive name
    //
    sprintf( m_DriveName, "GAMEDATA:%d", m_SlotNumber );

    //
    // Build base path for save data
    //
    sprintf( m_BasePath, "%s%s/USRDIR/savedata%d/", PS3_SAVEDATA_PATH, PS3_TITLE_ID, m_SlotNumber );

    //
    // Create a mutex for lock/unlock
    //
    radThreadCreateMutex( &m_pMutex, alloc );
    rAssert( m_pMutex != NULL );

    //
    // Create the drive thread.
    //
    m_pDriveThread = new( alloc ) radDriveThread( m_pMutex, alloc, 8 * 1024 );
    rAssert( m_pDriveThread != NULL );

    //
    // Media info
    //
    m_MediaInfo.m_FreeFiles = 0;
    m_MediaInfo.m_FreeSpace = 0;
    m_MediaInfo.m_SectorSize = 1;
    m_MediaInfo.m_VolumeName[0] = '\0';
    m_MediaInfo.m_MediaState = IRadDrive::MediaInfo::MediaPresent;
}

radPs3SaveDrive::~radPs3SaveDrive( void )
{
    m_pMutex->Release( );
    m_pDriveThread->Release( );
}

//=============================================================================
// Function:    radPs3SaveDrive::Lock
//=============================================================================

void radPs3SaveDrive::Lock( void )
{
    m_pMutex->Lock( );
}

//=============================================================================
// Function:    radPs3SaveDrive::Unlock
//=============================================================================

void radPs3SaveDrive::Unlock( void )
{
    m_pMutex->Unlock( );
}

//=============================================================================
// Function:    radPs3SaveDrive::GetCapabilities
//=============================================================================

unsigned int radPs3SaveDrive::GetCapabilities( void )
{
    return ( radDriveWriteable | radDriveEnumerable | radDriveFile | radDriveSaveGame | radDriveDirectory );
}

//=============================================================================
// Function:    radPs3SaveDrive::GetDriveName
//=============================================================================

const char* radPs3SaveDrive::GetDriveName( void )
{
    return m_DriveName;
}

//=============================================================================
// Function:    radPs3SaveDrive::Initialize
//=============================================================================

radDrive::CompletionStatus radPs3SaveDrive::Initialize( void )
{
    //
    // Check if the base path exists, create it if not
    //
    if( !CheckDirectoryExists( m_BasePath ) )
    {
        if( !CreateDirectoryPath( m_BasePath ) )
        {
            m_LastError = HardwareFailure;
            m_MediaInfo.m_MediaState = IRadDrive::MediaInfo::MediaNotPresent;
            return Error;
        }
    }

    //
    // Get free space info
    //
    uint64_t freeSize;
    uint32_t blockSize;
    CellFsErrno err = cellFsGetFreeSize( "/dev_hdd0/", &blockSize, &freeSize );
    if( err == CELL_FS_SUCCEEDED )
    {
        m_MediaInfo.m_FreeSpace = (unsigned int)( freeSize * blockSize );
    }
    else
    {
        m_MediaInfo.m_FreeSpace = 0;
    }

    m_MediaInfo.m_MediaState = IRadDrive::MediaInfo::MediaPresent;
    m_LastError = Success;
    return Complete;
}

//=============================================================================
// Function:    radPs3SaveDrive::OpenFile
//=============================================================================

radDrive::CompletionStatus radPs3SaveDrive::OpenFile
(
    const char*        fileName,
    radFileOpenFlags   flags,
    bool               writeAccess,
    radFileHandle*     pHandle,
    unsigned int*      pSize
)
{
    //
    // Build full path
    //
    char fullPath[ MAX_PS3SAVE_FILENAME ];
    BuildFilePath( fileName, fullPath, MAX_PS3SAVE_FILENAME );

    //
    // Determine open flags
    //
    int openFlags = 0;
    if( writeAccess )
    {
        openFlags = CELL_FS_O_RDWR;
    }
    else
    {
        openFlags = CELL_FS_O_RDONLY;
    }

    if( flags == CreateAlways )
    {
        openFlags |= CELL_FS_O_CREAT | CELL_FS_O_TRUNC;
    }
    else if( flags == OpenAlways )
    {
        openFlags |= CELL_FS_O_CREAT;
    }

    //
    // Open the file
    //
    int fd;
    CellFsErrno err = cellFsOpen( fullPath, openFlags, &fd, NULL, 0 );
    if( err != CELL_FS_SUCCEEDED )
    {
        if( err == CELL_FS_ENOENT )
        {
            m_LastError = FileNotFound;
        }
        else
        {
            m_LastError = HardwareFailure;
        }
        return Error;
    }

    //
    // Get file size
    //
    CellFsStat stat;
    err = cellFsFstat( fd, &stat );
    if( err == CELL_FS_SUCCEEDED )
    {
        *pSize = (unsigned int)stat.st_size;
    }
    else
    {
        *pSize = 0;
    }

    *pHandle = (radFileHandle)(intptr_t)fd;
    m_LastError = Success;
    return Complete;
}

//=============================================================================
// Function:    radPs3SaveDrive::OpenSaveGame
//=============================================================================

radDrive::CompletionStatus radPs3SaveDrive::OpenSaveGame
(
    const char*        fileName,
    radFileOpenFlags   flags,
    bool               writeAccess,
    radMemcardInfo*    memcardInfo,
    unsigned int       maxSize,
    radFileHandle*     pHandle,
    unsigned int*      pSize
)
{
    //
    // Just use normal file open for PS3
    //
    return OpenFile( fileName, flags, writeAccess, pHandle, pSize );
}

//=============================================================================
// Function:    radPs3SaveDrive::CloseFile
//=============================================================================

radDrive::CompletionStatus radPs3SaveDrive::CloseFile( radFileHandle handle, const char* fileName )
{
    int fd = (int)(intptr_t)handle;
    cellFsClose( fd );
    return Complete;
}

//=============================================================================
// Function:    radPs3SaveDrive::CommitFile
//=============================================================================

radDrive::CompletionStatus radPs3SaveDrive::CommitFile( radFileHandle handle, const char* fileName )
{
    int fd = (int)(intptr_t)handle;
    cellFsFsync( fd );
    return Complete;
}

//=============================================================================
// Function:    radPs3SaveDrive::ReadFile
//=============================================================================

radDrive::CompletionStatus radPs3SaveDrive::ReadFile
(
    radFileHandle handle,
    const char* fileName,
    IRadFile::BufferedReadState state,
    unsigned int position,
    void* pData,
    unsigned int bytesToRead,
    unsigned int* bytesRead,
    radMemorySpace pDataSpace
)
{
    rAssertMsg( pDataSpace == radMemorySpace_Local,
                "radFileSystem: radPs3SaveDrive: reads only supported for main memory." );

    int fd = (int)(intptr_t)handle;

    //
    // Seek to position
    //
    uint64_t pos;
    CellFsErrno err = cellFsLseek( fd, position, CELL_FS_SEEK_SET, &pos );
    if( err != CELL_FS_SUCCEEDED )
    {
        m_LastError = HardwareFailure;
        return Error;
    }

    //
    // Read data
    //
    uint64_t read;
    err = cellFsRead( fd, pData, bytesToRead, &read );
    if( err != CELL_FS_SUCCEEDED )
    {
        m_LastError = HardwareFailure;
        return Error;
    }

    *bytesRead = (unsigned int)read;
    m_LastError = Success;
    return Complete;
}

//=============================================================================
// Function:    radPs3SaveDrive::WriteFile
//=============================================================================

radDrive::CompletionStatus radPs3SaveDrive::WriteFile
(
    radFileHandle handle,
    const char* fileName,
    IRadFile::BufferedReadState state,
    unsigned int position,
    const void* pData,
    unsigned int bytesToWrite,
    unsigned int* bytesWritten,
    unsigned int* size,
    radMemorySpace pDataSpace
)
{
    rAssertMsg( pDataSpace == radMemorySpace_Local,
                "radFileSystem: radPs3SaveDrive: writes only supported from main memory." );

    int fd = (int)(intptr_t)handle;

    //
    // Seek to position
    //
    uint64_t pos;
    CellFsErrno err = cellFsLseek( fd, position, CELL_FS_SEEK_SET, &pos );
    if( err != CELL_FS_SUCCEEDED )
    {
        m_LastError = HardwareFailure;
        return Error;
    }

    //
    // Write data
    //
    uint64_t written;
    err = cellFsWrite( fd, pData, bytesToWrite, &written );
    if( err != CELL_FS_SUCCEEDED )
    {
        if( err == CELL_FS_ENOSPC )
        {
            m_LastError = NoFreeSpace;
        }
        else
        {
            m_LastError = HardwareFailure;
        }
        return Error;
    }

    *bytesWritten = (unsigned int)written;

    //
    // Update size
    //
    CellFsStat stat;
    err = cellFsFstat( fd, &stat );
    if( err == CELL_FS_SUCCEEDED )
    {
        *size = (unsigned int)stat.st_size;
    }

    m_LastError = Success;
    return Complete;
}

//=============================================================================
// Function:    radPs3SaveDrive::FindFirst
//=============================================================================

radDrive::CompletionStatus radPs3SaveDrive::FindFirst
(
    const char*                 searchSpec,
    IRadDrive::DirectoryInfo*   pDirectoryInfo,
    radFileDirHandle*           pHandle,
    bool                        firstSearch
)
{
    //
    // Build full path for directory
    //
    char fullPath[ MAX_PS3SAVE_FILENAME ];
    BuildFilePath( "", fullPath, MAX_PS3SAVE_FILENAME );

    //
    // Open the directory
    //
    int dirFd;
    CellFsErrno err = cellFsOpendir( fullPath, &dirFd );
    if( err != CELL_FS_SUCCEEDED )
    {
        pDirectoryInfo->m_Name[0] = '\0';
        pDirectoryInfo->m_Type = IRadDrive::DirectoryInfo::IsDone;
        m_LastError = FileNotFound;
        return Error;
    }

    //
    // Store directory handle
    //
    pHandle->m_Lsn = (unsigned int)dirFd;
    strcpy( pHandle->m_pSpec, searchSpec );

    return FindNext( pHandle, pDirectoryInfo );
}

//=============================================================================
// Function:    radPs3SaveDrive::FindNext
//=============================================================================

radDrive::CompletionStatus radPs3SaveDrive::FindNext( radFileDirHandle* pHandle, IRadDrive::DirectoryInfo* pDirectoryInfo )
{
    int dirFd = (int)pHandle->m_Lsn;

    while( true )
    {
        CellFsDirent entry;
        uint64_t read;
        CellFsErrno err = cellFsReaddir( dirFd, &entry, &read );

        if( err != CELL_FS_SUCCEEDED || read == 0 )
        {
            pDirectoryInfo->m_Name[0] = '\0';
            pDirectoryInfo->m_Type = IRadDrive::DirectoryInfo::IsDone;
            m_LastError = Success;
            return Complete;
        }

        //
        // Skip . and ..
        //
        if( strcmp( entry.d_name, "." ) == 0 || strcmp( entry.d_name, ".." ) == 0 )
        {
            continue;
        }

        //
        // Check if it matches the search spec
        //
        if( ::radStringMatchesWildCardPattern( entry.d_name, pHandle->m_pSpec ) )
        {
            strcpy( pDirectoryInfo->m_Name, entry.d_name );
            if( entry.d_type == CELL_FS_TYPE_DIRECTORY )
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

//=============================================================================
// Function:    radPs3SaveDrive::FindClose
//=============================================================================

radDrive::CompletionStatus radPs3SaveDrive::FindClose( radFileDirHandle* pHandle )
{
    int dirFd = (int)pHandle->m_Lsn;
    cellFsClosedir( dirFd );
    return Complete;
}

//=============================================================================
// Function:    radPs3SaveDrive::Format
//=============================================================================

radDrive::CompletionStatus radPs3SaveDrive::Format( void )
{
    //
    // Delete all files in the save directory
    //
    char fullPath[ MAX_PS3SAVE_FILENAME ];
    BuildFilePath( "", fullPath, MAX_PS3SAVE_FILENAME );

    int dirFd;
    CellFsErrno err = cellFsOpendir( fullPath, &dirFd );
    if( err != CELL_FS_SUCCEEDED )
    {
        return Complete;
    }

    CellFsDirent entry;
    uint64_t read;
    while( cellFsReaddir( dirFd, &entry, &read ) == CELL_FS_SUCCEEDED && read > 0 )
    {
        if( strcmp( entry.d_name, "." ) != 0 && strcmp( entry.d_name, ".." ) != 0 )
        {
            char filePath[ MAX_PS3SAVE_FILENAME ];
            sprintf( filePath, "%s%s", fullPath, entry.d_name );
            cellFsUnlink( filePath );
        }
    }

    cellFsClosedir( dirFd );
    m_LastError = Success;
    return Complete;
}

//=============================================================================
// Function:    radPs3SaveDrive::CreateDir
//=============================================================================

radDrive::CompletionStatus radPs3SaveDrive::CreateDir( const char* pName )
{
    char fullPath[ MAX_PS3SAVE_FILENAME ];
    BuildFilePath( pName, fullPath, MAX_PS3SAVE_FILENAME );

    CellFsErrno err = cellFsMkdir( fullPath, CELL_FS_DEFAULT_CREATE_MODE_1 );
    if( err != CELL_FS_SUCCEEDED && err != CELL_FS_EEXIST )
    {
        m_LastError = HardwareFailure;
        return Error;
    }

    m_LastError = Success;
    return Complete;
}

//=============================================================================
// Function:    radPs3SaveDrive::DestroyDir
//=============================================================================

radDrive::CompletionStatus radPs3SaveDrive::DestroyDir( const char* pName )
{
    char fullPath[ MAX_PS3SAVE_FILENAME ];
    BuildFilePath( pName, fullPath, MAX_PS3SAVE_FILENAME );

    CellFsErrno err = cellFsRmdir( fullPath );
    if( err != CELL_FS_SUCCEEDED )
    {
        m_LastError = HardwareFailure;
        return Error;
    }

    m_LastError = Success;
    return Complete;
}

//=============================================================================
// Function:    radPs3SaveDrive::DestroyFile
//=============================================================================

radDrive::CompletionStatus radPs3SaveDrive::DestroyFile( const char* filename )
{
    char fullPath[ MAX_PS3SAVE_FILENAME ];
    BuildFilePath( filename, fullPath, MAX_PS3SAVE_FILENAME );

    CellFsErrno err = cellFsUnlink( fullPath );
    if( err != CELL_FS_SUCCEEDED )
    {
        m_LastError = HardwareFailure;
        return Error;
    }

    m_LastError = Success;
    return Complete;
}

//=============================================================================
// Function:    radPs3SaveDrive::GetCreationSize
//=============================================================================

unsigned int radPs3SaveDrive::GetCreationSize( radMemcardInfo* memcardInfo, unsigned int size )
{
    //
    // PS3 HDD doesn't need special size calculations
    //
    return size;
}

//=============================================================================
// Function:    radPs3SaveDrive::BuildFilePath
//=============================================================================

void radPs3SaveDrive::BuildFilePath( const char* fileName, char* fullPath, unsigned int pathSize )
{
    //
    // Start with base path
    //
    strncpy( fullPath, m_BasePath, pathSize - 1 );
    fullPath[ pathSize - 1 ] = '\0';

    //
    // Append filename, converting backslashes to forward slashes
    //
    int len = strlen( fullPath );
    const char* src = fileName;

    //
    // Skip leading backslash
    //
    if( *src == '\\' )
    {
        src++;
    }

    while( *src && len < (int)(pathSize - 1) )
    {
        if( *src == '\\' )
        {
            fullPath[ len++ ] = '/';
        }
        else
        {
            fullPath[ len++ ] = *src;
        }
        src++;
    }
    fullPath[ len ] = '\0';
}

//=============================================================================
// Function:    radPs3SaveDrive::CheckDirectoryExists
//=============================================================================

bool radPs3SaveDrive::CheckDirectoryExists( const char* path )
{
    CellFsStat stat;
    CellFsErrno err = cellFsStat( path, &stat );
    return ( err == CELL_FS_SUCCEEDED && ( stat.st_mode & CELL_FS_S_IFDIR ) );
}

//=============================================================================
// Function:    radPs3SaveDrive::CreateDirectoryPath
//=============================================================================

bool radPs3SaveDrive::CreateDirectoryPath( const char* path )
{
    char temp[ MAX_PS3SAVE_FILENAME ];
    strncpy( temp, path, MAX_PS3SAVE_FILENAME - 1 );
    temp[ MAX_PS3SAVE_FILENAME - 1 ] = '\0';

    //
    // Create each directory in the path
    //
    char* p = temp;
    while( *p )
    {
        if( *p == '/' && p != temp )
        {
            *p = '\0';
            if( !CheckDirectoryExists( temp ) )
            {
                CellFsErrno err = cellFsMkdir( temp, CELL_FS_DEFAULT_CREATE_MODE_1 );
                if( err != CELL_FS_SUCCEEDED && err != CELL_FS_EEXIST )
                {
                    return false;
                }
            }
            *p = '/';
        }
        p++;
    }

    //
    // Create final directory
    //
    if( !CheckDirectoryExists( temp ) )
    {
        CellFsErrno err = cellFsMkdir( temp, CELL_FS_DEFAULT_CREATE_MODE_1 );
        if( err != CELL_FS_SUCCEEDED && err != CELL_FS_EEXIST )
        {
            return false;
        }
    }

    return true;
}

#endif // RAD_PS3

