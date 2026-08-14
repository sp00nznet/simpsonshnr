//=============================================================================
// Copyright (c) 2002 Radical Games Ltd.  All rights reserved.
// PS3 stub implementation
//=============================================================================

#include "buffer.hpp"

//============================================================================
// radSoundHalBufferPs3::radSoundHalBufferPs3
//============================================================================

radSoundHalBufferPs3::radSoundHalBufferPs3( radMemoryAllocator allocator )
    :
    radSoundObject( ),
    m_xIRadSoundHalAudioFormat( NULL ),
    m_xIRadMemoryObject( NULL ),
    m_SizeInFrames( 0 ),
    m_Looping( false ),
    m_Streaming( false )
{
}

//============================================================================
// radSoundHalBufferPs3::~radSoundHalBufferPs3
//============================================================================

radSoundHalBufferPs3::~radSoundHalBufferPs3( void )
{
}

//============================================================================
// radSoundHalBufferPs3::Initialize
//============================================================================

void radSoundHalBufferPs3::Initialize(
    IRadSoundHalAudioFormat * pIRadSoundHalAudioFormat,
    IRadMemoryObject * pIRadMemoryObject,
    unsigned int sizeInFrames,
    bool looping,
    bool streaming )
{
    m_xIRadSoundHalAudioFormat = pIRadSoundHalAudioFormat;
    m_xIRadMemoryObject = pIRadMemoryObject;
    m_SizeInFrames = sizeInFrames;
    m_Looping = looping;
    m_Streaming = streaming;
}

//============================================================================
// radSoundHalBufferPs3::GetFormat
//============================================================================

IRadSoundHalAudioFormat * radSoundHalBufferPs3::GetFormat( void )
{
    return m_xIRadSoundHalAudioFormat;
}

//============================================================================
// radSoundHalBufferPs3::GetMemoryObject
//============================================================================

IRadMemoryObject * radSoundHalBufferPs3::GetMemoryObject( void )
{
    return m_xIRadMemoryObject;
}

//============================================================================
// radSoundHalBufferPs3::IsLooping
//============================================================================

bool radSoundHalBufferPs3::IsLooping( void )
{
    return m_Looping;
}

//============================================================================
// radSoundHalBufferPs3::GetSizeInFrames
//============================================================================

unsigned int radSoundHalBufferPs3::GetSizeInFrames( void )
{
    return m_SizeInFrames;
}

//============================================================================
// radSoundHalBufferPs3::GetSizeInBytes
//============================================================================

unsigned int radSoundHalBufferPs3::GetSizeInBytes( void )
{
    if( m_xIRadSoundHalAudioFormat != NULL )
    {
        return m_xIRadSoundHalAudioFormat->FramesToBytes( m_SizeInFrames );
    }
    return 0;
}

//============================================================================
// radSoundHalBufferPs3::LoadAsync
//============================================================================

void radSoundHalBufferPs3::LoadAsync(
    IRadSoundHalDataSource * pIRadSoundHalDataSource,
    unsigned int startPositionInFrames,
    unsigned int numberOfFrames,
    IRadSoundHalBufferLoadCallback * pIRadSoundHalBufferLoadCallback )
{
    // PS3 stub - immediately call back with success
    if( pIRadSoundHalBufferLoadCallback != NULL )
    {
        pIRadSoundHalBufferLoadCallback->OnBufferLoadComplete( numberOfFrames );
    }
}

//============================================================================
// radSoundHalBufferPs3::ClearAsync
//============================================================================

void radSoundHalBufferPs3::ClearAsync(
    unsigned int startPositionInFrames,
    unsigned int numberOfFrames,
    IRadSoundHalBufferClearCallback * pIRadSoundHalBufferClearCallback )
{
    // PS3 stub - immediately call back with success
    if( pIRadSoundHalBufferClearCallback != NULL )
    {
        pIRadSoundHalBufferClearCallback->OnBufferClearComplete( );
    }
}

//============================================================================
// radSoundHalBufferPs3::GetMinTransferSize
//============================================================================

unsigned int radSoundHalBufferPs3::GetMinTransferSize( IRadSoundHalAudioFormat::SizeType sizeType )
{
    // Return 1 for minimal transfer granularity
    return 1;
}

//============================================================================
// radSoundHalBufferPs3::CancelAsyncOperations
//============================================================================

void radSoundHalBufferPs3::CancelAsyncOperations( void )
{
    // PS3 stub - nothing to cancel
}

//============================================================================
// radSoundHalBufferPs3::ReSetAudioFormat
//============================================================================

void radSoundHalBufferPs3::ReSetAudioFormat( IRadSoundHalAudioFormat * pIRadSoundHalAudioFormat )
{
    m_xIRadSoundHalAudioFormat = pIRadSoundHalAudioFormat;
}

//============================================================================
// Factory function
//============================================================================

IRadSoundHalBuffer * radSoundHalBufferCreate( radMemoryAllocator allocator )
{
    return new( "radSoundHalBufferPs3", allocator ) radSoundHalBufferPs3( allocator );
}
