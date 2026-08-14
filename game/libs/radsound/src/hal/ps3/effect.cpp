//=============================================================================
// Copyright (c) 2002 Radical Games Ltd.  All rights reserved.
// PS3 stub implementation - effects
//=============================================================================

#include <radsound_ps3.hpp>
#include "../../common/radsoundobject.hpp"

//=============================================================================
// PS3 Effect Stub Implementation
//=============================================================================

struct radSoundEffectPs3Stub
    :
    public IRadSoundEffectPs3,
    public radSoundObject
{
    IMPLEMENT_REFCOUNTED( "radSoundEffectPs3Stub" )

    radSoundEffectPs3Stub( radMemoryAllocator allocator )
        :
        radSoundObject( ),
        m_Enabled( false ),
        m_Gain( 1.0f ),
        m_MasterGain( 1.0f ),
        m_Mode( Off ),
        m_Delay( 0.0f ),
        m_Feedback( 0.0f ),
        m_ReverbTime( 1.0f ),
        m_Damping( 0.5f ),
        m_PreDelay( 0.0f ),
        m_ChorusDepth( 0.5f ),
        m_ChorusRate( 1.0f ),
        m_AttachedAux( 0xFFFFFFFF )
    {
    }

    virtual ~radSoundEffectPs3Stub( void ) {}

    // IRadSoundHalEffect
    virtual void Attach( unsigned int auxSend ) { m_AttachedAux = auxSend; }
    virtual void Detach( void ) { m_AttachedAux = 0xFFFFFFFF; }
    virtual void Update( void ) {}
    virtual void SetMasterGain( float masterGain ) { m_MasterGain = masterGain; }
    virtual float GetMasterGain( void ) { return m_MasterGain; }

    // IRadSoundEffectPs3
    virtual void SetEnabled( bool enabled ) { m_Enabled = enabled; }
    virtual bool GetEnabled( void ) { return m_Enabled; }

    virtual void SetGain( float gain ) { m_Gain = gain; }
    virtual float GetGain( void ) { return m_Gain; }

    virtual void SetMode( IRadSoundEffectPs3::Mode mode ) { m_Mode = mode; }
    virtual IRadSoundEffectPs3::Mode GetMode( void ) { return m_Mode; }

    virtual void SetDelay( float delayTime ) { m_Delay = delayTime; }
    virtual float GetDelayTime( void ) { return m_Delay; }

    virtual void SetFeedback( float feedBack ) { m_Feedback = feedBack; }
    virtual float GetFeedback( void ) { return m_Feedback; }

    virtual void SetReverbTime( float time ) { m_ReverbTime = time; }
    virtual float GetReverbTime( void ) { return m_ReverbTime; }

    virtual void SetDamping( float damping ) { m_Damping = damping; }
    virtual float GetDamping( void ) { return m_Damping; }

    virtual void SetPreDelay( float preDelay ) { m_PreDelay = preDelay; }
    virtual float GetPreDelay( void ) { return m_PreDelay; }

    virtual void SetChorusDepth( float depth ) { m_ChorusDepth = depth; }
    virtual float GetChorusDepth( void ) { return m_ChorusDepth; }

    virtual void SetChorusRate( float rate ) { m_ChorusRate = rate; }
    virtual float GetChorusRate( void ) { return m_ChorusRate; }

private:
    bool m_Enabled;
    float m_Gain;
    float m_MasterGain;
    Mode m_Mode;
    float m_Delay;
    float m_Feedback;
    float m_ReverbTime;
    float m_Damping;
    float m_PreDelay;
    float m_ChorusDepth;
    float m_ChorusRate;
    unsigned int m_AttachedAux;
};

//=============================================================================
// Factory function
//=============================================================================

IRadSoundEffectPs3 * radSoundCreateEffectPs3( radMemoryAllocator allocator )
{
    return new( "radSoundEffectPs3Stub", allocator ) radSoundEffectPs3Stub( allocator );
}

//=============================================================================
// PS3 System Info stubs
//=============================================================================

static RadSoundPs3InitOptions s_Ps3InitOptions;

void radSoundPs3GetSystemInfo( RadSoundPs3SystemInfo* pInfo )
{
    if( pInfo != NULL )
    {
        pInfo->totalVoices = s_Ps3InitOptions.maxVoices;
        pInfo->activeVoices = 0;
        pInfo->totalStreams = s_Ps3InitOptions.maxStreams;
        pInfo->activeStreams = 0;
        pInfo->audioMemoryTotal = s_Ps3InitOptions.audioBufferSize;
        pInfo->audioMemoryUsed = 0;
        pInfo->spuThreadUsage = 0;
    }
}

void radSoundPs3SetInitOptions( const RadSoundPs3InitOptions* pOptions )
{
    if( pOptions != NULL )
    {
        s_Ps3InitOptions = *pOptions;
    }
}
