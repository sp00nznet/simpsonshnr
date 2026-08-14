//=============================================================================
// Copyright (c) 2002 Radical Games Ltd.  All rights reserved.
//=============================================================================


//=============================================================================
//
// File:        ps3savedrive.hpp
//
// Subsystem:   Radical File System
//
// Description: This file contains all definitions and classes relevant to
//              PS3 HDD save drive for game saves.
//
// Revisions:   Based on Xbox save drive implementation
//
//=============================================================================

#ifndef PS3SAVEDRIVE_HPP
#define PS3SAVEDRIVE_HPP

//=============================================================================
// Include Files
//=============================================================================

#include "../common/drive.hpp"
#include "../common/drivethread.hpp"

//=============================================================================
// Defines
//=============================================================================

//
// PS3 save drive name format: GAMEDATA:[slot]
// e.g., GAMEDATA:0, GAMEDATA:1, etc.
//
static const char s_PS3SaveDrive[] = "GAMEDATA:?";
#define PS3SAVEDRIVE_SLOT_LOC 9

//
// Maximum file name length
//
#define MAX_PS3SAVE_FILENAME    256

//
// Save data directory path on HDD
//
#define PS3_SAVEDATA_PATH "/dev_hdd0/game/"

//=============================================================================
// Public Functions
//=============================================================================

//
// Every physical drive type must provide a drive factory.
//
void radPs3SaveDriveFactory( radDrive** ppDrive, const char* driveSpec, radMemoryAllocator alloc );

//=============================================================================
// Class Declarations
//=============================================================================

class radPs3SaveDrive : public radDrive
{
public:

    //
    // Constructor / destructor.
    //
    radPs3SaveDrive( const char* driveSpec, radMemoryAllocator alloc );
    virtual ~radPs3SaveDrive( void );

    void Lock( void );
    void Unlock( void );

    //
    // This member reports this physical drives capabilities
    //
    unsigned int GetCapabilities( void );

    const char* GetDriveName( void );

    CompletionStatus Initialize( void );

    CompletionStatus OpenFile( const char*        fileName,
                               radFileOpenFlags   flags,
                               bool               writeAccess,
                               radFileHandle*     pHandle,
                               unsigned int*      pSize );

    CompletionStatus OpenSaveGame( const char*        fileName,
                                   radFileOpenFlags   flags,
                                   bool               writeAccess,
                                   radMemcardInfo*    memcardInfo,
                                   unsigned int       maxSize,
                                   radFileHandle*     pHandle,
                                   unsigned int*      pSize );

    CompletionStatus CloseFile( radFileHandle handle, const char* fileName );

    CompletionStatus CommitFile( radFileHandle handle, const char* fileName );

    CompletionStatus ReadFile( radFileHandle handle,
                               const char* fileName,
                               IRadFile::BufferedReadState state,
                               unsigned int position,
                               void* pData,
                               unsigned int bytesToRead,
                               unsigned int* bytesRead,
                               radMemorySpace pDataSpace );

    CompletionStatus WriteFile( radFileHandle handle,
                                const char* fileName,
                                IRadFile::BufferedReadState state,
                                unsigned int position,
                                const void* pData,
                                unsigned int bytesToWrite,
                                unsigned int* bytesWritten,
                                unsigned int* size,
                                radMemorySpace pDataSpace );

    CompletionStatus FindFirst( const char*                 searchSpec,
                                IRadDrive::DirectoryInfo*   pDirectoryInfo,
                                radFileDirHandle*           pHandle,
                                bool                        firstSearch );

    CompletionStatus FindNext( radFileDirHandle* pHandle, IRadDrive::DirectoryInfo* pDirectoryInfo );

    CompletionStatus FindClose( radFileDirHandle* pHandle );

    CompletionStatus Format( void );

    CompletionStatus CreateDir( const char* pName );

    CompletionStatus DestroyDir( const char* pName );

    CompletionStatus DestroyFile( const char* filename );

    unsigned int GetCreationSize( radMemcardInfo* memcardInfo, unsigned int size );

private:
    void BuildFilePath( const char* fileName, char* fullPath, unsigned int pathSize );
    bool CheckDirectoryExists( const char* path );
    bool CreateDirectoryPath( const char* path );

    //
    // Drive name
    //
    char m_DriveName[ 16 ];

    //
    // Slot number (for multiple save slots)
    //
    int m_SlotNumber;

    //
    // Base path for this save drive
    //
    char m_BasePath[ 256 ];

    //
    // Mutex for critical sections
    //
    IRadThreadMutex* m_pMutex;
};

#endif // PS3SAVEDRIVE_HPP

