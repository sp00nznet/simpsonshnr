//=============================================================================
// Copyright (c) 2002 Radical Games Ltd.  All rights reserved.
// PS3 standalone stub implementation - provides all radsound functionality
// as no-ops for initial port testing.
//=============================================================================

#include <radsound.hpp>
#include <radsound_ps3.hpp>
#include <radsoundobject.hpp>
#include <radmemory.hpp>
#include <radtime.hpp>

//=============================================================================
// Debug channel
//=============================================================================

const char * radSoundDebugChannel = "radsound";

//=============================================================================
// radSoundObject static members (normally in common/radsoundobject.cpp)
//=============================================================================

const char * radSoundObject::s_SoundObjectName = "error!";
unsigned int radSoundObject::s_SoundObjectCount = 0;

static RadSoundFilePerformanceCallback * s_pFileCallback = NULL;

void RadSoundSetFilePerformanceCallback( RadSoundFilePerformanceCallback * pCallback )
{
    s_pFileCallback = pCallback;
}

//=============================================================================
// Base object for ref counting
//=============================================================================

class RadSoundStubObject : public radRefCount
{
public:
    RadSoundStubObject() : radRefCount( 0 ) {}
    virtual ~RadSoundStubObject() {}

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
// Audio Format Stub
//=============================================================================

class RadSoundHalAudioFormatStub : public IRadSoundHalAudioFormat, public RadSoundStubObject
{
public:
    IMPLEMENT_REFCOUNTED( "RadSoundHalAudioFormatStub" )

    RadSoundHalAudioFormatStub()
        : m_Encoding( PCM ), m_SampleRate( 44100 ), m_Channels( 2 ), m_BitResolution( 16 )
    {}

    virtual void Initialize( Encoding encoding, IRefCount * pIRefCount_CustomInfo,
        unsigned int samplingRate, unsigned int channels, unsigned int bitResolution )
    {
        m_Encoding = encoding;
        m_SampleRate = samplingRate;
        m_Channels = channels;
        m_BitResolution = bitResolution;
    }

    virtual Encoding GetEncoding() { return m_Encoding; }
    virtual IRefCount * GetCustomEncodingInfo() { return NULL; }
    virtual unsigned int GetNumberOfChannels() { return m_Channels; }
    virtual unsigned int GetFrameSizeInBytes() { return (m_BitResolution / 8) * m_Channels; }
    virtual unsigned int GetFrameSizeInSamples() { return m_Channels; }
    virtual unsigned int GetSampleRate() { return m_SampleRate; }
    virtual unsigned int GetSampleSizeInBits() { return m_BitResolution; }
    virtual bool Matches( IRadSoundHalAudioFormat * pOther ) { return true; }
    virtual unsigned int SamplesToBytes( unsigned int samples ) { return samples * (m_BitResolution / 8); }
    virtual unsigned int BytesToSamples( unsigned int bytes ) { return bytes / (m_BitResolution / 8); }
    virtual unsigned int SamplesToFrames( unsigned int samples ) { return samples / m_Channels; }
    virtual unsigned int FramesToSamples( unsigned int frames ) { return frames * m_Channels; }
    virtual unsigned int FramesToBytes( unsigned int frames ) { return frames * GetFrameSizeInBytes(); }
    virtual unsigned int BytesToFrames( unsigned int bytes ) { return bytes / GetFrameSizeInBytes(); }
    virtual unsigned int SamplesToMilliseconds( unsigned int samples ) { return (samples * 1000) / m_SampleRate; }
    virtual unsigned int MillisecondsToSamples( unsigned int ms ) { return (ms * m_SampleRate) / 1000; }
    virtual unsigned int BytesToMilliseconds( unsigned int bytes ) { return SamplesToMilliseconds( BytesToSamples( bytes ) ); }
    virtual unsigned int MillisecondsToBytes( unsigned int ms ) { return SamplesToBytes( MillisecondsToSamples( ms ) ); }
    virtual unsigned int FramesToMilliseconds( unsigned int frames ) { return SamplesToMilliseconds( FramesToSamples( frames ) ); }
    virtual unsigned int MillisecondsToFrames( unsigned int ms ) { return SamplesToFrames( MillisecondsToSamples( ms ) ); }
    virtual unsigned int ConvertSizeType( SizeType target, unsigned int size, SizeType source )
    {
        // Convert source to samples first
        unsigned int samples = 0;
        switch( source )
        {
            case Milliseconds: samples = MillisecondsToSamples( size ); break;
            case Frames: samples = FramesToSamples( size ); break;
            case Samples: samples = size; break;
            case Bytes: samples = BytesToSamples( size ); break;
        }
        // Convert samples to target
        switch( target )
        {
            case Milliseconds: return SamplesToMilliseconds( samples );
            case Frames: return SamplesToFrames( samples );
            case Samples: return samples;
            case Bytes: return SamplesToBytes( samples );
        }
        return size;
    }
    virtual unsigned int GetBitResolution() { return m_BitResolution; }

private:
    Encoding m_Encoding;
    unsigned int m_SampleRate;
    unsigned int m_Channels;
    unsigned int m_BitResolution;
};

//=============================================================================
// Memory Region Stub
//=============================================================================

class RadSoundHalMemoryRegionStub : public IRadSoundHalMemoryRegion, public RadSoundStubObject
{
public:
    IMPLEMENT_REFCOUNTED( "RadSoundHalMemoryRegionStub" )

    RadSoundHalMemoryRegionStub( unsigned int size ) : m_Size( size ) {}

    virtual IRadSoundHalMemoryRegion * CreateChildRegion( unsigned int sizeInBytes,
        unsigned int maxAllocations, const char * pIdentifier )
    {
        return new( RADMEMORY_ALLOC_DEFAULT ) RadSoundHalMemoryRegionStub( sizeInBytes );
    }

    virtual void CreateMemoryObject( IRadMemoryObject ** ppIRadMemoryObject,
        unsigned int bytes, const char * pIdentifier )
    {
        // Create a dummy memory object
        *ppIRadMemoryObject = NULL;
    }

    virtual IRadSoundHalMemoryRegion * GetParent() { return NULL; }
    virtual IRadSoundHalMemoryRegion * GetFirstChild() { return NULL; }
    virtual IRadSoundHalMemoryRegion * GetNextChild( IRadSoundHalMemoryRegion * pChild ) { return NULL; }
    virtual unsigned int GetSize() { return m_Size; }
    virtual void GetStats( unsigned int * pFreeMemory, unsigned * pNumObjects,
        unsigned int * pLargestBlock, bool recurseChildren )
    {
        if( pFreeMemory ) *pFreeMemory = m_Size;
        if( pNumObjects ) *pNumObjects = 0;
        if( pLargestBlock ) *pLargestBlock = m_Size;
    }

private:
    unsigned int m_Size;
};

//=============================================================================
// Buffer Stub
//=============================================================================

class RadSoundHalBufferStub : public IRadSoundHalBuffer, public RadSoundStubObject
{
public:
    IMPLEMENT_REFCOUNTED( "RadSoundHalBufferStub" )

    RadSoundHalBufferStub() : m_SizeInFrames( 0 ), m_Looping( false ) {}

    virtual void Initialize( IRadSoundHalAudioFormat * pFormat, IRadMemoryObject * pMemObj,
        unsigned int sizeInFrames, bool looping, bool streaming )
    {
        m_xFormat = pFormat;
        m_SizeInFrames = sizeInFrames;
        m_Looping = looping;
    }

    virtual IRadSoundHalAudioFormat * GetFormat() { return m_xFormat; }
    virtual IRadMemoryObject * GetMemoryObject() { return NULL; }
    virtual bool IsLooping() { return m_Looping; }
    virtual unsigned int GetSizeInFrames() { return m_SizeInFrames; }

    virtual void LoadAsync( IRadSoundHalDataSource * pDs, unsigned int startPos,
        unsigned int numFrames, IRadSoundHalBufferLoadCallback * pCallback )
    {
        if( pCallback ) pCallback->OnBufferLoadComplete( numFrames );
    }

    virtual void ClearAsync( unsigned int startPos, unsigned int numFrames,
        IRadSoundHalBufferClearCallback * pCallback )
    {
        if( pCallback ) pCallback->OnBufferClearComplete();
    }

    virtual unsigned int GetMinTransferSize( IRadSoundHalAudioFormat::SizeType st ) { return 1; }
    virtual void CancelAsyncOperations() {}
    virtual void ReSetAudioFormat( IRadSoundHalAudioFormat * pFormat ) { m_xFormat = pFormat; }

private:
    ref< IRadSoundHalAudioFormat > m_xFormat;
    unsigned int m_SizeInFrames;
    bool m_Looping;
};

//=============================================================================
// Voice Stub
//=============================================================================

class RadSoundHalVoiceStub : public IRadSoundHalVoice, public RadSoundStubObject
{
public:
    IMPLEMENT_REFCOUNTED( "RadSoundHalVoiceStub" )

    RadSoundHalVoiceStub() : m_Priority( 5 ), m_Playing( false ), m_Muted( false ),
        m_Volume( 1.0f ), m_Trim( 1.0f ), m_Pan( 0.0f ), m_Pitch( 1.0f ), m_PlaybackPos( 0 )
    {
        m_AuxMode[0] = m_AuxMode[1] = radSoundAuxMode_Off;
        m_AuxGain[0] = m_AuxGain[1] = 0.0f;
    }

    virtual void SetPriority( unsigned int p ) { m_Priority = p; }
    virtual unsigned int GetPriority() { return m_Priority; }
    virtual void SetBuffer( IRadSoundHalBuffer * pBuf ) { m_xBuffer = pBuf; }
    virtual IRadSoundHalBuffer * GetBuffer() { return m_xBuffer; }
    virtual void Play() { m_Playing = true; }
    virtual void Stop() { m_Playing = false; m_PlaybackPos = 0; }
    virtual bool IsPlaying() { return m_Playing; }
    virtual void SetPlaybackPositionInSamples( unsigned int pos ) { m_PlaybackPos = pos; }
    virtual unsigned int GetPlaybackPositionInSamples() { return m_PlaybackPos; }
    virtual void SetVolume( float v ) { m_Volume = v; }
    virtual float GetVolume() { return m_Volume; }
    virtual void SetTrim( float t ) { m_Trim = t; }
    virtual float GetTrim() { return m_Trim; }
    virtual void SetMuted( bool m ) { m_Muted = m; }
    virtual bool GetMuted() { return m_Muted; }
    virtual void SetPan( float p ) { m_Pan = p; }
    virtual float GetPan() { return m_Pan; }
    virtual void SetPitch( float p ) { m_Pitch = p; }
    virtual float GetPitch() { return m_Pitch; }
    virtual void SetAuxMode( unsigned int aux, radSoundAuxMode mode ) { if( aux < 2 ) m_AuxMode[aux] = mode; }
    virtual radSoundAuxMode GetAuxMode( unsigned int aux ) { return aux < 2 ? m_AuxMode[aux] : radSoundAuxMode_Off; }
    virtual void SetAuxGain( unsigned int aux, float gain ) { if( aux < 2 ) m_AuxGain[aux] = gain; }
    virtual float GetAuxGain( unsigned int aux ) { return aux < 2 ? m_AuxGain[aux] : 0.0f; }
    virtual void SetPositionalGroup( IRadSoundHalPositionalGroup * pGroup ) { m_xPosGroup = pGroup; }
    virtual IRadSoundHalPositionalGroup * GetPositionalGroup() { return m_xPosGroup; }

private:
    unsigned int m_Priority;
    ref< IRadSoundHalBuffer > m_xBuffer;
    ref< IRadSoundHalPositionalGroup > m_xPosGroup;
    bool m_Playing;
    bool m_Muted;
    float m_Volume, m_Trim, m_Pan, m_Pitch;
    unsigned int m_PlaybackPos;
    radSoundAuxMode m_AuxMode[2];
    float m_AuxGain[2];
};

//=============================================================================
// Positional Group Stub
//=============================================================================

class RadSoundHalPositionalGroupStub : public IRadSoundHalPositionalGroup, public RadSoundStubObject
{
public:
    IMPLEMENT_REFCOUNTED( "RadSoundHalPositionalGroupStub" )

    RadSoundHalPositionalGroupStub() : m_MinDist( 1.0f ), m_MaxDist( 100.0f ),
        m_ConeOutVol( 1.0f ), m_ConeIn( 360.0f ), m_ConeOut( 360.0f ),
        m_Occlusion( 0.0f ), m_Obstruction( 0.0f )
    {
        m_Position.m_x = m_Position.m_y = m_Position.m_z = 0.0f;
        m_Velocity.m_x = m_Velocity.m_y = m_Velocity.m_z = 0.0f;
        m_OrientFront.m_x = 0.0f; m_OrientFront.m_y = 0.0f; m_OrientFront.m_z = 1.0f;
        m_OrientTop.m_x = 0.0f; m_OrientTop.m_y = 1.0f; m_OrientTop.m_z = 0.0f;
    }

    virtual void SetPosition( radSoundVector * p ) { m_Position = *p; }
    virtual void GetPosition( radSoundVector * p ) { *p = m_Position; }
    virtual void SetVelocity( radSoundVector * v ) { m_Velocity = *v; }
    virtual void GetVelocity( radSoundVector * v ) { *v = m_Velocity; }
    virtual void SetOrientation( radSoundVector * pFront, radSoundVector * pTop ) { m_OrientFront = *pFront; m_OrientTop = *pTop; }
    virtual void GetOrientation( radSoundVector * pFront, radSoundVector * pTop ) { *pFront = m_OrientFront; *pTop = m_OrientTop; }
    virtual void SetMinMaxDistance( float min, float max ) { m_MinDist = min; m_MaxDist = max; }
    virtual void GetMinMaxDistance( float * pMin, float * pMax ) { *pMin = m_MinDist; *pMax = m_MaxDist; }
    virtual void SetConeOutsideVolume( float ov ) { m_ConeOutVol = ov; }
    virtual float GetConeOutsideVolume() { return m_ConeOutVol; }
    virtual void SetConeAngles( float in, float out ) { m_ConeIn = in; m_ConeOut = out; }
    virtual void GetConeAngles( float * pIn, float * pOut ) { *pIn = m_ConeIn; *pOut = m_ConeOut; }
    virtual void SetOcclusion( float o ) { m_Occlusion = o; }
    virtual float GetOcclusion() { return m_Occlusion; }
    virtual void SetObstruction( float o ) { m_Obstruction = o; }
    virtual float GetObstruction() { return m_Obstruction; }

private:
    radSoundVector m_Position, m_Velocity, m_OrientFront, m_OrientTop;
    float m_MinDist, m_MaxDist, m_ConeOutVol, m_ConeIn, m_ConeOut, m_Occlusion, m_Obstruction;
};

//=============================================================================
// Listener Stub
//=============================================================================

class RadSoundHalListenerStub : public IRadSoundHalListener, public RadSoundStubObject
{
public:
    IMPLEMENT_REFCOUNTED( "RadSoundHalListenerStub" )

    static RadSoundHalListenerStub * s_pInstance;

    RadSoundHalListenerStub() : m_DistFactor( 1.0f ), m_DopplerFactor( 1.0f ),
        m_RollOffFactor( 1.0f ), m_EnvEnabled( false ), m_EnvAuxSend( 0 )
    {
        m_Position.m_x = m_Position.m_y = m_Position.m_z = 0.0f;
        m_Velocity.m_x = m_Velocity.m_y = m_Velocity.m_z = 0.0f;
        m_OrientFront.m_x = 0.0f; m_OrientFront.m_y = 0.0f; m_OrientFront.m_z = 1.0f;
        m_OrientTop.m_x = 0.0f; m_OrientTop.m_y = 1.0f; m_OrientTop.m_z = 0.0f;
    }

    virtual void SetPosition( radSoundVector * p ) { m_Position = *p; }
    virtual void GetPosition( radSoundVector * p ) { *p = m_Position; }
    virtual void SetVelocity( radSoundVector * v ) { m_Velocity = *v; }
    virtual void GetVelocity( radSoundVector * v ) { *v = m_Velocity; }
    virtual void SetOrientation( radSoundVector * pFront, radSoundVector * pTop ) { m_OrientFront = *pFront; m_OrientTop = *pTop; }
    virtual void GetOrientation( radSoundVector * pFront, radSoundVector * pTop ) { *pFront = m_OrientFront; *pTop = m_OrientTop; }
    virtual void SetDistanceFactor( float f ) { m_DistFactor = f; }
    virtual float GetDistanceFactor() { return m_DistFactor; }
    virtual void SetDopplerFactor( float f ) { m_DopplerFactor = f; }
    virtual float GetDopplerFactor() { return m_DopplerFactor; }
    virtual void SetRollOffFactor( float f ) { m_RollOffFactor = f; }
    virtual float GetRollOffFactor() { return m_RollOffFactor; }
    virtual void SetEnvEffectsEnabled( bool e ) { m_EnvEnabled = e; }
    virtual bool GetEnvEffectsEnabled() { return m_EnvEnabled; }
    virtual void SetEnvironmentAuxSend( unsigned int aux ) { m_EnvAuxSend = aux; }
    virtual unsigned int GetEnvironmentAuxSend() { return m_EnvAuxSend; }

private:
    radSoundVector m_Position, m_Velocity, m_OrientFront, m_OrientTop;
    float m_DistFactor, m_DopplerFactor, m_RollOffFactor;
    bool m_EnvEnabled;
    unsigned int m_EnvAuxSend;
};

RadSoundHalListenerStub * RadSoundHalListenerStub::s_pInstance = NULL;

//=============================================================================
// System Stub
//=============================================================================

class RadSoundHalSystemStub : public IRadSoundHalSystem, public RadSoundStubObject
{
public:
    IMPLEMENT_REFCOUNTED( "RadSoundHalSystemStub" )

    static RadSoundHalSystemStub * s_pInstance;

    RadSoundHalSystemStub() : m_NumAuxSends( 0 ), m_OutputMode( radSoundOutputMode_Stereo ),
        m_LastServiceTime( 0 )
    {
        m_AuxGain[0] = m_AuxGain[1] = 1.0f;
    }

    virtual void Initialize( const SystemDescription & desc )
    {
        m_NumAuxSends = desc.m_NumAuxSends;
        m_xRootRegion = new( RADMEMORY_ALLOC_DEFAULT ) RadSoundHalMemoryRegionStub( 1024 * 1024 );

        if( RadSoundHalListenerStub::s_pInstance == NULL )
        {
            RadSoundHalListenerStub::s_pInstance = new( RADMEMORY_ALLOC_DEFAULT ) RadSoundHalListenerStub();
            RadSoundHalListenerStub::s_pInstance->AddRef();
        }
        m_LastServiceTime = radTimeGetMilliseconds();
    }

    virtual IRadSoundHalMemoryRegion * GetRootMemoryRegion() { return m_xRootRegion; }
    virtual unsigned int GetNumAuxSends() { return m_NumAuxSends; }
    virtual void SetAuxEffect( unsigned int aux, IRadSoundHalEffect * pEffect ) {}
    virtual IRadSoundHalEffect * GetAuxEffect( unsigned int aux ) { return NULL; }
    virtual void SetAuxGain( unsigned int aux, float gain ) { if( aux < 2 ) m_AuxGain[aux] = gain; }
    virtual float GetAuxGain( unsigned int aux ) { return aux < 2 ? m_AuxGain[aux] : 1.0f; }
    virtual void SetOutputMode( radSoundOutputMode mode ) { m_OutputMode = mode; }
    virtual radSoundOutputMode GetOutputMode() { return m_OutputMode; }
    virtual void Service() { m_LastServiceTime = radTimeGetMilliseconds(); }
    virtual void ServiceOncePerFrame() {}
    virtual void GetStats( Stats * pStats )
    {
        if( pStats )
        {
            pStats->m_NumBuffers = 0;
            pStats->m_NumVoices = 0;
            pStats->m_NumVoicesPlaying = 0;
            pStats->m_NumPosVoices = 0;
            pStats->m_NumPosVoicesPlaying = 0;
            pStats->m_BufferMemoryUsed = 0;
            pStats->m_EffectsMemoryUsed = 0;
            pStats->m_TotalFreeSoundMemory = 1024 * 1024;
        }
    }

private:
    unsigned int m_NumAuxSends;
    radSoundOutputMode m_OutputMode;
    unsigned int m_LastServiceTime;
    float m_AuxGain[2];
    ref< IRadSoundHalMemoryRegion > m_xRootRegion;
};

RadSoundHalSystemStub * RadSoundHalSystemStub::s_pInstance = NULL;

//=============================================================================
// PS3 Effect Stub
//=============================================================================

class RadSoundEffectPs3Stub : public IRadSoundEffectPs3, public RadSoundStubObject
{
public:
    IMPLEMENT_REFCOUNTED( "RadSoundEffectPs3Stub" )

    RadSoundEffectPs3Stub() : m_Enabled( false ), m_Gain( 1.0f ), m_MasterGain( 1.0f ),
        m_Mode( Off ), m_Delay( 0.0f ), m_Feedback( 0.0f ), m_ReverbTime( 1.0f ),
        m_Damping( 0.5f ), m_PreDelay( 0.0f ), m_ChorusDepth( 0.5f ), m_ChorusRate( 1.0f ) {}

    virtual void Attach( unsigned int auxSend ) {}
    virtual void Detach() {}
    virtual void Update() {}
    virtual void SetMasterGain( float g ) { m_MasterGain = g; }
    virtual float GetMasterGain() { return m_MasterGain; }

    virtual void SetEnabled( bool e ) { m_Enabled = e; }
    virtual bool GetEnabled() { return m_Enabled; }
    virtual void SetGain( float g ) { m_Gain = g; }
    virtual float GetGain() { return m_Gain; }
    virtual void SetMode( Mode m ) { m_Mode = m; }
    virtual Mode GetMode() { return m_Mode; }
    virtual void SetDelay( float d ) { m_Delay = d; }
    virtual float GetDelayTime() { return m_Delay; }
    virtual void SetFeedback( float f ) { m_Feedback = f; }
    virtual float GetFeedback() { return m_Feedback; }
    virtual void SetReverbTime( float t ) { m_ReverbTime = t; }
    virtual float GetReverbTime() { return m_ReverbTime; }
    virtual void SetDamping( float d ) { m_Damping = d; }
    virtual float GetDamping() { return m_Damping; }
    virtual void SetPreDelay( float d ) { m_PreDelay = d; }
    virtual float GetPreDelay() { return m_PreDelay; }
    virtual void SetChorusDepth( float d ) { m_ChorusDepth = d; }
    virtual float GetChorusDepth() { return m_ChorusDepth; }
    virtual void SetChorusRate( float r ) { m_ChorusRate = r; }
    virtual float GetChorusRate() { return m_ChorusRate; }

private:
    bool m_Enabled;
    float m_Gain, m_MasterGain;
    Mode m_Mode;
    float m_Delay, m_Feedback, m_ReverbTime, m_Damping, m_PreDelay, m_ChorusDepth, m_ChorusRate;
};

//=============================================================================
// HAL Factory Functions
//=============================================================================

void radSoundHalSystemInitialize( radMemoryAllocator allocator )
{
    rAssert( RadSoundHalSystemStub::s_pInstance == NULL );
    RadSoundHalSystemStub::s_pInstance = new( allocator ) RadSoundHalSystemStub();
    RadSoundHalSystemStub::s_pInstance->AddRef();
}

void radSoundHalSystemTerminate( void )
{
    if( RadSoundHalSystemStub::s_pInstance )
    {
        RadSoundHalSystemStub::s_pInstance->Release();
        RadSoundHalSystemStub::s_pInstance = NULL;
    }
    if( RadSoundHalListenerStub::s_pInstance )
    {
        RadSoundHalListenerStub::s_pInstance->Release();
        RadSoundHalListenerStub::s_pInstance = NULL;
    }
}

IRadSoundHalSystem * radSoundHalSystemGet( void )
{
    return RadSoundHalSystemStub::s_pInstance;
}

IRadSoundHalListener * radSoundHalListenerGet( void )
{
    return RadSoundHalListenerStub::s_pInstance;
}

IRadSoundHalVoice * radSoundHalVoiceCreate( radMemoryAllocator allocator )
{
    return new( allocator ) RadSoundHalVoiceStub();
}

IRadSoundHalBuffer * radSoundHalBufferCreate( radMemoryAllocator allocator )
{
    return new( allocator ) RadSoundHalBufferStub();
}

IRadSoundHalAudioFormat * radSoundHalAudioFormatCreate( radMemoryAllocator allocator )
{
    return new( allocator ) RadSoundHalAudioFormatStub();
}

IRadSoundHalPositionalGroup * radSoundHalPositionalGroupCreate( radMemoryAllocator allocator )
{
    return new( allocator ) RadSoundHalPositionalGroupStub();
}

unsigned int radSoundHalDataSourceReadAlignmentGet()
{
    return 128;
}

unsigned int radSoundHalDataSourceReadMultipleGet()
{
    return 1;
}

unsigned int radSoundHalBufferCalculateMemorySize(
    IRadSoundHalAudioFormat::SizeType resultType,
    unsigned int requestedSize,
    IRadSoundHalAudioFormat::SizeType requestedSizeType,
    IRadSoundHalAudioFormat * pFormat )
{
    if( pFormat == NULL ) return requestedSize;
    return pFormat->ConvertSizeType( resultType, requestedSize, requestedSizeType );
}

//=============================================================================
// PS3 Specific Functions
//=============================================================================

IRadSoundEffectPs3 * radSoundCreateEffectPs3( radMemoryAllocator allocator )
{
    return new( allocator ) RadSoundEffectPs3Stub();
}

static RadSoundPs3InitOptions s_Ps3InitOptions;

void radSoundPs3GetSystemInfo( RadSoundPs3SystemInfo* pInfo )
{
    if( pInfo )
    {
        pInfo->totalVoices = s_Ps3InitOptions.maxVoices;
        pInfo->activeVoices = 0;
        pInfo->totalStreams = s_Ps3InitOptions.maxStreams;
        pInfo->activeStreams = 0;
        pInfo->audioMemoryTotal = s_Ps3InitOptions.audioBufferSize;
        pInfo->audioMemoryUsed = 0;
        pInfo->spuThreadUsage = 0;
    }
}

void radSoundPs3SetInitOptions( const RadSoundPs3InitOptions* pOptions )
{
    if( pOptions )
    {
        s_Ps3InitOptions = *pOptions;
    }
}

//=============================================================================
// High-level radsound stubs
// These are the factories from radsound.hpp
//=============================================================================

// Stub data source that does nothing
class RadSoundDataSourceStub : public IRadSoundHalDataSource, public RadSoundStubObject
{
public:
    IMPLEMENT_REFCOUNTED( "RadSoundDataSourceStub" )

    RadSoundDataSourceStub() : m_State( Initialized ) {}

    virtual State GetState() { return m_State; }
    virtual IRadSoundHalAudioFormat * GetFormat() { return m_xFormat; }
    virtual unsigned int GetRemainingFrames() { return 0; }
    virtual void GetFramesAsync( void * pBytes, radMemorySpace destSpace,
        unsigned int numFrames, IRadSoundHalDataSourceCallback * pCallback )
    {
        if( pCallback ) pCallback->OnDataSourceFramesLoaded( 0 );
    }
    virtual const char * GetName() { return "stub"; }
    virtual unsigned int GetAvailableFrames() { return 0; }

    void SetFormat( IRadSoundHalAudioFormat * pFormat ) { m_xFormat = pFormat; }

private:
    State m_State;
    ref< IRadSoundHalAudioFormat > m_xFormat;
};

// Clip stub
class RadSoundClipStub : public IRadSoundClip, public RadSoundStubObject
{
public:
    IMPLEMENT_REFCOUNTED( "RadSoundClipStub" )

    RadSoundClipStub() : m_State( NoFile ), m_Looping( false ), m_Trim( 1.0f ) {}

    virtual void Initialize( IRadSoundHalDataSource * pDs, IRadSoundHalMemoryRegion * pRegion,
        bool looping, const char * pName )
    {
        m_Looping = looping;
        m_State = Initialized;
        if( pDs ) m_xFormat = pDs->GetFormat();
    }

    virtual IRadSoundHalMemoryRegion * GetMemoryRegion() { return NULL; }
    virtual bool GetLooping() { return m_Looping; }
    virtual IRadSoundHalAudioFormat * GetFormat() { return m_xFormat; }
    virtual void SetTrim( float t ) { m_Trim = t; }
    virtual float GetTrim() { return m_Trim; }
    virtual State GetState() { return m_State; }
    virtual bool WillRelease() { return false; }

private:
    State m_State;
    bool m_Looping;
    float m_Trim;
    ref< IRadSoundHalAudioFormat > m_xFormat;
};

// Clip Player stub
class RadSoundClipPlayerStub : public IRadSoundClipPlayer, public RadSoundStubObject
{
public:
    IMPLEMENT_REFCOUNTED( "RadSoundClipPlayerStub" )

    RadSoundClipPlayerStub() : m_State( NoClip ), m_Priority( 5 ), m_Muted( false ),
        m_Volume( 1.0f ), m_Trim( 1.0f ), m_Pitch( 1.0f ), m_Pan( 0.0f )
    {
        m_AuxMode[0] = m_AuxMode[1] = radSoundAuxMode_Off;
        m_AuxGain[0] = m_AuxGain[1] = 0.0f;
    }

    virtual void Play() { if( m_xClip ) m_State = Playing; }
    virtual void Stop() { m_State = Stopped; }
    virtual bool IsPlaying() { return m_State == Playing; }
    virtual void SetPriority( unsigned int p ) { m_Priority = p; }
    virtual unsigned int GetPriority() { return m_Priority; }
    virtual void SetMuted( bool m ) { m_Muted = m; }
    virtual bool GetMuted() { return m_Muted; }
    virtual void SetVolume( float v ) { m_Volume = v; }
    virtual float GetVolume() { return m_Volume; }
    virtual void SetTrim( float t ) { m_Trim = t; }
    virtual float GetTrim() { return m_Trim; }
    virtual void SetPitch( float p ) { m_Pitch = p; }
    virtual float GetPitch() { return m_Pitch; }
    virtual void SetPan( float p ) { m_Pan = p; }
    virtual float GetPan() { return m_Pan; }
    virtual void SetAuxMode( unsigned int aux, radSoundAuxMode mode ) { if( aux < 2 ) m_AuxMode[aux] = mode; }
    virtual radSoundAuxMode GetAuxMode( unsigned int aux ) { return aux < 2 ? m_AuxMode[aux] : radSoundAuxMode_Off; }
    virtual void SetAuxGain( unsigned int aux, float gain ) { if( aux < 2 ) m_AuxGain[aux] = gain; }
    virtual float GetAuxGain( unsigned int aux ) { return aux < 2 ? m_AuxGain[aux] : 0.0f; }
    virtual void SetPositionalGroup( IRadSoundHalPositionalGroup * pGroup ) { m_xPosGroup = pGroup; }
    virtual IRadSoundHalPositionalGroup * GetPositionalGroup() { return m_xPosGroup; }
    virtual unsigned int GetPlaybackTimeInSamples() { return 0; }
    virtual void SetClip( IRadSoundClip * pClip ) { m_xClip = pClip; if( pClip ) m_State = Stopped; else m_State = NoClip; }
    virtual IRadSoundClip * GetClip() { return m_xClip; }
    virtual State GetState() { return m_State; }
    virtual void SetPlaybackPosition( unsigned int pos, IRadSoundHalAudioFormat::SizeType st ) {}

private:
    State m_State;
    unsigned int m_Priority;
    bool m_Muted;
    float m_Volume, m_Trim, m_Pitch, m_Pan;
    radSoundAuxMode m_AuxMode[2];
    float m_AuxGain[2];
    ref< IRadSoundClip > m_xClip;
    ref< IRadSoundHalPositionalGroup > m_xPosGroup;
};

// Stream Player stub
class RadSoundStreamPlayerStub : public IRadSoundStreamPlayer, public RadSoundStubObject
{
public:
    IMPLEMENT_REFCOUNTED( "RadSoundStreamPlayerStub" )

    RadSoundStreamPlayerStub() : m_State( NoSource ), m_Priority( 5 ), m_Muted( false ),
        m_Volume( 1.0f ), m_Trim( 1.0f ), m_Pitch( 1.0f ), m_Pan( 0.0f ), m_LowWater( 0.25f )
    {
        m_AuxMode[0] = m_AuxMode[1] = radSoundAuxMode_Off;
        m_AuxGain[0] = m_AuxGain[1] = 0.0f;
    }

    virtual void Initialize( IRadSoundHalAudioFormat * pFormat, unsigned int size,
        IRadSoundHalAudioFormat::SizeType st, IRadSoundHalMemoryRegion * pRegion, const char * pId )
    {
        m_xFormat = pFormat;
    }

    virtual void InitializeAsync( unsigned int size, IRadSoundHalAudioFormat::SizeType st,
        IRadSoundHalMemoryRegion * pRegion, const char * pId ) {}

    virtual void SetLowWaterMark( float lw ) { m_LowWater = lw; }
    virtual float GetLowWaterMark() { return m_LowWater; }
    virtual IRadSoundHalAudioFormat * GetFormat() { return m_xFormat; }
    virtual void SetDataSource( IRadSoundHalDataSource * pDs ) { m_xDataSource = pDs; if( pDs ) m_State = Paused; else m_State = NoSource; }
    virtual IRadSoundHalDataSource * GetDataSource() { return m_xDataSource; }
    virtual State GetState() { return m_State; }

    virtual void Play() { if( m_xDataSource ) m_State = Playing; }
    virtual void Stop() { m_State = Paused; }
    virtual bool IsPlaying() { return m_State == Playing; }
    virtual void SetPriority( unsigned int p ) { m_Priority = p; }
    virtual unsigned int GetPriority() { return m_Priority; }
    virtual void SetMuted( bool m ) { m_Muted = m; }
    virtual bool GetMuted() { return m_Muted; }
    virtual void SetVolume( float v ) { m_Volume = v; }
    virtual float GetVolume() { return m_Volume; }
    virtual void SetTrim( float t ) { m_Trim = t; }
    virtual float GetTrim() { return m_Trim; }
    virtual void SetPitch( float p ) { m_Pitch = p; }
    virtual float GetPitch() { return m_Pitch; }
    virtual void SetPan( float p ) { m_Pan = p; }
    virtual float GetPan() { return m_Pan; }
    virtual void SetAuxMode( unsigned int aux, radSoundAuxMode mode ) { if( aux < 2 ) m_AuxMode[aux] = mode; }
    virtual radSoundAuxMode GetAuxMode( unsigned int aux ) { return aux < 2 ? m_AuxMode[aux] : radSoundAuxMode_Off; }
    virtual void SetAuxGain( unsigned int aux, float gain ) { if( aux < 2 ) m_AuxGain[aux] = gain; }
    virtual float GetAuxGain( unsigned int aux ) { return aux < 2 ? m_AuxGain[aux] : 0.0f; }
    virtual void SetPositionalGroup( IRadSoundHalPositionalGroup * pGroup ) { m_xPosGroup = pGroup; }
    virtual IRadSoundHalPositionalGroup * GetPositionalGroup() { return m_xPosGroup; }
    virtual unsigned int GetPlaybackTimeInSamples() { return 0; }

private:
    State m_State;
    unsigned int m_Priority;
    bool m_Muted;
    float m_Volume, m_Trim, m_Pitch, m_Pan, m_LowWater;
    radSoundAuxMode m_AuxMode[2];
    float m_AuxGain[2];
    ref< IRadSoundHalAudioFormat > m_xFormat;
    ref< IRadSoundHalDataSource > m_xDataSource;
    ref< IRadSoundHalPositionalGroup > m_xPosGroup;
};

// RSD File Data Source stub
class RadSoundRsdFileDataSourceStub : public IRadSoundRsdFileDataSource, public RadSoundStubObject
{
public:
    IMPLEMENT_REFCOUNTED( "RadSoundRsdFileDataSourceStub" )

    RadSoundRsdFileDataSourceStub() : m_State( Initialized ) {}

    virtual void InitializeFromFile( IRadFile * pFile, unsigned int pos,
        IRadSoundHalAudioFormat::SizeType posType, IRadSoundHalAudioFormat * pFormat )
    {
        m_xFormat = pFormat;
    }

    virtual void InitializeFromFileName( const char * pFileName, bool highPriority,
        unsigned int pos, IRadSoundHalAudioFormat::SizeType posType, IRadSoundHalAudioFormat * pFormat )
    {
        m_xFormat = pFormat;
    }

    virtual State GetState() { return m_State; }
    virtual IRadSoundHalAudioFormat * GetFormat() { return m_xFormat; }
    virtual unsigned int GetRemainingFrames() { return 0; }
    virtual void GetFramesAsync( void * pBytes, radMemorySpace destSpace,
        unsigned int numFrames, IRadSoundHalDataSourceCallback * pCallback )
    {
        if( pCallback ) pCallback->OnDataSourceFramesLoaded( 0 );
    }
    virtual const char * GetName() { return "rsd_stub"; }
    virtual unsigned int GetAvailableFrames() { return 0; }

private:
    State m_State;
    ref< IRadSoundHalAudioFormat > m_xFormat;
};

// Buffered Data Source stub
class RadSoundBufferedDataSourceStub : public IRadSoundBufferedDataSource, public RadSoundStubObject
{
public:
    IMPLEMENT_REFCOUNTED( "RadSoundBufferedDataSourceStub" )

    RadSoundBufferedDataSourceStub() : m_State( Initialized ), m_LowWater( 0.25f ) {}

    virtual void Initialize( radMemorySpace bufSpace, IRadMemoryAllocator * pAlloc,
        unsigned int size, IRadSoundHalAudioFormat::SizeType st,
        IRadSoundHalAudioFormat * pFormat, const char * pId )
    {
        m_xFormat = pFormat;
    }

    virtual void SetInputDataSource( IRadSoundHalDataSource * pDs ) { m_xInputDs = pDs; }
    virtual IRadSoundHalDataSource * GetInputDataSource() { return m_xInputDs; }
    virtual void SetLowWaterMark( float lw ) { m_LowWater = lw; }
    virtual float GetLowWaterMark() { return m_LowWater; }
    virtual bool IsBufferFull() { return true; }

    virtual State GetState() { return m_State; }
    virtual IRadSoundHalAudioFormat * GetFormat() { return m_xFormat; }
    virtual unsigned int GetRemainingFrames() { return 0; }
    virtual void GetFramesAsync( void * pBytes, radMemorySpace destSpace,
        unsigned int numFrames, IRadSoundHalDataSourceCallback * pCallback )
    {
        if( pCallback ) pCallback->OnDataSourceFramesLoaded( 0 );
    }
    virtual const char * GetName() { return "buffered_stub"; }
    virtual unsigned int GetAvailableFrames() { return 0; }

private:
    State m_State;
    float m_LowWater;
    ref< IRadSoundHalAudioFormat > m_xFormat;
    ref< IRadSoundHalDataSource > m_xInputDs;
};

// Stitched Data Source stub
class RadSoundStitchedDataSourceStub : public IRadSoundStitchedDataSource, public RadSoundStubObject
{
public:
    IMPLEMENT_REFCOUNTED( "RadSoundStitchedDataSourceStub" )

    RadSoundStitchedDataSourceStub() : m_State( Initialized ) {}

    virtual void InitializeFromAudioFormat( IRadSoundHalAudioFormat * pFormat ) { m_xFormat = pFormat; }
    virtual void InitializeFromDataSource( IRadSoundHalDataSource * pDs ) { if( pDs ) m_xFormat = pDs->GetFormat(); }
    virtual void SetStitchCallback( IRadSoundStitchCallback * pCallback, void * pUserData ) {}
    virtual void Reset() {}
    virtual void ResetAudioFormat( IRadSoundHalAudioFormat * pFormat ) { m_xFormat = pFormat; }

    virtual State GetState() { return m_State; }
    virtual IRadSoundHalAudioFormat * GetFormat() { return m_xFormat; }
    virtual unsigned int GetRemainingFrames() { return 0xFFFFFFFF; }
    virtual void GetFramesAsync( void * pBytes, radMemorySpace destSpace,
        unsigned int numFrames, IRadSoundHalDataSourceCallback * pCallback )
    {
        if( pCallback ) pCallback->OnDataSourceFramesLoaded( 0 );
    }
    virtual const char * GetName() { return "stitched_stub"; }
    virtual unsigned int GetAvailableFrames() { return 0; }

private:
    State m_State;
    ref< IRadSoundHalAudioFormat > m_xFormat;
};

// Memory Data Source stub
class RadSoundMemoryDataSourceStub : public IRadSoundMemoryDataSource, public RadSoundStubObject
{
public:
    IMPLEMENT_REFCOUNTED( "RadSoundMemoryDataSourceStub" )

    RadSoundMemoryDataSourceStub() : m_State( Initialized ) {}

    virtual void Initialize( IRadSoundHalAudioFormat * pFormat, IRadSoundMemorySpaceObject * pMemObj,
        unsigned int startOffset, unsigned int size, IRadSoundHalAudioFormat::SizeType st )
    {
        m_xFormat = pFormat;
    }

    virtual State GetState() { return m_State; }
    virtual IRadSoundHalAudioFormat * GetFormat() { return m_xFormat; }
    virtual unsigned int GetRemainingFrames() { return 0; }
    virtual void GetFramesAsync( void * pBytes, radMemorySpace destSpace,
        unsigned int numFrames, IRadSoundHalDataSourceCallback * pCallback )
    {
        if( pCallback ) pCallback->OnDataSourceFramesLoaded( 0 );
    }
    virtual const char * GetName() { return "memory_stub"; }
    virtual unsigned int GetAvailableFrames() { return 0; }

private:
    State m_State;
    ref< IRadSoundHalAudioFormat > m_xFormat;
};

// Memory Space Object stub
class RadSoundMemorySpaceObjectStub : public IRadSoundMemorySpaceObject, public RadSoundStubObject
{
public:
    IMPLEMENT_REFCOUNTED( "RadSoundMemorySpaceObjectStub" )

    RadSoundMemorySpaceObjectStub() : m_pAddress( NULL ), m_Size( 0 ), m_Space( radMemorySpace_Local ) {}

    virtual void Initialize( radMemorySpace space, IRadMemoryAllocator * pAlloc,
        unsigned int numBytes, unsigned int alignment, const char * pId )
    {
        m_Space = space;
        m_Size = numBytes;
    }

    virtual void * GetAddress() { return m_pAddress; }
    virtual IRadMemoryAllocator * GetAllocator() { return NULL; }
    virtual radMemorySpace GetMemorySpace() { return m_Space; }
    virtual unsigned int GetSizeInBytes() { return m_Size; }
    virtual const char * GetIdentifier() { return "mem_stub"; }

private:
    void * m_pAddress;
    unsigned int m_Size;
    radMemorySpace m_Space;
};

// Data Cache stub
class RadSoundDataCacheStub : public IRadSoundDataCache, public RadSoundStubObject
{
public:
    IMPLEMENT_REFCOUNTED( "RadSoundDataCacheStub" )

    RadSoundDataCacheStub() : m_State( Idle ) {}

    virtual void InitializeAsync( radMemorySpace space, IRadMemoryAllocator * pAlloc,
        IRadSoundHalDataSource * pDs, const char * pId )
    {
        if( pDs ) m_xFormat = pDs->GetFormat();
        m_State = Initialized;
    }

    virtual State GetState() { return m_State; }
    virtual IRadSoundHalDataSource * CreateDataSource( unsigned int startFrame, radMemoryAllocator alloc )
    {
        RadSoundDataSourceStub * pDs = new( alloc ) RadSoundDataSourceStub();
        pDs->SetFormat( m_xFormat );
        return pDs;
    }
    virtual bool WillRelease() { return false; }

private:
    State m_State;
    ref< IRadSoundHalAudioFormat > m_xFormat;
};

// ADPCM Decode Stream stub
class RadSoundAdpcmDecodeStreamStub : public IRadSoundAdpcmDecodeStream, public RadSoundStubObject
{
public:
    IMPLEMENT_REFCOUNTED( "RadSoundAdpcmDecodeStreamStub" )

    RadSoundAdpcmDecodeStreamStub() : m_State( Initialized ) {}

    virtual void Initialize( IRadSoundHalDataSource * pDs )
    {
        m_xInputDs = pDs;
        if( pDs ) m_xFormat = pDs->GetFormat();
    }

    virtual State GetState() { return m_State; }
    virtual IRadSoundHalAudioFormat * GetFormat() { return m_xFormat; }
    virtual unsigned int GetRemainingFrames() { return m_xInputDs ? m_xInputDs->GetRemainingFrames() : 0; }
    virtual void GetFramesAsync( void * pBytes, radMemorySpace destSpace,
        unsigned int numFrames, IRadSoundHalDataSourceCallback * pCallback )
    {
        if( pCallback ) pCallback->OnDataSourceFramesLoaded( 0 );
    }
    virtual const char * GetName() { return "adpcm_stub"; }
    virtual unsigned int GetAvailableFrames() { return 0; }

private:
    State m_State;
    ref< IRadSoundHalAudioFormat > m_xFormat;
    ref< IRadSoundHalDataSource > m_xInputDs;
};

//=============================================================================
// radsound.hpp Factory Functions
//=============================================================================

IRadSoundClip * radSoundClipCreate( radMemoryAllocator alloc )
{
    return new( alloc ) RadSoundClipStub();
}

IRadSoundClipPlayer * radSoundClipPlayerCreate( radMemoryAllocator alloc )
{
    return new( alloc ) RadSoundClipPlayerStub();
}

IRadSoundStreamPlayer * radSoundStreamPlayerCreate( radMemoryAllocator alloc )
{
    return new( alloc ) RadSoundStreamPlayerStub();
}

IRadSoundRsdFileDataSource * radSoundRsdFileDataSourceCreate( radMemoryAllocator alloc )
{
    return new( alloc ) RadSoundRsdFileDataSourceStub();
}

IRadSoundBufferedDataSource * radSoundBufferedDataSourceCreate( radMemoryAllocator alloc )
{
    return new( alloc ) RadSoundBufferedDataSourceStub();
}

IRadSoundStitchedDataSource * radSoundStitchedDataSourceCreate( radMemoryAllocator alloc )
{
    return new( alloc ) RadSoundStitchedDataSourceStub();
}

IRadSoundMemoryDataSource * radSoundMemoryDataSourceCreate( radMemoryAllocator alloc )
{
    return new( alloc ) RadSoundMemoryDataSourceStub();
}

IRadSoundMemorySpaceObject * radSoundMemorySpaceObjectCreate( radMemoryAllocator alloc )
{
    return new( alloc ) RadSoundMemorySpaceObjectStub();
}

IRadSoundDataCache * radSoundDataCacheCreate( radMemoryAllocator alloc )
{
    return new( alloc ) RadSoundDataCacheStub();
}

IRadSoundAdpcmDecodeStream * radSoundAdpcmDecodeStreamCreate( radMemoryAllocator alloc )
{
    return new( alloc ) RadSoundAdpcmDecodeStreamStub();
}
