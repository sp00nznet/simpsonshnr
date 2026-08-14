# Session Context - Simpsons Hit & Run PS3 Port

## Current Status: Matrix Loading Fixed - Scrooby Inventory Issue Blocking Rendering

### What's Working
- PS3 modules load (FS, IO, AUDIO, GCM, RESC, SYSUTIL, PNGDEC)
- radThread, radFile, radLoad work correctly
- Pure3D/PDDI initializes properly
- OpenGL ES context created via PSGL
- Textures load to memory (deferred GL upload working)
- Game runs at 60fps without crashes
- Matrix loading now correct (matches GL backend)

### What's Broken
- **Scrooby resources not found in inventory after loading**
- FrontEndRenderLayer reports IsRenderReady=0
- No actual rendering happening (black screen)

---

## Latest Session Progress

### 1. Matrix Loading Fix (COMPLETED)
The `ps3Context::LoadHardwareMatrix` was incorrectly transposing matrices. The PDDI matrix format is already compatible with OpenGL when cast to `float*`.

**Before (WRONG):**
```cpp
// Manual transpose that was incorrect
glMatrix[0]  = matrix->m[0][0];
glMatrix[1]  = matrix->m[1][0];  // Wrong!
// ... etc
glLoadMatrixf(glMatrix);
```

**After (CORRECT - matches GL backend):**
```cpp
void ps3Context::LoadHardwareMatrix(pddiMatrixType id)
{
    pddiMatrix tmp = *state.matrixStack[id]->Top();

    // Negate Z column for coordinate system (OpenGL is right-handed)
    tmp.m[0][2] = -tmp.m[0][2];
    tmp.m[1][2] = -tmp.m[1][2];
    tmp.m[2][2] = -tmp.m[2][2];
    tmp.m[3][2] = -tmp.m[3][2];

    glLoadMatrixf((float*)&tmp);  // Pass directly, no transpose needed
}
```

### 2. Scrooby Inventory Issue (CURRENT BLOCKER)

**Symptom:**
Files load successfully but aren't found when searched:
```
[P3DCallback::Done] status=0, load=30033e78, isDummyLoad=0
[ContinueLoading] Searching for 'bootup.p3d' in section 'ScroobyBootup'... NOT FOUND
```

**Loading Flow:**
1. `tFileHandler::LoadFile` creates temp `tEntityStore`, loads file, stores entities
2. `request->SetInventory(store)` sets the inventory on the request
3. radLoad finishes, calls `tLoadRequest::InternalCallback::Done()`
4. `InternalCallback::Done()` calls `inventory->Dump(section)` to transfer entities
5. radLoad calls external callback (`P3DCallback::Done`)
6. `P3DCallback::Done` calls `ContinueLoading()`
7. `ContinueLoading` searches inventory - **NOT FOUND**

**Possible Causes:**
1. Entity stored under different name than what's searched (e.g., full path vs basename)
2. Entity dumped to wrong inventory section
3. `InternalCallback::Done` not being called or not transferring correctly
4. Section mismatch between store and search

**Key Code Locations:**
- `game/libs/pure3d/p3d/loadmanager.cpp:526` - `InternalCallback::Done()` does inventory transfer
- `game/libs/scrooby/src/ResourceManager/FeResourceManager.cpp:394` - `ContinueLoading()` searches inventory
- `game/libs/scrooby/src/ResourceManager/FeResourceManager.h:349` - `P3DCallback::Done()` triggers ContinueLoading

---

## Files Modified

### Matrix Fix
- `game/libs/pure3d/pddi/ps3/ps3context.cpp` - Fixed LoadHardwareMatrix (line ~913)
- `game/libs/pure3d/pddi/base/basecontext.cpp` - Added SetCamera debug output
- `game/libs/pure3d/build/ps3/pddi/rebuild_all.ps1` - Added basecontext.cpp to build

### Debug Output
- `game/libs/scrooby/src/FeLoaders.cpp` - Added FeTextBibleLoader debug output
- `game/libs/scrooby/src/ResourceManager/FeResourceManager.cpp` - ContinueLoading debug output
- `game/libs/scrooby/src/ResourceManager/FeResourceManager.h` - P3DCallback::Done debug output

### Previous Fixes (Still Active)
- `game/libs/scrooby/src/ResourceManager/FeResourceManager.cpp` - Projects without screens callback fix
- `game/libs/pure3d/pddi/pddips3.hpp` - Include ps3primstream.hpp

---

## Next Steps to Resume

1. **Debug the inventory transfer:**
   - Add logging to `tLoadRequest::InternalCallback::Done()` in loadmanager.cpp
   - Log what section entities are being dumped to
   - Log what name entities are stored under

2. **Check entity naming:**
   - Verify what name the FeProject entity gets (full path? basename?)
   - Compare with what ContinueLoading searches for

3. **Verify section handling:**
   - Check if `p3d::inventory->SelectSection("ScroobyBootup")` is working
   - Verify the section exists before dumping

4. **Alternative approach:**
   - Check if this worked in a previous build (may be a regression)
   - Compare with PC/PS2 loading flow

---

## Build Commands
```bash
# Rebuild PDDI library (includes matrix fix)
cd game/libs/pure3d/build/ps3/pddi
powershell.exe -ExecutionPolicy Bypass -File rebuild_all.ps1

# Rebuild Scrooby library
cd game/libs/scrooby/build/ps3
powershell.exe -ExecutionPolicy Bypass -File build.ps1

# Link game
cd game/build/ps3
powershell.exe -ExecutionPolicy Bypass -File link_only.ps1

# Clear RPCS3 cache before testing (important after code changes!)
rm -rf "D:/emu/rpcs3-v0.0.26-14568-1852b370_win64/cache/SIMP00001/"*

# Output: game/cd/ps3/SIMP00001/PS3_GAME/USRDIR/EBOOT.BIN
```

---

## Repository Info
- **Project**: The Simpsons: Hit & Run (2003)
- **Developer**: Radical Entertainment
- **Original Platforms**: PS2, Xbox, GameCube, Windows
- **Target**: PlayStation 3 via PS3 SDK 3.40
- **Remote**: ssh://git@192.168.100.150:22/sp00nz/simpsonshnr.git

## Directory Structure
```
simpsonshnr/
├── game/
│   ├── build/ps3/          # PS3 build scripts
│   ├── code/main/          # ps3main.cpp, ps3platform.cpp
│   ├── libs/
│   │   ├── pure3d/pddi/ps3/  # PS3 graphics backend (matrix fix here)
│   │   ├── radcore/          # Threading, file I/O
│   │   ├── scrooby/          # GUI system (inventory issue here)
│   │   └── radcontent/       # Asset loading (radLoad)
│   └── cd/ps3/SIMP00001/     # Output PKG structure
├── README.md
└── SESSION_CONTEXT.md       # This file
```
