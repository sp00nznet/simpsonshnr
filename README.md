# The Simpsons: Hit & Run - Source Code Repository

This repository contains the complete source code for **The Simpsons: Hit & Run**, a multi-platform action-adventure video game originally developed by Radical Entertainment and released in 2003.

## Repository Overview

Source only. Following the layout of [q4a/simpsons](https://github.com/q4a/simpsons),
this repository tracks **only the engine and game source** — the working tree on disk
holds much more.

| Component | Description |
|-----------|-------------|
| `game/code` | Game source |
| `game/libs` | Radical middleware: pure3d, radcore, scrooby, sim, choreo, … |
| `game/libs/ps3sdkstubs` | Minimal Cell SDK header stubs written for this port |
| `game/build/<plat>` | Build scripts and makefiles |

### Not in this repository

Excluded by `.gitignore` — obtain or build these locally:

- **Console/vendor SDKs**: GameCube, PS2 (`deci2`), PS3, DirectX 8, Maya headers
- **Game assets**: art, audio, `.p3d`/`.rmv`/`.rms` data, disc images (`game/cd`, `Art/`)
- **Art pipeline & tools**: Maya plug-ins, exporters, `tools/`, sample apps
- **Design docs** (`documents/`), build outputs, logs

A PS3 build additionally needs the Cell toolchain + SDK 3.40 installed locally; paths
are set at the top of `game/libs/build_all_ps3.ps1`.

## Build Status

| Platform | Solution File | Status |
|----------|---------------|--------|
| Win32/PC | `game/build/win32/SRR2.sln` | Visual Studio 2022 compatible |
| Xbox | `game/build/xbox/SRR2.sln` | Requires Xbox Development Kit |
| GameCube | Requires CodeWarrior | Requires GC_SDK |
| PS2 | Legacy | Not configured |
| **PS3** | `game/build/ps3/rebuild_debug.bat` | **In Development** - Boots, debugging texture loading |

## Project Structure

```
simpsonshnr/
├── game/                    # Main game code
│   ├── code/               # Game source code (1,252+ files)
│   │   ├── ai/             # AI systems (actor behavior, pathfinding)
│   │   ├── actor/          # Character and entity system
│   │   ├── mission/        # Mission/level scripting
│   │   ├── gameflow/       # Game state management
│   │   ├── worldsim/       # World simulation
│   │   ├── render/         # Graphics rendering
│   │   ├── sound/          # Audio system
│   │   ├── camera/         # Camera system
│   │   ├── input/          # Input handling
│   │   ├── presentation/   # UI and HUD
│   │   ├── loading/        # Level loading
│   │   └── ...
│   ├── build/              # Build configurations
│   │   ├── win32/          # PC build (Visual Studio)
│   │   └── xbox/           # Xbox build
│   ├── libs/               # Third-party libraries
│   │   ├── pure3d/         # Graphics engine
│   │   ├── radcore/        # Core utilities
│   │   ├── radmath/        # Math library
│   │   ├── radsound/       # Audio system
│   │   ├── radmusic/       # Music system
│   │   ├── radmovie/       # Video playback
│   │   ├── radscript/      # Scripting system
│   │   ├── scrooby/        # UI framework
│   │   ├── poser/          # Animation system
│   │   └── choreo/         # Character choreography
│   └── cd/                 # Game assets for distribution
├── Art/                    # Art assets
│   ├── Character-models/   # 3D character models (50+ characters)
│   └── Character-animations/ # Character animations
├── documents/              # Documentation
│   ├── design/             # Game design documents
│   ├── art/                # Art specifications
│   └── ...
├── tools/                  # Development tools
│   ├── worldbuilder/       # Level world builder
│   ├── trackeditor/        # Track/path editor
│   ├── MayaTools/          # Maya integration
│   └── ...
├── GC_SDK/                 # GameCube SDK (retained for reference)
└── WORLD_MAPS.txt          # ASCII maps of all game worlds
```

## Building

### Windows (PC)

1. Open `game/build/win32/SRR2.sln` in Visual Studio 2022
2. Select configuration (Debug/Release/Tune)
3. Build solution

**Requirements:**
- Visual Studio 2022
- Windows SDK
- dgvoodoo2 (included as `dgvoodoo2.zip` for graphics compatibility)

### Xbox (Original)

1. Install Xbox Development Kit (XDK)
2. Open `game/build/xbox/SRR2.sln`
3. Build using provided batch scripts:
   - `build_xbox_full.bat` - Full build
   - `xbox_incremental.bat` - Incremental build

**Requirements:**
- Microsoft Xbox Development Kit (XDK) 2003
- Visual Studio with Xbox support

### GameCube

Requires CodeWarrior IDE and GameCube SDK. The `GC_SDK` folder contains the necessary SDK files.

### PlayStation 3 (In Development)

A PS3 port is in active development using PS3 SDK 3.40.

**Current Status**: Game boots and initializes, debugging texture loading issues.

**Build Instructions**:
```bash
# Set up PS3 SDK path (adjust for your installation)
# SDK expected at: D:\PS3.Full.3.40.SDK.PS3-DUPLEX

# Rebuild libraries (if needed)
cd game/libs/pure3d/build/ps3/Pure3D
powershell.exe -ExecutionPolicy Bypass -File build.ps1

cd game/libs/radcontent/build/ps3
powershell.exe -ExecutionPolicy Bypass -File build.ps1

# Build and link game
cd game/build/ps3
rebuild_debug.bat

# Output: game/cd/ps3/SIMP00001/PS3_GAME/USRDIR/EBOOT.BIN
```

**Requirements**:
- PS3 SDK 3.40 (Cell toolchain, PSGL libraries)
- RPCS3 emulator for testing

**PS3 Port Files**:
- `game/code/main/ps3main.cpp` - Entry point
- `game/code/main/ps3platform.cpp` - Platform implementation
- `game/libs/pure3d/pddi/ps3/` - PS3 graphics backend (PSGL)
- `game/libs/radcore/src/radthread/ps3thread.cpp` - Threading/semaphores

See `SESSION_CONTEXT.md` for detailed debugging notes.

## Key Features

- **Open World Gameplay**: Three distinct Springfield environments
  - Suburban Springfield (Levels 1, 4, 7)
  - Downtown Springfield (Levels 2, 5)
  - Entertainment District (Levels 3, 6)
- **Multiple Playable Characters**: Homer, Bart, Lisa, Marge, and Apu
- **Vehicle System**: 40+ drivable vehicles
- **Mission System**: Story missions and bonus objectives
- **Collectibles**: Cards, coins, costumes, and vehicles

## Documentation

Key documentation files in `/documents/`:

- **Design Documents**: Coin system, HUD, missions, vehicle AI, traffic systems
- **Art Specifications**: Character modeling, animation, rigging guides
- **Technical Specs**: World building, level design procedures

## File Statistics

| File Type | Count | Description |
|-----------|-------|-------------|
| `.cpp` | 2,508 | C++ source files |
| `.h/.hpp` | 3,902 | Header files |
| `.p3d` | 9,969 | Pure3D model/scene files |
| `.obj` | 4,179 | Wavefront OBJ models |
| `.mb` | 1,404 | Maya binary files |
| `.png` | 3,173 | PNG images |

## License

This is archival/preservation material. Original copyright belongs to Fox Interactive and Radical Entertainment (2003).

## Original Development

- **Developer**: Radical Entertainment
- **Publisher**: Fox Interactive / Vivendi Universal Games
- **Release Date**: September 16, 2003
- **Platforms**: PlayStation 2, Xbox, GameCube, Windows
