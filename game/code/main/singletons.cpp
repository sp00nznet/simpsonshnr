//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        singletons.cpp
//
// Description: Create and destroy all our singletons here.
//
// History:     + Created -- Darwin Chau
//
//=============================================================================

//========================================
// System Includes
//========================================
#include <raddebug.hpp>
#include <stdio.h>

#ifdef RAD_PS3
#include <sys/tty.h>
static void _SingletonPrint(const char* msg)
{
    unsigned int len = 0;
    const char* p = msg;
    while (*p++) len++;
    unsigned int written;
    sys_tty_write(0, msg, len, &written);
}
#define SINGLETON_DEBUG(msg) _SingletonPrint(msg "\n")
#else
#define SINGLETON_DEBUG(msg) do {} while(0)
#endif

//========================================
// Project Includes
//========================================
#include <main/singletons.h>

#include <ai/actionbuttonmanager.h>
#include <atc/atcmanager.h>
#include <camera/supercammanager.h>
#include <cards/cardgallery.h>
#include <cheats/cheatinputsystem.h>
#include <data/gamedatamanager.h>
#include <console/console.h>
#include <debug/debuginfo.h>
#include <debug/profiler.h>
#include <events/eventmanager.h>
#include <input/inputmanager.h>
#include <interiors/interiormanager.h>
#include <loading/loadingmanager.h>
#include <main/commandlineoptions.h>
#include <meta/triggervolumetracker.h>
#include <mission/missionmanager.h>
#include <mission/charactersheet/charactersheetmanager.h>
#include <mission/rewards/rewardsmanager.h>
#include <memory/srrmemory.h>
#include <mission/missionscriptloader.h>
#include <worldsim/Skidmarks/skidmarkmanager.h>
#include <presentation/mouthflapper.h>
#include <presentation/presentation.h>
#include <presentation/tutorialmanager.h>
#include <presentation/gui/guisystem.h>
#include <render/RenderFlow/renderflow.h>
#include <sound/soundmanager.h>
#include <worldsim/avatarmanager.h>
#include <worldsim/character/charactermanager.h>
#include <worldsim/worldphysicsmanager.h>
#include <worldsim/vehiclecentral.h>
#include <worldsim/coins/coinmanager.h>
#include <worldsim/coins/sparkle.h>
#include <worldsim/hitnrunmanager.h>
#include <ai/actor/actormanager.h>
#include <data/persistentworldmanager.h>
#include <worldsim/character/footprint/footprintmanager.h>
#include <memory/propstats.h>
#include <simcommon/simenvironment.hpp>

#ifdef RAD_WIN32
#include <data/config/gameconfigmanager.h>
#include <input/MouseCursor.h>
#endif

//=============================================================================
// Function:    void AddVariablesToWatcher
//=============================================================================
//
// Description: Adds lots of global variables to the watcher
// 
// Parameters:  None
//
// Returns:     None
//
//=============================================================================
#ifdef DEBUGWATCH
void AddVariablesToWatcher()
{
    MouthFlapper::AddVariablesToWatcher();
}
#endif

//=============================================================================
// Function:    CreateSingletons
//=============================================================================
//
// Description: Construct all the singletons 
// 
// Parameters:  None
//
// Returns:     None
//
//=============================================================================
void CreateSingletons()
{
    SINGLETON_DEBUG("Starting CreateSingletons...");

    //CREATE_MEMORYTRACKER();

    SINGLETON_DEBUG("Calling CREATE_DEBUGINFO...");
    CREATE_DEBUGINFO();
    SINGLETON_DEBUG("CREATE_DEBUGINFO done");

    SINGLETON_DEBUG("MEMTRACK_PUSH_GROUP...");
    MEMTRACK_PUSH_GROUP( "Singletons" );
    SINGLETON_DEBUG("MEMTRACK_PUSH_GROUP done");

    SINGLETON_DEBUG("Creating GameDataManager...");
    GameDataManager* pGameDataManager = GameDataManager::CreateInstance();
    rAssert( pGameDataManager != NULL );

#ifdef RAD_WIN32
    GameConfigManager* pGameConfigManager = GameConfigManager::CreateInstance();
    rAssert( pGameConfigManager != NULL );
#endif

    SINGLETON_DEBUG("Creating EventManager...");
    EventManager* pEventManager = EventManager::CreateInstance();
    rAssert( pEventManager != NULL );

    SINGLETON_DEBUG("Creating LoadingManager...");
    LoadingManager* pLoadingManager = LoadingManager::CreateInstance();
    rAssert( pLoadingManager != NULL );

    SINGLETON_DEBUG("Creating InputManager...");
    InputManager* pInputManager = InputManager::CreateInstance();
    rAssert( pInputManager != NULL );

    SINGLETON_DEBUG("Creating SkidmarkManager...");
    SkidmarkManager* pSkidmarkManager = SkidmarkManager::CreateInstance();
    rAssert( pSkidmarkManager != NULL );

    SINGLETON_DEBUG("Creating ATCManager...");
    //AttributeTableChunk Manager, has the the info of physprops in a table
    ATCManager * pATCManager =ATCManager::CreateInstance();
    rAssert(pATCManager != NULL);

    SINGLETON_DEBUG("Creating CardGallery...");
    CardGallery* pCardCallery = CardGallery::CreateInstance();
    rAssert( pCardCallery != NULL );

    SINGLETON_DEBUG("Creating CharacterSheetManager...");
    //CharacterSheetManager
    CharacterSheetManager* pCharacterSheetManager = CharacterSheetManager::CreateInstance();
    rAssert(pCharacterSheetManager != NULL);

    SINGLETON_DEBUG("Creating RewardsManager...");
    //RewardsManager
    RewardsManager* pRewardsManager = RewardsManager::CreateInstance();
    rAssert(pRewardsManager);

    MEMTRACK_PUSH_GROUP( "Console" );

    SINGLETON_DEBUG("Creating Console...");
    // must be around before VehicleCentral tries to tie in some shit
    // ...MissionScriptLoader too
    Console* pConsole = Console::CreateInstance();
    rAssert( pConsole != NULL );

    MEMTRACK_POP_GROUP( "Console" );



    SINGLETON_DEBUG("Creating VehicleCentral...");
    // must be done before init'ing worldphysicsmanager
    VehicleCentral* pVC = VehicleCentral::CreateInstance();
    rAssert(pVC != 0);

    SINGLETON_DEBUG("Creating WorldPhysicsManager...");
    sim::SimUnits::Initialize();
    WorldPhysicsManager* pWPM = WorldPhysicsManager::CreateInstance();
    rAssert(pWPM != 0);

    CREATE_PROFILER();

    SINGLETON_DEBUG("Creating PresentationManager...");
    PresentationManager* pPM = PresentationManager::CreateInstance();
    rAssert( pPM != NULL );

    SINGLETON_DEBUG("Creating CGuiSystem...");
    CGuiSystem* pGuiSystem = CGuiSystem::CreateInstance();
    rAssert( pGuiSystem != NULL );

    SINGLETON_DEBUG("Creating SoundManager...");
    SoundManager* pSoundManager =
        SoundManager::CreateInstance( CommandLineOptions::Get( CLO_MUTE ),
                                      CommandLineOptions::Get( CLO_NO_MUSIC ),
                                      CommandLineOptions::Get( CLO_NO_EFFECTS ),
                                      CommandLineOptions::Get( CLO_NO_DIALOG ) );
    rAssert( pSoundManager != NULL );

    SINGLETON_DEBUG("Creating MissionManager...");
    MissionManager* pMM = MissionManager::CreateInstance();
    rAssert( pMM != NULL );

    /*
    HeadToHeadManager* pH2HM = HeadToHeadManager::CreateInstance();
    rAssert( pH2HM != NULL );
    */

    SINGLETON_DEBUG("Creating MissionScriptLoader...");
    MissionScriptLoader* pMSL = MissionScriptLoader::CreateInstance();
    rAssert( pMSL != NULL );

    SINGLETON_DEBUG("Creating CharacterManager...");
    CharacterManager* pCM = CharacterManager::CreateInstance();
    rAssert( pCM != NULL );

    SINGLETON_DEBUG("Creating AvatarManager...");
    AvatarManager* pAM = AvatarManager::CreateInstance();
    rAssert( pAM != (AvatarManager*)0 );

    SINGLETON_DEBUG("Creating ActionButtonManager...");
    ActionButtonManager* pABM = ActionButtonManager::CreateInstance();
    rAssert( pABM != (ActionButtonManager*)0 );

    SINGLETON_DEBUG("Creating SuperCamManager...");
    SuperCamManager* pSCM = SuperCamManager::CreateInstance();
    rAssert( pSCM != NULL );

    SINGLETON_DEBUG("Creating TriggerVolumeTracker...");
    TriggerVolumeTracker::CreateInstance();

    SINGLETON_DEBUG("Creating InteriorManager...");
    InteriorManager* pInteriorManager = InteriorManager::CreateInstance();
    rAssert( pInteriorManager != NULL );

    SINGLETON_DEBUG("Creating CheatInputSystem...");
    CheatInputSystem* pCheatInputSystem = CheatInputSystem::CreateInstance();
    rAssert( pCheatInputSystem != NULL );

    SINGLETON_DEBUG("Creating TutorialManager...");
    TutorialManager* pTutorialManager = TutorialManager::CreateInstance();
    rAssert( pTutorialManager != NULL );

    SINGLETON_DEBUG("Creating ActorManager...");
    ActorManager* pActorManager = ActorManager::CreateInstance();
    rAssert( pActorManager != NULL );

    SINGLETON_DEBUG("All singletons created!");

    PersistentWorldManager* pPWManager = PersistentWorldManager::CreateInstance();
    rAssert( pPWManager );

    FootprintManager* pFootprintManager = FootprintManager::CreateInstance();
    rAssert( pFootprintManager != NULL );

    CoinManager* pCoinManager = CoinManager::CreateInstance();
    rAssert( pCoinManager );
    Sparkle* pSparkle = Sparkle::CreateInstance();
    rAssert( pSparkle );

	HitnRunManager* pHitnRunManager = HitnRunManager::CreateInstance();
	rAssert( pHitnRunManager );

    //
    // Create The RenderFlow Instance; this creates the RenderManager.
    // Create it under singletons.cpp because of co-dependency between singletons.
    //
    RenderFlow* pRenderFlow = RenderFlow::CreateInstance();
    rAssert( pRenderFlow );


#ifndef RAD_RELEASE
    if( CommandLineOptions::Get( CLO_PROP_STATS ) )
    {
        PropStats::EnableTracking();
    }   
#endif

	MEMTRACK_POP_GROUP("Singletons");

    AddVariablesToWatcher();
}


//=============================================================================
// Function:    DestroySingletons
//=============================================================================
//
// Description: Destroy all the singletons
// 
// Parameters:  None
//
// Returns:     None
//
//=============================================================================
void DestroySingletons()
{
    InteriorManager::DestroyInstance();

    ActionButtonManager::DestroyInstance();

    SuperCamManager::DestroyInstance();

    AvatarManager::DestroyInstance();

    CharacterManager::DestroyInstance();
    
    MissionScriptLoader::DestroyInstance();    
    
    //HeadToHeadManager::DestroyInstance();    
    
    MissionManager::DestroyInstance();
    
    SoundManager::DestroyInstance();
    
    CGuiSystem::DestroyInstance();

    PresentationManager::DestroyInstance();

    WorldPhysicsManager::DestroyInstance();

    VehicleCentral::DestroyInstance();
    
    Console::DestroyInstance();
       
    ATCManager::DestroyInstance();

    CharacterSheetManager::DestroyInstance();

    RewardsManager::DestroyInstance();

    CardGallery::DestroyInstance();

    TutorialManager::DestroyInstance();

    CoinManager::DestroyInstance();

    Sparkle::DestroyInstance();

    CheatInputSystem::DestroyInstance();

    HitnRunManager::DestroyInstance();

    InputManager::DestroyInstance();
    
    LoadingManager::DestroyInstance();

    SkidmarkManager::DestroyInstance();

    FootprintManager::DestroyInstance();

    ActorManager::DestroyInstance();

    PersistentWorldManager::DestroyInstance();

    TriggerVolumeTracker::DestroyInstance();

    RenderFlow::DestroyInstance();

    GameDataManager::DestroyInstance();

    EventManager::DestroyInstance();

#ifdef RAD_WIN32
    GameConfigManager::DestroyInstance();
#endif

    DESTROY_PROFILER();

    DESTROY_DEBUGINFO();

    //DESTROY_MEMORYTRACKER();
}
