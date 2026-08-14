//=============================================================================
// Copyright (c) 2002 Radical Games Ltd.  All rights reserved.
// PS3 stub implementation - main entry point
//=============================================================================

#include "system.hpp"
#include "listener.hpp"
#include "buffer.hpp"
#include "voice.hpp"
#include "../common/audioformat.hpp"
#include "../common/softwarepositionalgroup.hpp"
#include "../../common/radsoundobject.hpp"
#include <radsound_hal.hpp>

//============================================================================
// radSoundHalDataSourceReadAlignmentGet
//============================================================================

unsigned int radSoundHalDataSourceReadAlignmentGet( )
{
    // Return a reasonable alignment for PS3
    return 128;
}

//============================================================================
// radSoundHalDataSourceReadMultipleGet
//============================================================================

unsigned int radSoundHalDataSourceReadMultipleGet( )
{
    // Return a reasonable multiple for PS3
    return 1;
}

//============================================================================
// radSoundHalAudioFormatCreate
//============================================================================

IRadSoundHalAudioFormat * radSoundHalAudioFormatCreate( radMemoryAllocator allocator )
{
    return new( "radSoundHalAudioFormat", allocator ) radSoundHalAudioFormat( allocator );
}

//============================================================================
// radSoundHalPositionalGroupCreate
//============================================================================

IRadSoundHalPositionalGroup * radSoundHalPositionalGroupCreate( radMemoryAllocator allocator )
{
    return new( "radSoundSoftwarePositionalGroup", allocator ) radSoundSoftwarePositionalGroup( allocator );
}

//============================================================================
// radSoundHalBufferCalculateMemorySize
//============================================================================

unsigned int radSoundHalBufferCalculateMemorySize(
    IRadSoundHalAudioFormat::SizeType resultType,
    unsigned int requestedSize,
    IRadSoundHalAudioFormat::SizeType requestedSizeType,
    IRadSoundHalAudioFormat * pIRadSoundHalAudioFormat )
{
    // Convert to frames
    unsigned int sizeInFrames = pIRadSoundHalAudioFormat->ConvertSizeType(
        IRadSoundHalAudioFormat::Frames,
        requestedSize,
        requestedSizeType );

    // Round up to alignment
    unsigned int alignment = radSoundHalDataSourceReadAlignmentGet();
    unsigned int frameSize = pIRadSoundHalAudioFormat->GetFrameSizeInBytes();

    if( frameSize > 0 )
    {
        unsigned int alignmentInFrames = alignment / frameSize;
        if( alignmentInFrames == 0 ) alignmentInFrames = 1;
        sizeInFrames = ((sizeInFrames + alignmentInFrames - 1) / alignmentInFrames) * alignmentInFrames;
    }

    // Convert to requested result type
    return pIRadSoundHalAudioFormat->ConvertSizeType(
        resultType,
        sizeInFrames,
        IRadSoundHalAudioFormat::Frames );
}
