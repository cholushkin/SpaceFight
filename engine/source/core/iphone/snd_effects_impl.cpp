#if !TARGET_IPHONE_SIMULATOR
/*==================================================================================================
SoundEngine.cpp
==================================================================================================*/
#if !defined(__SoundEngine_cpp__)
#define __SoundEngine_cpp__

//==================================================================================================
//	Includes
//==================================================================================================

//	System Includes
#include <AudioToolbox/AudioToolbox.h>
#include <CoreFoundation/CFURL.h>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <map>
#include <vector>
#include <pthread.h>
#include <mach/mach.h>

// Local Includes
#include "snd_effects_impl.h"
#include "snd_music_impl.h"

#include "getCompressedAudioAsPcm.h"

#if MACOS_PLATFORM
#include <Carbon/Carbon.h>
void AudioSessionSetActive(bool){}
#endif

#define	AssertNoError(inMessage, inHandler)    \
  if(result != noErr)                          \
{                                              \
  printf("%s: %d\n", inMessage, (int)result);  \
  goto inHandler;                              \
}

#define AssertNoOALError(inMessage, inHandler) \
  if((result = alGetError()) != AL_NO_ERROR)   \
{                                              \
  printf("%s: %x\n", inMessage, (int)result);  \
  goto inHandler;                              \
}

class OpenALObject;

static OpenALObject*        sOpenALObject       = NULL;
static Float32              gMasterVolumeGain   = 1.0;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
typedef ALvoid	AL_APIENTRY	(*alBufferDataStaticProcPtr) (const ALint bid, ALenum format, ALvoid* data, ALsizei size, ALsizei freq);

ALvoid  alBufferDataStaticProc(const ALint bid, ALenum format, ALvoid* data, ALsizei size, ALsizei freq)
{
  static	alBufferDataStaticProcPtr	proc = NULL;

  if (proc == NULL)
    proc = (alBufferDataStaticProcPtr) alcGetProcAddress(NULL, (const ALCchar*) "alBufferDataStatic");

  if (proc)
    proc(bid, format, data, size, freq);

  return;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
typedef ALvoid	AL_APIENTRY	(*alcMacOSXMixerOutputRateProcPtr) (const ALdouble value);
ALvoid  alcMacOSXMixerOutputRateProc(const ALdouble value)
{
  static	alcMacOSXMixerOutputRateProcPtr	proc = NULL;

  if (proc == NULL)
    proc = (alcMacOSXMixerOutputRateProcPtr) alcGetProcAddress(NULL, (const ALCchar*) "alcMacOSXMixerOutputRate");

  if (proc)
    proc(value);

  return;
}

#pragma mark ***** OpenALThread *****
//==================================================================================================
//	Threading functions
//==================================================================================================
class	OpenALThread
{
  // returns the thread's priority as it was last set by the API
#define OpenALThread_SET_PRIORITY        0
  // returns the thread's priority as it was last scheduled by the Kernel
#define OpenALThread_SCHEDULED_PRIORITY  1

  //	Types
public:
  typedef void* (*ThreadRoutine)(void* inParameter);

  //	Constants
public:
  enum
  {
    kMinThreadPriority     = 1,
    kMaxThreadPriority     = 63,
    kDefaultThreadPriority = 31
  };

  // Construction/Destruction
public:
  OpenALThread(ThreadRoutine inThreadRoutine, void* inParameter)
    : mPThread(0)
    , mSpawningThreadPriority(getScheduledPriority(pthread_self(), OpenALThread_SET_PRIORITY))
    , mThreadRoutine(inThreadRoutine)
    , mThreadParameter(inParameter)
    , mPriority(kDefaultThreadPriority)
    , mFixedPriority(false)
    , mAutoDelete(true) { }

  ~OpenALThread() { }

  //	Properties
  bool IsRunning() const { return 0 != mPThread; }
  void SetAutoDelete(bool b) { mAutoDelete = b; }

  void SetPriority(UInt32 inPriority, bool inFixedPriority)
  {
    OSStatus result = noErr;
    mPriority = inPriority;
    mFixedPriority = inFixedPriority;
    if(mPThread != 0)
    {
      if (mFixedPriority)
      {
        thread_extended_policy_data_t theFixedPolicy;
        theFixedPolicy.timeshare = false; // set to true for a non-fixed thread
        result  = thread_policy_set(pthread_mach_thread_np(mPThread), THREAD_EXTENDED_POLICY, (thread_policy_t)&theFixedPolicy, THREAD_EXTENDED_POLICY_COUNT);
        if (result) {
          printf("OpenALThread::SetPriority: failed to set the fixed-priority policy");
          return;
        }
      }
      // We keep a reference to the spawning thread's priority around (initialized in the constructor), 
      // and set the importance of the child thread relative to the spawning thread's priority.
      thread_precedence_policy_data_t thePrecedencePolicy;

      thePrecedencePolicy.importance = mPriority - mSpawningThreadPriority;
      result =thread_policy_set(pthread_mach_thread_np(mPThread), THREAD_PRECEDENCE_POLICY, (thread_policy_t)&thePrecedencePolicy, THREAD_PRECEDENCE_POLICY_COUNT);
      if (result) {
        printf("OpenALThread::SetPriority: failed to set the precedence policy");
        return;
      }
    } 
  }
  //	Actions
  void Start()
  {
    if(mPThread != 0)
    {
      printf("OpenALThread::Start: can't start because the thread is already running\n");
      return;
    }

    OSStatus result;
    pthread_attr_t theThreadAttributes;

    result = pthread_attr_init(&theThreadAttributes);
    AssertNoError("Error initializing thread", end);

    result = pthread_attr_setdetachstate(&theThreadAttributes, PTHREAD_CREATE_DETACHED);
    AssertNoError("Error setting thread detach state", end);

    result = pthread_create(&mPThread, &theThreadAttributes, (ThreadRoutine)OpenALThread::Entry, this);
    AssertNoError("Error creating thread", end);

    pthread_attr_destroy(&theThreadAttributes);
    AssertNoError("Error destroying thread attributes", end);
end:
    return;
  }

  // Implementation
protected:
  static void* Entry(OpenALThread* inOpenALThread)
  {
    void* theAnswer = NULL;

    inOpenALThread->SetPriority(inOpenALThread->mPriority, inOpenALThread->mFixedPriority);

    if(inOpenALThread->mThreadRoutine != NULL)
      theAnswer = inOpenALThread->mThreadRoutine(inOpenALThread->mThreadParameter);

    inOpenALThread->mPThread = 0;
    if (inOpenALThread->mAutoDelete)
      delete inOpenALThread;
    return theAnswer;
  }

  static UInt32 getScheduledPriority(pthread_t inThread, int inPriorityKind)
  {
    thread_basic_info_data_t  threadInfo;
    policy_info_data_t        thePolicyInfo;
    unsigned int              count;

    if (inThread == NULL)
      return 0;

    // get basic info
    count = THREAD_BASIC_INFO_COUNT;
    thread_info (pthread_mach_thread_np (inThread), THREAD_BASIC_INFO, (thread_info_t)&threadInfo, &count);

    switch (threadInfo.policy) {
    case POLICY_TIMESHARE:
      count = POLICY_TIMESHARE_INFO_COUNT;
      thread_info(pthread_mach_thread_np (inThread), THREAD_SCHED_TIMESHARE_INFO, (thread_info_t)&(thePolicyInfo.ts), &count);
      if (inPriorityKind == OpenALThread_SCHEDULED_PRIORITY) {
        return thePolicyInfo.ts.cur_priority;
      }
      return thePolicyInfo.ts.base_priority;
      break;

    case POLICY_FIFO:
      count = POLICY_FIFO_INFO_COUNT;
      thread_info(pthread_mach_thread_np (inThread), THREAD_SCHED_FIFO_INFO, (thread_info_t)&(thePolicyInfo.fifo), &count);
      if ( (thePolicyInfo.fifo.depressed) && (inPriorityKind == OpenALThread_SCHEDULED_PRIORITY) ) {
        return thePolicyInfo.fifo.depress_priority;
      }
      return thePolicyInfo.fifo.base_priority;
      break;

    case POLICY_RR:
      count = POLICY_RR_INFO_COUNT;
      thread_info(pthread_mach_thread_np (inThread), THREAD_SCHED_RR_INFO, (thread_info_t)&(thePolicyInfo.rr), &count);
      if ( (thePolicyInfo.rr.depressed) && (inPriorityKind == OpenALThread_SCHEDULED_PRIORITY) ) {
        return thePolicyInfo.rr.depress_priority;
      }
      return thePolicyInfo.rr.base_priority;
      break;
    }

    return 0;
  }

  pthread_t      mPThread;
  UInt32         mSpawningThreadPriority;
  ThreadRoutine  mThreadRoutine;
  void*          mThreadParameter;
  SInt32         mPriority;
  bool           mFixedPriority;
  bool           mAutoDelete; //  delete self when thread terminates
};

//==================================================================================================
//	Helper functions
//==================================================================================================
OSStatus OpenFile(const char *inFilePath, AudioFileID &outAFID)
{

  CFURLRef theURL = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault, (UInt8*)inFilePath, strlen(inFilePath), false);
  if (theURL == NULL)
    return kSoundEngineErrFileNotFound;

#if TARGET_OS_IPHONE
  OSStatus result = AudioFileOpenURL(theURL, kAudioFileReadPermission, 0, &outAFID);
#else
  OSStatus result = AudioFileOpenURL(theURL, fsRdPerm, 0, &outAFID);
#endif
  CFRelease(theURL);
  AssertNoError("Error opening file", end);
end:
  return result;
}

OSStatus LoadFileDataInfo(const char *inFilePath, AudioFileID &outAFID, AudioStreamBasicDescription &outFormat, UInt64 &outDataSize)
{
  UInt32 thePropSize = sizeof(outFormat);
  OSStatus result = OpenFile(inFilePath, outAFID);
  AssertNoError("Error opening file", end);

  result = AudioFileGetProperty(outAFID, kAudioFilePropertyDataFormat, &thePropSize, &outFormat);
  AssertNoError("Error getting file format", end);

  thePropSize = sizeof(UInt64);
  result = AudioFileGetProperty(outAFID, kAudioFilePropertyAudioDataByteCount, &thePropSize, &outDataSize);
  AssertNoError("Error getting file data size", end);

end:
  return result;
}

#pragma mark ***** SoundEngineEffect *****
//==================================================================================================
//	SoundEngineEffect class
//==================================================================================================
class SoundEngineEffect
{
public:	
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  SoundEngineEffect(const char* inLoopPath, const char* inAttackPath, const char* inDecayPath, Boolean inDoLoop) 
    : mSourceID(0)
    , mAttackBufferID(0)
    , mLoopBufferID(0)
    , mDecayBufferID(0)
    , mLoopPath(inLoopPath)
    , mAttackPath(inAttackPath)
    , mDecayPath(inDecayPath)
    , mLoopData(NULL)
    , mAttackData(NULL)
    , mDecayData(NULL)
    , mLoopDataSize(0)
    , mAttackDataSize(0)
    , mDecayDataSize(0)
    , mIsLoopingEffect(inDoLoop)
    , mPlayThread(NULL)
    , mPlayThreadState(kPlayThreadState_Loop)
  { alGenSources(1, &mSourceID); }

  ~SoundEngineEffect()
  {
    alSourceStop(mSourceID);
    alDeleteSources(1, &mSourceID);
    alDeleteBuffers(1, &mLoopBufferID);
    alDeleteBuffers(1, &mAttackBufferID);
    alDeleteBuffers(1, &mDecayBufferID);
    if (mLoopData)
      free(mLoopData);
    if (mAttackData)
      free(mAttackData);
    if (mDecayData)
      free(mDecayData);
  }

  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Accessors
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  UInt32  GetEffectID()        { return mSourceID; }
  UInt32  GetPlayThreadState() { return mPlayThreadState; }
  Boolean HasAttackBuffer()    { return mAttackBufferID != 0; }

  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Helper Functions
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  ALenum GetALFormat(AudioStreamBasicDescription inFileFormat)
  {
    if (inFileFormat.mFormatID != kAudioFormatLinearPCM)
      return kSoundEngineErrInvalidFileFormat;

    if ((inFileFormat.mChannelsPerFrame > 2) || (inFileFormat.mChannelsPerFrame < 1))
      return kSoundEngineErrInvalidFileFormat;

    if(inFileFormat.mBitsPerChannel == 8)
      return (inFileFormat.mChannelsPerFrame == 1) ? AL_FORMAT_MONO8  : AL_FORMAT_STEREO8;
    else if(inFileFormat.mBitsPerChannel == 16)
      return (inFileFormat.mChannelsPerFrame == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

    return kSoundEngineErrInvalidFileFormat;
  }

  OSStatus LoadFileData(const char *inFilePath, void* &outData, UInt32 &outDataSize, ALuint &outBufferID)
  {
    AudioFileID theAFID = 0;
    OSStatus result = noErr;
    UInt64 theFileSize = 0;
    AudioStreamBasicDescription theFileFormat;

    if(NULL != strstr(inFilePath,".mp3"))
    {
      UInt32 getSampleRate = 44100;
      UInt32 AlFormat = 0;
      if(!getCompressedAudioAsPcm(inFilePath, false, outData, outDataSize, getSampleRate, AlFormat))
        return kSoundEngineErrInvalidFileFormat;
      alGenBuffers(1, &outBufferID);
      AssertNoOALError("Error generating buffer\n", fail);
      alBufferData(outBufferID, AlFormat, outData, outDataSize, getSampleRate);
      AssertNoOALError("Error attaching data to buffer\n", fail);
      return result;
    }

    result = LoadFileDataInfo(inFilePath, theAFID, theFileFormat, theFileSize);
    outDataSize = (UInt32)theFileSize;
    AssertNoError("Error loading file info", fail);

    outData = malloc(outDataSize);

    result = AudioFileReadBytes(theAFID, false, 0, &outDataSize, outData);
    AssertNoError("Error reading file data", fail);

    if (!TestAudioFormatNativeEndian(theFileFormat) && (theFileFormat.mBitsPerChannel > 8)) 
      return kSoundEngineErrInvalidFileFormat;

    alGenBuffers(1, &outBufferID);
    AssertNoOALError("Error generating buffer\n", fail);

    alBufferData(outBufferID, GetALFormat(theFileFormat), outData, outDataSize, theFileFormat.mSampleRate);
    //alBufferDataStaticProc(outBufferID, GetALFormat(theFileFormat), outData, outDataSize, theFileFormat.mSampleRate);
    AssertNoOALError("Error attaching data to buffer\n", fail);

    AudioFileClose(theAFID);
    return result;

fail:			
    if (theAFID)
      AudioFileClose(theAFID);
    if (outData)
    {
      free(outData);
      outData = NULL;
    }
    return result;
  }

  OSStatus AttachFilesToSource()
  {
    OSStatus result = AL_NO_ERROR;			
    // first check for the attack file. That will be first in the queue if present
    if (mAttackPath)
    {
      result = LoadFileData(mAttackPath, mAttackData, mAttackDataSize, mAttackBufferID);
      AssertNoError("Error loading attack file info", end)
    }

    result = LoadFileData(mLoopPath, mLoopData, mLoopDataSize, mLoopBufferID);
    AssertNoError("Error loading looping file info", end)

    // if one-shot effect, attach the buffer to the source now
    if (!mIsLoopingEffect)
    {
      alSourcei(mSourceID, AL_BUFFER, mLoopBufferID);
      AssertNoOALError("Error attaching file data to effect", end)
    }

    if (mDecayPath)
    {
      result = LoadFileData(mDecayPath, mDecayData, mDecayDataSize, mDecayBufferID);
      AssertNoError("Error loading decay file info", end)
    }
end:
      return result;
  }

  OSStatus ClearSourceBuffers()
  {
    OSStatus result = AL_NO_ERROR;
    ALint numQueuedBuffers = 0;
    ALuint *bufferIDs = (ALuint*)malloc(numQueuedBuffers * sizeof(ALint));
    alGetSourcei(mSourceID, AL_BUFFERS_QUEUED, &numQueuedBuffers);
    AssertNoOALError("Error getting OpenAL queued buffer size", end)

    alSourceUnqueueBuffers(mSourceID, numQueuedBuffers, bufferIDs);
    AssertNoOALError("Error unqueueing buffers from source", end);

end:
    free(bufferIDs);
    return result;
  }

  static void* PlaybackProc(void *args)
  {
    OSStatus result = AL_NO_ERROR;
    SoundEngineEffect *THIS = (SoundEngineEffect*)args;

    alSourcePlay(THIS->GetEffectID());
    AssertNoOALError("Error starting effect playback", end);

    // if attack buffer is present, wait until it has completed, then turn looping on
    if (THIS->HasAttackBuffer())
    {
      ALint numBuffersProcessed = 0;						
      while (numBuffersProcessed < 1)
      {
        alGetSourcei(THIS->GetEffectID(), AL_BUFFERS_PROCESSED, &numBuffersProcessed);
        AssertNoOALError("Error getting processed buffer number", end);
      }

      ALuint tmpBuffer = 0;
      alSourceUnqueueBuffers(THIS->GetEffectID(), 1, &tmpBuffer);
      AssertNoOALError("Error unqueueing buffers from source", end);
    }
    // now that we have processed the attack buffer, loop the main one
    THIS->SetLooping(true);

end:
    return NULL;
  }

  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Effect management
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  OSStatus Start(bool looped)
  {
    OSStatus result = AL_NO_ERROR;
    alSourceStop(mSourceID);
    AssertNoOALError("Error stopping source", end)

      mIsLoopingEffect = looped;
      if (!mIsLoopingEffect)
      {
        // if we are just playing one-short effects, start playback here
        alSourcePlay(mSourceID);
        return alGetError();
      }
      // for loops we need to spawn a new thread
      mPlayThread = new OpenALThread(PlaybackProc, (void*)this);
      // we want this to delete upon thread completion
      mPlayThreadState = kPlayThreadState_Loop;
      // clean up remnants from any previous playback of the source
      result = ClearSourceBuffers();
      AssertNoError("Error clearing buffers", end)

      // if the effect has an attack sample, queue this first
      if (HasAttackBuffer())
      {
        alSourceQueueBuffers(mSourceID, 1, &mAttackBufferID);
        AssertNoOALError("Error queueing buffers for attack", end);
        // turn on looping after the attack buffer has been processed
        SetLooping(false);
      }

      alSourceQueueBuffers(mSourceID, 1, &mLoopBufferID);
      AssertNoOALError("Error queueing looping buffer", end);
      mPlayThread->Start();
end:
      return result;
  }

  OSStatus StartDecay()
  {
    // turn off looping, and queue the decay buffer
    OSStatus result = AL_NO_ERROR;
    alSourcei(mSourceID, AL_LOOPING, 0);
    AssertNoOALError("Error turning off looping", end);
    alSourceQueueBuffers(mSourceID, 1, &mDecayBufferID);
    AssertNoOALError("Error queueing decay file", end);
end:
    return result;
  }

  OSStatus Stop(Boolean inDoDecay)
  {
    OSStatus result = AL_NO_ERROR;
    // for non looped effects and loops with no decay sample
    if ((mDecayBufferID == 0) || !inDoDecay)
    {
      // if no decay to play, just stop the source
      alSourceStop(mSourceID);
      AssertNoOALError("Error stopping source", end)
    }
    else
      return StartDecay();
end:
    return result;
  }

  OSStatus SetPitch(Float32 inValue)
  {
    alSourcef(mSourceID, AL_PITCH, inValue);
    return alGetError();
  }

  OSStatus SetLooping(Boolean inDoLoop)
  {
    ALint doLoop = inDoLoop ? 1 : 0;
    alSourcei(mSourceID, AL_LOOPING, doLoop);
    return alGetError();
  }

  OSStatus SetPosition(Float32 inX, Float32 inY, Float32 inZ)
  {
    alSource3f(mSourceID, AL_POSITION, inX, inY, inZ);
    return alGetError();
  }

  OSStatus SetMaxDistance(Float32 inValue)
  {
    alSourcef(mSourceID, AL_MAX_DISTANCE, inValue);
    return alGetError();
  }

  OSStatus SetReferenceDistance(Float32 inValue)
  {
    alSourcef(mSourceID, AL_REFERENCE_DISTANCE, inValue);
    return alGetError();
  }

  OSStatus SetLevel(Float32 inValue)
  {
    alSourcef(mSourceID, AL_GAIN, inValue * gMasterVolumeGain);
    return alGetError();
  }

  enum
  {
    kPlayThreadState_Loop   = 0,
    kPlayThreadState_Decay  = 1,
    kPlayThreadState_End    = 2
  };

private:
  ALuint        mSourceID;
  ALuint        mAttackBufferID;
  ALuint        mLoopBufferID;
  ALuint        mDecayBufferID;
  UInt32        mNumberBuffers;
  const char*   mLoopPath;
  const char*   mAttackPath;
  const char*   mDecayPath;
  void*         mLoopData;
  void*         mAttackData;
  void*         mDecayData;
  UInt32        mLoopDataSize;
  UInt32        mAttackDataSize;
  UInt32        mDecayDataSize;
  Boolean       mIsLoopingEffect;
  OpenALThread* mPlayThread;
  UInt32        mPlayThreadState;
};

#pragma mark ***** SoundEngineEffectMap *****
//==================================================================================================
//	SoundEngineEffectMap class
//==================================================================================================
class SoundEngineEffectMap 
  : std::multimap<UInt32, SoundEngineEffect*, std::less<ALuint> > 
{
public:
  // add a new context to the map
  void Add (const	ALuint	inEffectToken, SoundEngineEffect **inEffect)
  {
    iterator	it = upper_bound(inEffectToken);
    insert(it, value_type (inEffectToken, *inEffect));
  }

  SoundEngineEffect* Get(ALuint	inEffectToken) 
  {
    iterator	it = find(inEffectToken);
    if (it != end())
      return ((*it).second);
    return (NULL);
  }

  void Remove (const	ALuint	inSourceToken) {
    iterator 	it = find(inSourceToken);
    if (it != end())
      erase(it);
  }

  SoundEngineEffect* GetEffectByIndex(UInt32	inIndex) {
    iterator	it = begin();

    for (UInt32 i = 0; i < inIndex; i++) {
      if (it != end())
        ++it;
      else
        i = inIndex;
    }

    if (it != end())
      return ((*it).second);		
    return (NULL);
  }

  iterator GetIterator() { return begin(); }

  UInt32 Size  () const { return size(); }
  bool   Empty () const { return empty(); }
};

#pragma mark ***** OpenALObject *****
//==================================================================================================
//	OpenALObject class
//==================================================================================================
class OpenALObject
{	
public:	
  OpenALObject(Float32 inMixerOutputRate)
    :	mOutputRate(inMixerOutputRate),
    mGain(1.0),
    mContext(NULL),
    mDevice(NULL),
    mEffectsMap(NULL) 
  {
    mEffectsMap = new SoundEngineEffectMap();
  }

  ~OpenALObject() { Teardown(); }

  OSStatus Initialize()
  {
    OSStatus result = noErr;
    mDevice = alcOpenDevice(NULL);
    AssertNoOALError("Error opening output device", end)
      if(mDevice == NULL) { return kSoundEngineErrDeviceNotFound; }

      // if a mixer output rate was specified, set it here
      // must be done before the alcCreateContext() call
      if (mOutputRate)
        alcMacOSXMixerOutputRateProc(mOutputRate);

      // Create an OpenAL Context
      mContext = alcCreateContext(mDevice, NULL);
      AssertNoOALError("Error creating OpenAL context", end)

        alcMakeContextCurrent(mContext);
      AssertNoOALError("Error setting current OpenAL context", end)

end:
      return result;
  }

  void Teardown()
  {
    if (mEffectsMap) 
    {
      for (UInt32  i = 0; i < mEffectsMap->Size(); i++)
      {
        SoundEngineEffect	*theEffect = mEffectsMap->GetEffectByIndex(0);
        if (theEffect)
        {
          mEffectsMap->Remove(theEffect->GetEffectID());
          delete theEffect;
        }
      }
      delete mEffectsMap;
    }

    if (mContext)
    {
      // Cannot destroy the current context, so deselect it first.
      alcMakeContextCurrent(NULL);
      alcDestroyContext(mContext);
    }

    if (mDevice) alcCloseDevice(mDevice);

    mContext = NULL;
    mDevice = NULL;
  }

  OSStatus Interrupt()
  {
    alcMakeContextCurrent(NULL);
    alcSuspendContext(mContext);
    return alGetError();
  }

  OSStatus Resume()
  {
    alcMakeContextCurrent(mContext);
    alcProcessContext(mContext);
    return alGetError();
  }

  OSStatus SetListenerPosition(Float32 inX, Float32 inY, Float32 inZ)
  {
    alListener3f(AL_POSITION, inX, inY, inZ);
    return alGetError();
  }

  OSStatus SetListenerGain(Float32 inValue)
  {
    alListenerf(AL_GAIN, inValue);
    return alGetError();
  }

  OSStatus SetMaxDistance(Float32 inValue)
  {
    OSStatus result = 0;
    for (UInt32 i=0; i < mEffectsMap->Size(); i++)
    {
      SoundEngineEffect *theEffect = mEffectsMap->GetEffectByIndex(i);
      if ((result = theEffect->SetMaxDistance(inValue)) != AL_NO_ERROR)
        return result;
    }
    return result;			
  }

  OSStatus SetReferenceDistance(Float32 inValue)
  {
    OSStatus result = 0;
    for (UInt32 i=0; i < mEffectsMap->Size(); i++)
    {
      SoundEngineEffect *theEffect = mEffectsMap->GetEffectByIndex(i);
      if ((result = theEffect->SetReferenceDistance(inValue)) != AL_NO_ERROR)
        return result;
    }
    return result;	
  }

  OSStatus SetEffectsVolume(Float32 inValue)
  {
    OSStatus result = 0;
    for (UInt32 i=0; i < mEffectsMap->Size(); i++)
    {
      SoundEngineEffect *theEffect = mEffectsMap->GetEffectByIndex(i);
      if ((result = theEffect->SetLevel(inValue)) != AL_NO_ERROR)
        return result;
    }
    return result;	
  }

  OSStatus UpdateGain()
  {
    return SetEffectsVolume(mGain);
  }

  OSStatus LoadEffect(const char *inFilePath, UInt32 *outEffectID)
  {
    SoundEngineEffect *theEffect = new SoundEngineEffect(inFilePath, NULL, NULL, false);
    OSStatus result = theEffect->AttachFilesToSource();
    if (result == noErr)
    {
      *outEffectID = theEffect->GetEffectID();
      mEffectsMap->Add(*outEffectID, &theEffect);
    }
    else
    {
      delete theEffect;
    }
    return result;
  }

  OSStatus LoadLoopingEffect(const char *inLoopFilePath, const char *inAttackFilePath, const char *inDecayFilePath, UInt32 *outEffectID)
  {
    SoundEngineEffect *theEffect = new SoundEngineEffect(inLoopFilePath, inAttackFilePath, inDecayFilePath, true);
    OSStatus result = theEffect->AttachFilesToSource();
    if (result == noErr)
    {
      *outEffectID = theEffect->GetEffectID();
      mEffectsMap->Add(*outEffectID, &theEffect);
    }
    else
    {
      delete theEffect;
    }
    return result;
  }

  OSStatus UnloadEffect(UInt32 inEffectID)
  {
    SoundEngineEffect* effect = mEffectsMap->Get(inEffectID);
    effect->Stop(false);
    mEffectsMap->Remove(inEffectID);
    delete effect;
    return 0;
  }

  SoundEngineEffect* GetEffect(UInt32 inEffectID)
  {
    return mEffectsMap->Get(inEffectID);
  }

  OSStatus StartEffect(UInt32 inEffectID, bool looped)
  {
    SoundEngineEffect *theEffect = mEffectsMap->Get(inEffectID);
    return (theEffect) ? theEffect->Start(looped) : kSoundEngineErrInvalidID;
  }

  OSStatus StopEffect(UInt32 inEffectID, Boolean inDoDecay)
  {
    SoundEngineEffect *theEffect = mEffectsMap->Get(inEffectID);
    return (theEffect) ? theEffect->Stop(inDoDecay) : kSoundEngineErrInvalidID;
  }

  OSStatus SetEffectPitch(UInt32 inEffectID, Float32 inValue)
  {
    SoundEngineEffect *theEffect = mEffectsMap->Get(inEffectID);
    return (theEffect) ? theEffect->SetPitch(inValue) : kSoundEngineErrInvalidID;			
  }

  OSStatus SetEffectVolume(UInt32 inEffectID, Float32 inValue)
  {
    SoundEngineEffect *theEffect = mEffectsMap->Get(inEffectID);
    return (theEffect) ?  theEffect->SetLevel(inValue) : kSoundEngineErrInvalidID;
  }

  OSStatus	SetEffectPosition(UInt32 inEffectID, Float32 inX, Float32 inY, Float32 inZ)	
  {
    SoundEngineEffect *theEffect = mEffectsMap->Get(inEffectID);
    return (theEffect) ? theEffect->SetPosition(inX, inY, inZ) : kSoundEngineErrInvalidID;
  }

private:
  Float32                mOutputRate;
  Float32                mGain;
  ALCcontext*            mContext;
  ALCdevice*             mDevice;
  SoundEngineEffectMap*  mEffectsMap;
};

#pragma mark ***** API *****

//==================================================================================================
//	Sound Engine
//==================================================================================================

extern "C" OSStatus  SoundEngine_Initialize(Float32 inMixerOutputRate)
{
  if (sOpenALObject)
    delete sOpenALObject;

  sOpenALObject = new OpenALObject(inMixerOutputRate);

  return sOpenALObject->Initialize();
}

extern "C" OSStatus  SoundEngine_Teardown()
{
  if (sOpenALObject)
  {
    delete sOpenALObject;
    sOpenALObject = NULL;
  }
  MusicEngine_Teardown();
  return 0; 
}

extern "C" OSStatus  SoundEngine_Interrupt()
{
  MusicEngine_Interrupt();
  if (sOpenALObject != NULL)
    return sOpenALObject->Interrupt();

  return kSoundEngineErrUnitialized;
}

extern "C" OSStatus  SoundEngine_Resume()
{
  MusicEngine_Resume();
  if (sOpenALObject != NULL)
    return sOpenALObject->Resume();

  return kSoundEngineErrUnitialized;
}

extern "C" OSStatus  SoundEngine_SetMasterVolume(Float32 inValue)
{
  OSStatus result = noErr;
  gMasterVolumeGain = inValue;

  if (sOpenALObject) 
    return sOpenALObject->UpdateGain();

  return result;
}

extern "C" OSStatus  SoundEngine_SetListenerPosition(Float32 inX, Float32 inY, Float32 inZ)
{	
  return (sOpenALObject) ? sOpenALObject->SetListenerPosition(inX, inY, inZ) : kSoundEngineErrUnitialized;
}

extern "C" OSStatus  SoundEngine_SetListenerGain(Float32 inValue)
{
  return (sOpenALObject) ? sOpenALObject->SetListenerGain(inValue) : kSoundEngineErrUnitialized;
}

//////////////////////////////////////////////////////
// Effects

extern "C" OSStatus  SoundEngine_LoadEffect(const char* inPath, UInt32* outEffectID)
{
  OSStatus result = noErr;
  if (sOpenALObject == NULL)
  {
    sOpenALObject = new OpenALObject(0.0);
    result = sOpenALObject->Initialize();
  }	
  return (result) ? result : sOpenALObject->LoadEffect(inPath, outEffectID);
}

extern "C" OSStatus  SoundEngine_LoadLoopingEffect(const char* inLoopFilePath, const char* inAttackFilePath, const char* inDecayFilePath, UInt32* outEffectID)
{
  OSStatus result = noErr;
  if (sOpenALObject == NULL)
  {
    sOpenALObject = new OpenALObject(0.0);
    result = sOpenALObject->Initialize();
  }	
  return (result) ? result : sOpenALObject->LoadLoopingEffect(inLoopFilePath, inAttackFilePath, inDecayFilePath, outEffectID);
}

extern "C" OSStatus  SoundEngine_UnloadEffect(UInt32 inEffectID)
{
  return (sOpenALObject) ? sOpenALObject->UnloadEffect(inEffectID) : kSoundEngineErrUnitialized;
}


extern "C" OSStatus  SoundEngine_StartEffect(UInt32 inEffectID, bool looped)
{
  return (sOpenALObject) ? sOpenALObject->StartEffect(inEffectID, looped) : kSoundEngineErrUnitialized;
}

extern "C" OSStatus SoundEngine_Pause(UInt32 inEffectID)
{
  //To unpause just call play... 
  OSStatus result = AL_NO_ERROR;				
  alSourcePause(inEffectID);
  AssertNoOALError("Error Pausing source", end)
end:
  return alGetError();
}

extern "C" bool SoundEngine_isPlaying(UInt32 inEffectID)
{
  int result;
  alGetSourcei(inEffectID, AL_SOURCE_STATE, (ALint *)&result);
  return AL_PLAYING == result;
}

extern "C" OSStatus  SoundEngine_StopEffect(UInt32 inEffectID, Boolean inDoDecay)
{	
  return (sOpenALObject) ?  sOpenALObject->StopEffect(inEffectID, inDoDecay) : kSoundEngineErrUnitialized;
}

extern "C" OSStatus  SoundEngine_SetEffectPitch(UInt32 inEffectID, Float32 inValue)
{
  return (sOpenALObject) ? sOpenALObject->SetEffectPitch(inEffectID, inValue) : kSoundEngineErrUnitialized;
}

extern "C" OSStatus  SoundEngine_SetEffectLevel(UInt32 inEffectID, Float32 inValue)
{
  return (sOpenALObject) ? sOpenALObject->SetEffectVolume(inEffectID, inValue) : kSoundEngineErrUnitialized;
}

extern "C" OSStatus	SoundEngine_SetEffectPosition(UInt32 inEffectID, Float32 inX, Float32 inY, Float32 inZ)
{
  return (sOpenALObject) ? sOpenALObject->SetEffectPosition(inEffectID, inX, inY, inZ) : kSoundEngineErrUnitialized;	
}

extern "C" OSStatus  SoundEngine_SetEffectsVolume(Float32 inValue)
{
  return (sOpenALObject) ? sOpenALObject->SetEffectsVolume(inValue) : kSoundEngineErrUnitialized;
}

extern "C" OSStatus  SoundEngine_SetMaxDistance(Float32 inValue)
{
  return (sOpenALObject) ? sOpenALObject->SetMaxDistance(inValue) : kSoundEngineErrUnitialized;
}

extern "C" OSStatus  SoundEngine_SetReferenceDistance(Float32 inValue)
{
  return (sOpenALObject) ? sOpenALObject->SetReferenceDistance(inValue) : kSoundEngineErrUnitialized;
}

//////////////////////////////////////////////////////
// Music

extern "C" OSStatus  SoundEngine_LoadBackgroundMusicTrack(const char* inPath)
{
  return MusicEngine_LoadBackgroundMusicTrack(inPath);
}

extern "C" OSStatus  SoundEngine_UnloadBackgroundMusicTrack()
{
  return MusicEngine_UnloadBackgroundMusicTrack();
}

extern "C" OSStatus  SoundEngine_StartBackgroundMusic(Boolean looped)
{
  return MusicEngine_StartBackgroundMusic(looped);
}

extern "C" OSStatus  SoundEngine_StopBackgroundMusic(Boolean stopAtEnd)
{
  return MusicEngine_StopBackgroundMusic(stopAtEnd);
}

extern "C" OSStatus  SoundEngine_SetBackgroundMusicVolume(Float32 inValue)
{
  return MusicEngine_SetBackgroundMusicVolume(inValue);
}

extern "C" bool SoundEngine_hasTrackStopped()
{
  return MusicEngine_hasTrackStopped();
}

extern "C" void SoundEngine_pauseBackgroundMusic()
{
  MusicEngine_pauseBackgroundMusic();
}

extern "C" void  SoundEngine_resumeBackgroundMusic()
{
  MusicEngine_resumeBackgroundMusic();
}

#endif
#endif
