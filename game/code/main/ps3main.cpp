//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        ps3main.cpp
//
// Description: This file contains the main entry point to the game for PS3.
//
// History:     + Based on PS2 main implementation
//              + Ported to PS3
//
//=============================================================================

#ifdef RAD_PS3

//========================================
// System Includes
//========================================
#include <string.h>

// PS3 System includes
#include <cell/sysmodule.h>
#include <sys/process.h>
#include <sys/ppu_thread.h>
#include <sys/timer.h>
#include <sys/tty.h>
#include <cell/pad/libpad.h>
#include <stdio.h>

// TTY debug output for RPCS3 - uses sys_tty_write (no malloc, safe)
static void DebugPrint(const char* msg)
{
    unsigned int len = 0;
    const char* p = msg;
    while (*p++) len++;
    unsigned int written;
    sys_tty_write(SYS_TTYP_PPU_STDOUT, msg, len, &written);
}

// Foundation Tech
#include <raddebug.hpp>
#include <radobject.hpp>
#include <radthread.hpp>
#include <radtextdisplay.hpp>

// Pure3D
#include <p3d/entity.hpp>

//========================================
// Project Includes
//========================================
#include <main/game.h>
#include <main/ps3platform.h>
#include <main/singletons.h>
#include <main/commandlineoptions.h>
#include <memory/memoryutilities.h>
#include <memory/srrmemory.h>

//========================================
// PS3 System Thread Priority and Stack Size
//========================================
SYS_PROCESS_PARAM(1001, 0x100000)

//========================================
// Forward Declarations
//========================================
static void ProcessCommandLineArguments( int argc, char* argv[] );
static void ProcessCommandLineArgumentsFromFile( );
static void InitializePS3Modules( );
static void TerminatePS3Modules( );

void OutputHandler( const char* pString )
{
    static int ypos = 0;
    static IRadTextDisplay* textDisplay = 0;

    if( !textDisplay || (ypos > 20) )
    {
        if( textDisplay )
        {
            textDisplay->Release();
        }
        ::radTextDisplayGet( &textDisplay, GMA_TEMP );
        textDisplay->SetBackgroundColor( 0 );
        textDisplay->SetTextColor( 0xffffffff );
        textDisplay->Clear();
        ypos = 0;
    }

    if( textDisplay )
    {
        textDisplay->TextOutAt( pString, 0, ypos );
        textDisplay->SwapBuffers();
        ypos++;
    }

    sys_timer_usleep( 200000 ); // 200ms
}


//=============================================================================
// Function:    InitializePS3Modules
//=============================================================================
//
// Description: Load required PS3 system modules.
//
// Parameters:  None.
//
// Returns:     None.
//
//=============================================================================
void InitializePS3Modules()
{
    DebugPrint("=== InitializePS3Modules START ===\n");

    // Load required system modules with error checking
    int ret;

    DebugPrint("Loading FS module...\n");
    ret = cellSysmoduleLoadModule( CELL_SYSMODULE_FS );

    DebugPrint("Loading IO module...\n");
    ret = cellSysmoduleLoadModule( CELL_SYSMODULE_IO );

    DebugPrint("Loading USBD module...\n");
    ret = cellSysmoduleLoadModule( CELL_SYSMODULE_USBD );

    DebugPrint("Loading AUDIO module...\n");
    ret = cellSysmoduleLoadModule( CELL_SYSMODULE_AUDIO );

    DebugPrint("Loading GCM_SYS module...\n");
    ret = cellSysmoduleLoadModule( CELL_SYSMODULE_GCM_SYS );

    DebugPrint("Loading RESC module...\n");
    ret = cellSysmoduleLoadModule( CELL_SYSMODULE_RESC );

    DebugPrint("Loading SYSUTIL module...\n");
    ret = cellSysmoduleLoadModule( CELL_SYSMODULE_SYSUTIL );

    DebugPrint("Loading PNGDEC module...\n");
    ret = cellSysmoduleLoadModule( CELL_SYSMODULE_PNGDEC );

    // Initialize pad
    DebugPrint("Initializing cellPad...\n");
    ret = cellPadInit( CELL_PAD_MAX_PORT_NUM );

    DebugPrint("=== InitializePS3Modules DONE ===\n");
}


//=============================================================================
// Function:    TerminatePS3Modules
//=============================================================================
//
// Description: Unload PS3 system modules.
//
// Parameters:  None.
//
// Returns:     None.
//
//=============================================================================
void TerminatePS3Modules()
{
    // End pad
    cellPadEnd();

    // Unload system modules in reverse order
    cellSysmoduleUnloadModule( CELL_SYSMODULE_PNGDEC );
    cellSysmoduleUnloadModule( CELL_SYSMODULE_SYSUTIL );
    cellSysmoduleUnloadModule( CELL_SYSMODULE_RESC );
    cellSysmoduleUnloadModule( CELL_SYSMODULE_GCM_SYS );
    cellSysmoduleUnloadModule( CELL_SYSMODULE_AUDIO );
    cellSysmoduleUnloadModule( CELL_SYSMODULE_USBD );
    cellSysmoduleUnloadModule( CELL_SYSMODULE_IO );
    cellSysmoduleUnloadModule( CELL_SYSMODULE_FS );
}


//=============================================================================
// Function:    main
//=============================================================================
//
// Description: Main entry point for PS3.
//
// Parameters:  argc, number of command line arguments
//              argv, array of pointers to these tokens.
//
// Returns:     0 success, non zero error.
//
//=============================================================================
int main( int argc, char* argv[] )
{
    // Very first thing - sleep to prove main() is running
    // This will show up in RPCS3 logs as sys_timer activity
    sys_timer_usleep(100000);  // 100ms - should show in log

    // Debug output to trace initialization (using TTY for RPCS3)
    DebugPrint("=== PS3 MAIN STARTING ===\n");

    // Initialize PS3 system modules first
    DebugPrint("Loading PS3 modules...\n");
    InitializePS3Modules();
    DebugPrint("PS3 modules loaded OK\n");

    //
    // All settings get stored in GameDB.
    //
    DebugPrint("CommandLineOptions::InitDefaults...\n");
    CommandLineOptions::InitDefaults();
    DebugPrint("ProcessCommandLineArguments...\n");
    ProcessCommandLineArguments( argc, argv );

    //
    // Have to get FTech setup first so that we can use all the memory services.
    //
    DebugPrint("Initializing Foundation...\n");
    PS3Platform::InitializeFoundation();
    DebugPrint("Foundation initialized OK\n");

    DebugPrint("srand...\n");
    srand( Game::GetRandomSeed() );

#ifndef RAD_RELEASE
    DebugPrint("tName::SetAllocator...\n");
    tName::SetAllocator( GMA_DEBUG );
#endif

    DebugPrint("HeapMgr()->PushHeap...\n");
    HeapMgr()->PushHeap( GMA_PERSISTENT );

    // Process any command line options from the command.txt file
    DebugPrint("ProcessCommandLineArgumentsFromFile...\n");
    ProcessCommandLineArgumentsFromFile();
    DebugPrint("ProcessCommandLineArgumentsFromFile DONE\n");

    //
    // Instantiate all the singletons before doing anything else.
    //
    DebugPrint("=== Creating singletons (may take a while) ===\n");
    CreateSingletons();
    DebugPrint("=== Singletons created OK ===\n");

    //
    // Construct the platform object.
    //
    DebugPrint("Creating platform...\n");
    PS3Platform* pPlatform = PS3Platform::CreateInstance();
    rAssert( pPlatform != NULL );
    DebugPrint("Platform created OK\n");

    //
    // Create the game object.
    //
    DebugPrint("Creating game...\n");
    Game* pGame = Game::CreateInstance( pPlatform );
    rAssert( pGame != NULL );
    DebugPrint("Game created OK\n");

    //
    // Initialize the game.
    //
    DebugPrint("Initializing game...\n");
    pGame->Initialize();
    DebugPrint("Game initialized OK\n");

    HeapMgr()->PopHeap( GMA_PERSISTENT );

    //
    // Run it!  Control will not return from here until the game is stopped.
    //
    DebugPrint("=== ENTERING GAME LOOP ===\n");
    pGame->Run();

    //
    // Terminate the game (this frees all resources allocated by the game).
    //
    pGame->Terminate();

    //
    // Destroy the game object.
    //
    Game::DestroyInstance();

    //
    // Destroy the game and platform (do it in this order in case the game's
    // destructor references the platform.
    //
    PS3Platform::DestroyInstance();

    //
    // Show some debug output
    //
#ifdef RAD_DEBUG
    radObject::DumpObjects();
#endif

    //
    // Dump all the singletons.
    //
    DestroySingletons();

    //
    // Terminate PS3 modules
    //
    TerminatePS3Modules();

    //
    // Pass any error codes back to the operating system.
    //
    return 0;
}


//=============================================================================
// Function:    ProcessCommandLineArguments
//=============================================================================
//
// Description: Pick out the command line options and stuff them in
//              the game database.
//
// Parameters:  argc, number of command line arguments
//              argv, array of pointers to these tokens.
//
// Returns:     None.
//
//=============================================================================
void ProcessCommandLineArguments( int argc, char* argv[] )
{
    rDebugPrintf( "*************************************************************\n" );
    rDebugPrintf( "Command Line Args:\n" );

    //
    // Pick out all the command line options and store them in GameDB.
    // Also dump them to the output for handy dandy viewing.
    //
    int i;
    for( i = 0; i < argc; ++i )
    {
        char* argument = argv[i];

        rDebugPrintf( "arg%d: %s\n", i, argument );

        CommandLineOptions::HandleOption( argument );
    }

    if( !CommandLineOptions::Get( CLO_ART_STATS ) )
    {
        //CommandLineOptions::HandleOption( "noheaps" );
    }

    rDebugPrintf( "*************************************************************\n" );
}


void ProcessCommandLineArgumentsFromFile()
{
#ifndef FINAL
    // Looking for additional command line args being passed in from a file
    // it's for QA testing etc.

    DebugPrint("  -> About to open command.txt...\n");

    IRadFile* pfile = NULL;

    // Skip file open on PS3 for now - it may hang if file doesn't exist
#ifndef RAD_PS3
    ::radFileOpenSync( &pfile, "command.txt" );
#endif

    DebugPrint("  -> radFileOpenSync returned\n");

    if( pfile != NULL )
    {
        DebugPrint("  -> Reading command.txt...\n");
        char commandlinestring[256];
        commandlinestring[0] = '\0';

        pfile->ReadSync( commandlinestring, 255 );

        // QA created the command line file and wants to pass additional arguments
        char* argument = strtok( commandlinestring, " " );
        while( argument != NULL )
        {
            CommandLineOptions::HandleOption( argument );
            argument = strtok( NULL, " " );
        }
        pfile->Release();
        DebugPrint("  -> command.txt processed\n");
    }
    else
    {
        DebugPrint("  -> No command.txt found (OK)\n");
    }
#endif // FINAL
}

#endif // RAD_PS3
