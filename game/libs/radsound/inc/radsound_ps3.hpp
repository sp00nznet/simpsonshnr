//=============================================================================
// Copyright (c) 2002 Radical Games Ltd.  All rights reserved.
//=============================================================================

#ifndef RADSOUND_PS3_HPP
#define RADSOUND_PS3_HPP

#include <radsound_hal.hpp>

struct IRadSoundEffectPs3;

IRadSoundEffectPs3 * radSoundCreateEffectPs3( radMemoryAllocator allocator = RADMEMORY_ALLOC_DEFAULT );

//=============================================================================
// PS3 Sound Effect Interface
//
// Uses libmixer for effect processing and MultiStream for audio playback.
// Supports reverb, echo, and chorus effects through the Cell audio DSP.
//=============================================================================
struct IRadSoundEffectPs3
    :
    public IRadSoundHalEffect
{
    enum Mode
    {
        Off,
        Room,
        StudioA,
        StudioB,
        StudioC,
        Hall,
        Space,
        Echo,
        Delay,
        Pipe,
        // PS3-specific modes using MultiStream
        Chorus,
        Flanger
    };

    virtual void SetEnabled( bool enabled ) = 0;
    virtual bool GetEnabled( void ) = 0;

    virtual void SetGain( float gain ) = 0;
    virtual float GetGain( void ) = 0;

    virtual void SetMode( IRadSoundEffectPs3::Mode mode ) = 0;
    virtual IRadSoundEffectPs3::Mode GetMode( void ) = 0;

    //
    // Delay and Feedback are active only in Echo and Delay mode.
    // Both are measured from 0.0f to 1.0f
    //

    virtual void SetDelay( float delayTime ) = 0;
    virtual float GetDelayTime( void ) = 0;

    virtual void SetFeedback( float feedBack ) = 0;
    virtual float GetFeedback( void ) = 0;

    //
    // PS3-specific reverb parameters
    //

    virtual void SetReverbTime( float time ) = 0;
    virtual float GetReverbTime( void ) = 0;

    virtual void SetDamping( float damping ) = 0;
    virtual float GetDamping( void ) = 0;

    virtual void SetPreDelay( float preDelay ) = 0;
    virtual float GetPreDelay( void ) = 0;

    //
    // Chorus parameters (PS3 specific)
    //

    virtual void SetChorusDepth( float depth ) = 0;
    virtual float GetChorusDepth( void ) = 0;

    virtual void SetChorusRate( float rate ) = 0;
    virtual float GetChorusRate( void ) = 0;
};

//=============================================================================
// PS3 Audio System Information
//=============================================================================
struct RadSoundPs3SystemInfo
{
    unsigned int totalVoices;
    unsigned int activeVoices;
    unsigned int totalStreams;
    unsigned int activeStreams;
    unsigned int audioMemoryTotal;
    unsigned int audioMemoryUsed;
    unsigned int spuThreadUsage;
};

void radSoundPs3GetSystemInfo( RadSoundPs3SystemInfo* pInfo );

//=============================================================================
// PS3 Audio Initialization Options
//=============================================================================
struct RadSoundPs3InitOptions
{
    RadSoundPs3InitOptions()
        : maxVoices( 32 )
        , maxStreams( 8 )
        , audioBufferSize( 256 * 1024 )
        , useSpuThreads( true )
        , numSpuThreads( 1 )
    {
    }

    unsigned int maxVoices;
    unsigned int maxStreams;
    unsigned int audioBufferSize;
    bool useSpuThreads;
    unsigned int numSpuThreads;
};

void radSoundPs3SetInitOptions( const RadSoundPs3InitOptions* pOptions );

#endif // RADSOUND_PS3_HPP
