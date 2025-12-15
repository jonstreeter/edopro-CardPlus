@echo off
setlocal enabledelayedexpansion

echo ========================================
echo  EDOPro CardPlus - Asset Setup
echo ========================================
echo.

:: Check if we're in the right directory
if not exist "ygoprodll.exe" (
    echo ERROR: Please run this script from the EDOPro CardPlus directory
    echo        where ygoprodll.exe is located.
    pause
    exit /b 1
)

:: Create directories if they don't exist
if not exist "sound" mkdir sound
if not exist "skin" mkdir skin
if not exist "config" mkdir config
if not exist "textures" mkdir textures

echo Downloading EDOPro assets...
echo.

:: Asset URLs (from Project Ignis distribution)
set ASSETS_URL=https://github.com/ProjectIgnis/Distribution/releases/latest/download

:: Download sounds
echo [1/4] Downloading sound files...
if not exist "sound\summon.wav" (
    echo       Downloading sound pack...
    powershell -Command "& {Invoke-WebRequest -Uri 'https://github.com/ProjectIgnis/Distribution/raw/master/sound.zip' -OutFile 'sound_temp.zip' -UseBasicParsing}" 2>nul
    if exist "sound_temp.zip" (
        powershell -Command "Expand-Archive -Path 'sound_temp.zip' -DestinationPath '.' -Force" 2>nul
        del sound_temp.zip 2>nul
        echo       Sound files installed!
    ) else (
        echo       Could not download sounds. You can manually copy sound files later.
    )
) else (
    echo       Sound files already present.
)

:: Download textures
echo [2/4] Checking textures...
if not exist "textures\cover.png" (
    echo       Downloading texture pack...
    powershell -Command "& {Invoke-WebRequest -Uri 'https://github.com/ProjectIgnis/Distribution/raw/master/textures.zip' -OutFile 'textures_temp.zip' -UseBasicParsing}" 2>nul
    if exist "textures_temp.zip" (
        powershell -Command "Expand-Archive -Path 'textures_temp.zip' -DestinationPath '.' -Force" 2>nul
        del textures_temp.zip 2>nul
        echo       Textures installed!
    ) else (
        echo       Could not download textures. You can manually copy texture files later.
    )
) else (
    echo       Textures already present.
)

:: Download default skin
echo [3/4] Checking skins...
if not exist "skin\skin.xml" (
    echo       Creating default skin directory...
    if not exist "skin\Default" mkdir "skin\Default"
    echo       Skins can be downloaded from EDOPro distribution.
) else (
    echo       Skins already present.
)

:: Setup config
echo [4/4] Checking configuration...
if not exist "config\system.conf" (
    echo       Creating default configuration...
    (
        echo #config file
        echo use_d3d = 0
        echo antialias = 2
        echo errorlog = 1
        echo nickname = Player
        echo gamename = Game
        echo lastdeck = 
        echo textfont = fonts/NotoSansJP-Regular.otf 14
        echo numfont = fonts/NotoSansJP-Regular.otf
        echo serverport = 7911
        echo lastip = 127.0.0.1
        echo lastport = 7911
        echo roompass = 
        echo automonsterpos = 0
        echo autospellpos = 0
        echo randompos = 0
        echo autochain = 0
        echo waitchain = 0
        echo showchain = 0
        echo mute_opponent = 0
        echo mute_spectators = 0
        echo use_lflist = 1
        echo default_lflist = 0
        echo default_rule = 5
        echo hide_setname = 0
        echo hide_hint_button = 0
        echo control_mode = 0
        echo draw_field_spell = 1
        echo separate_clear_button = 1
        echo auto_search_limit = -1
        echo search_multiple_keywords = 1
        echo ignore_deck_changes = 0
        echo default_ot = 1
        echo enable_bot_mode = 1
        echo quick_animation = 0
        echo auto_save_replay = 0
        echo show_unofficial = 1
        echo show_scope_label = 1
        echo hide_passcode_scope = 0
        echo show_fps = 1
        echo draw_single_chain = 0
        echo skin_index = -1
        echo hide_hand_count = 0
        echo prefer_expansion_script = 1
        echo window_maximized = 0
        echo window_width = 1024
        echo window_height = 640
        echo resize_popup_menu = 0
        echo enable_pendulum_scale = 1
        echo chain_buttons = 1
        echo vsync = 1
        echo max_fps = 60
        echo dpi_scale = 1.000000
        echo accurate_bg_resize = 1
        echo enable_music = 1
        echo enable_sound = 1
        echo music_volume = 0.200000
        echo sound_volume = 0.200000
        echo save_hand_test_replay = 0
        echo loop_music = 1
        echo discord_integration = 0
    ) > config\system.conf
    echo       Configuration created!
) else (
    echo       Configuration already present.
)

echo.
echo ========================================
echo  Asset Setup Complete!
echo ========================================
echo.
echo You can now run ygoprodll.exe to start the game.
echo.
pause
