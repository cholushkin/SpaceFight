#include "snd_impl.h"
#include "core/common/com_types.h"
#include "core/common/com_misc.h"
#include "core/alg/alg_base.h"
#include "core/sound/snd_base.h"
#include "core/io/io_base.h"
#include "platform.h"

#include <windows.h>

// [ ] SoundImpl implement stubs

extern HWND g_hWnd;

#define BASS_CHECK EASSERT(BASS_OK == BASS_ErrorGetCode())

using namespace snd;

// -------------------------------- SoundImpl

SoundImpl* SoundImpl::Create(const char* szName, SoundManager& sm)
{
  io::FileStream stream;
  if (!stream.Open(szName, true, true))
    return NULL;
  long size = stream.Size();
  alg::ArrayPtr<i8> sample_data(new i8[size]);
  stream.Read((char*)sample_data.get(), size);
  return new SoundImpl(BASS_SampleLoad(TRUE, sample_data, 0, size, 4, BASS_SAMPLE_OVER_VOL),sm);
}

void SoundImpl::Play(bool looped)
{
  Stop();
  if(m_sample)
  {
    m_channel = BASS_SampleGetChannel(m_sample, FALSE);
    BASS_ChannelFlags(m_channel, looped ? BASS_SAMPLE_LOOP : 0, BASS_SAMPLE_LOOP);
    BASS_ChannelSetAttribute(m_channel, BASS_ATTRIB_VOL, m_mgr.GetVolumeSounds());
    BASS_ChannelPlay(m_channel, FALSE);
    BASS_CHECK;
  }
}

void SoundImpl::Stop()
{
  if(m_channel)
    BASS_ChannelStop(m_channel);
  m_channel = NULL;
}

void SoundImpl::SetVolume(f32 vol)
{
  if(m_channel)
    BASS_ChannelSetAttribute(m_channel, BASS_ATTRIB_VOL, vol * m_mgr.GetVolumeSounds());
}

bool SoundImpl::isPlaying() const
{
  return m_channel && BASS_ChannelIsActive(m_channel);
}

SoundImpl::~SoundImpl()
{
    if(m_sample)
    {
        BASS_SampleFree(m_sample);
        BASS_CHECK;
    }
}

// -------------------------------- SoundManagerImpl

SoundManagerImpl::SoundManagerImpl( u32 freq )
: m_musicVol(1.0f)
, m_curMusic(NULL)
, m_stream_data(NULL)
{
  EASSERT (HIWORD(BASS_GetVersion()) == BASSVERSION);
  m_isInitError = !!!BASS_Init(-1,freq,0,g_hWnd,NULL);
}

SoundManagerImpl::~SoundManagerImpl()
{
  StopMusic();
  BASS_Stop();
  BASS_Free();  
}

bool SoundManagerImpl::StartMusic( const char* szName, bool loop )
{
  if(m_isInitError)
    return false;
  if(m_curMusic)
    StopMusic();
  m_curMusic = MusicLoad(szName);
  BASS_ChannelFlags(m_curMusic, loop ? BASS_SAMPLE_LOOP : 0, BASS_SAMPLE_LOOP); // set LOOP flag
  BASS_ChannelPlay (m_curMusic, TRUE);
  BASS_ChannelSetAttribute(m_curMusic, BASS_ATTRIB_VOL, m_musicVol);
  BASS_CHECK;
  return true;
}

bool SoundManagerImpl::isMusicPlaying()
{
  if(m_isInitError)
    return false;
  return 0 != m_curMusic && BASS_ACTIVE_PLAYING == BASS_ChannelIsActive(m_curMusic);
}

void SoundManagerImpl::StopMusic()
{
  if(m_isInitError)
    return;
  if(m_curMusic)
  {
    BASS_StreamFree(m_curMusic);
    delete[] m_stream_data;
    m_curMusic = NULL;
    m_stream_data = NULL;
  }
}

void SoundManagerImpl::PauseMusic( bool pause )
{
  if(m_isInitError)
    return;
  if(m_curMusic)
  {
    if(pause)
      BASS_ChannelPause(m_curMusic);
    else
      BASS_ChannelPlay(m_curMusic,FALSE);
  }
}

void SoundManagerImpl::SetMusicVolume(f32 vol)
{
  if(m_isInitError)
    return;
  m_musicVol = vol;
  BASS_ChannelSetAttribute(m_curMusic, BASS_ATTRIB_VOL, m_musicVol);
}


HSTREAM SoundManagerImpl::MusicLoad(const char* szName)
{
  if(m_isInitError)
    return NULL;
  HMUSIC hm = NULL;

  io::FileStream stream;
  if (stream.Open(szName, true, true))
  {
    long size = stream.Size();
    m_stream_data = new i8[size];
    stream.Read((char*)m_stream_data, size);
    hm = BASS_StreamCreateFile(TRUE, m_stream_data, 0, size, 0);
    BASS_CHECK;
  }
  return hm;
}

void snd::SoundManagerImpl::SetSoundVolume( f32 vol )
{
  m_sfxVol = vol;
}
