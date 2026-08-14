//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        usercontrollerPS3.h
//
// Description: PS3-specific user controller class
//
// History:     Based on PS2 user controller implementation
//
//=============================================================================

#ifndef USERCONTROLLERPS3_H
#define USERCONTROLLERPS3_H

#ifdef RAD_PS3

//========================================
// Nested Includes
//========================================
#include <input/usercontroller.h>

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    UserControllerPS3 - PS3-specific controller implementation
//
//=============================================================================

class UserControllerPS3 : public UserController
{
public:
    UserControllerPS3();
    virtual ~UserControllerPS3();

    // Override to set up PS3-specific controller mappings
    virtual void Create( int id );
    virtual void Initialize( IRadController* pIController2 );
    virtual void ReleaseRadController( void );

    // PS3 DualShock 3 / SIXAXIS specific features
    void SetVibration( unsigned short leftMotor, unsigned short rightMotor );
    void StopVibration();

    // SIXAXIS motion sensing
    void GetAccelerometer( float& x, float& y, float& z );
    void GetGyroscope( float& pitch, float& yaw, float& roll );

    // Check for specific PS3 controller types
    bool IsSixAxisController() const { return m_isSixAxis; }
    bool IsDualShock3() const { return m_isDualShock3; }

private:
    // Prevent copying
    UserControllerPS3( const UserControllerPS3& original );
    UserControllerPS3& operator=( const UserControllerPS3& rhs );

    // Controller type flags
    bool m_isSixAxis;
    bool m_isDualShock3;

    // Vibration state
    unsigned short m_leftMotor;
    unsigned short m_rightMotor;
};

#endif // RAD_PS3

#endif // USERCONTROLLERPS3_H
