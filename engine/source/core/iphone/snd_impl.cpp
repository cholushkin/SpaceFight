#include "snd_impl.h"
#include "snd_effects_impl.h"
#include "core/math/mt_base.h"
#include "core/sound/snd_base.h"

// [ ] looped support in Play

using namespace snd;

extern "C" const char* GetResourcePath(const char* szFileName);

SoundImpl::SoundImpl(u32 snd, SoundManager& sm)
: m_snd(snd)
, m_mgr(sm){}

SoundImpl::~SoundImpl()
{
  SoundEngine_UnloadEffect(m_snd);
}

void SoundImpl::Play(bool looped)
{
  if(m_mgr.GetVolumeSounds() < EPSILON)
    return;
  SoundEngine_StartEffect(m_snd, looped);
}

void SoundImpl::Stop()
{
  SoundEngine_StopEffect(m_snd, false);
}

void SoundImpl::SetVolume(f32 vol)
{
  SoundEngine_SetEffectLevel(m_snd, vol);
}

bool SoundImpl::isPlaying() const
{
  return SoundEngine_isPlaying(m_snd);
}

SoundImpl* SoundImpl::Create(const char* szName, SoundManager& sm)
{
  UInt32 snd;
  if(0 != SoundEngine_LoadEffect(GetResourcePath(szName), &snd))
    return NULL;
  return new SoundImpl(snd, sm);
}

//////////////////////////////////////////////////////////////////////////
// SoundManagerImpl

SoundManagerImpl::SoundManagerImpl (u32 freq) {
  SoundEngine_Initialize(freq);
}

SoundManagerImpl::~SoundManagerImpl() {
  SoundEngine_Teardown();
}

bool SoundManagerImpl::StartMusic(const char* szName, bool loop)
{
  const char* path = GetResourcePath(szName);
  if(NULL == path)
    return false;
  SoundEngine_StopBackgroundMusic(false);
  SoundEngine_UnloadBackgroundMusicTrack();
  if(noErr != SoundEngine_LoadBackgroundMusicTrack(path))
    return false;
  return noErr == SoundEngine_StartBackgroundMusic(loop);
}

bool SoundManagerImpl::isMusicPlaying()
{
  return !SoundEngine_hasTrackStopped();
}

void SoundManagerImpl::StopMusic () {
  SoundEngine_StopBackgroundMusic(false);
}

void SoundManagerImpl::PauseMusic(bool pause)
{
  if(pause)
    SoundEngine_pauseBackgroundMusic();
  else
    SoundEngine_resumeBackgroundMusic();
}

void SoundManagerImpl::SetSoundVolume(f32 vol)
{
  SoundEngine_SetEffectsVolume(vol);
}

void SoundManagerImpl::SetMusicVolume(f32 vol)
{
  vol = mt::Clamp(vol, 0.0f, 1.0f);
  SoundEngine_SetBackgroundMusicVolume(vol);
}