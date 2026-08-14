//=============================================================================
// Copyright (c) 2002 Radical Games Ltd.  All rights reserved.
//=============================================================================


//=============================================================================
//
// File:        ps3bddrive.hpp
//
// Subsystem:   Radical File System
//
// Description: PS3 Blu-ray drive - FILE-BASED implementation for RPCS3.
//              Uses cellFs* APIs for direct file access.
//
// Revisions:   Rewritten for file-based access (RPCS3 compatible)
//
//=============================================================================

#ifndef PS3BDDRIVE_HPP
#define PS3BDDRIVE_HPP

//=============================================================================
// Include Files
//=============================================================================

#include "../common/drive.hpp"
#include "../common/drivethread.hpp"
#include "../common/buffereddrive.hpp"

//=============================================================================
// Defines
//=============================================================================

//
// Sector size (used for read alignment calculations)
//
#define PS3_BD_SECTOR_SIZE     2048
#define PS3_MAX_SECTOR_SIZE    2048
#define MAX_PS3FILENAMELEN     256

//
// Number of sectors in the transfer buffer.
//
#define PS3_BD_DRIVE_TRANSFER_BUFFER_SECTORS  64
#define PS3_BD_DRIVE_TRANSFER_BUFFER_SIZE (PS3_BD_DRIVE_TRANSFER_BUFFER_SECTORS * PS3_MAX_SECTOR_SIZE)

//
// Alignment for transfer buffer
//
#define PS3_BD_DRIVE_ALIGNMENT   128

//
// Search spec length
//
#define RAD_PS3_SPEC_LEN 64

//=============================================================================
// Statics
//=============================================================================

//
// The name of the drive.
//
static const char s_PS3BDDriveName[ ] = "BDVD:";

//=============================================================================
// Public Functions
//=============================================================================

//
// Every physical drive type must provide a drive factory.
//
void radPs3BdDriveFactory( radDrive** ppDrive, const char* driveSpec, radMemoryAllocator alloc );

//=============================================================================
// Class Declarations
//=============================================================================

class radPs3BdDrive : public radBufferedReader,
                      public radDrive
{
public:

    //
    // Constructor / destructor.
    //
    radPs3BdDrive( radMemoryAllocator alloc );
    virtual ~radPs3BdDrive( void );

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

    CompletionStatus CloseFile( radFileHandle handle, const char* fileName );

    CompletionStatus ReadAligned( radFileHandle handle,
                                  const char* fileName,
                                  unsigned int sector,
                                  unsigned int numSectors,
                                  void* pData,
                                  radMemorySpace pDataSpace );

     CompletionStatus ReadBuffered( radFileHandle handle,
                                    const char* fileName,
                                    unsigned int sector,
                                    unsigned int numSectors,
                                    unsigned int position,
                                    unsigned int numBytes,
                                    void* pData,
                                    radMemorySpace pDataSpace );

    unsigned int GetReadBufferSectors( void );

    CompletionStatus FindFirst( const char*                 searchSpec,
                                IRadDrive::DirectoryInfo*   pDirectoryInfo,
                                radFileDirHandle*           pHandle,
                                bool                        firstSearch );

    CompletionStatus FindNext( radFileDirHandle* pHandle, IRadDrive::DirectoryInfo* pDirectoryInfo );

    CompletionStatus FindClose( radFileDirHandle* pHandle );

    //
    // radBufferedReader
    //
    IMPLEMENT_BUFFERED_READ;

private:
    unsigned int            m_OpenFiles;

    //
    // Buffer for buffered reads
    //
    unsigned char  m_SectorBufferSpace[ PS3_BD_DRIVE_TRANSFER_BUFFER_SIZE + PS3_BD_DRIVE_ALIGNMENT ];
    unsigned char* m_SectorBuffer;

    //
    // Mutex for critical sections
    //
    IRadThreadMutex*    m_pMutex;
};

#endif // PS3BDDRIVE_HPP
