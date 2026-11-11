#include "global.h"
#include "ui_intro_options.h"
#include "strings.h"
#include "bg.h"
#include "data.h"
#include "decompress.h"
#include "event_data.h"
#include "naming_screen.h"
#include "field_weather.h"
#include "gpu_regs.h"
#include "graphics.h"
#include "item.h"
#include "item_menu.h"
#include "item_menu_icons.h"
#include "list_menu.h"
#include "item_icon.h"
#include "item_use.h"
#include "international_string_util.h"
#include "main.h"
#include "malloc.h"
#include "menu.h"
#include "menu_helpers.h"
#include "palette.h"
#include "party_menu.h"
#include "scanline_effect.h"
#include "script.h"
#include "sound.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "text_window.h"
#include "trainer_see.h"
#include "overworld.h"
#include "event_data.h"
#include "constants/items.h"
#include "constants/field_weather.h"
#include "constants/songs.h"
#include "constants/rgb.h"
#include "mgba_printf/mgba.h"
#include "mgba_printf/mini_printf.h"

/*
 * 
 */
 
//==========DEFINES==========//

#define OPTION_NAME_LENGTH 150
#define MAX_OPTIONS_PER_SETTING 5
#define MAX_OPTION_DESCRIPTION_LENGTH 50
#define NUM_OPTIONS_ON_SCREEN 5 

enum{
    SETTING_DIFFICULTY,
    SETTING_LEVEL_CAP,
    SETTING_BOSS_RUSH,
    SETTING_RANDOMIZER_MODE,
    SETTING_RANDOMIZER_ABILITY_MODE,
    SETTING_RANDOMIZER_INNATE_MODE,
    SETTING_RANDOMIZER_MOVE_MODE,
    SETTING_RANDOMIZER_TYPE_MODE,
    SETTING_INDIVIDUAL_COLORS,
    SETTING_PERMANENT_MEGA_MODE,
    SETTING_NUZLOCKE_CAPTURES,
    NUM_INTRO_OPTIONS,
};

struct MenuResources
{
    MainCallback savedCallback;     // determines callback to run when we exit. e.g. where do we want to go after closing the menu
    u8 gfxLoadState;
    u8 currentOptionId;
    u8 currentFirstOption;
    u8 temporal_settings[NUM_INTRO_OPTIONS];
};

enum WindowIds
{
    WINDOW_1,
};

//==========EWRAM==========//
static EWRAM_DATA struct MenuResources *sMenuDataPtr = NULL;
static EWRAM_DATA u8 *sBg1TilemapBuffer = NULL;

//==========STATIC=DEFINES==========//
static void Menu_RunSetup(void);
static bool8 Menu_DoGfxSetup(void);
static bool8 Menu_InitBgs(void);
static void Menu_FadeAndBail(void);
static bool8 Menu_LoadGraphics(void);
static void Menu_InitWindows(void);
static void PrintToWindow(u8 windowId, u8 colorIdx);
static void Task_MenuWaitFadeIn(u8 taskId);
static void Task_MenuMain(u8 taskId);

//==========CONST=DATA==========//
static const struct BgTemplate sMenuBgTemplates[] =
{
    {
        .bg = 0,    // windows, etc
        .charBaseIndex = 0,
        .mapBaseIndex = 31,
        .priority = 1
    }, 
    {
        .bg = 1,    // this bg loads the UI tilemap
        .charBaseIndex = 3,
        .mapBaseIndex = 30,
        .priority = 2
    },
    {
        .bg = 2,    // this bg loads the UI tilemap
        .charBaseIndex = 0,
        .mapBaseIndex = 28,
        .priority = 0
    }
};

static const struct WindowTemplate sMenuWindowTemplates[] = 
{
    [WINDOW_1] = 
    {
        .bg = 0,            // which bg to print text on
        .tilemapLeft = 0,   // position from left (per 8 pixels)
        .tilemapTop = 0,    // position from top (per 8 pixels)
        .width = 30,        // width (per 8 pixels)
        .height = 20,       // height (per 8 pixels)
        .paletteNum = 0,    // palette index to use for text
        .baseBlock = 1,     // tile start in VRAM
    }
};

static const u32 sMenuTiles[]          = INCBIN_U32("graphics/ui_menus/intro_options/tiles.4bpp.lz");
static const u32 sMenuTilemap[]        = INCBIN_U32("graphics/ui_menus/intro_options/tilemap.bin.lz");
static const u16 sMenuPalette[]        = INCBIN_U16("graphics/ui_menus/intro_options/palette.gbapal");
static const u8 sOptionMenuSelector[]  = INCBIN_U8("graphics/ui_menus/intro_options/selector.4bpp");
static const u8 sOptionMenuSelector2[] = INCBIN_U8("graphics/ui_menus/intro_options/selector2.4bpp");

enum Colors
{
    FONT_BLACK,
    FONT_WHITE,
    FONT_RED,
    FONT_BLUE,
};

static const u8 sMenuWindowFontColors[][3] = 
{
    [FONT_BLACK]  = {TEXT_COLOR_TRANSPARENT,  2,  1},
    [FONT_WHITE]  = {TEXT_COLOR_TRANSPARENT,  15, TEXT_COLOR_TRANSPARENT},
    [FONT_RED]    = {TEXT_COLOR_TRANSPARENT,  TEXT_COLOR_RED,        TEXT_COLOR_LIGHT_GRAY},
    [FONT_BLUE]   = {TEXT_COLOR_TRANSPARENT,  TEXT_COLOR_BLUE,       TEXT_COLOR_LIGHT_GRAY},
};

//==========FUNCTIONS==========//
// UI loader template
void Task_OpenIntroOptionMenuFromStartMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        CleanupOverworldWindowsAndTilemaps();
        Intro_Options_Menu_Init(CB2_ReturnToFieldWithOpenMenu);
        DestroyTask(taskId);
    }
}

static void SaveOptionsData()
{
    gSaveBlock2Ptr->gameDifficulty          = sMenuDataPtr->temporal_settings[SETTING_DIFFICULTY];
    gSaveBlock2Ptr->levelCaps               = sMenuDataPtr->temporal_settings[SETTING_LEVEL_CAP];
    if (sMenuDataPtr->temporal_settings[SETTING_RANDOMIZER_MODE] < 2) {
        gSaveBlock2Ptr->encounterRandomizedMode = sMenuDataPtr->temporal_settings[SETTING_RANDOMIZER_MODE];
        gSaveBlock2Ptr->encounterRandomizedLegendaryMode = FALSE;
    }
    else if (sMenuDataPtr->temporal_settings[SETTING_RANDOMIZER_MODE] == 2) {
        gSaveBlock2Ptr->encounterRandomizedMode = TRUE;
        gSaveBlock2Ptr->encounterRandomizedLegendaryMode = TRUE;
    }
    else {
        gSaveBlock2Ptr->encounterRandomizedMode = FALSE;
        gSaveBlock2Ptr->encounterRandomizedLegendaryMode = TRUE;
    }
    gSaveBlock2Ptr->innaterandomizedMode    = sMenuDataPtr->temporal_settings[SETTING_RANDOMIZER_INNATE_MODE];
    gSaveBlock2Ptr->abilityRandomizedMode   = sMenuDataPtr->temporal_settings[SETTING_RANDOMIZER_ABILITY_MODE];
    gSaveBlock2Ptr->individualColors        = sMenuDataPtr->temporal_settings[SETTING_INDIVIDUAL_COLORS];
    gSaveBlock2Ptr->moveRandomizedMode      = sMenuDataPtr->temporal_settings[SETTING_RANDOMIZER_MOVE_MODE];
    gSaveBlock2Ptr->typeRandomizedMode      = sMenuDataPtr->temporal_settings[SETTING_RANDOMIZER_TYPE_MODE];
    SetBossRushEnabled(sMenuDataPtr->temporal_settings[SETTING_BOSS_RUSH]);
    gSaveBlock2Ptr->permanentMegaMode       = sMenuDataPtr->temporal_settings[SETTING_PERMANENT_MEGA_MODE];
    gSaveBlock2Ptr->nuzlockeCaptures        = sMenuDataPtr->temporal_settings[SETTING_NUZLOCKE_CAPTURES];
}

static void LoadOptionsData()
{
    sMenuDataPtr->temporal_settings[SETTING_DIFFICULTY] = gSaveBlock2Ptr->gameDifficulty;
    sMenuDataPtr->temporal_settings[SETTING_LEVEL_CAP] = gSaveBlock2Ptr->levelCaps;
    if (gSaveBlock2Ptr->encounterRandomizedLegendaryMode == FALSE && gSaveBlock2Ptr->encounterRandomizedMode == TRUE) {
        sMenuDataPtr->temporal_settings[SETTING_RANDOMIZER_MODE] = 1;
    }
    else if (gSaveBlock2Ptr->encounterRandomizedLegendaryMode == TRUE && gSaveBlock2Ptr->encounterRandomizedMode == TRUE) {
        sMenuDataPtr->temporal_settings[SETTING_RANDOMIZER_MODE] = 2;
    }
    else if (gSaveBlock2Ptr->encounterRandomizedLegendaryMode == FALSE && gSaveBlock2Ptr->encounterRandomizedMode == FALSE) {
        sMenuDataPtr->temporal_settings[SETTING_RANDOMIZER_MODE] = 0;
    }
    else {
        sMenuDataPtr->temporal_settings[SETTING_RANDOMIZER_MODE] = 3;
    }
    sMenuDataPtr->temporal_settings[SETTING_RANDOMIZER_INNATE_MODE]  = gSaveBlock2Ptr->innaterandomizedMode;
    sMenuDataPtr->temporal_settings[SETTING_RANDOMIZER_ABILITY_MODE] = gSaveBlock2Ptr->abilityRandomizedMode;
    sMenuDataPtr->temporal_settings[SETTING_INDIVIDUAL_COLORS]       = gSaveBlock2Ptr->individualColors;
    sMenuDataPtr->temporal_settings[SETTING_RANDOMIZER_MOVE_MODE]    = gSaveBlock2Ptr->moveRandomizedMode;
    sMenuDataPtr->temporal_settings[SETTING_RANDOMIZER_TYPE_MODE]    = gSaveBlock2Ptr->typeRandomizedMode;
    sMenuDataPtr->temporal_settings[SETTING_BOSS_RUSH]               = IsBossRushEnabled();
    sMenuDataPtr->temporal_settings[SETTING_PERMANENT_MEGA_MODE]     = gSaveBlock2Ptr->permanentMegaMode;
    sMenuDataPtr->temporal_settings[SETTING_NUZLOCKE_CAPTURES]       = gSaveBlock2Ptr->nuzlockeCaptures;
}

// This is our main initialization function if you want to call the menu from elsewhere
void Intro_Options_Menu_Init(MainCallback callback)
{
    u8 i;
    if ((sMenuDataPtr = AllocZeroed(sizeof(struct MenuResources))) == NULL)
    {
        SetMainCallback2(callback);
        return;
    }
    
    // initialize stuff
    sMenuDataPtr->gfxLoadState = 0;
    sMenuDataPtr->currentOptionId = 0;
    sMenuDataPtr->currentFirstOption = 0;

    for (i = 0; i < NUM_INTRO_OPTIONS; i++) {
        sMenuDataPtr->temporal_settings[i] = 0; 
    }

    LoadOptionsData();
    
    sMenuDataPtr->savedCallback = callback;
    
    SetMainCallback2(Menu_RunSetup);
}

static void Menu_RunSetup(void)
{
    while (1)
    {
        if (Menu_DoGfxSetup() == TRUE)
            break;
    }
}

static void Menu_MainCB(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void Menu_VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static bool8 Menu_DoGfxSetup(void)
{
    switch (gMain.state)
    {
    case 0:
        DmaClearLarge16(3, (void *)VRAM, VRAM_SIZE, 0x1000);
        SetVBlankHBlankCallbacksToNull();
        ClearScheduledBgCopiesToVram();
        gMain.state++;
        break;
    case 1:
        ScanlineEffect_Stop();
        FreeAllSpritePalettes();
        ResetPaletteFade();
        ResetSpriteData();
        ResetTasks();
        gMain.state++;
        break;
    case 2:
        if (Menu_InitBgs())
        {
            sMenuDataPtr->gfxLoadState = 0;
            gMain.state++;
        }
        else
        {
            Menu_FadeAndBail();
            return TRUE;
        }
        break;
    case 3:
        if (Menu_LoadGraphics() == TRUE)
            gMain.state++;
        break;
    case 4:
        LoadMessageBoxAndBorderGfx();
        Menu_InitWindows();
        gMain.state++;
        break;
    case 5:
        PrintToWindow(WINDOW_1, FONT_BLACK);
        CreateTask(Task_MenuWaitFadeIn, 0);
        BlendPalettes(0xFFFFFFFF, 16, RGB_BLACK);
        gMain.state++;
        break;
    case 6:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB_BLACK);
        gMain.state++;
        break;
    default:
        SetVBlankCallback(Menu_VBlankCB);
        SetMainCallback2(Menu_MainCB);
        return TRUE;
    }
    return FALSE;
}

#define try_free(ptr) ({        \
    void ** ptr__ = (void **)&(ptr);   \
    if (*ptr__ != NULL)                \
        Free(*ptr__);                  \
})

static void Menu_FreeResources(void)
{
    FlagClear(FLAG_SYS_DEBUG_MENU_OPENED);
    try_free(sMenuDataPtr);
    try_free(sBg1TilemapBuffer);
    FreeAllWindowBuffers();
}


static void Task_MenuWaitFadeAndBail(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sMenuDataPtr->savedCallback);
        Menu_FreeResources();
        DestroyTask(taskId);
    }
}

static void Menu_FadeAndBail(void)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB_BLACK);
    CreateTask(Task_MenuWaitFadeAndBail, 0);
    SetVBlankCallback(Menu_VBlankCB);
    SetMainCallback2(Menu_MainCB);
}

static bool8 Menu_InitBgs(void)
{
    ResetAllBgsCoordinates();
    sBg1TilemapBuffer = Alloc(0x800);
    if (sBg1TilemapBuffer == NULL)
        return FALSE;
    
    memset(sBg1TilemapBuffer, 0, 0x800);
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sMenuBgTemplates, NELEMS(sMenuBgTemplates));
    SetBgTilemapBuffer(1, sBg1TilemapBuffer);
    ScheduleBgCopyTilemapToVram(1);
    ShowBg(0);
    ShowBg(1);
    ShowBg(2);
    return TRUE;
}

static bool8 Menu_LoadGraphics(void)
{
    switch (sMenuDataPtr->gfxLoadState)
    {
    case 0:
        ResetTempTileDataBuffers();
        DecompressAndCopyTileDataToVram(1, sMenuTiles, 0, 0, 0);
        sMenuDataPtr->gfxLoadState++;
        break;
    case 1:
        if (FreeTempTileDataBuffersIfPossible() != TRUE)
        {
            LZDecompressWram(sMenuTilemap, sBg1TilemapBuffer);
            sMenuDataPtr->gfxLoadState++;
        }
        break;
    case 2:
        LoadPalette(sMenuPalette, 0, 32);
        sMenuDataPtr->gfxLoadState++;
        break;
    default:
        sMenuDataPtr->gfxLoadState = 0;
        return TRUE;
    }
    return FALSE;
}

static void Menu_InitWindows(void)
{
    INIT_WINDOWS(sMenuWindowTemplates);
    DeactivateAllTextPrinters();
    ScheduleBgCopyTilemapToVram(0);
    
    FillWindowPixelBuffer(WINDOW_1, 0);
    LoadUserWindowBorderGfx(WINDOW_1, 720, 14 * 16);
    PutWindowTilemap(WINDOW_1);
    CopyWindowToVram(WINDOW_1, 3);
    
    ScheduleBgCopyTilemapToVram(2);
}


static void PressedDownButton() {
    u8 halfScreen      = (NUM_OPTIONS_ON_SCREEN) / 2;    //When it starts scrolling the options
    u8 finalhalfScreen = NUM_INTRO_OPTIONS - halfScreen; //When it stops 5

    if (sMenuDataPtr->currentOptionId < halfScreen) {
        //If you are below the option where it starts scrolling
        sMenuDataPtr->currentOptionId++;
    }
	else if (sMenuDataPtr->currentOptionId >= (NUM_INTRO_OPTIONS - 1)) { 
        //If you are in the last option go to the first one
		sMenuDataPtr->currentOptionId    = 0;
		sMenuDataPtr->currentFirstOption = 0;
    }
    else if (sMenuDataPtr->currentOptionId >= (finalhalfScreen - 1)) {
        //If you are near the end of the options
        sMenuDataPtr->currentOptionId++;
    }
	else {
        //Start Scrolling
        sMenuDataPtr->currentOptionId++;
        sMenuDataPtr->currentFirstOption++;
    }
}

static void PressedUpButton() {
    u8 halfScreen      = (NUM_OPTIONS_ON_SCREEN) / 2;    //When it starts scrolling the options
    u8 finalhalfScreen = NUM_INTRO_OPTIONS - halfScreen; //When it stops 5

    if (sMenuDataPtr->currentOptionId > halfScreen &&            //You are after it starts scrolling
       sMenuDataPtr->currentOptionId <= (finalhalfScreen - 1)) { //You are below it stops scrolling
        //Scrolls
        sMenuDataPtr->currentOptionId--;
        sMenuDataPtr->currentFirstOption--;
    }
	else if (sMenuDataPtr->currentOptionId == 0) { 
        //If you are in the first option go to the last one
		sMenuDataPtr->currentOptionId    = NUM_INTRO_OPTIONS - 1;
		sMenuDataPtr->currentFirstOption = NUM_INTRO_OPTIONS - NUM_OPTIONS_ON_SCREEN;
    }
    else {
        sMenuDataPtr->currentOptionId--;
    }
}

struct OptionData
{
    const u8 title[OPTION_NAME_LENGTH];
    const u8 options[OPTION_NAME_LENGTH][OPTION_NAME_LENGTH];
    const u8 optionDescription[OPTION_NAME_LENGTH][OPTION_NAME_LENGTH];
    u8 numOptions;
};

const struct OptionData Intro_Options[NUM_INTRO_OPTIONS] = {
    [SETTING_DIFFICULTY] =
    {
        .title = _("难度"),
        .options = { 
            _("简单"),
            _("进阶"),
            _("精英"),
            _("地狱"),
            },
        .optionDescription = { 
            _("对手不分配努力值，你则有。战斗中\n无法使用背包。强制关闭换人提示。"),
            _("队伍与简单模式相同，但：每个对手\n都有自定义努力值。这是标准难度。"),
            _("在进阶模式基础上：AI 拥有全部天赋，\n你需要解锁。队伍更强，仅供高手挑战。"),
            _("在精英模式基础上：AI 更聪明，会违规\n配招配特性。纯粹的折磨在等着你！"),
            },
        .numOptions = 4,
    },
    [SETTING_LEVEL_CAP] =
    {
        .title = _("等级上限"),
        .options = { 
            _("简单"),
            _("进阶"),
            _("精英"),
            //_("Off"),
            },
        .optionDescription = { 
            _("想轻松一点就选它。前六枚徽章\n设等级上限。联盟前可升到 100 级。"),
            _("适合初次接触难度改版。每枚徽章解锁新的\n等级上限。挑战冠军前到不了 100 级。"),
            _("最高难度，真正的挑战。常需琢磨\n全新战术。准备好迎接痛苦了吗？"),
            //_("No Level Cap Description"),
            },
        //.numOptions = 4,
        .numOptions = 3,
    },
    [SETTING_BOSS_RUSH] =
    {
        .title = _("首领连战"),
        .options = {
            _("关闭"),
            _("开启"),
            },
        .optionDescription = {
            _("训练家仍会正常发现你。"),
            _("普通训练家不会发现你。主动对话仍可\n发起对战。随时可以关闭。"),
            },
        .numOptions = 2,
    },
    [SETTING_RANDOMIZER_MODE] =
    {
        .title = _("遭遇随机化"),
        .options = { 
            _("关闭"),
            _("普通"),
            _("传说"),
            _("同步")
            },
        .optionDescription = { 
            _("遭遇设置保持不变。"),
            _("野生遭遇完全随机。不影响固定遭遇，\n例如赠送或传说宝可梦。"),
            _("野生遭遇完全随机。可能遇到传说宝可梦，\n仅影响普通遭遇。"),
            _("野生遭遇按权重随机。出现的宝可梦\n与所在地区相关。")
            },
        .numOptions = 4,
    },
    [SETTING_RANDOMIZER_ABILITY_MODE] =
    {
        .title = _("特性随机化"),
        .options = { 
            _("关闭"),
            _("开启"),
            },
        .optionDescription = { 
            _("特性设置保持不变。"),
            _("特性完全随机。例如一只小拉达是大力士，\n另一只则是降雨。"),
            },
        .numOptions = 2,
    },
    [SETTING_RANDOMIZER_INNATE_MODE] =
    {
        .title = _("天赋随机化"),
        .options = { 
            _("关闭"),
            _("开启"),
            },
        .optionDescription = { 
            _("天赋设置保持不变。"),
            _("天赋完全随机。例如一只巨沼怪是加速，\n另一只是食草。"),
            },
        .numOptions = 2,
    },
    [SETTING_INDIVIDUAL_COLORS] =
    {
        .title = _("个体配色"),
        .options = { 
            _("关闭"),
            _("开启"),
            },
        .optionDescription = { 
            _("熟悉的普通配色。"),
            _("每只宝可梦配色都不同。\n可随时开关。"),
            },
        .numOptions = 2,
    },
    [SETTING_RANDOMIZER_MOVE_MODE] =
    {
        .title = _("招式随机化"),
        .options = { 
            _("关闭"),
            _("开启"),
            },
        .optionDescription = { 
            _("招式设置保持不变。"),
            _("招式完全随机。\n包括升级、学习器、教学、遗传。"),
            },
        .numOptions = 2,
    },
    [SETTING_RANDOMIZER_TYPE_MODE] =
    {
        .title = _("属性随机化"),
        .options = { 
            _("关闭"),
            _("开启"),
            },
        .optionDescription = { 
            _("属性设置保持不变。"),
            _("宝可梦属性完全随机。例如妙蛙种子可能\n是火/幽灵。战斗界面会提示属性变化。"),
            },
        .numOptions = 2,
    },
    [SETTING_PERMANENT_MEGA_MODE] =
    {
        .title = _("永久Mega进化"),
        .options = { 
            _("关闭"),
            _("开启"),
            },
        .optionDescription = { 
            _("常规Mega进化。"),
            _("宝可梦可像普通进化那样\n进化为Mega形态。"),
            },
        .numOptions = 2,
    },
    [SETTING_NUZLOCKE_CAPTURES] =
    {
        .title = _("Nuzlocke捕捉"),
        .options = { 
            _("关闭"),
            _("开启"),
            },
        .optionDescription = { 
            _("常规捕捉。"),
            _("纽兹洛克规则：每个区域只能遭遇一只。\n该路线的搜索器与野生遭遇关闭。"),
            },
        .numOptions = 2,
    },
    
};

static const u8 sText_Menu_Title[] = _("Elite Redux 设置 - 按 START 保存");
static void PrintToWindow(u8 windowId, u8 colorIdx)
{
    u8 i;
    u8 currentOption      = sMenuDataPtr->currentOptionId;
    u8 currentFirstOption = sMenuDataPtr->currentFirstOption;
    u8 cursorHeight       = (sMenuDataPtr->currentOptionId - sMenuDataPtr->currentFirstOption) * 2;
    u8 x = 2;
    u8 y = 0;
    u8 x2 = 0;
    
    FillWindowPixelBuffer(windowId, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));

    //Title
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8), 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF,  sText_Menu_Title);

    x = 3;
    y = 3;
    for (i = 0; i < NUM_OPTIONS_ON_SCREEN; i++) {
        //Title
        AddTextPrinterParameterized4(windowId, FONT_NORMAL, (x * 8) + x2, (y * 8), 0, 0, sMenuWindowFontColors[colorIdx], 0xFF,  Intro_Options[currentFirstOption + i].title);
        //Current Option
        x2 = 140;
        AddTextPrinterParameterized4(windowId, FONT_NORMAL, (x * 8) + x2, (y * 8), 0, 0, sMenuWindowFontColors[colorIdx], 0xFF,  Intro_Options[currentFirstOption + i].options[sMenuDataPtr->temporal_settings[currentFirstOption + i]]);
        y = y + 2;
        x2 = 0;
    }

    //Description
    // 中文改造：描述框白底只有 y=118..153 共 36 px，FONT_SMALL 行高 = 汉字字身 12 px，
    // 三行正好填满会把首行压上边框、末行贴底。译文统一排成两行，起点从 116 下移到 122，
    // 两行落在 122/134，上下各留 4/8 px。改动见 work/cn/任务23_修复记录04.md。
    x = 3;
    y = 14;
    AddTextPrinterParameterized4(windowId, FONT_SMALL, (x * 8) + x2, (y * 8) + 10, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF,  Intro_Options[sMenuDataPtr->currentOptionId].optionDescription[sMenuDataPtr->temporal_settings[currentOption]]);

    //Cursor
    x = 2;
    y = 3 + cursorHeight;
    x2 = 140;
    BlitBitmapToWindow(windowId, sOptionMenuSelector,  (x * 8) + x2, (y * 8), 8, 16);
    x = 9;
    BlitBitmapToWindow(windowId, sOptionMenuSelector2, (x * 8) + x2, (y * 8), 8, 16);

    PutWindowTilemap(windowId);
    CopyWindowToVram(windowId, 3);
}

static void Task_MenuWaitFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
        gTasks[taskId].func = Task_MenuMain;
}

static void Task_MenuTurnOff(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        if (FlagGet(FLAG_SYS_DEBUG_MENU_OPENED))
            SetMainCallback2(sMenuDataPtr->savedCallback);
        else
            DoNamingScreen(0, gSaveBlock2Ptr->playerName, gSaveBlock2Ptr->playerGender, 0, 0, sMenuDataPtr->savedCallback);
        Menu_FreeResources();
        DestroyTask(taskId);
    }
}

/* This is the meat of the UI. This is where you wait for player inputs and can branch to other tasks accordingly */
static void Task_MenuMain(u8 taskId)
{
    if (JOY_NEW(START_BUTTON))
    {
        SaveOptionsData();
        PlaySE(SE_PC_OFF);
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_MenuTurnOff;
    }

    
    if (JOY_NEW(DPAD_LEFT))
    {
        if (sMenuDataPtr->temporal_settings[sMenuDataPtr->currentOptionId] == 0) {
            sMenuDataPtr->temporal_settings[sMenuDataPtr->currentOptionId] = Intro_Options[sMenuDataPtr->currentOptionId].numOptions - 1;
        }
        else {
            sMenuDataPtr->temporal_settings[sMenuDataPtr->currentOptionId]--;
        }
        PlaySE(SE_SELECT);
        PrintToWindow(WINDOW_1, FONT_BLACK);
    }

    if (JOY_NEW(DPAD_RIGHT))
    {
        if (sMenuDataPtr->temporal_settings[sMenuDataPtr->currentOptionId] == Intro_Options[sMenuDataPtr->currentOptionId].numOptions - 1) {
            sMenuDataPtr->temporal_settings[sMenuDataPtr->currentOptionId] = 0;
        }
        else {
            sMenuDataPtr->temporal_settings[sMenuDataPtr->currentOptionId]++;
        }
        PlaySE(SE_SELECT);
        PrintToWindow(WINDOW_1, FONT_BLACK);
    }

    if (JOY_NEW(DPAD_UP))
    {
        PressedUpButton();
        PlaySE(SE_SELECT);
        PrintToWindow(WINDOW_1, FONT_BLACK);
    }

    if (JOY_NEW(DPAD_DOWN))
    {
        PressedDownButton();
        PlaySE(SE_SELECT);
        PrintToWindow(WINDOW_1, FONT_BLACK);
    }
}
