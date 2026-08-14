//=============================================================================
// Copyright (c) 2002 Radical Games Ltd.  All rights reserved.
// PS3 stub implementation
//=============================================================================

#ifndef RADSOUND_HAL_VOICE_PS3_HPP
#define RADSOUND_HAL_VOICE_PS3_HPP

#include <radsound_hal.hpp>
#include "../../common/radsoundobject.hpp"

struct radSoundHalVoicePs3
    :
    public IRadSoundHalVoice,
    public radSoundObject,
    public radLinkedClass< radSoundHalVoicePs3 >
{
    IMPLEMENT_REFCOUNTED( "radSoundHalVoicePs3" )

    radSoundHalVoicePs3( radMemoryAllocator allocator );
    virtual ~radSoundHalVoicePs3( void );

    // IRadSoundHalVoice
    virtual void SetPriority( unsigned int priority );
    virtual unsigned int GetPriority( void );

    virtual void SetBuffer( IRadSoundHalBuffer * pIRadSoundHalBuffer );
    virtual IRadSoundHalBuffer * GetBuffer( void );

    virtual void Play( void );
    virtual void Stop( void );
    virtual bool IsPlaying( void );

    virtual void SetPlaybackPositionInSamples( unsigned int position );
    virtual unsigned int GetPlaybackPositionInSamples( void );

    virtual void SetVolume( float volume );
    virtual float GetVolume( void );

    virtual void SetTrim( float trim );
    virtual float GetTrim( void );

    virtual void SetMuted( bool muteOn );
    virtual bool GetMuted( void );

    virtual void SetPan( float pan );
    virtual float GetPan( void );

    virtual void SetPitch( float pitch );
    virtual float GetPitch( void );

    virtual void SetAuxMode( unsigned int aux, radSoundAuxMode mode );
    virtual radSoundAuxMode GetAuxMode( unsigned int aux );

    virtual void SetAuxGain( unsigned int aux, float gain );
    virtual float GetAuxGain( unsigned int aux );

    virtual void SetPositionalGroup( IRadSoundHalPositionalGroup * pIRshpg );
    virtual IRadSoundHalPositionalGroup * GetPositionalGroup( void );

private:
    unsigned int m_Priority;
    ref< IRadSoundHalBuffer > m_xBuffer;
    ref< IRadSoundHalPositionalGroup > m_xPositionalGroup;
    bool m_Playing;
    bool m_Muted;
    float m_Volume;
    float m_Trim;
    float m_Pan;
    float m_Pitch;
    unsigned int m_PlaybackPosition;
    radSoundAuxMode m_AuxMode[2];
    float m_AuxGain[2];
};

#endif // RADSOUND_HAL_VOICE_PS3_HPP
