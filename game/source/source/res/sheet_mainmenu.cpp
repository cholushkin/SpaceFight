// autogenerated by SheetSprGen v.1.0, do not modify
#include "sheet_mainmenu.h"
#include "resources.h"
#include "ext/primitives/r_sheet_sprite.h"

const r::SheetSprite* mainmenu::GetSprite( r::Render& r, res::ResourcesPool& rpool ) { 
  return r::Sprite::Load(RES_MAINMENU_SPR, rpool, r);
}