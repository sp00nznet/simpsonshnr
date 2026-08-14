//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        soundfilehandler.cpp
//
// Description: Implement SoundFileHandler, which represents sound in the
//              loading queue
//
// History:     19/07/2002 + Created -- Darren
//
//=============================================================================

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================
#include <loading/soundfilehandler.h>

#include <sound/soundmanager.h>
#include <memory/srrmemory.h>
#include <string.h>



//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// SoundFileHandler::SoundFileHandler
//==============================================================================
// Description: Constructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SoundFileHandler::SoundFileHandler()
{
}

//==============================================================================
// SoundFileHandler::~SoundFileHandler
//==============================================================================
// Description: Destructor.
//
// Parameters:	None.
//
// Return:      N/A.
//
//==============================================================================
SoundFileHandler::~SoundFileHandler()
{
}

//=============================================================================
// SoundFileHandler::LoadFile
//=============================================================================
// Description: Load sound file asynchronously
//
// Parameters:  filename - name of file to load
//              pCallback - callback to invoke when loading complete
//              pUserData - user data, unused
//
// Return:     void 
//
//=============================================================================
void SoundFileHandler::LoadFile( const char* filename, 
                                 FileHandler::LoadFileCallback* pCallback,
                                 void* pUserData,
                                 GameMemoryAllocator heap )
{
    mpCallback = pCallback;
    mpUserData = pUserData;

#ifdef RAD_PS3
    //
    // PS3 sound is stubbed (radsound_ps3_stub.cpp) and the .rms sample banks
    // are not in the disc image, so nothing can consume them. Report those
    // complete straight away rather than letting the sound system size an
    // allocation from a header it never read -- that asks the music heap for
    // ~2GB. Tuning scripts still load: daSoundTuner reads its namespace out
    // of them and asserts if they are missing.
    //
    if( filename != NULL )
    {
        int len = strlen( filename );
        if( len > 4 && strcasecmp( filename + len - 4, ".rms" ) == 0 )
        {
            mpCallback->OnLoadFileComplete( mpUserData );
            return;
        }
    }
#endif

    //
    // Pass the load request on to the sound system, giving it this object
    // for notification of completion
    //
    GetSoundManager()->LoadSoundFile( filename, this );
}

//=============================================================================
// SoundFileHandler::LoadFileSync
//=============================================================================
// Description: Load sound file synchronously
//
// Parameters:  filename - name of file to load
//
// Return:      void 
//
//=============================================================================
void SoundFileHandler::LoadFileSync( const char* filename )
{
    //
    // This shouldn't get called.  We don't do synchronous in sound.
    //
    rAssert( false );
}

//=============================================================================
// SoundFileHandler::LoadCompleted
//=============================================================================
// Description: Inform loading manager when asynchronous load completed
//
// Parameters:  None
//
// Return:      void 
//
//=============================================================================
void SoundFileHandler::LoadCompleted()
{
    rAssert( mpCallback != NULL );
    mpCallback->OnLoadFileComplete( mpUserData );
}

//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************
