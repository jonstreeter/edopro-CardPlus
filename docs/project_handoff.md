# EDOPro Modular Card Renderer - Requirements

## Project Goal

Create a high-quality modular card renderer that produces sharp, correctly scaled card images across all resolutions, with proper positioning of all card elements.

## Quality Requirements

1. **Sharp rendering** - No pixelation or blurriness
2. **Correct scaling** - Maintain aspect ratio at all display sizes
3. **Accurate positioning** - All elements positioned correctly
4. **Transparent backgrounds** - No grey or solid backgrounds
5. **High fidelity** - Professional quality appearance

## Technical Specifications

### Base Resolution
- **Width**: 1180 pixels
- **Height**: 1720 pixels  
- **Aspect Ratio**: 0.686 (standard Yu-Gi-Oh! card)

### Card Elements (Render Order)

1. Frame texture
2. Cropped card art
3. Attribute icon
4. Level/Rank stars  
5. Card name text
6. Monster type line
7. ATK/DEF/LINK stats
8. Spell/Trap type label + icon
9. Effect description text
10. Pendulum scales (if applicable)
11. Pendulum effect text (if applicable)
12. Link arrows (if applicable)

### Element Positioning (at 1180x1720)

**Card Art Regions:**
- Regular cards: (146, 318) to (1036, 1208) = 890×890 box
- Pendulum cards: (82, 310) to (1100, 1072) = 1018×762 box

**Text Regions:**
- Stats (ATK/DEF): (700, 1615) to (1080, 1685)
- Effect text: (65, 1370) to (1115, 1610)
- Name: Upper card area
- Type line: Mid-card area

**Icons:**
- Level/Rank stars: 80×80 pixels each
- Attribute icon: Proportionally scaled

### Font Specifications

**Font Files** (located in `textures/modular/font/`):
- `YGOSmallCaps.ttf` - Card names
- `MatrixRegular.ttf` - Effect text, labels
- `MatrixBold.ttf` - Stats (ATK/DEF)

**Font Sizes** (at base resolution):
- Name: 88-96px
- Effect: 44-48px
- Stats: 55-60px
- Labels: 35-40px

**Requirements:**
- Antialiasing enabled
- High-quality rendering
- Proper alpha channel support

## Asset Sources

### Frame & Icon Textures
**Location**: `textures/modular/`

Contains:
- Card frames (normal, pendulum, spell, trap, etc.)
- Attribute icons (LIGHT, DARK, WATER, etc.)
- Level/Rank star icons
- Link arrow markers
- Type icons

### Cropped Card Art

**Download URLs:**
- High-res: `https://mdygo2048.daominah.uk/{code}.jpg`
- Standard: `https://mdygo.daominah.uk/{code}.jpg`

**Cache Location**: `pics_modular/{code}.png`

**Download Strategy:**
- Prefer high-res when available
- Fallback to standard if high-res fails
- Store locally to avoid re-downloading
- Download asynchronously (non-blocking)

### Card Database
**Location**: `cards.cdb`  
Contains card metadata (names, stats, types, effects)

## Rendering Requirements

### Pipeline
1. Create render target texture at 1180×1720
2. Draw frame texture
3. Draw cropped art with bilinear filtering
4. Render text with antialiased fonts
5. Draw icons and symbols
6. Scale final texture to display size with high-quality filtering
7. Preserve alpha transparency throughout

### Quality Standards
- Use bilinear or better filtering for all texture operations
- Maintain transparency (alpha channel) at all stages
- Scale with aspect ratio preservation
- Apply high-quality anti-aliased scaling when displaying

### Display Integration
- Render to fixed 1180×1720 regardless of display size
- Calculate aspect-preserving destination rectangle
- Use highest quality scaling algorithm available
- Enable alpha blending for transparent areas

## Configuration

**Enable/Disable:**
- `gGameConfig->modularCardRenderer` - Toggle modular renderer

**Art Preference:**
- `gGameConfig->modularArtHighRes` - Prefer high-res downloads

## Build & Run

```cmd
quick_build.bat    # Build project
run_app.bat        # Launch application
```

## File Structure

**Core Files:**
- `gframe/modular_card_renderer.h/cpp` - Main renderer
- `gframe/modular_art_manager.h/cpp` - Art download/cache
- `gframe/drawing.cpp` - Display integration

**Assets:**
- `textures/modular/` - Frames, icons, fonts
- `pics_modular/` - Downloaded art cache

## GitHub Resources

**Main Projects:**
- **EDOPro**: [https://github.com/ProjectIgnis/EDOPro](https://github.com/ProjectIgnis/EDOPro)
- **Distribution (Assets)**: [https://github.com/ProjectIgnis/Distribution](https://github.com/ProjectIgnis/Distribution)

**Related Repositories:**
- **OCGCore**: [https://github.com/ProjectIgnis/OCGcore](https://github.com/ProjectIgnis/OCGcore)
- **Card Scripts**: [https://github.com/ProjectIgnis/CardScripts](https://github.com/ProjectIgnis/CardScripts)
- **BabelCDB (Card Database)**: [https://github.com/ProjectIgnis/BabelCDB](https://github.com/ProjectIgnis/BabelCDB)

**Asset Sources:**
- Art API documentation and sources available in Distribution repository
- Modular textures and fonts in Distribution assets

**Card Template Reference: daominah-card-engine (RECOMMENDED)**

Located in: `tools/daominah-card-engine/`

**Why This is the Superior Reference:**

1. **Exact Resolution Match**: Uses 1180×1720 base resolution (see `card_frame/size_1180x1720.txt`) - matches EDOPro requirements perfectly
2. **Complete Asset Library**: Includes all frame types, icons, and assets you need
3. **Production-Quality Code**: ~1800 lines of well-structured JavaScript with:
   - Precise positioning calculations for all card elements
   - Dynamic font sizing algorithms (`chooseFontSize()`, `fitTextOneLine()`)
   - Text overflow handling and automatic scaling
   - Proper baseline alignment for multi-line text
4. **All Card Types**: Supports regular, pendulum, link, xyz, synchro, ritual, fusion, spell, trap
5. **Canvas Rendering**: Uses HTML5 Canvas with quality optimizations:
   - Downsampling for high quality
   - Anti-aliasing techniques
   - High-resolution export support
6. **Local & Ready**: Already in your project - no need for external dependencies

**Key Functions to Study:**
- `renderCard()` - Main rendering pipeline (line 724)
- `renderCardFrame()` - Frame selection logic (line 760)
- `fitTextOneLine()` - Text scaling with transform (line 448)
- `chooseFontSize()` - Dynamic font size calculation (line 485)
- `renderCardEffect()` - Multi-line text rendering

**Other Options (Less Suitable):**
- **Yugilife** - Focus on print quality but non-standard resolution
- **Ygocarder** - Resource-intensive, avoids 4K rendering
- **Card Conjurer** - More generic, not Yu-Gi-Oh! specific

**Recommendation**: Port the daominah-card-engine rendering logic to C++/Irrlicht. The coordinate systems, scaling algorithms, and text rendering approaches translate directly.




