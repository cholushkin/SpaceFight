#ifndef SND_IMPL_H
#define SND_IMPL_H

#include "core/common/com_misc.h"
#include "bass.h"

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
  HSAMPLE  m_sample;
  HCHANNEL m_channel;
  SoundImpl(HSAMPLE sample, SoundManager& sm)
    : m_sample(sample)
    , m_mgr(sm)
    , m_channel(NULL)
  {}
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
private:
  HSTREAM MusicLoad(const char* szName);

  f32     m_musicVol;
  f32     m_sfxVol;
  HSTREAM m_curMusic;
  void*   m_stream_data;
  bool    m_isInitError;
};

NAMESPACE_END(snd)
#endif // SND_IMPL_H