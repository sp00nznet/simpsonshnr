//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        ps3reverbcontroller.h
//
// Description: Declaration for the PS3ReverbController class, which provides
//              the PS3-specific reverb control using libmixer
//
// History:     Based on PS2 reverb controller implementation
//
//=============================================================================

#ifndef PS3REVERBCONTROLLER_H
#define PS3REVERBCONTROLLER_H

//========================================
// Nested Includes
//========================================
#include <sound/soundfx/reverbcontroller.h>

//========================================
// Forward References
//========================================
struct IRadSoundEffectPs3;

//=============================================================================
//
// Synopsis:    PS3ReverbController
//
//=============================================================================

class PS3ReverbController : public ReverbController
{
    public:
        PS3ReverbController();
        virtual ~PS3ReverbController();

        void SetReverbOn( reverbSettings* settings );
        void SetReverbOff();

    private:
        //Prevent wasteful constructor creation.
        PS3ReverbController( const PS3ReverbController& original );
        PS3ReverbController& operator=( const PS3ReverbController& rhs );

        //
        // Radsound's PS3 reverb interface
        //
        IRadSoundEffectPs3* m_reverbInterface;
};


#endif // PS3REVERBCONTROLLER_H
