#ifndef ani_sprite_h__
#define ani_sprite_h__

#include "core/render/r_render.h"
#include "ext/primitives/r_sheet_sprite.h"

NAMESPACE_BEGIN( ani )

//////////////////////////////////////////////////////////////////////////
// ASpriteData

class ASpriteData
{
  DENY_COPY(ASpriteData);
public:
  enum eAnimType
  {
    atOnce = 0,
    atLoop = 1,
    atPong = 2,
    atMax
  };

  static const ASpriteData* Load(res::ResID, res::ResourcesPool&, r::Render& r);

  struct Anim
  {
    Anim()
      : from(0)
      , to(0)
      , type(atMax)
      , ftime(1/30.0f)
      , name(NULL){}

    u16         from;  // in m_framesmap
    u16         to;    // in m_framesmap
    f32         ftime; // frame time = 1.0f/FPS
    eAnimType   type;
    const char* name;  // into m_names
  };

  i16         GetID  (const char* name) const; // -1 if not found
  const char* GetName(u16) const;

  const Anim* GetAnim(const char* name) const;
  const Anim* GetAnim(u16         id  ) const;

  void Draw(u16 frame_in_map, r::Render&, const mt::v2f& pos) const;
  void Draw(u16 frame_in_map, r::Render&, const mt::v2f& pos, const mt::v2f& scale) const;
  void Draw(u16 frame_in_map, r::Render&, const mt::v2f& pos, const mt::v2f& scale, f32& rot) const;

  void Draw(u16 frame_in_map, r::Render&, const mt::Matrix23f& m) const;

  mt::Rectf GetFrameRect(u16 frame_in_map) const;

  ~ASpriteData();
private:
  ASpriteData();
  bool isValid() const;
  Anim*     m_anims;
  u16       m_nanims;

  u16*      m_framesmap;
  char*     m_names; // separated with NULLs
  r::Sprite m_sprite;
};

//////////////////////////////////////////////////////////////////////////
// ASprite

class ASprite
{
  DENY_COPY(ASprite);
public:
  ASprite(const ASpriteData&);
  virtual ~ASprite(){}

  bool Play(const char* name);
  bool Play(u16);   // by ID
  void Stop();      // reset m_frame to 0
  void Pause(bool); // keep m_frame unchanged
  // reverse?

  bool isPlaying() const { return m_playing; }

  void Update(f32 dt);
  void Draw(r::Render&, const mt::v2f& pos) const;
  void Draw(r::Render&, const mt::v2f& pos, const mt::v2f& scale) const;
  void Draw(r::Render&, const mt::v2f& pos, const mt::v2f& scale, f32& rot) const;

  void Draw(r::Render&, const mt::Matrix23f& m) const;

  mt::Rectf GetRect() const;

  const ASpriteData&       GetDate () const { return m_data; }
  const ASpriteData::Anim* GetAnim () const { return m_anim; }
  u16                      GetFrame() const { return m_frame; }
protected:
  virtual void onStop() {}
  virtual void onLoop() {} // also 'onPong'

  const ASpriteData& m_data;
private:

  bool  m_playing;
  bool  m_reverse;
  u16   m_frame; // between ASpriteData::Anim::from and ...::to
  f32   m_ftl;   // frame time left

  const ASpriteData::Anim* m_anim;
};

NAMESPACE_END( ani )

#endif // ani_sprite_h__
