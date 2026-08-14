//=============================================================================
// Copyright (c) 2002 Radical Games Ltd.  All rights reserved.
//=============================================================================


#ifndef RADSOUNDWIN_HPP
#define RADSOUNDWIN_HPP

#include <initguid.h>
#include <dsound.h>

// GUID_NULL is not always defined in modern Windows SDKs
#ifndef GUID_NULL
DEFINE_GUID(GUID_NULL, 0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
#endif

//============================================================================
// Helper Functions
//============================================================================

unsigned int  radSoundFloatAngleToULongWin( float angle );
float         radSoundULongAngleToFloatWin( unsigned int angle );
signed long   radSoundVolumeDbToHardwareWin( float volume );
float         radSoundVolumeHardwareToDbWin( signed long hardwareVolume );
unsigned long radSoundPercentageToHardwarePitchWin( float pitch, unsigned int normalFrequency );

#endif // RADSOUNDWIN32_HPP