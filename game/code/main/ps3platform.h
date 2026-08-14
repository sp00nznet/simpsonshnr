//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        ps3platform.h
//
// Description: Abstracts the differences for setting up and shutting down
//              the different platforms - PS3 implementation.
//
// History:     + Based on PS2 platform implementation
//              + Ported to PS3 using PSGL/GCM
//
//=============================================================================

#ifndef PS3PLATFORM_H
#define PS3PLATFORM_H

//========================================
// Nested Includes
//========================================
#include <main/platform.h> // base class

//========================================
// Forward References
//========================================
struct IRadCementLibrary;
struct IRadMemoryHeap;
class tPlatform;
class tContext;

//=============================================================================
//
// Synopsis:    Provides abstraction for setting up and closing down the PS3.
//
//=============================================================================
class PS3Platform : public Platform
{
    public:

        // Static Methods for accessing this singleton.
        static PS3Platform* CreateInstance();
        static PS3Platform* GetInstance();
        static void DestroyInstance();

        // Had to workaround our nice clean design cause FTech must be init'ed
        // before anything else is done.
        static void InitializeFoundation();
        static void InitializeMemory();

        // Implement Platform interface.
        virtual void InitializePlatform();
        virtual void ShutdownPlatform();

        virtual void LaunchDashboard();
        virtual void ResetMachine();
        virtual void DisplaySplashScreen( SplashScreen screenID,
                                          const char* overlayText = NULL,
                                          float fontScale = 1.0f,
                                          float textPosX = 0.0f,
                                          float textPosY = 0.0f,
                                          tColour textColour = tColour( 255,255,255 ),
                                          int fadeFrames = 3 );

        virtual void DisplaySplashScreen( const char* textureName,
                                          const char* overlayText = NULL,
                                          float fontScale = 1.0f,
                                          float textPosX = 0.0f,
                                          float textPosY = 0.0f,
                                          tColour textColour = tColour( 255,255,255 ),
                                          int fadeFrames = 3 );

        virtual bool OnDriveError( radFileError error, const char* pDriveName, void* pUserData );
        virtual void OnControllerError(const char *msg);

        // PS3 specific methods
        void SetResolution( int width, int height );
        int GetResolutionWidth() const { return mResolutionWidth; }
        int GetResolutionHeight() const { return mResolutionHeight; }

        bool CheckForStartupButtons( void );

    protected:

        virtual void InitializeFoundationDrive();
        virtual void ShutdownFoundation();

        virtual void InitializePure3D();
        virtual void ShutdownPure3D();

    private:

        // Constructors, Destructors, and Operators
        PS3Platform();
        virtual ~PS3Platform();

        // Unused Constructors, Destructors, and Operators
        PS3Platform( const PS3Platform& aPlatform );
        PS3Platform& operator=( const PS3Platform& aPlatform );

        // Pointer to the one and only instance of this singleton.
        static PS3Platform* spInstance;

        // Pure 3D attributes
        tPlatform* mpPlatform;
        tContext* mpContext;

        // Resolution settings
        int mResolutionWidth;
        int mResolutionHeight;

        static IRadCementLibrary* s_MainCement;
};

#endif // PS3PLATFORM_H
