#ifndef SND_IMPL_H
#define SND_IMPL_H

#include "core/common/com_misc.h"

NAMESPACE_BEGIN(snd)

class SoundManager;

class SoundImpl
{
public:
  ~SoundImpl();
  void Play(bool looped);
  void Stop();
  void SetVolume(f32);
  bool isPlaying() const;

  static SoundImpl* Create(const char* szName, SoundManager& sm);

private:
  u32 m_snd;
  SoundImpl(u32 snd, SoundManager& sm);
  SoundManager& m_mgr;
  SoundImpl& operator=(const SoundImpl&);
};

class SoundManagerImpl
{
public:
  SoundManagerImpl (u32 freq);
  ~SoundManagerImpl();
  bool StartMusic(const char* szName, bool loop);
  bool isMusicPlaying();
  void StopMusic ();
  void PauseMusic(bool pause);
  void SetMusicVolume(f32 vol);
  void SetSoundVolume(f32 vol);
};

NAMESPACE_END(snd)

#endif // SND_IMPL_H
