#ifndef in_touchtracker_h__
#define in_touchtracker_h__

#include "core/common/com_misc.h"
#include "core/math/mt_base.h"

namespace in
{
  struct InputEvent {
    enum eType {
      ieTouch = 0,
      ieKey   = 1,
    };
    enum eAction {
      iaUp      = 0,
      iaDown    = 1,
      iaRepeat  = 2,
      iaCancel  = 3,
    };

    eAction m_action;
    eType   m_type;
    mt::v2i16   m_data;
  };

  class TouchTracker {
    const static u32 s_invalidId = ~(0U);
    const static u32 s_maxTouches = 5;
  public:
    struct Touch {
      u32   id;
      mt::v2i16 pos;
      mt::v2i16 initial_pos;

      Touch()
        : id(s_invalidId) {}
      Touch(u32 id, const mt::v2i16& p)
        : id(id), pos(p), initial_pos(p) {}

      bool operator == (const Touch& other) const {
        return id == other.id;
      }

      bool isValid() const {
        return s_invalidId != id;
      }
    };

    void Input (const mt::v2i16& p, InputEvent::eAction type, unsigned int id);
    void Begin (const mt::v2i16& p, unsigned int id);
    void Move  (const mt::v2i16& p, unsigned int id);
    void End   (const mt::v2i16& p, unsigned int id);
    void Cancel(const mt::v2i16& p, unsigned int id);
    void Reset();
    void KillTouches();

    u16          GetNumTouches()        const;
    Touch const* GetTouch     (u16 num) const;
    mt::v2i16    GetPosition  (u16 num) const;

  private:
    Touch m_touches[s_maxTouches];
  };

} // namespace in

#endif // in_touchtracker_h__
