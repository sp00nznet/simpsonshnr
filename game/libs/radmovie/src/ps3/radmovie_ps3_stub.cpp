//=============================================================================
// Copyright (c) 2002 Radical Games Ltd.  All rights reserved.
// PS3 stub implementation - Movie player (no video playback)
//=============================================================================

#include <radmovie2.hpp>
#include <radmemory.hpp>
#include <radtime.hpp>

//=============================================================================
// Constants
//=============================================================================

const unsigned int radMovie_NoAudioTrack = 0xFFFFFFFF;
const char * radMovieDebugChannel2 = "radmovie";

//=============================================================================
// Base object for ref counting
//=============================================================================

class RadMovieStubObject : public radRefCount
{
public:
    RadMovieStubObject() : radRefCount( 0 ) {}
    virtual ~RadMovieStubObject() {}

    void * operator new( size_t size, radMemoryAllocator allocator )
    {
        return radMemoryAlloc( allocator, size );
    }

    void operator delete( void * pMemory )
    {
        radMemoryFree( pMemory );
    }
};

//=============================================================================
// Render Strategy Stub
//=============================================================================

class RadMovieRenderStrategyStub : public IRadMovieRenderStrategy, public RadMovieStubObject
{
public:
    IMPLEMENT_REFCOUNTED( "RadMovieRenderStrategyStub" )

    RadMovieRenderStrategyStub() {}
    virtual ~RadMovieRenderStrategyStub() {}

    // Non-BINK, Non-XBOX version (PS3 uses this)
    virtual void SetParameters( unsigned int width, unsigned int height ) {}
    virtual bool Render( void * pBuffer ) { return true; }
};

//=============================================================================
// Movie Player Stub
//=============================================================================

class RadMoviePlayer2Stub : public IRadMoviePlayer2, public RadMovieStubObject
{
public:
    IMPLEMENT_REFCOUNTED( "RadMoviePlayer2Stub" )

    RadMoviePlayer2Stub()
        : m_State( NoData )
        , m_Volume( 1.0f )
        , m_Pan( 0.0f )
    {}

    virtual ~RadMoviePlayer2Stub() {}

    // PS3 version of Initialize
    virtual void Initialize(
        IRadMovieRenderLoop * pRenderLoop,
        IRadMovieRenderStrategy * pRenderStrategy )
    {
        m_xRenderLoop = pRenderLoop;
        m_xRenderStrategy = pRenderStrategy;
    }

    virtual bool Render()
    {
        // PS3 stub - nothing to render
        return true;
    }

    virtual void Load( const char * pVideoFileName, unsigned int audioTrackIndex )
    {
        m_State = ReadyToPlay;
    }

    virtual void Unload()
    {
        m_State = NoData;
    }

    virtual void Play()
    {
        // PS3 stub - immediately transition to ready (movie "finished")
        m_State = ReadyToPlay;
    }

    virtual void Pause()
    {
        if( m_State == Playing )
        {
            m_State = ReadyToPlay;
        }
    }

    virtual void SetPan( float pan ) { m_Pan = pan; }
    virtual float GetPan() { return m_Pan; }
    virtual void SetVolume( float vol ) { m_Volume = vol; }
    virtual float GetVolume() { return m_Volume; }
    virtual State GetState() { return m_State; }
    virtual unsigned int GetCurrentFrameNumber() { return 0; }
    virtual float GetFrameRate() { return 30.0f; }

    virtual bool GetVideoFrameInfo( VideoFrameInfo * pInfo )
    {
        if( pInfo )
        {
            pInfo->Width = 640;
            pInfo->Height = 480;
        }
        return true;
    }

private:
    State m_State;
    float m_Volume;
    float m_Pan;
    ref< IRadMovieRenderLoop > m_xRenderLoop;
    ref< IRadMovieRenderStrategy > m_xRenderStrategy;
};

//=============================================================================
// Global state
//=============================================================================

static bool s_Initialized = false;

//=============================================================================
// Global Functions
//=============================================================================

void radMovieInitialize2( radMemoryAllocator allocator )
{
    s_Initialized = true;
}

void radMovieTerminate2( void )
{
    s_Initialized = false;
}

void radMovieService2( void )
{
    // PS3 stub - nothing to service
}

//=============================================================================
// Factory Functions
//=============================================================================

IRadMoviePlayer2 * radMoviePlayerCreate2( radMemoryAllocator alloc )
{
    return new( alloc ) RadMoviePlayer2Stub();
}

IRadMovieRenderStrategy * radMovieSimpleFullScreenRenderStrategyCreate( radMemoryAllocator alloc )
{
    return new( alloc ) RadMovieRenderStrategyStub();
}
