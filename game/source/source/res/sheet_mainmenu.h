// autogenerated by SheetSprGen v.1.0, do not modify
#ifndef sheet_mainmenu_h___
#define sheet_mainmenu_h___

namespace r {
  class Render;
  class SheetSprite;
} // namespace r

namespace res {
  class ResourcesPool;
} // namespace res

class mainmenu
{
public:
  enum eFrame
  {
      MainMenuBackground = 0,
      Ship1 = 1,
      LabelP1 = 2,
      LabelP2 = 3,
      LabelP3 = 4,
      LabelP4 = 5,
      LabelP5 = 6,
      LabelP6 = 7,
      LabelP7 = 8,
      LabelP8 = 9,
      LabelVictory = 10,
      IconControllerKeyboardLeftPart = 11,
      IconControllerKeyboardRightPart = 12,
      aCount = 13
  };
  static const r::SheetSprite* GetSprite( r::Render& r, res::ResourcesPool& rpool );
};

#endif // #ifndef sheet_mainmenu_h___
