#include "ext/input/in_touchtracker.h"

namespace in
{
  using namespace mt;
  //////////////////////////////////////////////////////////////////////////
  // TouchTracker

  void TouchTracker::Input(const v2i16& p, InputEvent::eAction type, unsigned int id)
  {
    if(InputEvent::iaUp == type)
      End(p, id);
    else if(InputEvent::iaDown == type)
      Begin(p, id);
    else if(InputEvent::iaRepeat == type)
      Move(p, id);
    else if(InputEvent::iaCancel == type)
      Cancel(p, id);
  }

  void TouchTracker::Begin(const v2i16& p, unsigned int id)
  {
    for(unsigned int i = 0; s_maxTouches != i; ++i)
      if(m_touches[i].id == id)
        return;
    for(unsigned int i = 0; s_maxTouches != i; ++i)  {
      if(m_touches[i].isValid())
        continue;
      m_touches[i] = Touch(id, p);
      return;
    }
    EALWAYS_ASSERT("no free touches");
  }

  void TouchTracker::Move(const v2i16& p, unsigned int id)
  {
    for(unsigned int i = 0; s_maxTouches != i; ++i)
    {
      if(m_touches[i].id != id)
        continue;
      m_touches[i].pos = p;
      return;
    }
    //  EALWAYS_ASSERT("unknown touch");
  }

  void TouchTracker::End(const v2i16& /*p*/, unsigned int id)
  {
    for(unsigned int i = 0; s_maxTouches != i; ++i) {
      if(m_touches[i].id != id)
        continue;
      m_touches[i].id = s_invalidId;
      return;
    }
    //EALWAYS_ASSERT("unknown touch");
  }

  void TouchTracker::Cancel(const v2i16& /*p*/, unsigned int id)
  {
    for(unsigned int i = 0; s_maxTouches != i; ++i) {
      if(m_touches[i].id != id)
        continue;
      m_touches[i].id = s_invalidId;
      return;
    }
    EALWAYS_ASSERT("unknown touch");
  }

  u16 TouchTracker::GetNumTouches() const
  {
    u16 res = 0;
    for(unsigned int i = 0; s_maxTouches != i; ++i)
      if(m_touches[i].isValid())
        ++res;
    return res;
  }

  const TouchTracker::Touch* TouchTracker::GetTouch(u16 num) const
  {
    for(unsigned int i = 0; s_maxTouches != i; ++i)
    {
      if(!m_touches[i].isValid())
        continue;
      if(0 == num)
        return &m_touches[i];
      --num;
    }
    EALWAYS_ASSERT("touch id is out of bounds");
    return NULL;
  }

  void TouchTracker::KillTouches()
  {
    for(unsigned int i = 0; s_maxTouches != i; ++i) 
      m_touches[i].id = s_invalidId;
  }

  void TouchTracker::Reset()
  {
    for(unsigned int i = 0; s_maxTouches != i; ++i)
      if(m_touches[i].isValid())
        m_touches[i].initial_pos = m_touches[i].pos;
  }

  v2i16 TouchTracker::GetPosition(u16 num) const
  {
    const Touch* t = GetTouch(num);
    return t ? t->pos : v2i16(0, 0);
  }

} // namespace in
