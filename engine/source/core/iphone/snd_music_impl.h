#ifndef snd_music_impl_h
#define snd_music_impl_h

#include "snd_effects_impl.h"

#if defined(__cplusplus)
extern "C"
{
#endif

OSStatus  MusicEngine_Initialize();
OSStatus  MusicEngine_Teardown();
OSStatus  MusicEngine_Interrupt();
OSStatus  MusicEngine_Resume();
OSStatus  MusicEngine_LoadBackgroundMusicTrack(const char* inPath);
OSStatus  MusicEngine_UnloadBackgroundMusicTrack();
OSStatus  MusicEngine_StartBackgroundMusic(bool looped);
OSStatus  MusicEngine_StopBackgroundMusic(Boolean inStopAtEnd);
OSStatus  MusicEngine_SetBackgroundMusicVolume(Float32 inValue);
void      MusicEngine_pauseBackgroundMusic();
void      MusicEngine_resumeBackgroundMusic();
bool      MusicEngine_hasTrackStopped();

#if defined(__cplusplus)
}
#endif 

#endif // snd_music_impl_h