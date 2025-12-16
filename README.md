# EDOPro CardPlus

A fork of [EDOPro](https://github.com/edo9300/edopro) featuring an enhanced **modular card renderer** that displays cards with high-quality, dynamically composed artwork similar to official digital Yu-Gi-Oh! games.

![CardPlus Preview](docs/cardplus-preview.png)

## Features

### Modular Card Renderer
- **Dynamic card composition** - Cards are rendered in real-time using separate texture layers for frames, art, icons, and text
- **High-resolution output** - 1180x1720 pixel card renders for crisp display
- **Authentic styling** - Uses official-style fonts and layouts matching Master Duel/Duel Links aesthetics
- **Support for all card types**:
  - Monster cards (Normal, Effect, Fusion, Synchro, Xyz, Link, Ritual)
  - Pendulum cards with proper scale displays
  - Spell and Trap cards with type icons
- **Intelligent text scaling** - Effect text automatically sizes to fit the text box

### Enhanced UI
- Bilinear filtering for smooth card scaling
- Proper text alignment and spacing throughout

## Download

### Windows Build
📥 **[Download Latest Windows Release](https://github.com/jonstreeter/edopro-CardPlus/releases)**

### Installation
1. Download and extract the ZIP file
2. Run `ygoprodll.exe`

That's it! All game assets are included in the release.

### Network Play
CardPlus is **fully compatible** with regular EDOPro for online play. You can connect to the official ProjectIgnis servers and play against users running standard EDOPro.

## Building from Source

### Prerequisites
- Visual Studio 2022
- vcpkg (for dependencies)
- premake5

### Build Steps
```bash
# Generate project with sound support
premake5 vs2022 --sound=miniaudio

# Build Release configuration
msbuild build/ygo.sln /p:Configuration=Release /p:Platform=Win32
```

## Project Structure

```
CardPlus-specific files:
├── gframe/
│   ├── modular_card_renderer.cpp/h  # Core rendering engine
│   └── modular_art_manager.cpp/h    # Art texture management
├── textures/modular/
│   ├── card_frame/                   # Card frame overlays
│   ├── font/                         # Typography assets
│   └── icon/                         # Attribute/type icons
```

## Credits

- **EDOPro** - [edo9300/edopro](https://github.com/edo9300/edopro)
- **Project Ignis** - Original EDOPro development team
- **daominah-card-engine** - Reference implementation for card layout

## License

EDOPro CardPlus is free/libre and open source software licensed under the GNU Affero General Public License, version 3 or later.

Yu-Gi-Oh! is a trademark of Shueisha and Konami. This project is not affiliated with or endorsed by Shueisha or Konami.
