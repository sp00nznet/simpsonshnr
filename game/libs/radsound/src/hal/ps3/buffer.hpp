//=============================================================================
// Copyright (c) 2002 Radical Games Ltd.  All rights reserved.
// PS3 stub implementation
//=============================================================================

#ifndef RADSOUND_HAL_BUFFER_PS3_HPP
#define RADSOUND_HAL_BUFFER_PS3_HPP

#include <radsound_hal.hpp>
#include "../../common/radsoundobject.hpp"

struct radSoundHalBufferPs3
    :
    public IRadSoundHalBuffer,
    public radSoundObject,
    public radLinkedClass< radSoundHalBufferPs3 >
{
    IMPLEMENT_REFCOUNTED( "radSoundHalBufferPs3" )

    radSoundHalBufferPs3( radMemoryAllocator allocator );
    virtual ~radSoundHalBufferPs3( void );

    // IRadSoundHalBuffer
    virtual void Initialize(
        IRadSoundHalAudioFormat * pIRadSoundHalAudioFormat,
        IRadMemoryObject * pIRadMemoryObject,
        unsigned int sizeInFrames,
        bool looping,
        bool streaming );

    virtual IRadSoundHalAudioFormat * GetFormat( void );
    virtual IRadMemoryObject * GetMemoryObject( void );
    virtual bool IsLooping( void );
    virtual unsigned int GetSizeInFrames( void );

    virtual void LoadAsync(
        IRadSoundHalDataSource * pIRadSoundHalDataSource,
        unsigned int startPositionInFrames,
        unsigned int numberOfFrames,
        IRadSoundHalBufferLoadCallback * pIRadSoundHalBufferLoadCallback );

    virtual void ClearAsync(
        unsigned int startPositionInFrames,
        unsigned int numberOfFrames,
        IRadSoundHalBufferClearCallback * pIRadSoundHalBufferClearCallback );

    virtual unsigned int GetMinTransferSize( IRadSoundHalAudioFormat::SizeType sizeType );
    virtual void CancelAsyncOperations( void );
    virtual void ReSetAudioFormat( IRadSoundHalAudioFormat * pIRadSoundHalAudioFormat );

    unsigned int GetSizeInBytes( void );

private:
    ref< IRadSoundHalAudioFormat > m_xIRadSoundHalAudioFormat;
    ref< IRadMemoryObject > m_xIRadMemoryObject;
    unsigned int m_SizeInFrames;
    bool m_Looping;
    bool m_Streaming;
};

#endif // RADSOUND_HAL_BUFFER_PS3_HPP
