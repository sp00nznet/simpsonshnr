//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        usercontrollerPS3.cpp
//
// Description: Implementation of PS3-specific user controller class
//
// History:     Based on PS2 user controller implementation
//
//=============================================================================

#ifdef RAD_PS3

//========================================
// System Includes
//========================================
#include <cell/pad/libpad.h>
#include <radcontroller.hpp>

//========================================
// Project Includes
//========================================
#include <input/usercontrollerPS3.h>

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// UserControllerPS3::UserControllerPS3
//==============================================================================
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//==============================================================================
UserControllerPS3::UserControllerPS3() :
    m_isSixAxis( false ),
    m_isDualShock3( false ),
    m_leftMotor( 0 ),
    m_rightMotor( 0 )
{
}

//==============================================================================
// UserControllerPS3::~UserControllerPS3
//==============================================================================
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//==============================================================================
UserControllerPS3::~UserControllerPS3()
{
    StopVibration();
}

//==============================================================================
// UserControllerPS3::Create
//==============================================================================
// Description: Initial setup of controller
//
// Parameters:  id - controller index
//
// Return:      None.
//
//==============================================================================
void UserControllerPS3::Create( int id )
{
    UserController::Create( id );

    // Check what type of controller is connected
    CellPadCapabilityInfo capInfo;
    if( cellPadGetCapabilityInfo( id, &capInfo ) == CELL_PAD_OK )
    {
        // Check for SIXAXIS motion sensing capability
        m_isSixAxis = ( capInfo.info[CELL_PAD_CAPABILITY_INFO_SENSOR_MODE] != 0 );

        // Check for DualShock 3 vibration capability
        m_isDualShock3 = ( capInfo.info[CELL_PAD_CAPABILITY_INFO_MOTOR] != 0 );
    }
}

//==============================================================================
// UserControllerPS3::Initialize
//==============================================================================
// Description: Set up controller mapping
//
// Parameters:  pIController2 - radcontroller interface
//
// Return:      None.
//
//==============================================================================
void UserControllerPS3::Initialize( IRadController* pIController2 )
{
    UserController::Initialize( pIController2 );

    // Enable sensor mode if available
    if( m_isSixAxis )
    {
        cellPadSetSensorMode( m_controllerId, CELL_PAD_SENSOR_MODE_ON );
    }

    // Enable vibration if available
    if( m_isDualShock3 )
    {
        cellPadSetActDirect( m_controllerId, NULL );
    }
}

//==============================================================================
// UserControllerPS3::ReleaseRadController
//==============================================================================
// Description: Release the controller
//
// Parameters:  None.
//
// Return:      None.
//
//==============================================================================
void UserControllerPS3::ReleaseRadController()
{
    StopVibration();
    UserController::ReleaseRadController();
}

//==============================================================================
// UserControllerPS3::SetVibration
//==============================================================================
// Description: Set vibration motor values
//
// Parameters:  leftMotor - left motor intensity (0-255)
//              rightMotor - right motor intensity (0-255)
//
// Return:      None.
//
//==============================================================================
void UserControllerPS3::SetVibration( unsigned short leftMotor, unsigned short rightMotor )
{
    if( !m_isDualShock3 || !m_bConnected )
    {
        return;
    }

    m_leftMotor = leftMotor;
    m_rightMotor = rightMotor;

    // Set up actuator data for DualShock 3
    // Byte 0: Duration for small motor (0=disable)
    // Byte 1: Power for small motor (0-255)
    // Byte 2: Duration for large motor (0=disable)
    // Byte 3: Power for large motor (0-255)
    CellPadActParam actParam;
    actParam.motor[0] = ( leftMotor > 0 ) ? 1 : 0;    // Small motor duration
    actParam.motor[1] = leftMotor;                      // Small motor power
    actParam.motor[2] = ( rightMotor > 0 ) ? 1 : 0;   // Large motor duration
    actParam.motor[3] = rightMotor;                     // Large motor power
    actParam.motor[4] = 0;
    actParam.motor[5] = 0;

    cellPadSetActDirect( m_controllerId, &actParam );
}

//==============================================================================
// UserControllerPS3::StopVibration
//==============================================================================
// Description: Stop all vibration
//
// Parameters:  None.
//
// Return:      None.
//
//==============================================================================
void UserControllerPS3::StopVibration()
{
    if( !m_isDualShock3 )
    {
        return;
    }

    m_leftMotor = 0;
    m_rightMotor = 0;

    CellPadActParam actParam;
    actParam.motor[0] = 0;
    actParam.motor[1] = 0;
    actParam.motor[2] = 0;
    actParam.motor[3] = 0;
    actParam.motor[4] = 0;
    actParam.motor[5] = 0;

    cellPadSetActDirect( m_controllerId, &actParam );
}

//==============================================================================
// UserControllerPS3::GetAccelerometer
//==============================================================================
// Description: Get SIXAXIS accelerometer data
//
// Parameters:  x, y, z - output accelerometer values (-1.0 to 1.0)
//
// Return:      None.
//
//==============================================================================
void UserControllerPS3::GetAccelerometer( float& x, float& y, float& z )
{
    x = y = z = 0.0f;

    if( !m_isSixAxis || !m_bConnected )
    {
        return;
    }

    CellPadData padData;
    if( cellPadGetData( m_controllerId, &padData ) == CELL_PAD_OK )
    {
        if( padData.len >= CELL_PAD_LEN_CHANGE_SENSOR_ON )
        {
            // Accelerometer data is in button array indices 20-22
            // Values range from 0-1023, with 512 being neutral
            x = ( padData.button[CELL_PAD_BTN_OFFSET_SENSOR_X] - 512 ) / 512.0f;
            y = ( padData.button[CELL_PAD_BTN_OFFSET_SENSOR_Y] - 512 ) / 512.0f;
            z = ( padData.button[CELL_PAD_BTN_OFFSET_SENSOR_Z] - 512 ) / 512.0f;
        }
    }
}

//==============================================================================
// UserControllerPS3::GetGyroscope
//==============================================================================
// Description: Get SIXAXIS gyroscope data
//
// Parameters:  pitch, yaw, roll - output gyroscope values
//
// Return:      None.
//
//==============================================================================
void UserControllerPS3::GetGyroscope( float& pitch, float& yaw, float& roll )
{
    pitch = yaw = roll = 0.0f;

    if( !m_isSixAxis || !m_bConnected )
    {
        return;
    }

    CellPadData padData;
    if( cellPadGetData( m_controllerId, &padData ) == CELL_PAD_OK )
    {
        if( padData.len >= CELL_PAD_LEN_CHANGE_SENSOR_ON )
        {
            // Gyroscope data is in button array index 23
            // Note: SIXAXIS only has 1-axis gyro (yaw/roll)
            float gyroValue = ( padData.button[CELL_PAD_BTN_OFFSET_SENSOR_G] - 512 ) / 512.0f;
            yaw = gyroValue;
        }
    }
}

#endif // RAD_PS3
