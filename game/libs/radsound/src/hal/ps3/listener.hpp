//=============================================================================
// Copyright (c) 2002 Radical Games Ltd.  All rights reserved.
// PS3 stub implementation
//=============================================================================

#ifndef RADSOUND_HAL_LISTENER_PS3_HPP
#define RADSOUND_HAL_LISTENER_PS3_HPP

#include <radsound_hal.hpp>
#include "../../common/radsoundobject.hpp"

struct radSoundHalListenerPs3
    :
    public IRadSoundHalListener,
    public radSoundObject
{
    IMPLEMENT_REFCOUNTED( "radSoundHalListenerPs3" )

    static void Initialize( radMemoryAllocator allocator );
    static void Terminate( void );
    static radSoundHalListenerPs3 * GetInstance( void );

    radSoundHalListenerPs3( radMemoryAllocator allocator );
    virtual ~radSoundHalListenerPs3( void );

    // IRadSoundHalListener
    virtual void SetPosition( radSoundVector * pPosition );
    virtual void GetPosition( radSoundVector * pPosition );
    virtual void SetVelocity( radSoundVector * pVelocity );
    virtual void GetVelocity( radSoundVector * pVelocity );
    virtual void SetOrientation( radSoundVector * pFront, radSoundVector * pTop );
    virtual void GetOrientation( radSoundVector * pFront, radSoundVector * pTop );

    virtual void SetDistanceFactor( float factor );
    virtual float GetDistanceFactor( void );
    virtual void SetDopplerFactor( float factor );
    virtual float GetDopplerFactor( void );
    virtual void SetRollOffFactor( float factor );
    virtual float GetRollOffFactor( void );

    virtual void SetEnvEffectsEnabled( bool enabled );
    virtual bool GetEnvEffectsEnabled( void );
    virtual void SetEnvironmentAuxSend( unsigned int auxsend );
    virtual unsigned int GetEnvironmentAuxSend( void );

private:
    radSoundVector m_Position;
    radSoundVector m_Velocity;
    radSoundVector m_OrientFront;
    radSoundVector m_OrientTop;
    float m_DistanceFactor;
    float m_DopplerFactor;
    float m_RollOffFactor;
    bool m_EnvEffectsEnabled;
    unsigned int m_EnvironmentAuxSend;

    static radSoundHalListenerPs3 * s_pTheListener;
};

#endif // RADSOUND_HAL_LISTENER_PS3_HPP
