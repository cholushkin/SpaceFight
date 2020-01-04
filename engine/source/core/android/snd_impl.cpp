#include "snd_impl.h"
#include "core/common/com_types.h"
#include "core/common/com_misc.h"

#include "jni-stuff.h"

extern JNIEnv* env;
extern const char * const sCLASS_NAME;

NAMESPACE_BEGIN(snd)

SoundImpl* SoundImpl::Create(const char* pszPath, SoundManager& sm)
{
  if(NULL == pszPath)
    return NULL;

  if(NULL == env)
    return NULL;

  ClassRef cls(*env, env->FindClass(sCLASS_NAME));
  jmethodID m = env->GetStaticMethodID(cls, "createSound", "(Ljava/lang/String;)I");
  if(NULL == m)
    return NULL;

  int id = env->CallStaticIntMethod(cls, m, env->NewStringUTF(pszPath));

  return new SoundImpl(id, sm);
}

void SoundImpl::Play(bool looped)
{
  ClassRef cls(*env, env->FindClass(sCLASS_NAME));
  jmethodID m = env->GetStaticMethodID(cls, "playSound", "(IZ)V");
  if(NULL != m)
    env->CallStaticVoidMethod(cls, m, m_id, looped);
}

void SoundImpl::Stop()
{
  ClassRef cls(*env, env->FindClass(sCLASS_NAME));
  jmethodID m = env->GetStaticMethodID(cls, "stopSound", "(I)V");
  if(NULL != m)
    env->CallStaticVoidMethod(cls, m, m_id);
}

void SoundImpl::SetVolume(f32 vol)
{
  ClassRef cls(*env, env->FindClass(sCLASS_NAME));
  jmethodID m = env->GetStaticMethodID(cls, "setVolume", "(IF)V");
  if(NULL != m)
    env->CallStaticVoidMethod(cls, m, m_id, vol);
}

bool SoundImpl::isPlaying() const
{
  ClassRef cls(*env, env->FindClass(sCLASS_NAME));
  jmethodID m = env->GetStaticMethodID(cls, "isPlaying", "(I)Z");
  if(NULL == m)
    return false;
  return  env->CallStaticBooleanMethod(cls, m, m_id);
}

SoundImpl::~SoundImpl()
{
  ClassRef cls(*env, env->FindClass(sCLASS_NAME));
  jmethodID m = env->GetStaticMethodID(cls, "destroySound", "(I)V");
  if(NULL != m)
    env->CallStaticVoidMethod(cls, m, m_id);
}

SoundManagerImpl::SoundManagerImpl( u32 freq )
{
}

SoundManagerImpl::~SoundManagerImpl()
{
}

bool SoundManagerImpl::StartMusic( const char* pszPath, bool loop )
{
  if(NULL == pszPath)
    return false;

  if(NULL == env)
    return false;

  ClassRef cls(*env, env->FindClass(sCLASS_NAME));
  jmethodID m = env->GetStaticMethodID(cls, "startMusic", "(Ljava/lang/String;Z)Z");
  if(NULL == m)
    return false;

  return env->CallStaticBooleanMethod(cls, m, env->NewStringUTF(pszPath), loop);
}

bool SoundManagerImpl::isMusicPlaying()
{
  if(NULL == env)
    return false;

  ClassRef cls(*env, env->FindClass(sCLASS_NAME));
  jmethodID m = env->GetStaticMethodID(cls, "isMusicPlaying", "()Z");
  if(NULL == m)
    return false;

  return env->CallStaticBooleanMethod(cls, m);
}

void SoundManagerImpl::StopMusic()
{
  ClassRef cls(*env, env->FindClass(sCLASS_NAME));
  jmethodID m = env->GetStaticMethodID(cls, "stopMusic", "()V");
  if(NULL != m)
    env->CallStaticVoidMethod(cls, m);
}

void SoundManagerImpl::PauseMusic( bool pause )
{
  ClassRef cls(*env, env->FindClass(sCLASS_NAME));
  jmethodID m = env->GetStaticMethodID(cls, "pauseMusic", "(Z)V");
  if(NULL != m)
    env->CallStaticVoidMethod(cls, m, pause);
}

void SoundManagerImpl::SetMusicVolume(f32 vol)
{
  ClassRef cls(*env, env->FindClass(sCLASS_NAME));
  jmethodID m = env->GetStaticMethodID(cls, "setMusicVolume", "(F)V");
  if(NULL != m)
    env->CallStaticVoidMethod(cls, m, vol);
}

void SoundManagerImpl::SetSoundVolume(f32 vol)
{
  ClassRef cls(*env, env->FindClass(sCLASS_NAME));
  jmethodID m = env->GetStaticMethodID(cls, "setSoundVolume", "(F)V");
  if(NULL != m)
    env->CallStaticVoidMethod(cls, m, vol);
}

NAMESPACE_END(snd)