//===========================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   PS3Platform
//
// Description: Abstracts the differences for setting up and shutting down
//              the different platforms - PS3 implementation.
//
// History:     + Based on PS2 platform implementation
//              + Ported to PS3 using PSGL/GCM
//
//===========================================================================

//========================================
// System Includes
//========================================
#ifdef RAD_PS3

// PS3 System includes
#include <cell/sysmodule.h>
#include <sys/process.h>
#include <sys/memory.h>
#include <sys/ppu_thread.h>
#include <sys/timer.h>
#include <cell/pad/libpad.h>
#include <cell/cell_fs.h>

// PSGL (OpenGL-like API for PS3)
#include <PSGL/psgl.h>
#include <PSGL/psglu.h>

// Standard Lib
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Pure 3D
#include <p3d/anim/compositedrawable.hpp>
#include <p3d/anim/expression.hpp>
#include <p3d/anim/multicontroller.hpp>
#include <p3d/anim/polyskin.hpp>
#include <p3d/anim/sequencer.hpp>
#include <p3d/anim/skeleton.hpp>
#include <p3d/camera.hpp>
#include <p3d/gameattr.hpp>
#include <p3d/image.hpp>
#include <p3d/imagefont.hpp>
#include <p3d/light.hpp>
#include <p3d/locator.hpp>
#include <p3d/platform.hpp>
#include <p3d/scenegraph/scenegraph.hpp>
#include <p3d/sprite.hpp>
#include <p3d/utility.hpp>
#include <p3d/texture.hpp>
#include <p3d/file.hpp>
#include <p3d/shader.hpp>
#include <p3d/matrixstack.hpp>
#include <p3d/memory.hpp>
#include <p3d/bmp.hpp>
#include <p3d/png.hpp>
#include <p3d/targa.hpp>
#include <p3d/font.hpp>
#include <p3d/texturefont.hpp>
#include <p3d/unicode.hpp>
#include <pddi/pddiext.hpp>

// Pure 3D: Loader-specific
#include <render/Loaders/GeometryWrappedLoader.h>
#include <render/Loaders/StaticEntityLoader.h>
#include <render/Loaders/StaticPhysLoader.h>
#include <render/Loaders/TreeDSGLoader.h>
#include <render/Loaders/FenceLoader.h>
#include <render/Loaders/IntersectLoader.h>
#include <render/Loaders/AnimCollLoader.h>
#include <render/Loaders/AnimDSGLoader.h>
#include <render/Loaders/DynaPhysLoader.h>
#include <render/Loaders/InstStatPhysLoader.h>
#include <render/Loaders/InstStatEntityLoader.h>
#include <render/Loaders/WorldSphereLoader.h>
#include <render/Loaders/BillboardWrappedLoader.h>
#include <render/Loaders/instparticlesystemloader.h>
#include <render/Loaders/breakableobjectloader.h>
#include <render/Loaders/lensflareloader.h>
#include <render/Loaders/AnimDynaPhysLoader.h>
#include <p3d/shadow.hpp>
#include <p3d/anim/animatedobject.hpp>
#include <p3d/anim/vertexanimkey.hpp>
#include <p3d/effects/particleloader.hpp>
#include <p3d/effects/opticloader.hpp>

// This is so we can get the name of the file that's failing.
#include <../src/radfile/common/requests.hpp>

// Foundation Tech
#include <raddebug.hpp>
#include <radthread.hpp>
#include <radplatform.hpp>
#include <radtime.hpp>
#include <radmemorymonitor.hpp>
#include <raddebugcommunication.hpp>
#include <raddebugwatch.hpp>
#include <radfile.hpp>
#include <radmovie2.hpp>
#include <radload/radload.hpp>
#include <radtextdisplay.hpp>

// sim - for InstallSimLoaders
#include <simcommon/simutility.hpp>
// StateProps
#include <stateprop/statepropdata.hpp>

// To turn off movies during an error.
#include <presentation/fmvplayer/fmvplayer.h>
#include <presentation/presentation.h>

//========================================
// Project Includes
//========================================
#include <input/inputmanager.h>
#include <main/ps3platform.h>
#include <main/game.h>
#include <main/commandlineoptions.h>
#include <memory/srrmemory.h>
#include <render/RenderManager/RenderManager.h>
#include <render/Loaders/AllWrappers.h>

#include <loading/locatorloader.h>
#include <loading/cameradataloader.h>
#include <loading/roadloader.h>
#include <loading/pathloader.h>
#include <loading/intersectionloader.h>
#include <loading/roaddatasegmentloader.h>
#include <atc/atcloader.h>

#include <debug/debuginfo.h>

#include <main/errorsPS3.h>

#include <sound/soundmanager.h>

#include <presentation/presentation.h>
#include <presentation/gui/guitextbible.h>
#include <data/gamedatamanager.h>

#include <cheats/cheatinputsystem.h>

#include <mission/gameplaymanager.h>
#include <pddi/pddi.hpp>

#define PS3_SECTION "PS3_SECTION"

//******************************************************************************
//
// Global Data, Local Data, Local Classes
//
//******************************************************************************

IRadCementLibrary* PS3Platform::s_MainCement = NULL;

// Static pointer to instance of singleton.
PS3Platform* PS3Platform::spInstance = NULL;

// The Adlib font.  <sigh>
unsigned char gFont[] =
#include <font/defaultfont.h>

//
// This value define the resolution of the rendering area.
// PS3 supports 720p (1280x720) and 1080p (1920x1080)
//
static const int WindowSizeX = 1280;
static const int WindowSizeY = 720;

//
// The depth of the rendering area.
//
static const int WindowBPP = 32;

void LoadMemP3DFile( unsigned char* buffer, unsigned int size, tEntityStore* store )
{
    tFileMem* file = new tFileMem(buffer,size);
    file->AddRef();
    file->SetFilename("memfile.p3d");
    p3d::loadManager->GetP3DHandler()->Load( file, p3d::inventory );
    file->Release();
}

extern bool gIgnoreLastFrameSyncCheck;

//******************************************************************************
//
// Public Member Functions
//
//******************************************************************************

//==============================================================================
// PS3Platform::CreateInstance
//==============================================================================
//
// Description: Creates the PS3Platform.
//
// Parameters:  None.
//
// Return:      Pointer to the PS3Platform.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
PS3Platform* PS3Platform::CreateInstance()
{
MEMTRACK_PUSH_GROUP( "PS3Platform" );
    rAssert( spInstance == NULL );

    spInstance = new(GMA_PERSISTENT) PS3Platform;
    rAssert( spInstance );
MEMTRACK_POP_GROUP("PS3Platform");

    return spInstance;
}

//==============================================================================
// PS3Platform::GetInstance
//==============================================================================
//
// Description: - Access point for the PS3Platform singleton.
//
// Parameters:  None.
//
// Return:      Pointer to the PS3Platform.
//
// Constraints: This is a singleton so only one instance is allowed.
//
//==============================================================================
PS3Platform* PS3Platform::GetInstance()
{
    rAssert( spInstance != NULL );

    return spInstance;
}


//==============================================================================
// PS3Platform::DestroyInstance
//==============================================================================
//
// Description: Destroy the PS3Platform.
//
// Parameters:  None.
//
// Return:      None.
//
//==============================================================================
void PS3Platform::DestroyInstance()
{
    rAssert( spInstance != NULL );

    delete( GMA_PERSISTENT, spInstance );
    spInstance = NULL;
}


//==============================================================================
// PS3Platform::InitializeFoundation
//==============================================================================
// Description: Get FTech ready to go.
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: The FTech systems must be initialized in a particular order.
//              Consult their documentation before changing.
//
//==============================================================================
void PS3Platform::InitializeFoundation()
{
    //
    // Initialize the memory heaps
    //
    PS3Platform::InitializeMemory();

#ifndef FINAL
    //
    // Register an out-of-memory display handler in case something goes bad
    // while allocating the heaps
    //
    ::radMemorySetOutOfMemoryCallback( PrintOutOfMemoryMessage, NULL );
#endif

    //
    // Initialize memory monitor by JamesCo. TM.
    //
    if( CommandLineOptions::Get( CLO_MEMORY_MONITOR) )
    {
        const int KB = 1024;
        ::radMemoryMonitorInitialize( 100 * KB, GMA_DEBUG );
    }

    // Setup the memory heaps
    //
    HeapMgr()->PrepareHeapsStartup ();

    // Seed the heap stack with this
    //
    HeapMgr()->PushHeap (GMA_PERSISTENT);

    //
    // Initialize the platform system for PS3
    // Using Blu-ray disc as the game media
    //
    ::radPlatformInitialize( NULL,
                             IOPMediaHost,
                             GameMediaDVD,  // Will use Blu-ray
                             NULL,
                             GMA_PERSISTENT );

    //
    // Initialize the timer system
    //
    ::radTimeInitialize();

    //
    // Initialize the debug communication system.
    // NOTE: Disabled on PS3 - causes crash in rDbgComTarget::Initialize
    //
#if !defined(FINAL) && !defined(RAD_PS3)
    ::radDbgComTargetInitialize( HostIO,
                                 radDbgComDefaultPort,
                                 NULL,
                                 GMA_DEBUG );
#endif

    //
    // Initialize the Watcher.
    // NOTE: Disabled on PS3 - causes trap in DebugWatchClient constructor
    //
#if defined(DEBUGWATCH) && !defined(RAD_PS3)
    ::radDbgWatchInitialize( "SRR2",
                             16384 * 32,
                             GMA_DEBUG );
#endif // DEBUGWATCH

    //
    // Initialize the file system.
    //
    ::radFileInitialize( 50,
                         32,
                         GMA_PERSISTENT );

    ::radLoadInitialize();

    // On PS3, always use disc drive path for now
    // HOSTDRIVE is not available in the same way as on devkits
#ifdef RAD_PS3
    ::radSetDefaultDrive( "BDVD:" );
#else
    // Set default drive based on command line options
    if( CommandLineOptions::Get( CLO_CD_FILES_ONLY ) )
    {
        ::radSetDefaultDrive( "BDVD:" );
    }
    else
    {
        ::radSetDefaultDrive( "HOSTDRIVE:" );
    }
#endif

    ::radDriveMount( 0, GMA_PERSISTENT );

    //
    // On PS3, synchronously load the art.rcf file if running from disc.
    //
    if( CommandLineOptions::Get( CLO_CD_FILES_ONLY ) )
    {
        radFileRegisterCementLibrarySync( & s_MainCement, "art.rcf" );
    }

    //
    // Initialize the new movie player
    //
    ::radMovieInitialize2( GMA_PERSISTENT );

    //
    // Init math optimizations
    //
    ::radMathInitialize();

    HeapMgr()->PopHeap (GMA_PERSISTENT);
}



//==============================================================================
// PS3Platform::InitializeMemory
//==============================================================================
//
// Description:
//
// Parameters:
//
// Return:
//
//==============================================================================
void PS3Platform::InitializeMemory()
{
    //
    // Only do this once!
    //
    if( gMemorySystemInitialized == true )
    {
        return;
    }

    gMemorySystemInitialized = true;

    //
    // Initialize thread system.
    //
    ::radThreadInitialize();

    //
    // Initialize memory system.
    //
    ::radMemoryInitialize();
}




//==============================================================================
// PS3Platform::InitializePlatform
//==============================================================================
// Description: Get the PS3 ready to go.
//
// Parameters:  None.
//
// Return:      None.
//
//==============================================================================
void PS3Platform::InitializePlatform()
{
    HeapMgr()->PushHeap (GMA_PERSISTENT);

    InitializePure3D();

    // Add anything here that needs to be before the
    // drive is opened.
    DisplaySplashScreen( Error ); // blank screen

    //
    // This is SLOW so do it last.
    //
    InitializeFoundationDrive();

    //
    // Initialize the input system
    //
    GetInputManager()->Init();

    HeapMgr()->PopHeap (GMA_PERSISTENT);
}


//==============================================================================
// PS3Platform::ShutdownPlatform
//==============================================================================
// Description: Shut down the PS3.
//
// Parameters:  None.
//
// Return:      None.
//
//==============================================================================
void PS3Platform::ShutdownPlatform()
{
    ShutdownPure3D();
    ShutdownFoundation();
}

void PS3Platform::ResetMachine()
{
    // On PS3, we can use sys_game_process_exitspawn to restart
    // For now, just exit to XMB
    sys_process_exit(0);
}

//=============================================================================
// PS3Platform::LaunchDashboard
//=============================================================================
// Description: Return to PS3 XMB
//
// Parameters:  ()
//
// Return:      void
//
//=============================================================================
void PS3Platform::LaunchDashboard()
{
    GetLoadingManager()->CancelPendingRequests();

    // Make sure sounds shut down too.
    GetSoundManager()->SetMasterVolume( 0.0f );

    DisplaySplashScreen( FadeToBlack );

    GetPresentationManager()->StopAll();

    // Clean up
    GameDataManager::DestroyInstance();

    p3d::loadManager->CancelAll();

    SoundManager::DestroyInstance();

    ShutdownPlatform();

    // Exit to XMB
    sys_process_exit(0);
}


//=============================================================================
// PS3Platform::DisplaySplashScreen
//=============================================================================
// Description: Display splash screen with overlay text
//
// Parameters:  screenID - which splash screen to show
//              overlayText - text to display
//              fontScale - scale of the font
//              textPosX, textPosY - position of text
//              textColour - color of text
//              fadeFrames - number of frames to fade in
//
// Return:      void
//
//=============================================================================
void PS3Platform::DisplaySplashScreen( SplashScreen screenID,
                                       const char* overlayText,
                                       float fontScale,
                                       float textPosX,
                                       float textPosY,
                                       tColour textColour,
                                       int fadeFrames )
{
    DisplaySplashScreen( NULL, overlayText, fontScale, textPosX, textPosY, textColour, fadeFrames );
}

//=============================================================================
// PS3Platform::DisplaySplashScreen
//=============================================================================
// Description: Display splash screen with texture and overlay text
//
// Parameters:  textureName - name of texture to display
//              overlayText - text to display
//              fontScale - scale of the font
//              textPosX, textPosY - position of text
//              textColour - color of text
//              fadeFrames - number of frames to fade in
//
// Return:      void
//
//=============================================================================
void PS3Platform::DisplaySplashScreen( const char* textureName,
                                       const char* overlayText,
                                       float fontScale,
                                       float textPosX,
                                       float textPosY,
                                       tColour textColour,
                                       int fadeFrames )
{
    p3d::pddi->DrawSync();

    HeapMgr()->PushHeap( GMA_TEMP );
    p3d::inventory->PushSection();
    p3d::inventory->AddSection( PS3_SECTION );
    p3d::inventory->SelectSection( PS3_SECTION );

    P3D_UNICODE unicodeText[256];

    // Save the current Projection mode so I can restore it later
    pddiProjectionMode pm = p3d::pddi->GetProjectionMode();
    p3d::pddi->SetProjectionMode(PDDI_PROJECTION_DEVICE);

    pddiCullMode cm = p3d::pddi->GetCullMode();
    p3d::pddi->SetCullMode(PDDI_CULL_NONE);


    //CREATE THE FONT
    tTextureFont* thisFont = NULL;

    // Convert memory buffer into a texturefont.
    //
    LoadMemP3DFile( gFont, DEFAULTFONT_SIZE, p3d::inventory );

    thisFont = p3d::find<tTextureFont>("adlibn_20");
    rAssert( thisFont );

    thisFont->AddRef();
    tShader* fontShader = thisFont->GetShader();

    p3d::AsciiToUnicode( overlayText, unicodeText, 256 );

    // Make the missing letter into something I can see
    //
    thisFont->SetMissingLetter(p3d::ConvertCharToUnicode('j'));

    int a = 0;

    do
    {
        p3d::pddi->SetColourWrite(true, true, true, true);
        p3d::pddi->SetClearColour( pddiColour(0,0,0) );
        p3d::pddi->BeginFrame();
        p3d::pddi->Clear(PDDI_BUFFER_COLOUR);

        // This is for fading in the font and shaders.
        int bright = 255;
        if (a < fadeFrames) bright = (a * 255) / fadeFrames;
        if ( bright > 255 ) bright = 255;
        tColour c(bright, bright, bright, 255);

        // Display font
        if (overlayText != NULL)
        {
            tColour colour = textColour;
            colour.SetAlpha( bright );

            thisFont->SetColour( colour );

            p3d::pddi->SetProjectionMode(PDDI_PROJECTION_ORTHOGRAPHIC);
            p3d::stack->Push();
            p3d::stack->LoadIdentity();

            p3d::stack->Translate( textPosX, textPosY, 1.5f);
            float scaleSize = 1.0f / 480.0f;
            p3d::stack->Scale(scaleSize * fontScale, scaleSize* fontScale , 1.0f);

            if ( textPosX != 0.0f || textPosY != 0.0f )
            {
                thisFont->DisplayText( unicodeText );
            }
            else
            {
                thisFont->DisplayText( unicodeText, 3 );
            }

            p3d::stack->Pop();
        }

        p3d::pddi->EndFrame();
        p3d::context->SwapBuffers();

        ++a;

    } while (a <= fadeFrames);

    p3d::pddi->SetCullMode(cm);
    p3d::pddi->SetProjectionMode(pm);

    p3d::pddi->DrawSync();

    gIgnoreLastFrameSyncCheck = true;

    // Should do this after a vsync.
    thisFont->Release();

    p3d::inventory->RemoveSectionElements(PS3_SECTION);
    p3d::inventory->DeleteSection(PS3_SECTION);
    p3d::inventory->PopSection();

    gIgnoreLastFrameSyncCheck = false;

    HeapMgr()->PopHeap(GMA_TEMP);
}


//******************************************************************************
//
// Protected Member Functions
//
//******************************************************************************


//==============================================================================
// PS3Platform::InitializeFoundationDrive
//==============================================================================
// Description: Get FTech ready to go.
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: The FTech systems must be initialized in a particular order.
//              Consult their documentation before changing.
//
//==============================================================================
void PS3Platform::InitializeFoundationDrive()
{
    //
    // Get the drive and hold it open for the life of the game.
    // This is a costly operation so we only want to do it once.
    //
    // On PS3, use BDVD: which is the registered drive name in ps3bddrive.hpp
    // This maps to /app_home/ in RPCS3 or /dev_bdvd/ on real hardware
    //
    ::radDriveOpen( &mpIRadDrive,
                    "BDVD:",
                    NormalPriority,
                    GMA_PERSISTENT );

    if (mpIRadDrive != NULL)
    {
        mpIRadDrive->RegisterErrorHandler( this, NULL );
    }

    // Don't assert - let the game continue and fail more gracefully
    // rAssert( mpIRadDrive != NULL );
}


//==============================================================================
// PS3Platform::ShutdownFoundation
//==============================================================================
// Description: Shut down Foundation Tech.
//
// Parameters:  None.
//
// Return:      None.
//
// Constraints: The FTech systems must be terminated in the reverse order that
//              they were initialized in.
//
//==============================================================================
void PS3Platform::ShutdownFoundation()
{
    //
    // Release the main cement file.
    //
    if( s_MainCement != NULL )
    {
        s_MainCement->Release();
        s_MainCement = NULL;
    }

    //
    // Release the drive we've held open since the beginning.
    //
    if( mpIRadDrive != NULL )
    {
        mpIRadDrive->Release();
        mpIRadDrive = NULL;
    }

    //
    // Shutdown the systems in the reverse order.
    //
    ::radDriveUnmount();
    ::radFileTerminate();
#ifdef DEBUGWATCH
    ::radDbgWatchTerminate();
#endif
    if( CommandLineOptions::Get( CLO_MEMORY_MONITOR) )
    {
        ::radMemoryMonitorTerminate();
    }
#ifndef FINAL
    ::radDbgComTargetTerminate();
#endif
    ::radTimeTerminate();
    ::radPlatformTerminate();
    ::radMemoryTerminate();
    ::radThreadTerminate();
    ::radMovieTerminate2();
}


//==============================================================================
// PS3Platform::InitializePure3D
//==============================================================================
// Description: Get Pure3D ready to go.
//
// Parameters:  None.
//
// Return:      None.
//
//==============================================================================
void PS3Platform::InitializePure3D()
{
MEMTRACK_PUSH_GROUP( "PS3Platform" );

    //
    // Initialise Pure3D platform object.
    // This call differs between different platforms.
    //
    mpPlatform = tPlatform::Create();
    rAssert( mpPlatform != NULL );

    //
    // Initialize the Pure3D context object.
    //
    tContextInitData init;

    //
    // These values set the resolution.
    // PS3 supports 720p by default.
    //
    init.xsize = WindowSizeX;

    // PS3 doesn't need PAL/NTSC distinction
    init.pal = false;

    // Enable vsync
    init.lockToVsync = true;

    //
    // Create the context.
    //
    mpContext = mpPlatform->CreateContext( &init );
    rAssert( mpContext != NULL );

    //
    // Assign this context to the platform.
    //
    mpPlatform->SetActiveContext( mpContext );

    p3d::pddi->EnableZBuffer( true );

    //
    // This call installs chunk handlers for all the primary chunk types that
    // Pure3D supports.
    //
    P3DASSERT(p3d::context);
    tP3DFileHandler* p3d = new(GMA_PERSISTENT) tP3DFileHandler;
    p3d::context->GetLoadManager()->AddHandler(p3d, "p3d");
    p3d::context->GetLoadManager()->AddHandler(new(GMA_PERSISTENT) tPNGHandler, "png");

    if( CommandLineOptions::Get( CLO_FE_UNJOINED ) )
    {
        p3d::context->GetLoadManager()->AddHandler(new(GMA_PERSISTENT) tBMPHandler, "bmp");
        p3d::context->GetLoadManager()->AddHandler(new(GMA_PERSISTENT) tTargaHandler, "tga");
    }
    else
    {
        p3d::context->GetLoadManager()->AddHandler(new(GMA_PERSISTENT) tBMPHandler, "p3d");
        p3d::context->GetLoadManager()->AddHandler(new(GMA_PERSISTENT) tPNGHandler, "p3d");
        p3d::context->GetLoadManager()->AddHandler(new(GMA_PERSISTENT) tTargaHandler, "p3d");
    }

    GeometryWrappedLoader* pGWL =
       (GeometryWrappedLoader*)GetAllWrappers()->mpLoader( AllWrappers::msGeometry );
    pGWL->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pGWL );

    StaticEntityLoader* pSEL =
       (StaticEntityLoader*)GetAllWrappers()->mpLoader( AllWrappers::msStaticEntity );
    pSEL->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pSEL );

    StaticPhysLoader* pSPL =
       (StaticPhysLoader*)GetAllWrappers()->mpLoader( AllWrappers::msStaticPhys );
    pSPL->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pSPL );

    TreeDSGLoader* pTDL =
       (TreeDSGLoader*)GetAllWrappers()->mpLoader( AllWrappers::msTreeDSG );
    pTDL->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pTDL );

    FenceLoader* pFL =
       (FenceLoader*)GetAllWrappers()->mpLoader( AllWrappers::msFenceEntity );
    pFL->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pFL );

    IntersectLoader* pIL =
       (IntersectLoader*)GetAllWrappers()->mpLoader( AllWrappers::msIntersectDSG );
    pIL->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pIL );

    AnimCollLoader* pACL =
        (AnimCollLoader*)GetAllWrappers()->mpLoader( AllWrappers::msAnimCollEntity );
    pACL->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pACL );

    AnimDSGLoader* pAnimDSGLoader =
        (AnimDSGLoader*)GetAllWrappers()->mpLoader( AllWrappers::msAnimEntity );
    pAnimDSGLoader->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pAnimDSGLoader );

    DynaPhysLoader* pDPL =
        (DynaPhysLoader*)GetAllWrappers()->mpLoader( AllWrappers::msDynaPhys );
    pDPL->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pDPL );

    InstStatPhysLoader* pISPL =
        (InstStatPhysLoader*)GetAllWrappers()->mpLoader( AllWrappers::msInstStatPhys );
    pISPL->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pISPL );

    InstStatEntityLoader* pISEL =
        (InstStatEntityLoader*)GetAllWrappers()->mpLoader( AllWrappers::msInstStatEntity );
    pISEL->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pISEL );

    LocatorLoader* pLL =
        (LocatorLoader*)GetAllWrappers()->mpLoader( AllWrappers::msLocator);
    pLL->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pLL );

    RoadLoader* pRL =
        (RoadLoader*)GetAllWrappers()->mpLoader( AllWrappers::msRoadSegment);
    pRL->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pRL );

    PathLoader* pPL =
        (PathLoader*)GetAllWrappers()->mpLoader( AllWrappers::msPathSegment);
    pPL->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pPL );

    WorldSphereLoader* pWSL =
        (WorldSphereLoader*)GetAllWrappers()->mpLoader( AllWrappers::msWorldSphere);
    pWSL->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pWSL );

    LensFlareLoader* pLSL =
        (LensFlareLoader*)GetAllWrappers()->mpLoader( AllWrappers::msLensFlare);
    pLSL->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pLSL );

    BillboardWrappedLoader* pBWL =
        (BillboardWrappedLoader*)GetAllWrappers()->mpLoader( AllWrappers::msBillboard);
    pBWL->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pBWL );

    InstParticleSystemLoader* pInstParticleSystemLoader =
        (InstParticleSystemLoader*) GetAllWrappers()->mpLoader( AllWrappers::msInstParticleSystem);
    pInstParticleSystemLoader->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pInstParticleSystemLoader );

    BreakableObjectLoader* pBreakableObjectLoader =
        (BreakableObjectLoader*) GetAllWrappers()->mpLoader( AllWrappers::msBreakableObject);
    pBreakableObjectLoader->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pBreakableObjectLoader );

    AnimDynaPhysLoader* pAnimDynaPhysLoader =
        (AnimDynaPhysLoader*) GetAllWrappers()->mpLoader( AllWrappers::msAnimDynaPhys);
    pAnimDynaPhysLoader->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pAnimDynaPhysLoader );

    AnimDynaPhysWrapperLoader* pAnimWrapperLoader =
        (AnimDynaPhysWrapperLoader*) GetAllWrappers()->mpLoader( AllWrappers::msAnimDynaPhysWrapper);
    pAnimWrapperLoader->SetRegdListener( GetRenderManager(), 0 );
    p3d->AddHandler( pAnimWrapperLoader );

    p3d->AddHandler(new(GMA_PERSISTENT) tTextureLoader);
    p3d->AddHandler( new(GMA_PERSISTENT) tSetLoader );
    p3d->AddHandler(new(GMA_PERSISTENT) tShaderLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tCameraLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tGameAttrLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tLightLoader);

    p3d->AddHandler(new(GMA_PERSISTENT) tLocatorLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tLightGroupLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tImageLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tTextureFontLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tImageFontLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tSpriteLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tSkeletonLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tPolySkinLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tCompositeDrawableLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tAnimationLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tFrameControllerLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tMultiControllerLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tAnimatedObjectFactoryLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tAnimatedObjectLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tParticleSystemFactoryLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tParticleSystemLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tLensFlareGroupLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) sg::Loader);

    p3d->AddHandler(new(GMA_PERSISTENT) tExpressionGroupLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tExpressionMixerLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) tExpressionLoader);

    p3d->AddHandler(new(GMA_PERSISTENT) tVertexAnimKeyLoader);

    // ATCloader
    p3d->AddHandler(new(GMA_PERSISTENT) ATCLoader);

    tSEQFileHandler* sequencerFileHandler = new(GMA_PERSISTENT) tSEQFileHandler;
    p3d::loadManager->AddHandler(sequencerFileHandler, "seq");

    // sim lib
    sim::InstallSimLoaders();

    p3d->AddHandler(new(GMA_PERSISTENT) CameraDataLoader, SRR2::ChunkID::WALKERCAM );
    p3d->AddHandler(new(GMA_PERSISTENT) CameraDataLoader, SRR2::ChunkID::FOLLOWCAM );
    p3d->AddHandler(new(GMA_PERSISTENT) IntersectionLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) RoadDataSegmentLoader);
    p3d->AddHandler(new(GMA_PERSISTENT) CStatePropDataLoader);
MEMTRACK_POP_GROUP("PS3Platform");

    p3d::context->SetClearColour(tColour(0,0,0));
    p3d::pddi->Clear(PDDI_BUFFER_ALL);
    p3d::context->SwapBuffers();
    p3d::pddi->Clear(PDDI_BUFFER_ALL);
}


//==============================================================================
// PS3Platform::ShutdownPure3D
//==============================================================================
// Description: Clean up and shut down Pure3D.
//
// Parameters:  None.
//
// Return:      None.
//
//==============================================================================
void PS3Platform::ShutdownPure3D()
{
    //
    // Clean-up the Pure3D Inventory
    //
    p3d::inventory->RemoveAllElements();
    p3d::inventory->DeleteAllSections();

    //
    // Clean-up the space taken by the Pure 3D context.
    //
    if( mpContext != NULL )
    {
        mpPlatform->DestroyContext( mpContext );
        mpContext = NULL;
    }

    //
    // Clean-up the space taken by the Pure 3D platform.
    //
    if( mpPlatform != NULL )
    {
        tPlatform::Destroy( mpPlatform );
        mpPlatform = NULL;
    }
}

//==============================================================================
// PS3Platform::SetResolution
//==============================================================================
// Description: Set the display resolution
//
// Parameters:  width, height
//
// Return:      None.
//
//==============================================================================
void PS3Platform::SetResolution( int width, int height )
{
    mResolutionWidth = width;
    mResolutionHeight = height;
}

//==============================================================================
// PS3Platform::CheckForStartupButtons
//==============================================================================

bool PS3Platform::CheckForStartupButtons( void )
{
    CellPadData padData;
    bool buttonsPushed = false;

    // Initialize pad library if not already done
    cellPadInit(CELL_PAD_MAX_PORT_NUM);

    for( int port = 0; port < CELL_PAD_MAX_PORT_NUM; port++ )
    {
        CellPadInfo2 padInfo;
        if( cellPadGetInfo2( &padInfo ) == CELL_OK )
        {
            if( padInfo.port_status[port] & CELL_PAD_STATUS_CONNECTED )
            {
                if( cellPadGetData( port, &padData ) == CELL_OK )
                {
                    // Check if X and Triangle are pressed
                    // PS3 controller button mapping
                    if( padData.len > 0 )
                    {
                        // Digital buttons are in button[2] and button[3]
                        // Cross = bit 6 in button[3], Triangle = bit 4 in button[3]
                        if( (padData.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_CROSS) &&
                            (padData.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_TRIANGLE) )
                        {
                            buttonsPushed = true;
                            break;
                        }
                    }
                }
            }
        }
    }

    return buttonsPushed;
}

void PS3Platform::OnControllerError(const char *msg)
{
    bool inFrame = p3d::context->InFrame();

    if ( inFrame ) p3d::context->EndFrame( true );
    DisplaySplashScreen( Error, msg, 0.7f, 0.0f, 0.0f, tColour(255, 255, 255), 0 );
    if ( inFrame ) p3d::context->BeginFrame( );
    mErrorState = CTL_ERROR;
    mPauseForError = true;

    if ( GetPresentationManager()->GetFMVPlayer()->IsPlaying() )
    {
        GetPresentationManager()->GetFMVPlayer()->Pause( );
    }
    else
    {
        GetSoundManager()->StopForMovie();
    }
}

//=============================================================================
// PS3Platform::OnDriveError
//=============================================================================
// Description: Handle disc/drive errors
//
// Parameters:  error - the error that occurred
//              pDriveName - name of the drive
//              pUserData - user data
//
// Return:      bool
//
//=============================================================================
bool PS3Platform::OnDriveError( radFileError error, const char* pDriveName, void* pUserData )
{
    bool inFrame = p3d::context->InFrame();

    const int NUM_RADFILE_ERRORS = 13;
    unsigned int errorIndex = error;

#ifdef PAL
    switch( CGuiTextBible::GetCurrentLanguage() )
    {
        case Scrooby::XL_FRENCH:
        {
            errorIndex += 1 * NUM_RADFILE_ERRORS;
            break;
        }
        case Scrooby::XL_GERMAN:
        {
            errorIndex += 2 * NUM_RADFILE_ERRORS;
            break;
        }
        case Scrooby::XL_SPANISH:
        {
            errorIndex += 3 * NUM_RADFILE_ERRORS;
            break;
        }
        default:
        {
            break;
        }
    }
#endif // PAL

    rAssert( errorIndex < sizeof( ERROR_STRINGS ) / sizeof( ERROR_STRINGS[ 0 ] ) );

    switch ( error )
    {
    case Success:
        {
            if ( mErrorState != NONE )
            {
                if ( inFrame ) p3d::context->EndFrame( true );
                DisplaySplashScreen( FadeToBlack );
                if ( inFrame ) p3d::context->BeginFrame( );
                mErrorState = NONE;
                mPauseForError = false;
            }

            if ( GetPresentationManager()->GetFMVPlayer()->IsPlaying() )
            {
                GetPresentationManager()->GetFMVPlayer()->UnPause( );
            }
            else
            {
                GetSoundManager()->ResumeAfterMovie();
            }
            return true;
            break;
        }
    case FileNotFound:
        {
            if ( CommandLineOptions::Get( CLO_FILE_NOT_FOUND ) )
            {
                rAssert( pUserData != NULL );

                radFileRequest* request = static_cast<radFileRequest*>( pUserData );
                const char* fileName = request->GetFilename();

                // Get rid of the slashes.
                unsigned int i;
                unsigned int lastIndex = 0;
                for ( i = 0; i < strlen( fileName ); ++i )
                {
                    if ( fileName[ i ] == '\\' || fileName[ i ] == '/' )
                    {
                        lastIndex = i;
                    }
                }

                unsigned int adjustedIndex = lastIndex == 0 ? lastIndex : lastIndex + 1;

                char adjustedName[32];
                strncpy( adjustedName, &fileName[adjustedIndex], ( strlen( fileName ) - lastIndex ) );
                adjustedName[ strlen( fileName ) - lastIndex ] = '\0';

                char errorString[256];
                sprintf( errorString, "%s:\n%s", ERROR_STRINGS[errorIndex], adjustedName );
                if ( inFrame ) p3d::context->EndFrame( true );
                DisplaySplashScreen( Error, errorString, 1.0f, 0.0f, 0.0f, tColour(255, 255, 255), 0 );
                if ( inFrame ) p3d::context->BeginFrame( );
                mErrorState = P_ERROR;
                mPauseForError = true;

                if ( GetPresentationManager()->GetFMVPlayer()->IsPlaying() )
                {
                    GetPresentationManager()->GetFMVPlayer()->Pause( );
                }
                else
                {
                    GetSoundManager()->StopForMovie();
                }
                return true;
            }
            else
            {
                // This could be a hack.
                error = WrongMedia;
                // Fall through.
            }
        }
    case ShellOpen:
    case WrongMedia:
    case NoMedia:
    case HardwareFailure:
        {
            // This could be the wrong disc.
            if ( inFrame ) p3d::context->EndFrame( true );
            DisplaySplashScreen( Error, ERROR_STRINGS[errorIndex], 1.0f, 0.0f, 0.0f, tColour(255, 255, 255), 0 );
            if ( inFrame ) p3d::context->BeginFrame( );
            mErrorState = P_ERROR;
            mPauseForError = true;

            if ( GetPresentationManager()->GetFMVPlayer()->IsPlaying() )
            {
                GetPresentationManager()->GetFMVPlayer()->Pause( );
            }
            else
            {
                GetSoundManager()->StopForMovie();
            }
            return true;
        }
    default:
        {
            // Others are not supported.
            rAssert( false );
        }
    }

    return false;
}


//******************************************************************************
//
// Private Member Functions
//
//******************************************************************************

//==============================================================================
// PS3Platform::PS3Platform
//==============================================================================
// Description: Constructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//==============================================================================
PS3Platform::PS3Platform() :
    mpPlatform( NULL ),
    mpContext( NULL ),
    mResolutionWidth( WindowSizeX ),
    mResolutionHeight( WindowSizeY )
{
}


//==============================================================================
// PS3Platform::~PS3Platform
//==============================================================================
// Description: Destructor.
//
// Parameters:  None.
//
// Return:      N/A.
//
//==============================================================================
PS3Platform::~PS3Platform()
{
}

#endif // RAD_PS3
