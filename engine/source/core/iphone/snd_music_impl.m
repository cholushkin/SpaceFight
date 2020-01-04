#import "snd_music_impl.h"
#import <AVFoundation/AVFoundation.h>

#if !TARGET_OS_IPHONE
# import <AppKit/NSSound.h>
# define  AVAudioPlayer NSSound
#endif // !TARGET_OS_IPHONE 

static AVAudioPlayer* g_audioPlayer = nil;

OSStatus  MusicEngine_Initialize()
{
  return 0;
}

OSStatus  MusicEngine_Teardown()
{
  return MusicEngine_UnloadBackgroundMusicTrack();
}

OSStatus MusicEngine_Interrupt()
{
  MusicEngine_pauseBackgroundMusic();
  return 0;
}

OSStatus  MusicEngine_Resume()
{
  MusicEngine_resumeBackgroundMusic();
  return 0;
}

OSStatus  MusicEngine_LoadBackgroundMusicTrack(const char* inPath)
{
  MusicEngine_UnloadBackgroundMusicTrack();
  NSError* err = nil;
  NSURL* fileURL = [NSURL fileURLWithPath:[[NSString alloc] initWithCString: inPath encoding:NSASCIIStringEncoding]];
#if TARGET_OS_IPHONE
  g_audioPlayer = [[AVAudioPlayer alloc] initWithContentsOfURL:fileURL error:&err];
#else // TARGET_IS_IPHONE
  g_audioPlayer = [[NSSound alloc] initWithContentsOfURL:fileURL byReference:YES];
#endif // TARGET_IS_IPHONE
  if(g_audioPlayer == nil || err) {
    g_audioPlayer = nil;
    return kSoundEngineErrInvalidFileFormat;
  }
  return 0;
}

OSStatus  MusicEngine_UnloadBackgroundMusicTrack()
{
  if(g_audioPlayer) {
    [g_audioPlayer stop];
  }
  g_audioPlayer = nil;
  return 0;
}

OSStatus  MusicEngine_StartBackgroundMusic(bool looped)
{
  if(!g_audioPlayer)
    return kSoundEngineErrUnitialized;
#if TARGET_OS_IPHONE
  [g_audioPlayer prepareToPlay];
  g_audioPlayer.numberOfLoops = looped ? -1 : 0;
#else // TARGET_OS_IPHONE
 [g_audioPlayer setLoops:looped];
#endif // TARGET_OS_IPHONE
  [g_audioPlayer play];
  return 0;
}

OSStatus  MusicEngine_StopBackgroundMusic(Boolean inStopAtEnd)
{
  if(!g_audioPlayer)
    return kSoundEngineErrUnitialized;
  if(!inStopAtEnd)
    [g_audioPlayer stop];
  else
  {
#if TARGET_OS_IPHONE
  g_audioPlayer.numberOfLoops = 0;
#else // TARGET_OS_IPHONE
 [g_audioPlayer setLoops:false];
#endif // TARGET_OS_IPHONE
  }
  return 0;
}

OSStatus  MusicEngine_SetBackgroundMusicVolume(Float32 inValue)
{
  if(!g_audioPlayer)
    return kSoundEngineErrUnitialized;
  [g_audioPlayer setVolume:inValue];
  return 0;
}

void MusicEngine_pauseBackgroundMusic()
{
  if(g_audioPlayer)
    [g_audioPlayer pause];
}

void MusicEngine_resumeBackgroundMusic()
{
  if(g_audioPlayer)
#if TARGET_OS_IPHONE
    [g_audioPlayer play];
#else
    [g_audioPlayer resume];
#endif
}

bool MusicEngine_hasTrackStopped()
{
  return !g_audioPlayer || ![g_audioPlayer isPlaying];
}

