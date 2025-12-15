#include "modular_card_renderer.h"
#include "CGUITTFont/CGUITTFont.h"
#include "data_manager.h"
#include "game_config.h"
#include "image_manager.h"
#include "logging.h"
#include "utils.h"
#include <ITexture.h>
#include <IVideoDriver.h>

#include <cmath>

namespace ygo {

// ModularCardData constructor with defaults
ModularCardData::ModularCardData()
    : cardName(L""), cardType(CardType::MONSTER),
      cardSubtype(CardSubtype::MONSTER_NORMAL), cardEffect(L""), cardId(0),
      monsterAttribute(MonsterAttribute::DARK), monsterType(L"Warrior"),
      monsterLevelRankLink(0), monsterATK(0), monsterDEF(0),
      monsterATKStr(L"0"), monsterDEFStr(L"0"), isPendulum(false),
      pendulumScale(0), pendulumEffect(L""), isNormalMonster(false) {}

ModularCardRenderer::ModularCardRenderer(irr::IrrlichtDevice *device)
    : device(device), driver(device->getVideoDriver()),
      env(device->getGUIEnvironment()), renderTarget(nullptr),
      fontCardName(nullptr), fontEffect(nullptr), fontStats(nullptr) {
  // Enable mipmaps for the render target to ensure smooth scaling
  bool prevMipMap =
      driver->getTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS);
  driver->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, true);

  // Create the render target texture (1180x1720)
  renderTarget = driver->addRenderTargetTexture(
      irr::core::dimension2d<irr::u32>(BASE_WIDTH, BASE_HEIGHT),
      "ModularCardRenderTarget", irr::video::ECF_A8R8G8B8);

  // Restore previous mipmap setting
  driver->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, prevMipMap);

  // Load fonts using CGUITTFont - proper EDOPro font loading
  // Font files from reference CSS:
  // fontCardName: YGOSmallCaps.ttf - for card name
  // fontCardName: matrix_bold_small_caps.ttf - for card name (small caps style)
  // fontCardType: StoneSerifSmallCapsBold.ttf - for monster type line
  // fontATKValue: MatrixRegular.ttf - for ATK/DEF values
  GameConfig::TextFont nameFont{
      EPRO_TEXT("textures/modular/font/matrix_bold_small_caps.ttf"), 114};
  GameConfig::TextFont effectFont{
      EPRO_TEXT("textures/modular/font/StoneSerifStd-Medium.ttf"), 42};
  GameConfig::TextFont effectFontMedium{
      EPRO_TEXT("textures/modular/font/StoneSerifStd-Medium.ttf"),
      38}; // Between 42 and 32
  GameConfig::TextFont effectFontSmall{
      EPRO_TEXT("textures/modular/font/StoneSerifStd-Medium.ttf"), 32};
  GameConfig::TextFont effectFontSmaller{
      EPRO_TEXT("textures/modular/font/StoneSerifStd-Medium.ttf"),
      28}; // Between 32 and 24
  GameConfig::TextFont effectFontTiny{
      EPRO_TEXT("textures/modular/font/StoneSerifStd-Medium.ttf"), 24};
  // Italic font for normal monster flavor text
  GameConfig::TextFont effectFontItalic{
      EPRO_TEXT("textures/modular/font/StoneSerifITC-MediumItalic.ttf"), 42};
  GameConfig::TextFont statsFont{
      EPRO_TEXT("textures/modular/font/MatrixRegular.ttf"), 64};
  GameConfig::TextFont typeLineFont{
      EPRO_TEXT("textures/modular/font/StoneSerifSmallCapsBold.ttf"), 38};
  GameConfig::TextFont cardIdFont{
      EPRO_TEXT("textures/modular/font/StoneSerifStd-Medium.ttf"),
      32}; // Slightly larger than 26px box to look good

  // Fallback fonts removed as per user request to use single font
  // (StoneSerifStd)
  static GameConfig::FallbackFonts fallbackFonts;
  // if (fallbackFonts.empty()) {
  //    fallbackFonts.push_back({EPRO_TEXT("C:/Windows/Fonts/arial.ttf"), 0});
  // }

  fontCardName =
      irr::gui::CGUITTFont::createTTFont(env, nameFont, fallbackFonts);
  fontEffect = irr::gui::CGUITTFont::createTTFont(
      env, effectFont, fallbackFonts); // StoneSerifStd
  fontEffectMedium =
      irr::gui::CGUITTFont::createTTFont(env, effectFontMedium, fallbackFonts);
  fontEffectSmall =
      irr::gui::CGUITTFont::createTTFont(env, effectFontSmall, fallbackFonts);
  fontEffectSmaller =
      irr::gui::CGUITTFont::createTTFont(env, effectFontSmaller, fallbackFonts);
  fontEffectTiny =
      irr::gui::CGUITTFont::createTTFont(env, effectFontTiny, fallbackFonts);
  fontEffectItalic =
      irr::gui::CGUITTFont::createTTFont(env, effectFontItalic, fallbackFonts);
  fontStats = irr::gui::CGUITTFont::createTTFont(env, statsFont, fallbackFonts);
  fontTypeLine =
      irr::gui::CGUITTFont::createTTFont(env, typeLineFont, fallbackFonts);
  fontCardId =
      irr::gui::CGUITTFont::createTTFont(env, cardIdFont, fallbackFonts);

  // Load level/rank star textures
  levelStarTexture =
      driver->getTexture("textures/modular/icon/GUI_T_Icon1_Other_Level.png");
  rankStarTexture =
      driver->getTexture("textures/modular/icon/GUI_T_Icon1_Other_Rank.png");

  // Load link arrow textures
  linkArrowTextures[LinkArrow::UP] =
      driver->getTexture("textures/modular/icon/L_U.png");
  linkArrowTextures[LinkArrow::DOWN] =
      driver->getTexture("textures/modular/icon/L_D.png");
  linkArrowTextures[LinkArrow::LEFT] =
      driver->getTexture("textures/modular/icon/L_L.png");
  linkArrowTextures[LinkArrow::RIGHT] =
      driver->getTexture("textures/modular/icon/L_R.png");
  linkArrowTextures[LinkArrow::UP_LEFT] =
      driver->getTexture("textures/modular/icon/L_UL.png");
  linkArrowTextures[LinkArrow::UP_RIGHT] =
      driver->getTexture("textures/modular/icon/L_UR.png");
  linkArrowTextures[LinkArrow::DOWN_LEFT] =
      driver->getTexture("textures/modular/icon/L_DL.png");
  linkArrowTextures[LinkArrow::DOWN_RIGHT] =
      driver->getTexture("textures/modular/icon/L_DR.png");
}

ModularCardRenderer::~ModularCardRenderer() {
  // Fonts are managed by Irrlicht environment, no need to drop them explicitly
  // The render target will be cleaned up by the driver

  // Clean up cached textures (Irrlicht manages these, but clear our maps)
  frameTextures.clear();
  iconTextures.clear();
  nameFontCache.clear();
}

irr::gui::IGUIFont *ModularCardRenderer::GetNameFontForSize(int sizeHalfPx) {
  // Check cache first
  auto it = nameFontCache.find(sizeHalfPx);
  if (it != nameFontCache.end()) {
    return it->second;
  }

  // Create new font at this size (convert half-pixels to pixels)
  int sizePx = sizeHalfPx / 2;
  if (sizePx < 20)
    sizePx = 20; // Minimum readable size

  GameConfig::TextFont nameFont{
      EPRO_TEXT("textures/modular/font/YGO_Card_NA.ttf"), sizePx};
  GameConfig::FallbackFonts fallbackFonts;

  irr::gui::IGUIFont *newFont =
      irr::gui::CGUITTFont::createTTFont(env, nameFont, fallbackFonts);

  // Cache and return
  nameFontCache[sizeHalfPx] = newFont;
  return newFont;
}

irr::video::ITexture *
ModularCardRenderer::RenderCard(const ModularCardData &card,
                                irr::video::ITexture *artTexture) {
  ErrorLog("ModularCardRenderer::RenderCard called for card: {}",
           Utils::ToUTF8IfNeeded(card.cardName));

  if (!renderTarget) {
    ErrorLog("ModularCardRenderer: renderTarget is null!");
    return nullptr;
  }

  if (!driver) {
    ErrorLog("ModularCardRenderer: driver is null!");
    return nullptr;
  }

  ErrorLog("ModularCardRenderer: Setting render target...");

  // Set render target
  driver->setRenderTarget(
      renderTarget, true, true,
      irr::video::SColor(0, 0, 0, 0)); // Transparent background for layering

  ErrorLog("ModularCardRenderer: Rendering components...");

  // Render individual components (matching daominah-card-engine order)
  // Draw Art FIRST, then partial Frame on top (to mask edges)
  RenderCardArt(artTexture, card);
  RenderCardFrame(card);
  RenderCardAttribute(card);
  RenderCardTypeLevelRank(card);
  RenderLinkArrow(card);
  RenderCardName(card);
  RenderMonsterTypeLine(card); // [Dragon/Effect] type line
  RenderCardEffect(card);
  RenderPendulum(card);
  RenderStats(card); // ATK/DEF or LINK rating
  RenderCardId(card);

  if (card.cardType == CardType::SPELL || card.cardType == CardType::TRAP) {
    RenderSpellTrapTypeLine(card);
  }

  // Reset render target
  driver->setRenderTarget(0, true, true);

  // Regenerate mipmaps now that rendering is complete
  renderTarget->regenerateMipMapLevels();

  return renderTarget;
}

const wchar_t *ModularCardRenderer::GetSpellTrapIconPath(CardSubtype subtype) {
  switch (subtype) {
  case CardSubtype::SPELL_NORMAL:
    return L"textures/modular/icon/attr_SPELL.png"; // Fallback? Logic says no
                                                    // icon.
  case CardSubtype::TRAP_NORMAL:
    return L"textures/modular/icon/attr_TRAP.png"; // Fallback? Logic says no
                                                   // icon.

  // Mapped from MapImg in index.js and file list
  case CardSubtype::TRAP_COUNTER:
    return L"textures/modular/icon/GUI_T_Icon1_Icon01.png";
  case CardSubtype::SPELL_FIELD:
    return L"textures/modular/icon/GUI_T_Icon1_Icon02.png";
  case CardSubtype::SPELL_EQUIP:
    return L"textures/modular/icon/GUI_T_Icon1_Icon03.png";
  case CardSubtype::SPELL_CONTINUOUS:
  case CardSubtype::TRAP_CONTINUOUS:
    return L"textures/modular/icon/GUI_T_Icon1_Icon04.png";
  case CardSubtype::SPELL_QUICKPLAY:
    return L"textures/modular/icon/GUI_T_Icon1_Icon05.png";
  case CardSubtype::SPELL_RITUAL:
    return L"textures/modular/icon/GUI_T_Icon1_Icon06.png";
  default:
    return nullptr;
  }
}

void ModularCardRenderer::RenderSpellTrapTypeLine(const ModularCardData &card) {
  if (!fontTypeLine)
    return;

  // CSS: .cRenderCardType right: 110px, top: 210px.
  // Canvas Width 1180.
  // Right 110 => Right Edge = 1070.
  // Width 480 => Left Edge = 590.
  // Text Align Right.

  // Icon: .cRenderCardSubtype right: 136px, top: 210px.
  // Right 136 => Right Edge = 1044.
  // Width 72px => Left Edge = 972.
  // Height 72px.
  // Icon Y = 210.

  std::wstring typeText;
  bool hasIcon = false;
  const wchar_t *iconPath = nullptr;

  // Logic from index.js:
  // If Normal: "[Spell Card]" or "[Trap Card]"
  // If Subtype: "[Spell Card     ]" (5 spaces)

  // Note: index.js uses non-breaking spaces " " (0xA0).
  // We can use normal spaces if font handles them, or wide char 0xA0.
  // Let's use normal spaces first. "     " (5 spaces).

  if (card.cardType == CardType::SPELL) {
    if (card.cardSubtype == CardSubtype::SPELL_NORMAL) {
      typeText = L"[Spell Card]";
    } else {
      typeText = L"[Spell Card     ]";
      hasIcon = true;
      iconPath = GetSpellTrapIconPath(card.cardSubtype);
    }
  } else if (card.cardType == CardType::TRAP) {
    if (card.cardSubtype == CardSubtype::TRAP_NORMAL) {
      typeText = L"[Trap Card]";
    } else {
      typeText = L"[Trap Card     ]";
      hasIcon = true;
      iconPath = GetSpellTrapIconPath(card.cardSubtype);
    }
  }

  // Render Text
  // Rect: 590, 210, 1070, 290 (height 80)
  irr::core::rect<irr::s32> textRect(590, 210, 1070, 290);
  // Draw right aligned
  // fontTypeLine->draw does not support alignment param other than center?
  // We need to measure text and adjust X.
  irr::core::dimension2d<irr::u32> textDim =
      fontTypeLine->getDimension(typeText.c_str());
  int textX = 1070 - textDim.Width;
  irr::core::rect<irr::s32> drawRect(textX, 210, 1070, 290);

  irr::video::SColor textColor(255, 255, 255, 255); // White text for S/T?
  // index.js renderCardName logic:
  // if Spell/Trap -> color = "white".
  // Wait, renderCardType?
  // index.js line 925: renderCardTypeLevelRank... doesn't explicitly set color.
  // Inherits? The frame is S/T... usually text is white on S/T frames?
  // But renderCardName explicitly sets white.
  // Let's check index.css for .cRenderCardType color.
  // Default color black?
  // S/T names are white. Type line?
  // Type line on S/T cards is usually BLACK.
  // Re-check index.js line 1211: renderMiscFooter sets colors.
  // No mention of CardType color modification in renderCardTypeLevelRank other
  // than display. I'll assume BLACK for now.

  textColor = irr::video::SColor(255, 0, 0, 0); // Black

  fontTypeLine->draw(typeText.c_str(), drawRect, textColor, false, true);

  // Render Icon if needed
  if (hasIcon && iconPath) {
    irr::video::ITexture *iconTex = driver->getTexture(iconPath);
    if (iconTex) {
      // Icon Rect: Right 136 -> X 972. Width 72.
      // X: 1044 (Right Edge) - 72 = 972.
      // Y: 210.
      // W: 72, H: 72.
      irr::core::rect<irr::s32> iconRect(972, 210, 972 + 72, 210 + 72);

      // Draw with alpha channel
      driver->draw2DImage(
          iconTex, iconRect,
          irr::core::rect<irr::s32>(0, 0, iconTex->getOriginalSize().Width,
                                    iconTex->getOriginalSize().Height),
          nullptr, nullptr, true);
    }
  }
}

void ModularCardRenderer::RenderCardId(const ModularCardData &card) {
  if (!fontCardId || card.cardId == 0)
    return;

  // CSS: .cRenderKonamiCardID { left:70, top:1640, width:500, height:26 }
  // Position for card ID (8-digit code)
  irr::core::rect<irr::s32> idRect(70, 1640, 70 + 500, 1640 + 26 + 10);

  std::wstring idStr = std::to_wstring(card.cardId);
  // Pad with leading zeros to 8 digits if needed
  while (idStr.length() < 8) {
    idStr = L"0" + idStr;
  }

  irr::video::SColor textColor(255, 0, 0, 0); // Black text
  fontCardId->draw(idStr.c_str(), idRect, textColor, false,
                   true); // Centered vertically
}
void ModularCardRenderer::RenderCardFrame(const ModularCardData &card) {
  irr::video::ITexture *frameTexture = LoadFrameTexture(card);

  if (frameTexture) {
    // Draw the frame at full size (1180x1720)
    driver->draw2DImage(
        frameTexture, irr::core::rect<irr::s32>(0, 0, BASE_WIDTH, BASE_HEIGHT),
        irr::core::rect<irr::s32>(0, 0, frameTexture->getSize().Width,
                                  frameTexture->getSize().Height),
        nullptr, nullptr, true);
  }
}

void ModularCardRenderer::RenderCardArt(irr::video::ITexture *artTexture,
                                        const ModularCardData &card) {
  if (!artTexture)
    return;

  // Art regions from reference CSS
  irr::core::rect<irr::s32> artRegion;
  irr::core::rect<irr::s32> srcRect;

  int artWidth = artTexture->getSize().Width;
  int artHeight = artTexture->getSize().Height;

  if (card.isPendulum) {
    // Pendulum cards: left:82, top:310, width:1018, height:762
    // Scale to fit width while preserving aspect ratio
    int targetWidth = 1018;
    int targetHeight = 762;

    // Calculate scaled height based on target width
    float aspectRatio = (float)artWidth / (float)artHeight;
    int scaledHeight = (int)(targetWidth / aspectRatio);

    if (scaledHeight <= targetHeight) {
      // Image is wider than or equal to target - scale to height and center
      // horizontally
      int scaledWidth = (int)(targetHeight * aspectRatio);
      int xOffset = (scaledWidth - targetWidth) / 2;
      artRegion = irr::core::rect<irr::s32>(82, 310, 82 + targetWidth,
                                            310 + targetHeight);
      // Crop source horizontally to center
      int srcCropX = (int)((float)xOffset / scaledWidth * artWidth);
      int srcWidth = (int)((float)targetWidth / scaledWidth * artWidth);
      srcRect = irr::core::rect<irr::s32>(srcCropX, 0, srcCropX + srcWidth,
                                          artHeight);
    } else {
      // Image is taller - scale to width and crop bottom
      artRegion = irr::core::rect<irr::s32>(82, 310, 82 + targetWidth,
                                            310 + targetHeight);
      // Only show top portion of source that fits in aspect ratio
      int srcHeight = (int)((float)targetHeight / scaledHeight * artHeight);
      srcRect = irr::core::rect<irr::s32>(0, 0, artWidth, srcHeight);
    }
  } else {
    // Regular cards: left:146, top:318, width:890, height:890
    artRegion = irr::core::rect<irr::s32>(146, 318, 146 + 890, 318 + 890);
    srcRect = irr::core::rect<irr::s32>(0, 0, artWidth, artHeight);
  }

  // Draw the art texture with bilinear filtering
  driver->draw2DImage(artTexture, artRegion, srcRect, nullptr, nullptr, true);
}

void ModularCardRenderer::RenderCardName(const ModularCardData &card) {
  if (card.cardName.empty() || !fontCardName)
    return;

  // Name region from CSS: left:88, top:96, width:880, height:76
  const int nameX = 88;
  const int nameY = 96;
  const int nameWidth = 880;
  const int nameHeight = 76;

  // Determine text color based on card type
  irr::video::SColor textColor(255, 0, 0, 0); // Black default

  if (card.cardType == CardType::SPELL || card.cardType == CardType::TRAP) {
    textColor = irr::video::SColor(255, 255, 255, 255); // White
  } else if (card.cardSubtype == CardSubtype::MONSTER_XYZ ||
             card.cardSubtype == CardSubtype::MONSTER_LINK) {
    textColor = irr::video::SColor(255, 255, 255, 255); // White
  }

  // Measure text at full size
  irr::core::dimension2d<irr::u32> textDim =
      fontCardName->getDimension(card.cardName.c_str());
  int textWidth = textDim.Width;
  int textHeight = textDim.Height;

  // Calculate horizontal scale factor (like CSS transform: scaleX)
  float scaleX = 1.0f;
  if (textWidth > nameWidth) {
    scaleX = (float)nameWidth / (float)textWidth;
  }

  // Scaled destination width
  int destWidth = (int)(textWidth * scaleX);
  int destHeight = textHeight;

  // Center vertically within the name box
  int destY = nameY + (nameHeight - destHeight) / 2;

  // Draw text directly - for horizontal compression we use a scaled destination
  // rect Since Irrlicht's font draw doesn't support scaling, we draw normally
  // if fits For overflow, we render to temp texture then scale

  if (scaleX >= 1.0f) {
    // Text fits - draw normally
    irr::core::rect<irr::s32> nameRect(nameX, destY, nameX + nameWidth,
                                       destY + textHeight);
    fontCardName->draw(card.cardName.c_str(), nameRect, textColor, false,
                       false);
  } else {
    // Text needs horizontal compression
    // Create temporary render target for the text
    irr::video::ITexture *tempTarget = driver->addRenderTargetTexture(
        irr::core::dimension2d<irr::u32>(textWidth, textHeight + 20),
        "TempNameText", irr::video::ECF_A8R8G8B8);

    if (tempTarget) {
      // Render text to temp texture
      driver->setRenderTarget(tempTarget, true, true,
                              irr::video::SColor(0, 0, 0, 0));
      irr::core::rect<irr::s32> tempRect(0, 0, textWidth, textHeight + 20);
      fontCardName->draw(card.cardName.c_str(), tempRect, textColor, false,
                         false);

      // Switch back to main render target
      driver->setRenderTarget(renderTarget, false, false);

      // Draw scaled text to final destination
      irr::core::rect<irr::s32> srcRect(0, 0, textWidth, textHeight);
      irr::core::rect<irr::s32> destRect(nameX, destY, nameX + destWidth,
                                         destY + destHeight);
      driver->draw2DImage(tempTarget, destRect, srcRect, nullptr, nullptr,
                          true);

      // Clean up temp texture
      driver->removeTexture(tempTarget);
    }
  }
}

void ModularCardRenderer::RenderCardAttribute(const ModularCardData &card) {
  // Load attribute icon
  std::string iconName;

  if (card.cardType == CardType::SPELL) {
    iconName = "attr_SPELL";
  } else if (card.cardType == CardType::TRAP) {
    iconName = "attr_TRAP";
  } else {
    // Monster attribute
    switch (card.monsterAttribute) {
    case MonsterAttribute::DARK:
      iconName = "attr_DARK";
      break;
    case MonsterAttribute::EARTH:
      iconName = "attr_EARTH";
      break;
    case MonsterAttribute::FIRE:
      iconName = "attr_FIRE";
      break;
    case MonsterAttribute::LIGHT:
      iconName = "attr_LIGHT";
      break;
    case MonsterAttribute::WATER:
      iconName = "attr_WATER";
      break;
    case MonsterAttribute::WIND:
      iconName = "attr_WIND";
      break;
    case MonsterAttribute::DIVINE:
      iconName = "attr_DIVINE";
      break;
    }
  }

  irr::video::ITexture *attrIcon = LoadIconTexture(iconName);
  if (attrIcon) {
    // Attribute icon from CSS: left:974, top:78, width:122, height:122
    irr::core::rect<irr::s32> iconRect(974, 78, 974 + 122, 78 + 122);
    driver->draw2DImage(attrIcon, iconRect,
                        irr::core::rect<irr::s32>(0, 0,
                                                  attrIcon->getSize().Width,
                                                  attrIcon->getSize().Height),
                        nullptr, nullptr, true);
  }
}

void ModularCardRenderer::RenderCardTypeLevelRank(const ModularCardData &card) {
  // Only render for monsters (not spell/trap)
  if (card.cardType != CardType::MONSTER && card.cardType != CardType::TOKEN)
    return;

  // Link monsters don't have level/rank
  if (card.cardSubtype == CardSubtype::MONSTER_LINK)
    return;

  int numStars = card.monsterLevelRankLink;
  if (numStars <= 0 || numStars > 12)
    return;

  // Determine which star texture to use
  irr::video::ITexture *starTexture = nullptr;
  bool isXyz = (card.cardSubtype == CardSubtype::MONSTER_XYZ);

  if (isXyz) {
    starTexture = rankStarTexture; // Black stars for XYZ
  } else {
    starTexture = levelStarTexture; // Red stars for level
  }

  if (!starTexture)
    return;

  // Star sizing from reference: starWidth = Math.floor(levelsRanksWidth / 12 -
  // 2) levelsRanksWidth = 988 (from CSS), so starWidth = floor(988/12 - 2) =
  // floor(82.3 - 2) = 80
  const int starSize = 80;
  const int starSpacing = 2;
  const int starRowHeight = 80; // CSS height
  const int starY =
      210 + (starRowHeight - starSize) / 2; // Center vertically in row
  const int starRowStart = 100;
  const int starRowWidth = 988;

  // Calculate total width of all stars with spacing
  int totalStarsWidth = numStars * starSize + (numStars - 1) * starSpacing;

  // For level stars: right-aligned (start from right edge minus total width)
  // For XYZ rank stars: left-aligned (start from left edge)
  int firstStarX;
  if (isXyz) {
    // XYZ: left-aligned
    firstStarX = starRowStart;
  } else {
    // Level: right-aligned
    firstStarX = starRowStart + starRowWidth - totalStarsWidth;
  }

  for (int i = 0; i < numStars; i++) {
    int x = firstStarX + i * (starSize + starSpacing);

    irr::core::rect<irr::s32> starRect(x, starY, x + starSize,
                                       starY + starSize);
    driver->draw2DImage(
        starTexture, starRect,
        irr::core::rect<irr::s32>(0, 0, starTexture->getSize().Width,
                                  starTexture->getSize().Height),
        nullptr, nullptr, true);
  }
}

void ModularCardRenderer::RenderLinkArrow(const ModularCardData &card) {
  // Only render for Link monsters
  if (card.cardSubtype != CardSubtype::MONSTER_LINK)
    return;

  // Link arrow positions from CSS reference
  struct ArrowPosition {
    LinkArrow arrow;
    int x, y, w, h;
  };

  // CSS positions (1180x1720 canvas):
  // UpLeft: left:106, top:278, 80x80
  // Up: left:500, top:256, 180x70
  // UpRight: right:102 (=998), top:278, 80x80
  // Left: left:80, top:672, 70x180
  // Right: right:76 (=1034), top:672, 70x180
  // DownLeft: left:106, top:1168, 80x80
  // Down: left:500, top:1200, 180x70
  // DownRight: right:102 (=998), top:1168, 80x80
  const ArrowPosition arrowPositions[] = {
      {LinkArrow::UP_LEFT, 106, 278, 80, 80},
      {LinkArrow::UP, 500, 256, 180, 70},
      {LinkArrow::UP_RIGHT, 994, 278, 80, 80},
      {LinkArrow::LEFT, 80, 672, 70, 180},
      {LinkArrow::RIGHT, 1030, 672, 70, 180},
      {LinkArrow::DOWN_LEFT, 106, 1168, 80, 80},
      {LinkArrow::DOWN, 500, 1200, 180, 70},
      {LinkArrow::DOWN_RIGHT, 994, 1168, 80, 80}};

  for (const auto &pos : arrowPositions) {
    // Check if this arrow is active
    bool isActive = false;
    for (const auto &arrow : card.linkArrows) {
      if (arrow == pos.arrow) {
        isActive = true;
        break;
      }
    }

    if (isActive) {
      auto it = linkArrowTextures.find(pos.arrow);
      if (it != linkArrowTextures.end() && it->second) {
        irr::core::rect<irr::s32> arrowRect(pos.x, pos.y, pos.x + pos.w,
                                            pos.y + pos.h);
        driver->draw2DImage(
            it->second, arrowRect,
            irr::core::rect<irr::s32>(0, 0, it->second->getSize().Width,
                                      it->second->getSize().Height),
            nullptr, nullptr, true);
      }
    }
  }
}

void ModularCardRenderer::RenderCardEffect(const ModularCardData &card) {
  if (!fontEffect || card.cardEffect.empty())
    return;

  // Effect text region - varies by card type
  // CSS references from index.css:
  // .cRenderSpellTrapEffect: left:92, bottom:112, 1000x310
  // .cRenderMonsterEffect: left:92, bottom:158, 1000x218
  // For pendulum cards, monster effect is in smaller box below pendulum area
  irr::core::rect<irr::s32> effectRect;
  int boxHeight;
  const int TOP_PADDING = 4; // Small padding below type line

  if (card.cardType == CardType::SPELL || card.cardType == CardType::TRAP) {
    // Spell/Trap: left:92, bottom:112px on 1720 canvas = top:1298, height:310
    effectRect = irr::core::rect<irr::s32>(92, 1298 + TOP_PADDING, 92 + 1000,
                                           1720 - 112);
    boxHeight = 310 - TOP_PADDING;
  } else if (card.isPendulum) {
    // Pendulum monster effect: smaller box, starts after pendulum effect area
    // Must be below Type Line (ends at 1334)
    effectRect = irr::core::rect<irr::s32>(92, 1344 + TOP_PADDING, 92 + 1000,
                                           1344 + 218);
    boxHeight = 218 - TOP_PADDING;
  } else {
    // Regular monster: left:92, bottom:158 on 1720 = top:1344, height:218
    effectRect = irr::core::rect<irr::s32>(92, 1344 + TOP_PADDING, 92 + 1000,
                                           1344 + 218);
    boxHeight = 218 - TOP_PADDING;
  }

  irr::video::SColor textColor(255, 0, 0, 0);
  std::wstring text = card.cardEffect;
  int maxWidth = effectRect.getWidth();

  // Helper lambda to calculate lines for a given font
  auto calculateLines = [&](irr::gui::IGUIFont *font) {
    std::vector<std::wstring> lines;
    std::wstring currentLine;
    std::wstring word;

    for (size_t i = 0; i <= text.length(); i++) {
      wchar_t c = (i < text.length()) ? text[i] : L' ';

      if (c == L' ' || c == L'\n' || i == text.length()) {
        std::wstring testLine =
            currentLine + (currentLine.empty() ? L"" : L" ") + word;
        irr::core::dimension2d<irr::u32> dim =
            font->getDimension(testLine.c_str());

        if (dim.Width > (irr::u32)maxWidth && !currentLine.empty()) {
          lines.push_back(currentLine);
          currentLine = word;
        } else {
          currentLine = testLine;
        }
        word.clear();

        if (c == L'\n') {
          lines.push_back(currentLine);
          currentLine.clear();
        }
      } else {
        word += c;
      }
    }
    if (!currentLine.empty()) {
      lines.push_back(currentLine);
    }
    return lines;
  };

  // For normal monsters, use italic font for flavor text
  irr::gui::IGUIFont *baseFont = fontEffect;
  if (card.isNormalMonster && fontEffectItalic) {
    baseFont = fontEffectItalic;
  }

  // Font tier cascade: 42px → 38px → 32px → 28px → 24px
  irr::gui::IGUIFont *selectedFont = baseFont;
  int lineHeight = 42;
  std::vector<std::wstring> lines = calculateLines(baseFont);

  // Tier 2: 38px
  if (fontEffectMedium && (int)lines.size() * lineHeight > boxHeight) {
    lines = calculateLines(fontEffectMedium);
    selectedFont = fontEffectMedium;
    lineHeight = 38;
  }
  // Tier 3: 32px
  if (fontEffectSmall && (int)lines.size() * lineHeight > boxHeight) {
    lines = calculateLines(fontEffectSmall);
    selectedFont = fontEffectSmall;
    lineHeight = 32;
  }
  // Tier 4: 28px
  if (fontEffectSmaller && (int)lines.size() * lineHeight > boxHeight) {
    lines = calculateLines(fontEffectSmaller);
    selectedFont = fontEffectSmaller;
    lineHeight = 28;
  }
  // Tier 5: 24px
  if (fontEffectTiny && (int)lines.size() * lineHeight > boxHeight) {
    lines = calculateLines(fontEffectTiny);
    selectedFont = fontEffectTiny;
    lineHeight = 24;
  }

  // Draw lines
  int currentY = effectRect.UpperLeftCorner.Y;
  for (const auto &line : lines) {
    if (currentY + lineHeight > effectRect.LowerRightCorner.Y)
      break;
    irr::core::rect<irr::s32> lineRect(effectRect.UpperLeftCorner.X, currentY,
                                       effectRect.LowerRightCorner.X,
                                       currentY + lineHeight);
    selectedFont->draw(line.c_str(), lineRect, textColor, false, false);
    currentY += lineHeight;
  }
}

void ModularCardRenderer::RenderPendulum(const ModularCardData &card) {
  if (!card.isPendulum)
    return;

  // Render pendulum scales on left and right
  // CSS: .cRenderPScaleLeft: left:98, bottom:480, 56x72
  // CSS: .cRenderPScaleRight: right:98, bottom:480, 56x72
  // Bottom 480 on 1720 canvas = Y = 1720 - 480 - 72 = 1168
  if (fontStats) {
    std::wstring scaleStr = std::to_wstring(card.pendulumScale);
    irr::video::SColor scaleColor(255, 0, 0, 0);

    // Left scale position from CSS: left:98, bottom:480, 56x72
    irr::core::rect<irr::s32> leftScaleRect(98, 1168, 98 + 56, 1168 + 72);
    fontStats->draw(scaleStr.c_str(), leftScaleRect, scaleColor, true, true);

    // Right scale position from CSS: right:98 = 1180-98-56 = 1026
    irr::core::rect<irr::s32> rightScaleRect(1026, 1168, 1026 + 56, 1168 + 72);
    fontStats->draw(scaleStr.c_str(), rightScaleRect, scaleColor, true, true);
  }

  // Pendulum effect from CSS: left:184, top:1086, 816x188
  // Small 4px top padding for breathing room
  if (fontEffectSmall && !card.pendulumEffect.empty()) {
    const int TOP_PADDING = 4;
    irr::core::rect<irr::s32> pEffectRect(184, 1086 + TOP_PADDING, 184 + 816,
                                          1086 + 188);
    int boxHeight = 188 - TOP_PADDING;
    int maxWidth = pEffectRect.getWidth();
    irr::video::SColor textColor(255, 0, 0, 0);
    std::wstring text = card.pendulumEffect;

    ErrorLog("RenderPendulum: Text length={}, boxHeight={}, startY={}",
             text.length(), boxHeight, pEffectRect.UpperLeftCorner.Y);

    // Word wrap helper - same as RenderCardEffect
    auto wrapText = [&](irr::gui::IGUIFont *font) {
      std::vector<std::wstring> lines;
      std::wstring currentLine, word;
      for (size_t i = 0; i <= text.length(); i++) {
        wchar_t c = (i < text.length()) ? text[i] : L' ';
        if (c == L' ' || c == L'\n' || i == text.length()) {
          std::wstring testLine =
              currentLine + (currentLine.empty() ? L"" : L" ") + word;
          if (font->getDimension(testLine.c_str()).Width > (irr::u32)maxWidth &&
              !currentLine.empty()) {
            lines.push_back(currentLine);
            currentLine = word;
          } else {
            currentLine = testLine;
          }
          word.clear();
          if (c == L'\n') {
            lines.push_back(currentLine);
            currentLine.clear();
          }
        } else {
          word += c;
        }
      }
      if (!currentLine.empty())
        lines.push_back(currentLine);
      return lines;
    };

    // Helper to get actual font height
    auto getFontHeight = [](irr::gui::IGUIFont *font) -> int {
      return font->getDimension(L"Ag").Height;
    };

    // Pendulum effect font cascade: 32px → 28px → 24px
    irr::gui::IGUIFont *selectedFont = fontEffectSmall;
    std::vector<std::wstring> lines = wrapText(fontEffectSmall);
    int lineHeight = getFontHeight(fontEffectSmall);
    int tier = 1;

    // Tier 2: 28px
    if (fontEffectSmaller && (int)lines.size() * lineHeight > boxHeight) {
      lines = wrapText(fontEffectSmaller);
      selectedFont = fontEffectSmaller;
      lineHeight = getFontHeight(fontEffectSmaller);
      tier = 2;
    }
    // Tier 3: 24px
    if (fontEffectTiny && (int)lines.size() * lineHeight > boxHeight) {
      lines = wrapText(fontEffectTiny);
      selectedFont = fontEffectTiny;
      lineHeight = getFontHeight(fontEffectTiny);
      tier = 3;
    }

    ErrorLog("RenderPendulum: Using tier {}, lineHeight={}, lines={}, "
             "startY={}, rectTop={}, rectBottom={}",
             tier, lineHeight, lines.size(), pEffectRect.UpperLeftCorner.Y,
             pEffectRect.UpperLeftCorner.Y, pEffectRect.LowerRightCorner.Y);

    // Draw lines - start from the TOP of the effect box (after padding)
    // Use vcenter=false to draw text at the top of each line rect
    int currentY = pEffectRect.UpperLeftCorner.Y;
    for (const auto &line : lines) {
      if (currentY + lineHeight > pEffectRect.LowerRightCorner.Y)
        break;
      irr::core::rect<irr::s32> lineRect(
          pEffectRect.UpperLeftCorner.X, currentY,
          pEffectRect.LowerRightCorner.X, currentY + lineHeight);
      // vcenter=false draws from top of rect, not centered
      selectedFont->draw(line.c_str(), lineRect, textColor, false, false);
      currentY += lineHeight;
    }

    ErrorLog("RenderPendulum: Drew {} lines, final Y={}", lines.size(),
             currentY);
  }
}

void ModularCardRenderer::RenderMonsterTypeLine(const ModularCardData &card) {
  // Only render for monsters (not spell/trap)
  if (card.cardType != CardType::MONSTER && card.cardType != CardType::TOKEN)
    return;

  if (!fontTypeLine || card.monsterTypeLine.empty())
    return;

  // CSS: .cRenderMonsterAbilities: left:100, top:1292, width:980, height:42
  irr::core::rect<irr::s32> typeRect(100, 1292, 100 + 980, 1292 + 42);
  irr::video::SColor textColor(255, 0, 0, 0); // Black text

  // Type line text like "[Dragon / Effect]" - uses bold StoneSerifSmallCaps
  // font
  fontTypeLine->draw(card.monsterTypeLine.c_str(), typeRect, textColor, false,
                     true);
}

void ModularCardRenderer::RenderStats(const ModularCardData &card) {
  // Only render for monsters
  if (card.cardType != CardType::MONSTER)
    return;

  if (!fontStats)
    return;

  // Draw horizontal separator line above ATK/DEF
  // CSS: .cRenderMonsterSplitLine: left:92, bottom:152, width:996, height:4
  // y = 1720 - 152 - 4 = 1564
  irr::video::SColor lineColor(255, 0, 0, 0); // Black
  driver->draw2DRectangle(
      lineColor, irr::core::rect<irr::s32>(92, 1564, 92 + 996, 1564 + 4));

  irr::video::SColor textColor(255, 0, 0, 0); // Black text

  // Canvas Width: 1180px
  // Right edge should be at 1180 - 102 = 1078 (from CSS: right:102px)
  const int RIGHT_EDGE = 1078;
  const int LABEL_VALUE_GAP = 4; // Gap between label and value
  const int SECTION_GAP = 16;    // Gap between ATK and DEF/LINK sections

  // Get all text dimensions first
  std::wstring atkLabel = L"ATK/";
  std::wstring atkStr =
      (card.monsterATK >= 0) ? std::to_wstring(card.monsterATK) : L"?";
  irr::core::dimension2d<irr::u32> atkLabelDim =
      fontStats->getDimension(atkLabel.c_str());
  irr::core::dimension2d<irr::u32> atkDim =
      fontStats->getDimension(atkStr.c_str());

  std::wstring defLabel, defStr;
  irr::core::dimension2d<irr::u32> defLabelDim, defDim;

  if (card.cardSubtype == CardSubtype::MONSTER_LINK) {
    defLabel = L"LINK-";
    defStr = std::to_wstring(card.monsterLevelRankLink);
  } else {
    defLabel = L"DEF/";
    defStr = (card.monsterDEF >= 0) ? std::to_wstring(card.monsterDEF) : L"?";
  }
  defLabelDim = fontStats->getDimension(defLabel.c_str());
  defDim = fontStats->getDimension(defStr.c_str());

  // Calculate total width of the block
  int totalWidth = atkLabelDim.Width + LABEL_VALUE_GAP + atkDim.Width +
                   SECTION_GAP + defLabelDim.Width + LABEL_VALUE_GAP +
                   defDim.Width;

  // Calculate starting X so right edge aligns to RIGHT_EDGE
  int startX = RIGHT_EDGE - totalWidth;

  // Draw ATK section
  int currentX = startX;
  irr::core::rect<irr::s32> atkLabelRect(currentX, 1570,
                                         currentX + atkLabelDim.Width, 1614);
  fontStats->draw(atkLabel.c_str(), atkLabelRect, textColor, false, true);
  currentX += atkLabelDim.Width + LABEL_VALUE_GAP;

  irr::core::rect<irr::s32> atkValueRect(currentX, 1570,
                                         currentX + atkDim.Width, 1614);
  fontStats->draw(atkStr.c_str(), atkValueRect, textColor, false, true);
  currentX += atkDim.Width + SECTION_GAP;

  // Draw DEF/LINK section
  irr::core::rect<irr::s32> defLabelRect(currentX, 1570,
                                         currentX + defLabelDim.Width, 1614);
  fontStats->draw(defLabel.c_str(), defLabelRect, textColor, false, true);
  currentX += defLabelDim.Width + LABEL_VALUE_GAP;

  irr::core::rect<irr::s32> defValueRect(currentX, 1570,
                                         currentX + defDim.Width, 1614);
  fontStats->draw(defStr.c_str(), defValueRect, textColor, false, true);
}

std::string
ModularCardRenderer::GetFramePath(const ModularCardData &card) const {
  std::string basePath = "textures/modular/card_frame/";

  if (card.cardType == CardType::SPELL) {
    return basePath + "spell.png";
  } else if (card.cardType == CardType::TRAP) {
    return basePath + "trap.png";
  } else if (card.cardType == CardType::TOKEN) {
    return basePath + "monster_token.png";
  } else { // Monster
    if (card.isPendulum) {
      switch (card.cardSubtype) {
      case CardSubtype::MONSTER_NORMAL:
        return basePath + "pendulum_normal.png";
      case CardSubtype::MONSTER_EFFECT:
        return basePath + "pendulum_effect.png";
      case CardSubtype::MONSTER_RITUAL:
        return basePath + "pendulum_ritual.png";
      case CardSubtype::MONSTER_FUSION:
        return basePath + "pendulum_fusion.png";
      case CardSubtype::MONSTER_SYNCHRO:
        return basePath + "pendulum_synchro.png";
      case CardSubtype::MONSTER_XYZ:
        return basePath + "pendulum_xyz.png";
      default:
        return basePath + "pendulum_normal.png";
      }
    } else {
      switch (card.cardSubtype) {
      case CardSubtype::MONSTER_NORMAL:
        return basePath + "monster_normal.png";
      case CardSubtype::MONSTER_EFFECT:
        return basePath + "monster_effect.png";
      case CardSubtype::MONSTER_RITUAL:
        return basePath + "monster_ritual.png";
      case CardSubtype::MONSTER_FUSION:
        return basePath + "monster_fusion.png";
      case CardSubtype::MONSTER_SYNCHRO:
        return basePath + "monster_synchro.png";
      case CardSubtype::MONSTER_XYZ:
        return basePath + "monster_xyz.png";
      case CardSubtype::MONSTER_LINK:
        return basePath + "monster_link.png";
      default:
        return basePath + "monster_normal.png";
      }
    }
  }
}

irr::video::ITexture *
ModularCardRenderer::LoadFrameTexture(const ModularCardData &card) {
  std::string framePath = GetFramePath(card);

  // Check cache first
  auto it = frameTextures.find(framePath);
  if (it != frameTextures.end()) {
    return it->second;
  }

  // Load and cache
  irr::video::ITexture *texture = driver->getTexture(framePath.c_str());
  if (texture) {
    frameTextures[framePath] = texture;
  }

  return texture;
}

irr::video::ITexture *
ModularCardRenderer::LoadIconTexture(const std::string &iconName) {
  std::string iconPath = "textures/modular/icon/" + iconName + ".png";

  // Check cache first
  auto it = iconTextures.find(iconPath);
  if (it != iconTextures.end()) {
    return it->second;
  }

  // Load and cache
  irr::video::ITexture *texture = driver->getTexture(iconPath.c_str());
  if (texture) {
    iconTextures[iconPath] = texture;
  }

  return texture;
}

float ModularCardRenderer::ChooseFontSize(const std::wstring &text, int width,
                                          int height,
                                          irr::gui::IGUIFont *font) {
  // Simple implementation - start at 42 and reduce if needed
  // Full implementation would measure text and iterate
  return 42.0f;
}

void ModularCardRenderer::FitTextOneLine(const std::wstring &text,
                                         irr::core::rect<irr::s32> rect,
                                         irr::gui::IGUIFont *font,
                                         float scaleFont) {
  // Simple implementation - just draw the text
  // Full implementation would scale horizontally if text is too wide
  if (font) {
    font->draw(text.c_str(), rect, irr::video::SColor(255, 0, 0, 0), true,
               true);
  }
}

} // namespace ygo
