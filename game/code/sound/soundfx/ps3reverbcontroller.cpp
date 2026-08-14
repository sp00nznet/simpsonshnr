//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        ps3reverbcontroller.cpp
//
// Description: Implementation of PS3ReverbController class
//
// History:     Based on PS2 reverb controller implementation
//
//=============================================================================

//========================================
// System Includes
//========================================
#ifdef RAD_PS3

#include <radsound_ps3.hpp>

//========================================
// Project Includes
//========================================
#include <sound/soundfx/ps3reverbcontroller.h>
#include <sound/soundfx/reverbsettings.h>

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
// PS3ReverbController::PS3ReverbController
//==============================================================================
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//==============================================================================
PS3ReverbController::PS3ReverbController() :
    m_reverbInterface( NULL )
{
    // Create the PS3 reverb effect
    m_reverbInterface = radSoundCreateEffectPs3( GMA_AUDIO_PERSISTENT );

    if( m_reverbInterface != NULL )
    {
        m_reverbInterface->AddRef();
        registerReverbEffect( m_reverbInterface );
    }
}

//==============================================================================
// PS3ReverbController::~PS3ReverbController
//==============================================================================
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//==============================================================================
PS3ReverbController::~PS3ReverbController()
{
    if( m_reverbInterface != NULL )
    {
        m_reverbInterface->Release();
        m_reverbInterface = NULL;
    }
}

//==============================================================================
// PS3ReverbController::SetReverbOn
//==============================================================================
// Description: Turn on reverb effect with the given settings
//
// Parameters:  settings - reverb settings to apply
//
// Return:      None.
//
//==============================================================================
void PS3ReverbController::SetReverbOn( reverbSettings* settings )
{
    if( m_reverbInterface == NULL || settings == NULL )
    {
        return;
    }

    // Apply reverb settings
    // Map PS2-style reverb modes to PS3 equivalents
    int ps2Mode = settings->GetPS2ReverbMode();
    IRadSoundEffectPs3::Mode ps3Mode;

    switch( ps2Mode )
    {
        case 0:  // Off
            ps3Mode = IRadSoundEffectPs3::Off;
            break;
        case 1:  // Room
            ps3Mode = IRadSoundEffectPs3::Room;
            break;
        case 2:  // StudioA
            ps3Mode = IRadSoundEffectPs3::StudioA;
            break;
        case 3:  // StudioB
            ps3Mode = IRadSoundEffectPs3::StudioB;
            break;
        case 4:  // StudioC
            ps3Mode = IRadSoundEffectPs3::StudioC;
            break;
        case 5:  // Hall
            ps3Mode = IRadSoundEffectPs3::Hall;
            break;
        case 6:  // Space
            ps3Mode = IRadSoundEffectPs3::Space;
            break;
        case 7:  // Echo
            ps3Mode = IRadSoundEffectPs3::Echo;
            break;
        case 8:  // Delay
            ps3Mode = IRadSoundEffectPs3::Delay;
            break;
        case 9:  // Pipe
            ps3Mode = IRadSoundEffectPs3::Pipe;
            break;
        default:
            ps3Mode = IRadSoundEffectPs3::Room;
            break;
    }

    m_reverbInterface->SetMode( ps3Mode );
    m_reverbInterface->SetDelay( settings->GetPS2Delay() );
    m_reverbInterface->SetFeedback( settings->GetPS2Feedback() );

    // Set up fade parameters
    prepareFadeSettings( settings->GetGain(),
                         settings->GetFadeInTime(),
                         settings->GetFadeOutTime() );

    m_reverbInterface->SetEnabled( true );
}

//==============================================================================
// PS3ReverbController::SetReverbOff
//==============================================================================
// Description: Turn off reverb effect
//
// Parameters:  None.
//
// Return:      None.
//
//==============================================================================
void PS3ReverbController::SetReverbOff()
{
    if( m_reverbInterface != NULL )
    {
        startFadeOut();
    }
}

#endif // RAD_PS3
