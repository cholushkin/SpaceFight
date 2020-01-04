#include "ext/primitives/ani_sprite.h"
#include "core/io/io_base.h"

#include <string> // remove me
#include <memory> // remove me

NAMESPACE_BEGIN( ani )

//////////////////////////////////////////////////////////////////////////
// ASprite

ASprite::ASprite(const ASpriteData& data)
: m_data(data)
, m_playing(false)
, m_reverse(false)
, m_frame(0)
, m_anim(NULL)
{
}

bool ASprite::Play(const char* name)
{
  i16 id = m_data.GetID(name);
  return -1 != id && Play(u16(id));
}

bool ASprite::Play(u16 id)
{
  m_playing = false;
  m_reverse = false;
  m_anim = m_data.GetAnim(id);
  if(m_anim)
  {
    m_frame = m_anim->from;
    m_ftl   = m_anim->ftime;
    m_playing = true;
  }
  return m_playing;
}

void ASprite::Stop()
{
  m_frame = m_anim ? m_anim->from : 0;
  m_playing = false;
  onStop();
  //m_anim = NULL; ?
}

void ASprite::Pause(bool p)
{
  m_playing = !p;
}

void ASprite::Update(f32 dt)
{
  if(!m_playing || !m_anim)
    return;

  m_ftl -= dt;

  while(m_ftl < 0.0f)
  {
    if((m_reverse ? m_anim->from : m_anim->to) != m_frame)
      m_reverse ? --m_frame : ++m_frame;
    else // we reached the end
    {
      if(ASpriteData::atOnce == m_anim->type)
      {
        m_anim    = NULL;
        m_playing = false;
        onStop();
      }
      else
      {
        if(ASpriteData::atPong == m_anim->type)
          m_reverse = !m_reverse;
        m_frame = m_reverse ? m_anim->to : m_anim->from;
        if(ASpriteData::atLoop == m_anim->type || !m_reverse)
          onLoop();
      }
    } // of End
    if(m_anim && m_playing)
      m_ftl += m_anim->ftime;
    else
      break;
  }

}

void ASprite::Draw(r::Render& r, const mt::v2f& pos) const
{
  m_data.Draw(m_frame, r, pos);
}

void ASprite::Draw(r::Render& r, const mt::v2f& pos, const mt::v2f& scale) const
{
  m_data.Draw(m_frame, r, pos, scale);
}

void ASprite::Draw(r::Render& r, const mt::v2f& pos, const mt::v2f& scale, f32& rot) const
{
  m_data.Draw(m_frame, r, pos, scale, rot);
}

void ASprite::Draw(r::Render& r, const mt::Matrix23f& m) const
{
  m_data.Draw(m_frame, r, m);
}

mt::Rectf ASprite::GetRect() const
{
  return m_data.GetFrameRect(m_frame);
}

//////////////////////////////////////////////////////////////////////////
// ASpriteData

bool ASpriteData::isValid() const
{
  return m_framesmap && m_anims;
}

i16 ASpriteData::GetID(const char* name) const
{
  for(u16 i = 0; m_nanims != i; ++i)
    if(0 == strcmp(name, m_anims[i].name))
      return i;
  return -1;
}

const char* ASpriteData::GetName(u16 aidx) const
{
  EASSERT(aidx < m_nanims);
  return m_anims[aidx].name;
}

const ASpriteData::Anim* ASpriteData::GetAnim(const char* name) const
{
  i32 id = GetID(name);
  return -1 == id ? NULL : GetAnim(u16(id));
}

const ASpriteData::Anim* ASpriteData::GetAnim(u16 id) const
{
  EASSERT(id < m_nanims);
  return &m_anims[id];
}

void ASpriteData::Draw(u16 frame_in_map, r::Render& r, const mt::v2f& pos) const
{
  EASSERT(isValid());
  if(!isValid())
    return;
  m_sprite.Draw(r, m_framesmap[frame_in_map], pos, mt::COLOR_WHITE);
}

void ASpriteData::Draw(u16 frame_in_map, r::Render& r, const mt::v2f& pos, const mt::v2f& scale) const
{
  EASSERT(isValid());
  if(!isValid())
    return;
  m_sprite.Draw(r, m_framesmap[frame_in_map], pos, mt::COLOR_WHITE, scale);
}

void ASpriteData::Draw(u16 frame_in_map, r::Render& r, const mt::v2f& pos, const mt::v2f& scale, f32& rot) const
{
  EASSERT(isValid());
  if(!isValid())
    return;
  m_sprite.Draw(r, m_framesmap[frame_in_map], pos, mt::COLOR_WHITE, scale, rot, true);
}

void ASpriteData::Draw(u16 frame_in_map, r::Render& r, const mt::Matrix23f& m) const
{
  EASSERT(isValid());
  if(!isValid())
    return;
  m_sprite.Draw(r, m_framesmap[frame_in_map], m, mt::COLOR_WHITE);
}

mt::Rectf ASpriteData::GetFrameRect(u16 frame_in_map) const
{
  if(isValid())
    return m_sprite.GetRect(m_framesmap[frame_in_map]);
  return mt::Rectf();
}

ASpriteData::ASpriteData()
: m_anims(NULL)
, m_nanims(0)
, m_framesmap(NULL)
, m_names(NULL)
{
}

ASpriteData::~ASpriteData()
{
  m_nanims = 0;
  SAFE_DELETE_ARRAY(m_anims);
  SAFE_DELETE_ARRAY(m_framesmap);
  SAFE_DELETE_ARRAY(m_names);
}

const ASpriteData* ASpriteData::Load(res::ResID sprid, res::ResourcesPool& rpool, r::Render& r)
{
  ASpriteData* ps = (ASpriteData*)rpool.Get(sprid);
  if(NULL != ps)
    return ps;

  const char* resName = res::ResourcesPool::GetNameFromID(sprid);
  if(NULL == resName)
    return NULL;

  io::FileStream f;
  if(!f.Open(resName, true, true))
    return NULL;

  char magic[4];
  f.Read(&magic[0], sizeof(magic));

  std::auto_ptr<ASpriteData> p(new ASpriteData());
  ps = p.get();
  f.Read16(p->m_nanims);
  p->m_anims = new ASpriteData::Anim[p->m_nanims];

  u16 len(0);
  u16 fps(0);
  u16 type(0);
  u16 frame(0);
  for(u16 a = 0; p->m_nanims != a; ++a)
  {
    f.Read16(len);
    f.Read16(fps);
    f.Read16(type);
    p->m_anims[a].from = frame; frame += len;
    p->m_anims[a].to   = frame - 1;
    p->m_anims[a].ftime = 1.0f / fps;
    p->m_anims[a].type  = (eAnimType)type;
  }

  // load frame map
  f.Read16(len);
  p->m_framesmap = new u16[len];
  io::ReadArray16(f, p->m_framesmap, len);

  // anim names
  f.Read16(len);
  p->m_names = new char[len];
  f.Read(p->m_names, len);

  size_t aniidx = 0;
  char* pn = &p->m_names[0];
  p->m_anims[aniidx++].name = pn;
  for(; p->m_nanims != aniidx;)
  {
    while(*(pn++));
    p->m_anims[aniidx++].name = pn;
  }

  // load sprite // $$$ get rid of std::string
  std::string sprName(resName);
  sprName.replace(sprName.end()-4,sprName.end(),".spr");

  if(!r::Sprite::Load( ps->m_sprite,
    res::ResourcesPool::GetIDFromName(sprName.c_str()),
    rpool,
    r))
    return NULL;

  return rpool.PutGeneric(sprid, p.release());
}

NAMESPACE_END( ani )
