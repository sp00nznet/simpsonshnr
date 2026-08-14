#include <sound/soundfx/soundeffectplayer.cpp>
#include <sound/soundfx/soundfxfrontendlogic.cpp>
#include <sound/soundfx/soundfxgameplaylogic.cpp>
#include <sound/soundfx/soundfxlogic.cpp>
#include <sound/soundfx/soundfxpauselogic.cpp>
#include <sound/soundfx/reverbcontroller.cpp>
#include <sound/soundfx/reverbsettings.cpp>
#include <sound/soundfx/positionalsoundsettings.cpp>

#ifdef RAD_PS2
#include <sound/soundfx/ps2reverbcontroller.cpp>
#elif defined RAD_PS3
#include <sound/soundfx/ps3reverbcontroller.cpp>
#elif defined RAD_WIN32
#include <sound/soundfx/win32reverbcontroller.cpp>
#elif defined RAD_XBOX
#include <sound/soundfx/xboxreverbcontroller.cpp>
#else
#include <sound/soundfx/gcreverbcontroller.cpp>
#endif