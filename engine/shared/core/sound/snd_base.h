#ifndef snd_base_h__
#define snd_base_h__

#include "core/common/com_misc.h"
#include "core/res/res_base.h"

NAMESPACE_BEGIN(snd)

//////////////////////////////////////////////////////////////////////////
// Sound

class SoundImpl;

class Sound
{
public:
  void Play(bool looped = false);
  void Stop();
  void SetVolume(f32);
  bool isPlaying() const;

  Sound(SoundImpl* pImpl) : m_pImpl(pImpl){}
  ~Sound();
private:
  SoundImpl* m_pImpl;
};

class SoundManagerImpl;

class SoundManager
{
public:
  SoundManager();
  ~SoundManager();
  
  Sound* GetSound(res::ResID  id, res::ResourcesPool& rpool);
//Sound* GetSound(const char* id, res::ResourcesPool& rpool); // need an ID for res pool
  void SetVolumeSounds(f32 vol);
  f32  GetVolumeSounds() const;

  bool StartMusic(res::ResID,  bool loop = true);
  bool StartMusic(const char*, bool loop = true);
  bool isMusicPlaying() const;
  void StopMusic ();
  void PauseMusic(bool pause);
  void SetMusicVolume(f32 vol);
private:
  SoundManagerImpl* m_pimpl;
  f32 m_volSnd;
};

NAMESPACE_END(snd)

#endif // snd_base_h__
