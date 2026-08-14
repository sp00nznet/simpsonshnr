//=============================================================================
// Copyright (c) 2002 Radical Games Ltd.  All rights reserved.
// PS3 stub implementation - No audio output
//=============================================================================

#include "system.hpp"
#include "listener.hpp"
#include "../common/banner.hpp"
#include "../common/memoryregion.hpp"
#include "../../common/radsoundupdatableobject.hpp"
#include <radtime.hpp>
#include <radmemory.hpp>

//================================================================================
// Static Members
//================================================================================

radSoundHalSystem * radSoundHalSystem::s_pRsdSystem = NULL;

//============================================================================
// radSoundHalSystem::radSoundHalSystem
//============================================================================

radSoundHalSystem::radSoundHalSystem( radMemoryAllocator allocator )
    :
    radSoundObject( ),
    m_NumAuxSends( 0 ),
    m_OutputMode( radSoundOutputMode_Stereo ),
    m_LastServiceTime( ::radTimeGetMilliseconds( ) )
{
    s_pRsdSystem = this;
    m_Gain[0] = 1.0f;
    m_Gain[1] = 1.0f;
    ::radSoundPrintBanner( );
}

//============================================================================
// radSoundHalSystem::~radSoundHalSystem
//============================================================================

radSoundHalSystem::~radSoundHalSystem( void )
{
    radSoundHalListenerPs3::Terminate( );
    radSoundHalMemoryRegion::Terminate( );
    s_pRsdSystem = NULL;
}

//============================================================================
// radSoundHalSystem::Initialize
//============================================================================

void radSoundHalSystem::Initialize( const SystemDescription & systemDescription )
{
    m_NumAuxSends = systemDescription.m_NumAuxSends;

    // PS3 stub - allocate minimal memory region
    // Just use a small stub region since we're not actually playing audio
    const unsigned int stubMemorySize = 64 * 1024; // 64KB stub
    void* pStubMemory = ::radMemoryAllocAligned(
        GetThisAllocator( ),
        stubMemorySize,
        radSoundHalDataSourceReadAlignmentGet( ) );

    radSoundHalMemoryRegion::Initialize(
        pStubMemory,
        stubMemorySize,
        systemDescription.m_MaxRootAllocations,
        radSoundHalDataSourceReadAlignmentGet( ),
        radMemorySpace_Local,
        GetThisAllocator( ) );

    radSoundHalListenerPs3::Initialize( GetThisAllocator( ) );
}

//============================================================================
// radSoundHalSystem::GetRootMemoryRegion
//============================================================================

IRadSoundHalMemoryRegion * radSoundHalSystem::GetRootMemoryRegion( void )
{
    return radSoundHalMemoryRegion::GetRootRegion( );
}

//============================================================================
// radSoundHalSystem::GetNumAuxSends
//============================================================================

unsigned int radSoundHalSystem::GetNumAuxSends( )
{
    return m_NumAuxSends;
}

//============================================================================
// radSoundHalSystem::SetOutputMode
//============================================================================

void radSoundHalSystem::SetOutputMode( radSoundOutputMode mode )
{
    m_OutputMode = mode;
}

//============================================================================
// radSoundHalSystem::GetOutputMode
//============================================================================

radSoundOutputMode radSoundHalSystem::GetOutputMode( void )
{
    return m_OutputMode;
}

//============================================================================
// radSoundHalSystem::Service
//============================================================================

void radSoundHalSystem::Service( void )
{
    unsigned int now = ::radTimeGetMilliseconds( );
    radSoundUpdatableObject::UpdateAll( now - m_LastServiceTime );
    m_LastServiceTime = now;
}

//============================================================================
// radSoundHalSystem::ServiceOncePerFrame
//============================================================================

void radSoundHalSystem::ServiceOncePerFrame( void )
{
    // PS3 stub - no positional audio updates
}

//============================================================================
// radSoundHalSystem::GetStats
//============================================================================

void radSoundHalSystem::GetStats( IRadSoundHalSystem::Stats * pStats )
{
    rAssert( pStats );
    pStats->m_NumBuffers = 0;
    pStats->m_NumVoices = 0;
    pStats->m_NumVoicesPlaying = 0;
    pStats->m_NumPosVoices = 0;
    pStats->m_NumPosVoicesPlaying = 0;
    pStats->m_BufferMemoryUsed = 0;
    pStats->m_EffectsMemoryUsed = 0;
    pStats->m_TotalFreeSoundMemory = 0;
}

//============================================================================
// radSoundHalSystem::SetAuxEffect
//============================================================================

void radSoundHalSystem::SetAuxEffect( unsigned int auxNumber, IRadSoundHalEffect * pIRadSoundHalEffect )
{
    // PS3 stub - no effects
}

//============================================================================
// radSoundHalSystem::GetAuxEffect
//============================================================================

IRadSoundHalEffect * radSoundHalSystem::GetAuxEffect( unsigned int auxNumber )
{
    return NULL;
}

//============================================================================
// radSoundHalSystem::SetAuxGain
//============================================================================

void radSoundHalSystem::SetAuxGain( unsigned int aux, float gain )
{
    if( aux < 2 )
    {
        m_Gain[ aux ] = gain;
    }
}

//============================================================================
// radSoundHalSystem::GetAuxGain
//============================================================================

float radSoundHalSystem::GetAuxGain( unsigned int aux )
{
    if( aux < 2 )
    {
        return m_Gain[ aux ];
    }
    return 1.0f;
}

//================================================================================
// ::radSoundHalSystemGet
//================================================================================

IRadSoundHalSystem * radSoundHalSystemGet( void )
{
    rAssert( radSoundHalSystem::s_pRsdSystem != NULL );
    return radSoundHalSystem::s_pRsdSystem;
}

//================================================================================
// ::radSoundHalSystemInitialize
//================================================================================

void radSoundHalSystemInitialize( radMemoryAllocator allocator )
{
    rAssert( radSoundHalSystem::s_pRsdSystem == NULL );
    new( "radSoundHalSystem", allocator ) radSoundHalSystem( allocator );
    radSoundHalSystem::s_pRsdSystem->AddRef( );
}

//================================================================================
// ::radSoundHalSystemTerminate
//================================================================================

void radSoundHalSystemTerminate( void )
{
    rAssert( radSoundHalSystem::s_pRsdSystem != NULL );
    radSoundHalSystem::s_pRsdSystem->Release( );
}
