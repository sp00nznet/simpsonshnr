//=============================================================================
// Copyright (c) 2002 Radical Games Ltd.  All rights reserved.
// PS3 stub implementation
//=============================================================================

#ifndef SOUNDSYSTEM_PS3_HPP
#define SOUNDSYSTEM_PS3_HPP

#include <radsound_hal.hpp>
#include "../../common/radsoundobject.hpp"

struct radSoundHalSystem
    :
    public IRadSoundHalSystem,
    public radSoundObject
{
    IMPLEMENT_REFCOUNTED( "radSoundHalSystem" )

    radSoundHalSystem( radMemoryAllocator allocator );
    ~radSoundHalSystem( void );

    // IRadSoundHalSystem
    virtual void Initialize( const SystemDescription & systemDescription );
    virtual IRadSoundHalMemoryRegion * GetRootMemoryRegion( void );
    virtual unsigned int GetNumAuxSends( void );
    virtual void SetOutputMode( radSoundOutputMode mode );
    virtual radSoundOutputMode GetOutputMode( void );
    virtual void GetStats( IRadSoundHalSystem::Stats * pStats );
    virtual void SetAuxEffect( unsigned int auxNumber, IRadSoundHalEffect * pIRadSoundHalEffect );
    virtual IRadSoundHalEffect * GetAuxEffect( unsigned int auxNumber );
    virtual void SetAuxGain( unsigned int aux, float gain );
    virtual float GetAuxGain( unsigned int aux );
    virtual void Service( void );
    virtual void ServiceOncePerFrame( void );

    static radSoundHalSystem * GetInstance( void ) { return s_pRsdSystem; }

    unsigned int    m_NumAuxSends;
    float           m_Gain[ 2 ];
    radSoundOutputMode m_OutputMode;
    unsigned int m_LastServiceTime;

    static radSoundHalSystem * s_pRsdSystem;
};

#endif // SOUNDSYSTEM_PS3_HPP
