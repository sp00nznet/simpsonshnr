//=============================================================================
// Copyright (c) 2002 Radical Games Ltd.  All rights reserved.
// PS3 stub implementation
//=============================================================================

#include "voice.hpp"

//============================================================================
// radSoundHalVoicePs3::radSoundHalVoicePs3
//============================================================================

radSoundHalVoicePs3::radSoundHalVoicePs3( radMemoryAllocator allocator )
    :
    radSoundObject( ),
    m_Priority( 5 ),
    m_xBuffer( NULL ),
    m_xPositionalGroup( NULL ),
    m_Playing( false ),
    m_Muted( false ),
    m_Volume( 1.0f ),
    m_Trim( 1.0f ),
    m_Pan( 0.0f ),
    m_Pitch( 1.0f ),
    m_PlaybackPosition( 0 )
{
    m_AuxMode[0] = radSoundAuxMode_Off;
    m_AuxMode[1] = radSoundAuxMode_Off;
    m_AuxGain[0] = 0.0f;
    m_AuxGain[1] = 0.0f;
}

//============================================================================
// radSoundHalVoicePs3::~radSoundHalVoicePs3
//============================================================================

radSoundHalVoicePs3::~radSoundHalVoicePs3( void )
{
}

//============================================================================
// radSoundHalVoicePs3::SetPriority
//============================================================================

void radSoundHalVoicePs3::SetPriority( unsigned int priority )
{
    m_Priority = priority;
}

//============================================================================
// radSoundHalVoicePs3::GetPriority
//============================================================================

unsigned int radSoundHalVoicePs3::GetPriority( void )
{
    return m_Priority;
}

//============================================================================
// radSoundHalVoicePs3::SetBuffer
//============================================================================

void radSoundHalVoicePs3::SetBuffer( IRadSoundHalBuffer * pIRadSoundHalBuffer )
{
    m_xBuffer = pIRadSoundHalBuffer;
}

//============================================================================
// radSoundHalVoicePs3::GetBuffer
//============================================================================

IRadSoundHalBuffer * radSoundHalVoicePs3::GetBuffer( void )
{
    return m_xBuffer;
}

//============================================================================
// radSoundHalVoicePs3::Play
//============================================================================

void radSoundHalVoicePs3::Play( void )
{
    // PS3 stub - just set state, no actual playback
    m_Playing = true;
}

//============================================================================
// radSoundHalVoicePs3::Stop
//============================================================================

void radSoundHalVoicePs3::Stop( void )
{
    m_Playing = false;
    m_PlaybackPosition = 0;
}

//============================================================================
// radSoundHalVoicePs3::IsPlaying
//============================================================================

bool radSoundHalVoicePs3::IsPlaying( void )
{
    return m_Playing;
}

//============================================================================
// radSoundHalVoicePs3::SetPlaybackPositionInSamples
//============================================================================

void radSoundHalVoicePs3::SetPlaybackPositionInSamples( unsigned int position )
{
    m_PlaybackPosition = position;
}

//============================================================================
// radSoundHalVoicePs3::GetPlaybackPositionInSamples
//============================================================================

unsigned int radSoundHalVoicePs3::GetPlaybackPositionInSamples( void )
{
    return m_PlaybackPosition;
}

//============================================================================
// radSoundHalVoicePs3::SetVolume
//============================================================================

void radSoundHalVoicePs3::SetVolume( float volume )
{
    m_Volume = volume;
}

//============================================================================
// radSoundHalVoicePs3::GetVolume
//============================================================================

float radSoundHalVoicePs3::GetVolume( void )
{
    return m_Volume;
}

//============================================================================
// radSoundHalVoicePs3::SetTrim
//============================================================================

void radSoundHalVoicePs3::SetTrim( float trim )
{
    m_Trim = trim;
}

//============================================================================
// radSoundHalVoicePs3::GetTrim
//============================================================================

float radSoundHalVoicePs3::GetTrim( void )
{
    return m_Trim;
}

//============================================================================
// radSoundHalVoicePs3::SetMuted
//============================================================================

void radSoundHalVoicePs3::SetMuted( bool muteOn )
{
    m_Muted = muteOn;
}

//============================================================================
// radSoundHalVoicePs3::GetMuted
//============================================================================

bool radSoundHalVoicePs3::GetMuted( void )
{
    return m_Muted;
}

//============================================================================
// radSoundHalVoicePs3::SetPan
//============================================================================

void radSoundHalVoicePs3::SetPan( float pan )
{
    m_Pan = pan;
}

//============================================================================
// radSoundHalVoicePs3::GetPan
//============================================================================

float radSoundHalVoicePs3::GetPan( void )
{
    return m_Pan;
}

//============================================================================
// radSoundHalVoicePs3::SetPitch
//============================================================================

void radSoundHalVoicePs3::SetPitch( float pitch )
{
    m_Pitch = pitch;
}

//============================================================================
// radSoundHalVoicePs3::GetPitch
//============================================================================

float radSoundHalVoicePs3::GetPitch( void )
{
    return m_Pitch;
}

//============================================================================
// radSoundHalVoicePs3::SetAuxMode
//============================================================================

void radSoundHalVoicePs3::SetAuxMode( unsigned int aux, radSoundAuxMode mode )
{
    if( aux < 2 )
    {
        m_AuxMode[ aux ] = mode;
    }
}

//============================================================================
// radSoundHalVoicePs3::GetAuxMode
//============================================================================

radSoundAuxMode radSoundHalVoicePs3::GetAuxMode( unsigned int aux )
{
    if( aux < 2 )
    {
        return m_AuxMode[ aux ];
    }
    return radSoundAuxMode_Off;
}

//============================================================================
// radSoundHalVoicePs3::SetAuxGain
//============================================================================

void radSoundHalVoicePs3::SetAuxGain( unsigned int aux, float gain )
{
    if( aux < 2 )
    {
        m_AuxGain[ aux ] = gain;
    }
}

//============================================================================
// radSoundHalVoicePs3::GetAuxGain
//============================================================================

float radSoundHalVoicePs3::GetAuxGain( unsigned int aux )
{
    if( aux < 2 )
    {
        return m_AuxGain[ aux ];
    }
    return 0.0f;
}

//============================================================================
// radSoundHalVoicePs3::SetPositionalGroup
//============================================================================

void radSoundHalVoicePs3::SetPositionalGroup( IRadSoundHalPositionalGroup * pIRshpg )
{
    m_xPositionalGroup = pIRshpg;
}

//============================================================================
// radSoundHalVoicePs3::GetPositionalGroup
//============================================================================

IRadSoundHalPositionalGroup * radSoundHalVoicePs3::GetPositionalGroup( void )
{
    return m_xPositionalGroup;
}

//============================================================================
// Factory function
//============================================================================

IRadSoundHalVoice * radSoundHalVoiceCreate( radMemoryAllocator allocator )
{
    return new( "radSoundHalVoicePs3", allocator ) radSoundHalVoicePs3( allocator );
}
