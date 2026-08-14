//=============================================================================
// Copyright (c) 2002 Radical Games Ltd.  All rights reserved.
// PS3 stub implementation
//=============================================================================

#include "listener.hpp"

//============================================================================
// Static Members
//============================================================================

radSoundHalListenerPs3 * radSoundHalListenerPs3::s_pTheListener = NULL;

//============================================================================
// radSoundHalListenerPs3::Initialize
//============================================================================

void radSoundHalListenerPs3::Initialize( radMemoryAllocator allocator )
{
    rAssert( s_pTheListener == NULL );
    s_pTheListener = new( "radSoundHalListenerPs3", allocator ) radSoundHalListenerPs3( allocator );
    s_pTheListener->AddRef();
}

//============================================================================
// radSoundHalListenerPs3::Terminate
//============================================================================

void radSoundHalListenerPs3::Terminate( void )
{
    if( s_pTheListener != NULL )
    {
        s_pTheListener->Release();
        s_pTheListener = NULL;
    }
}

//============================================================================
// radSoundHalListenerPs3::GetInstance
//============================================================================

radSoundHalListenerPs3 * radSoundHalListenerPs3::GetInstance( void )
{
    return s_pTheListener;
}

//============================================================================
// radSoundHalListenerPs3::radSoundHalListenerPs3
//============================================================================

radSoundHalListenerPs3::radSoundHalListenerPs3( radMemoryAllocator allocator )
    :
    radSoundObject( ),
    m_DistanceFactor( 1.0f ),
    m_DopplerFactor( 1.0f ),
    m_RollOffFactor( 1.0f ),
    m_EnvEffectsEnabled( false ),
    m_EnvironmentAuxSend( 0 )
{
    m_Position.m_x = 0.0f;
    m_Position.m_y = 0.0f;
    m_Position.m_z = 0.0f;
    m_Velocity.m_x = 0.0f;
    m_Velocity.m_y = 0.0f;
    m_Velocity.m_z = 0.0f;
    m_OrientFront.m_x = 0.0f;
    m_OrientFront.m_y = 0.0f;
    m_OrientFront.m_z = 1.0f;
    m_OrientTop.m_x = 0.0f;
    m_OrientTop.m_y = 1.0f;
    m_OrientTop.m_z = 0.0f;
}

//============================================================================
// radSoundHalListenerPs3::~radSoundHalListenerPs3
//============================================================================

radSoundHalListenerPs3::~radSoundHalListenerPs3( void )
{
}

//============================================================================
// radSoundHalListenerPs3::SetPosition
//============================================================================

void radSoundHalListenerPs3::SetPosition( radSoundVector * pPosition )
{
    m_Position = *pPosition;
}

//============================================================================
// radSoundHalListenerPs3::GetPosition
//============================================================================

void radSoundHalListenerPs3::GetPosition( radSoundVector * pPosition )
{
    *pPosition = m_Position;
}

//============================================================================
// radSoundHalListenerPs3::SetVelocity
//============================================================================

void radSoundHalListenerPs3::SetVelocity( radSoundVector * pVelocity )
{
    m_Velocity = *pVelocity;
}

//============================================================================
// radSoundHalListenerPs3::GetVelocity
//============================================================================

void radSoundHalListenerPs3::GetVelocity( radSoundVector * pVelocity )
{
    *pVelocity = m_Velocity;
}

//============================================================================
// radSoundHalListenerPs3::SetOrientation
//============================================================================

void radSoundHalListenerPs3::SetOrientation( radSoundVector * pFront, radSoundVector * pTop )
{
    m_OrientFront = *pFront;
    m_OrientTop = *pTop;
}

//============================================================================
// radSoundHalListenerPs3::GetOrientation
//============================================================================

void radSoundHalListenerPs3::GetOrientation( radSoundVector * pFront, radSoundVector * pTop )
{
    *pFront = m_OrientFront;
    *pTop = m_OrientTop;
}

//============================================================================
// radSoundHalListenerPs3::SetDistanceFactor
//============================================================================

void radSoundHalListenerPs3::SetDistanceFactor( float factor )
{
    m_DistanceFactor = factor;
}

//============================================================================
// radSoundHalListenerPs3::GetDistanceFactor
//============================================================================

float radSoundHalListenerPs3::GetDistanceFactor( void )
{
    return m_DistanceFactor;
}

//============================================================================
// radSoundHalListenerPs3::SetDopplerFactor
//============================================================================

void radSoundHalListenerPs3::SetDopplerFactor( float factor )
{
    m_DopplerFactor = factor;
}

//============================================================================
// radSoundHalListenerPs3::GetDopplerFactor
//============================================================================

float radSoundHalListenerPs3::GetDopplerFactor( void )
{
    return m_DopplerFactor;
}

//============================================================================
// radSoundHalListenerPs3::SetRollOffFactor
//============================================================================

void radSoundHalListenerPs3::SetRollOffFactor( float factor )
{
    m_RollOffFactor = factor;
}

//============================================================================
// radSoundHalListenerPs3::GetRollOffFactor
//============================================================================

float radSoundHalListenerPs3::GetRollOffFactor( void )
{
    return m_RollOffFactor;
}

//============================================================================
// radSoundHalListenerPs3::SetEnvEffectsEnabled
//============================================================================

void radSoundHalListenerPs3::SetEnvEffectsEnabled( bool enabled )
{
    m_EnvEffectsEnabled = enabled;
}

//============================================================================
// radSoundHalListenerPs3::GetEnvEffectsEnabled
//============================================================================

bool radSoundHalListenerPs3::GetEnvEffectsEnabled( void )
{
    return m_EnvEffectsEnabled;
}

//============================================================================
// radSoundHalListenerPs3::SetEnvironmentAuxSend
//============================================================================

void radSoundHalListenerPs3::SetEnvironmentAuxSend( unsigned int auxsend )
{
    m_EnvironmentAuxSend = auxsend;
}

//============================================================================
// radSoundHalListenerPs3::GetEnvironmentAuxSend
//============================================================================

unsigned int radSoundHalListenerPs3::GetEnvironmentAuxSend( void )
{
    return m_EnvironmentAuxSend;
}

//============================================================================
// Global functions
//============================================================================

IRadSoundHalListener * radSoundHalListenerGet( void )
{
    return radSoundHalListenerPs3::GetInstance();
}
