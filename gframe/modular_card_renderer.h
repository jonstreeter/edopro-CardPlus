#ifndef MODULAR_CARD_RENDERER_H
#define MODULAR_CARD_RENDERER_H

#include "config.h"
#include <irrlicht.h>
#include <map>
#include <string>
#include <vector>

namespace ygo {

// Card type constants
enum class CardType { MONSTER, SPELL, TRAP, TOKEN };

enum class CardSubtype {
  MONSTER_NORMAL,
  MONSTER_EFFECT,
  MONSTER_RITUAL,
  MONSTER_FUSION,
  MONSTER_SYNCHRO,
  MONSTER_XYZ,
  MONSTER_LINK,
  SPELL_NORMAL,
  SPELL_QUICKPLAY,
  SPELL_RITUAL,
  SPELL_CONTINUOUS,
  SPELL_FIELD,
  SPELL_EQUIP,
  TRAP_NORMAL,
  TRAP_COUNTER,
  TRAP_CONTINUOUS
};

enum class MonsterAttribute { DARK, EARTH, FIRE, LIGHT, WATER, WIND, DIVINE };

enum class LinkArrow {
  UP_LEFT,
  UP,
  UP_RIGHT,
  LEFT,
  RIGHT,
  DOWN_LEFT,
  DOWN,
  DOWN_RIGHT
};

// Card data structure
struct ModularCardData {
  std::wstring cardName;
  CardType cardType;
  CardSubtype cardSubtype;
  std::wstring cardEffect;
  uint32_t cardId; // Card ID number (e.g., 46986414 for Blue-Eyes)

  MonsterAttribute monsterAttribute;
  std::wstring monsterType;
  std::wstring monsterTypeLine; // Full type line like "[Dragon / Effect]"
  int monsterLevelRankLink;
  int monsterATK;
  int monsterDEF;
  std::wstring monsterATKStr;
  std::wstring monsterDEFStr;
  std::vector<std::wstring> monsterAbilities;
  std::vector<LinkArrow> linkArrows;

  bool isPendulum;
  int pendulumScale;
  std::wstring pendulumEffect;

  bool isNormalMonster; // Normal monsters have italic flavor text

  // Constructor with defaults
  ModularCardData();
};

// Main renderer class
class ModularCardRenderer {
public:
  // Base resolution constants (1180x1720)
  static constexpr int BASE_WIDTH = 1180;
  static constexpr int BASE_HEIGHT = 1720;

  ModularCardRenderer(irr::IrrlichtDevice *device);
  ~ModularCardRenderer();

  // Main rendering function
  irr::video::ITexture *RenderCard(const ModularCardData &card,
                                   irr::video::ITexture *artTexture = nullptr);

  // Individual rendering components (matching daominah-card-engine)
  void RenderCardFrame(const ModularCardData &card);
  void RenderCardArt(irr::video::ITexture *artTexture,
                     const ModularCardData &card);
  void RenderCardName(const ModularCardData &card);
  void RenderCardAttribute(const ModularCardData &card);
  void RenderCardTypeLevelRank(const ModularCardData &card);
  void RenderLinkArrow(const ModularCardData &card);
  void RenderCardEffect(const ModularCardData &card);
  void RenderPendulum(const ModularCardData &card);
  void RenderMonsterTypeLine(const ModularCardData &card);
  void RenderStats(const ModularCardData &card);

  // Helper functions
  float ChooseFontSize(const std::wstring &text, int width, int height,
                       irr::gui::IGUIFont *font);
  void FitTextOneLine(const std::wstring &text, irr::core::rect<irr::s32> rect,
                      irr::gui::IGUIFont *font, float scaleFont = 1.0f);

private:
  irr::IrrlichtDevice *device;
  irr::video::IVideoDriver *driver;
  irr::gui::IGUIEnvironment *env;

  // Render target for the card
  irr::video::ITexture *renderTarget;

  // Font cache
  irr::gui::IGUIFont *fontCardName;
  irr::gui::IGUIFont *fontEffect;        // 42px tier 1
  irr::gui::IGUIFont *fontEffectMedium;  // 38px tier 2
  irr::gui::IGUIFont *fontEffectSmall;   // 32px tier 3
  irr::gui::IGUIFont *fontEffectSmaller; // 28px tier 4
  irr::gui::IGUIFont *fontEffectTiny;    // 24px tier 5
  irr::gui::IGUIFont *fontEffectItalic;  // 42px italic for normal monsters
  irr::gui::IGUIFont *fontStats;
  irr::gui::IGUIFont *fontTypeLine;
  irr::gui::IGUIFont *fontCardId; // Font for 8-digit card ID

  void RenderCardId(const ModularCardData &card);

  // Name font cache for dynamic sizing (keyed by size in half-pixels)
  std::map<int, irr::gui::IGUIFont *> nameFontCache;

  // Get or create name font at specific size
  irr::gui::IGUIFont *GetNameFontForSize(int sizeHalfPx);

  // Level/Rank star textures
  irr::video::ITexture *levelStarTexture;
  irr::video::ITexture *rankStarTexture;

  void RenderSpellTrapTypeLine(const ModularCardData &card);
  const wchar_t *GetSpellTrapIconPath(CardSubtype subtype);

  // Link arrow textures
  std::map<LinkArrow, irr::video::ITexture *> linkArrowTextures;

  // Frame texture cache
  std::map<std::string, irr::video::ITexture *> frameTextures;
  std::map<std::string, irr::video::ITexture *> iconTextures;

  // Helper functions
  irr::video::ITexture *LoadFrameTexture(const ModularCardData &card);
  irr::video::ITexture *LoadIconTexture(const std::string &iconName);
  std::string GetFramePath(const ModularCardData &card) const;
};

} // namespace ygo

#endif // MODULAR_CARD_RENDERER_H
