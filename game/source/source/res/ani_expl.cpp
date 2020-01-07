#include "ani_expl.h"

const u32 ani_expl::sFrameCounts[aCount] = {
  12,
}; 

const u32 ani_expl::sDirs[aCount] = {
  1,
}; 

u32 ani_expl::GetFrame( eAnimations anim, u32 frame, u8 dir ) { 
  int frameSum = 0;
  for(int i = 0; i<anim; ++i)
      frameSum += sDirs[i]*sFrameCounts[i];

  return dir*sFrameCounts[anim]+frame+frameSum;
}


u32 ani_expl::GetNDirs( eAnimations anim ) { 
  return sDirs[anim];
}


u32 ani_expl::GetNFrames( eAnimations anim ) { 
  return sFrameCounts[anim];
}


u32 ani_expl::GetFrame( eAnimations anim, f32 time, u8 dir ) { 
  const u32 frame = (u32)(time) % GetNFrames(anim);
  return GetFrame(anim, frame, dir);
}


const CharacterSprite* ani_expl::CreateSprite( r::Render& r, res::ResourcesPool& rpool ) { 
  const r::SheetSprite* spr = (r::SheetSprite*)rpool.Get(RES_EXPL_SPR);
  if (spr)
    return spr;
  spr = r::SheetSprite::Load(RES_EXPL_SPR, rpool, r);
  if (!spr)
  {
    delete spr;
    return NULL;
  }
  rpool.PutGeneric(RES_EXPL_SPR, spr);
  return spr;
}


