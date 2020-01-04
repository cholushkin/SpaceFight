#include "core/sound/snd_base.h"
#include "core/res/res_base.h"
#include "io_file.h"
#include "platform.h"

#ifdef WIN32
# include "win/snd_impl.h"
#endif

#ifdef ANDROID_PLATFORM
# include "android/snd_impl.h"
#endif

#ifdef __APPLE__
# include "iPhone/snd_impl.h"
#endif


using namespace res;
using namespace snd;
//////////////////////////////////////////////////////////////////////////
// Sound

void Sound::Play(bool looped)
{
  EASSERT(m_pImpl);
  if(m_pImpl)
    m_pImpl->Play(looped);
}

void Sound::Stop()
{
  EASSERT(m_pImpl);
  if(m_pImpl)
    m_pImpl->Stop();
}

void Sound::SetVolume(f32 vol)
{
  EASSERT(m_pImpl);
  if(m_pImpl)
    m_pImpl->SetVolume(vol);
}

bool Sound::isPlaying() const
{
  EASSERT(m_pImpl);
  return m_pImpl && m_pImpl->isPlaying();
}

Sound::~Sound()
{
  SAFE_DELETE(m_pImpl);
}

SoundManager::SoundManager()
: m_volSnd(1.0f)
, m_pimpl(new SoundManagerImpl(44100))
{
}

SoundManager::~SoundManager()
{
  SAFE_DELETE(m_pimpl);
}

bool SoundManager::StartMusic(ResID id, bool loop)
{
  if(const char* name = ResourcesPool::GetNameFromID(id))
    return StartMusic(name, loop);
  return false;
}

bool SoundManager::StartMusic(const char* name, bool loop)
{
  return m_pimpl && m_pimpl->StartMusic(name, loop);
}

bool SoundManager::isMusicPlaying() const
{
  return m_pimpl && m_pimpl->isMusicPlaying(); 
}

void SoundManager::StopMusic()
{
  if(m_pimpl)
    m_pimpl->StopMusic();
}

void SoundManager::PauseMusic(bool pause)
{
  if(m_pimpl)
    m_pimpl->PauseMusic(pause);
}

void SoundManager::SetMusicVolume(f32 vol)
{
  if(m_pimpl)
    m_pimpl->SetMusicVolume(vol);
}

Sound* SoundManager::GetSound(ResID id, ResourcesPool& p)
{
  Sound* pS = (Sound*)p.Get(id);
  if(NULL != pS)
    return pS;
  const char* name = p.GetNameFromID(id);
  SoundImpl* pSI = SoundImpl::Create(name, *this);
  if(!pSI)
    return NULL;
  pS = new Sound(pSI);
  p.PutGeneric(id, pS);
  return pS;
}

void snd::SoundManager::SetVolumeSounds( f32 vol )
{
  m_volSnd = vol;
  if(m_pimpl)
    m_pimpl->SetSoundVolume(m_volSnd);
}

f32 snd::SoundManager::GetVolumeSounds() const
{
  return m_volSnd;
}
