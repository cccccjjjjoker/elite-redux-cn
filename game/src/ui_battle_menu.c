#include "global.h"
#include "ui_battle_menu.h"
#include "strings.h"
#include "bg.h"
#include "battle.h"
#include "battle_main.h"
#include "battle_util.h"
#include "battle_script_commands.h"
#include "battle_anim.h"
#include "battle_ai_main.h"
#include "data.h"
#include "decompress.h"
#include "event_data.h"
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
#include "pokemon_icon.h"
#include "pokemon_summary_screen.h"
#include "scanline_effect.h"
#include "script.h"
#include "sound.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "text_window.h"
#include "overworld.h"
#include "util.h"
#include "event_data.h"
#include "generated/constants/abilities.h"
#include "generated/constants/species.h"
#include "constants/items.h"
#include "constants/field_weather.h"
#include "constants/songs.h"
#include "constants/rgb.h"
#include "mgba_printf/mgba.h"
#include "mgba_printf/mini_printf.h"
#include "trig.h"
#include "abilities.hh"

//==========DEFINES==========//
enum {
    SPRITE_ARR_ID_FIELD_ICON,
    SPRITE_ARR_ID_SELECTOR,
    SPRITE_ARR_ID_MON_ICON_1,
    SPRITE_ARR_ID_MON_ICON_2,
    SPRITE_ARR_ID_MON_ICON_3,
    SPRITE_ARR_ID_MON_ICON_4,
    SPRITE_ARR_ID_STATUS,
    // SPRITE_ARR_ID_TYPE_1,
    // SPRITE_ARR_ID_TYPE_2,
    // SPRITE_ARR_ID_TYPE_3,
    SPRITE_ARR_HELD_ITEM,
    SPRITE_ARR_ID_MON_ICON_1_SPEED,
    SPRITE_ARR_ID_MON_ICON_2_SPEED,
    SPRITE_ARR_ID_MON_ICON_3_SPEED,
    SPRITE_ARR_ID_MON_ICON_4_SPEED,
    SPRITE_ARR_ID_MON_ICON_1_PARTY_PLAYER,
    SPRITE_ARR_ID_MON_ICON_2_PARTY_PLAYER,
    SPRITE_ARR_ID_MON_ICON_3_PARTY_PLAYER,
    SPRITE_ARR_ID_MON_ICON_4_PARTY_PLAYER,
    SPRITE_ARR_ID_MON_ICON_5_PARTY_PLAYER,
    SPRITE_ARR_ID_MON_ICON_6_PARTY_PLAYER,
    SPRITE_ARR_ID_MON_ICON_1_PARTY_ENEMY,
    SPRITE_ARR_ID_MON_ICON_2_PARTY_ENEMY,
    SPRITE_ARR_ID_MON_ICON_3_PARTY_ENEMY,
    SPRITE_ARR_ID_MON_ICON_4_PARTY_ENEMY,
    SPRITE_ARR_ID_MON_ICON_5_PARTY_ENEMY,
    SPRITE_ARR_ID_MON_ICON_6_PARTY_ENEMY,
    NUM_SPRITES,
};

enum {
    FIELD_INFO_WEATHER,
    FIELD_INFO_TERRAIN,
    FIELD_INFO_INVERSE_ROOM,
    FIELD_INFO_TRICK_ROOM,
    FIELD_INFO_GRAVITY,
    FIELD_INFO_WONDER_ROOM,
    FIELD_INFO_MAGIC_ROOM,
    FIELD_INFO_FAIRY_LOCK,
    FIELD_INFO_MUD_SPORT,
    FIELD_INFO_WATER_SPORT,
    FIELD_INFO_ION_DELUGE,
    FIELD_INFO_QUASH,
    FIELD_INFO_CLEAR_SKIES,
    NUM_FIELD_INFO,
};

enum {
    SIDE_INFO_AURORA_VEIL,
    SIDE_INFO_REFLECT,
    SIDE_INFO_LIGHT_SCREEN,
    SIDE_INFO_TAILWIND,
    SIDE_INFO_SPIKES,
    SIDE_INFO_TOXIC_SPIKES,
    SIDE_INFO_STEALTH_ROCK,
    SIDE_INFO_CREEPING_THORNS,
    SIDE_INFO_STICKY_WEB,
    SIDE_INFO_SAFEGUARD,
    SIDE_INFO_MIST,
    SIDE_INFO_RAINBOW,
    SIDE_INFO_SEA_OF_FIRE,
    SIDE_INFO_SWAMP,
    SIDE_INFO_SMOKESCREEN,
    SIDE_INFO_HOT_COALS,
    SIDE_INFO_CALTROPS,
    SIDE_INFO_QUICK_GUARD,
    NUM_SIDE_INFO,
};

enum {
    STATUS_INFO_EXTRA_ABILITIES,
    // Status 1
    STATUS_INFO_PRIMARY,
    // Status 2
    STATUS_INFO_CONFUSION,
    STATUS_INFO_ENRAGED,
    STATUS_INFO_DRENCHED,
    STATUS_INFO_FUTURE_SIGHT,
    // STATUS_INFO_FLINCHED,
    STATUS_INFO_UPROAR,
    STATUS_INFO_BIDE,
    STATUS_INFO_INFATUATION,
    STATUS_INFO_FOCUS_ENERGY,
    STATUS_INFO_TRANSFORMED,
    STATUS_INFO_ESCAPE_PREVENTION,
    STATUS_INFO_CURSED,
    STATUS_INFO_FORESIGHT,
    STATUS_INFO_DEFENSE_CURL,
    STATUS_INFO_TORMENT,
    // Status 3
    STATUS_INFO_LEECHSEED,
    STATUS_INFO_PERISH_SONG,
    STATUS_INFO_MINIMIZED,
    STATUS_INFO_CHARGED_UP,
    STATUS_INFO_ROOTED,
    STATUS_INFO_YAWN,
    STATUS_INFO_GRUDGE,
    STATUS_INFO_CANT_CRIT,
    STATUS_INFO_GASTRO_ACID,
    STATUS_INFO_EMBARGO,
    STATUS_INFO_SMACKED_DOWN,
    STATUS_INFO_MIRACLE_EYED,
    STATUS_INFO_HEAL_BLOCKED,
    STATUS_INFO_AQUA_RING,
    STATUS_INFO_MAGNET_RISE,
    STATUS_INFO_SEMI_INVULNERABLE,
    // STATUS_INFO_UNDERGROUND,
    // STATUS_INFO_UNDERWATER,
    // STATUS_INFO_PHANTOM,FORCE,
    // STATUS_INFO_ON_AIR,
    STATUS_INFO_ELECTRIFIED,
    STATUS_INFO_PLASMA_FIST,
    STATUS_INFO_COILED,
    STATUS_INFO_CUTTHROAT,
    STATUS_INFO_PROTOSYNTHESIS,
    STATUS_INFO_QUARK_DRIVE,
    STATUS_INFO_GHASTLY_ECHO,
    STATUS_INFO_COMMANDED,
    STATUS_INFO_VIOLENT_RUSH,
    STATUS_INFO_RAPID_RESPONSE,
    STATUS_INFO_READIED_ACTION,
    STATUS_INFO_SHOWDOWN_MODE,
    STATUS_INFO_DRAGON_CHEER,
    STATUS_INFO_WRAPPED,
    STATUS_INFO_PARASITIC_SPORES,
    STATUS_INFO_FEAR,
    STATUS_INFO_ON_THE_PROWL,
    STATUS_INFO_DAZED,
    STATUS_INFO_TREPIDATION,
    STATUS_INFO_STOCKPILE,
    STATUS_INFO_SOOTHSAYER,
    STATUS_INFO_ICE_STATUE,
    // Battle Events (Extraskills)
    STATUS_INFO_EXTRA_ATTACK,
    STATUS_INFO_EXTRA_DEFENSE,
    STATUS_INFO_EXTRA_SPATTACK,
    STATUS_INFO_EXTRA_SPDEFENSE,
    STATUS_INFO_EXTRA_SPEED,
    NUM_STATUS_INFO,
};

#define MAX_TARGETS 2

// 252+ SpA Abomasnow Blizzard vs. 168 HP / 0 SpD Abomasnow: 178-211 (49 - 58.1%) -- 97.7% chance to 2HKO
struct DamageCalculation {
    bool8 calculated;
    // Damage
    s32 minDamage;
    u8 minDamagePercentage;
    s32 maxDamage;
    u8 maxDamagePercentage;
    // KO
    u8 hits2KO;
    s32 chance2KO;
    // NatureStuff
    u8 attackerNatureUp;
    u8 attackerNatureDown;
    u8 targetNatureUp;
    u8 targetNatureDown;
    // u8 *DamageCalculationText;
    u8 targetEvs[NUM_STATS];
    u8 battlerEvs[NUM_STATS];
    bool8 noDamage;
};

struct MenuResources {
    MainCallback savedCallback;  // determines callback to run when we exit. e.g. where do we want to go after closing the menu
    u8 gfxLoadState;
    u8 battlerId;
    u8 modeId;
    u8 tabId;
    u8 fieldTabId;
    u8 moveModeId;
    u8 dmgCalculatorTarget;
    u8 spriteIds[NUM_SPRITES];
    u8 fieldInfo[NUM_FIELD_INFO];
    u8 currentFieldInfo;
    u8 numFields;
    u8 SideInfoPlayer[NUM_SIDE_INFO];
    u8 currentSideInfoPlayer;
    u8 numSideInfoPlayer;
    u8 SideInfoEnemy[NUM_SIDE_INFO];
    u8 currentSideInfoEnemy;
    u8 speedModeId;
    u8 numSideInfoEnemy;
    u8 BattlerStatus[NUM_STATUS_INFO][MAX_BATTLERS_COUNT];
    u8 CurrentStatusInfo[MAX_BATTLERS_COUNT];
    u8 numStatusInfo[MAX_BATTLERS_COUNT];
    struct DamageCalculation damageCalculation[MAX_BATTLERS_COUNT][MAX_TARGETS * 2][MAX_MON_MOVES];
    bool8 partySelectorMode;
    u8 partyMenuSelectorID_X;
    u8 partyMenuSelectorID_Y;
    bool8 isDoubleBattle;
    bool8 partyIconsCreated;
};

enum WindowIds {
    WINDOW_1,
};

enum battler_TabIds {
    TAB_STATS,
    TAB_ABILITIES,
    TAB_MOVES,
    TAB_DAMAGE_CALCULATOR,
    TAB_STATUS,
    NUM_TABS,
};

enum field_TabsIds {
    TAB_PARTY,
    TAB_SPEED,
    TAB_FIELD,
    TAB_PLAYER_SIDE,
    TAB_ENEMY_SIDE,
    NUM_FIELD_TABS,
};

enum modes { MODE_BATTLER2, MODE_BATTLER0, MODE_FIELD, MODE_BATTLER1, MODE_BATTLER3, NUM_MODES };

enum move_modes {
    MOVE_MODE_NORMAL,
    MOVE_MODE_DESCRIPTION,
    // MOVE_MODE_DAMAGE_CALCULATOR,
    MOVE_MODE_AFFECTED_1,
    MOVE_MODE_AFFECTED_2,
    NUM_MOVE_MODES
};

//==========EWRAM==========//
static EWRAM_DATA struct MenuResources* sMenuDataPtr = NULL;
static EWRAM_DATA MainCallback tempsavedCallback = NULL;  // Temporary Callback
static EWRAM_DATA u8* sBg1TilemapBuffer = NULL;

//==========STATIC=DEFINES==========//
static void Battle_Menu_RunSetup(void);
static bool8 Menu_DoGfxSetup(void);
static bool8 Menu_InitBgs(void);
static void Menu_FadeAndBail(void);
static bool8 Menu_LoadGraphics(void);
static void Menu_InitWindows(void);
static void PrintToWindow(u8 windowId, u8 colorIdx);
static void PrintPartyTab(void);
static void PrintFieldTab(void);
static void PrintSpeedTab(void);
static void PrintSideTab(u8 side);
static void PrintStatsTab(void);
static void PrintAbilityTab(void);
static void PrintStatusTab(void);
static void PrintDamageCalulatorTab(void);
static void Task_MenuWaitFadeIn(u8 taskId);
static void Task_MenuMain(u8 taskId);
static void PrintMoveInfo(MoveEnum move, u8 x, u8 y, u8 moveIdx);

static u8 ShowSpeciesIcon(u8 num);
static u8 ShowSpeciesIconSpeed(u8 battler, u8 x, u8 y);
static void FreeEveryPartyMonIconSprite(void);
static void FreePartySpeciesIconSprite(u8 num, bool8 isEnemyMon);
static u8 ShowSpeciesIconParty(u8 num, bool8 isEnemyParty, u8 x, u8 y);
static void FreeEveryMonIconSprite(void);
static void FreeSpeciesIconSprite(u8 battler);
static void SetSpriteInvisibility(u8 spriteArrayId, bool8 invisible);
static void SetMonTypeIcons(void);
static void SetTypeIconSpritePosAndPal(u8 typeId, u8 x, u8 y, u8 spriteArrayId);
static void CreateSetStatusSprite(void);
static void DestroyBattleMenuSprite(u8 spriteArrayId);
static void setBattler(void);
static void LoadTilemapFromMode(void);
static void PrintPage(void);

static void ShowFieldIcon(void);
static void CreateSelectorSprite(void);
static void FreeItemIconSprite(void);
void FreeFieldSprite(void);
void FreeSelectorSprite(void);
static void CalculateDamage(u8 battler, u8 target, u8 moveIndex);

//==========CONST=DATA==========//
static const struct BgTemplate sMenuBgTemplates[] = {{.bg = 0,  // windows, etc
                                                      .charBaseIndex = 0,
                                                      .mapBaseIndex = 31,
                                                      .priority = 1},
                                                     {.bg = 1,  // this bg loads the UI tilemap
                                                      .charBaseIndex = 3,
                                                      .mapBaseIndex = 30,
                                                      .priority = 2},
                                                     {.bg = 2,  // this bg loads the UI tilemap
                                                      .charBaseIndex = 0,
                                                      .mapBaseIndex = 28,
                                                      .priority = 0}};

static const struct WindowTemplate sMenuWindowTemplates[] = {
    [WINDOW_1] =
        {
            .bg = 0,           // which bg to print text on
            .tilemapLeft = 0,  // position from left (per 8 pixels)
            .tilemapTop = 0,   // position from top (per 8 pixels)
            .width = 30,       // width (per 8 pixels)
            .height = 20,      // height (per 8 pixels)
            .paletteNum = 0,   // palette index to use for text
            .baseBlock = 1,    // tile start in VRAM
        },
};

static const u32 sMenuTiles[] = INCBIN_U32("graphics/ui_menus/battle_menu/tiles.4bpp.lz");
static const u8 sStatDownArrow[] = INCBIN_U8("graphics/ui_menus/battle_menu/stat_down_arrow.4bpp");
static const u8 sStatUpArrow[] = INCBIN_U8("graphics/ui_menus/battle_menu/stat_up_arrow.4bpp");
static const u8 sCheck[] = INCBIN_U8("graphics/ui_menus/battle_menu/check.4bpp");
static const u8 sSelector2[] = INCBIN_U8("graphics/ui_menus/battle_menu/selector_2.4bpp");

// Palettes
static const u16 sMenuPalette[] = INCBIN_U16("graphics/ui_menus/battle_menu/palette.gbapal");
static const u16 sMenuPalette_Blue[] = INCBIN_U16("graphics/ui_menus/battle_menu/palette_blue.gbapal");
static const u16 sMenuPalette_Yellow[] = INCBIN_U16("graphics/ui_menus/battle_menu/palette_yellow.gbapal");
static const u16 sMenuPalette_Green[] = INCBIN_U16("graphics/ui_menus/battle_menu/palette_green.gbapal");
static const u16 sMenuPalette_Red[] = INCBIN_U16("graphics/ui_menus/battle_menu/palette_red.gbapal");

// Battler Tabs
static const u32 sMenu_Tilemap_Singles_Battler_Status[] = INCBIN_U32("graphics/ui_menus/battle_menu/titlemap_singles_battler_status.bin.lz");
static const u32 sMenu_Tilemap_Doubles_Battler_Status[] = INCBIN_U32("graphics/ui_menus/battle_menu/titlemap_doubles_battler_status.bin.lz");
//
static const u32 sMenu_Tilemap_Singles_Battler_Abilities[] = INCBIN_U32("graphics/ui_menus/battle_menu/titlemap_singles_battler_abilities.bin.lz");
static const u32 sMenu_Tilemap_Doubles_Battler_Abilities[] = INCBIN_U32("graphics/ui_menus/battle_menu/titlemap_doubles_battler_abilities.bin.lz");
// Field Tabs
static const u32 sMenu_Tilemap_Singles_Field[] = INCBIN_U32("graphics/ui_menus/battle_menu/titlemap_singles_field.bin.lz");
static const u32 sMenu_Tilemap_Doubles_Field[] = INCBIN_U32("graphics/ui_menus/battle_menu/titlemap_doubles_field.bin.lz");
static const u32 sMenu_Tilemap_Party_Info[] = INCBIN_U32("graphics/ui_menus/battle_menu/tilemap_field_party.bin.lz");
static const u32 sMenu_Tilemap_Doubles_Party_Info[] = INCBIN_U32("graphics/ui_menus/battle_menu/tilemap_doubles_field_party.bin.lz");
//
static const u32 sMenu_Tilemap_Singles_Speed[] = INCBIN_U32("graphics/ui_menus/battle_menu/titlemap_singles_field_speed.bin.lz");
static const u32 sMenu_Tilemap_Doubles_Speed[] = INCBIN_U32("graphics/ui_menus/battle_menu/titlemap_doubles_field_speed.bin.lz");

enum Colors {
    FONT_BLACK,
    FONT_WHITE,
    FONT_RED,
    FONT_BLUE,
};
static const u8 sMenuWindowFontColors[][3] = {
    [FONT_BLACK] = {TEXT_COLOR_TRANSPARENT, 3, 2},
    [FONT_WHITE] = {TEXT_COLOR_TRANSPARENT, 1, 3},
    [FONT_RED] = {TEXT_COLOR_TRANSPARENT, 14, 2},
    [FONT_BLUE] = {TEXT_COLOR_TRANSPARENT, 13, 2},
};

//==========FUNCTIONS==========//
// UI loader template
void Task_OpenBattleMenuFromStartMenu(u8 taskId) {
    if (!gPaletteFade.active) {
        CleanupOverworldWindowsAndTilemaps();
        UI_Battle_Menu_Init(CB2_ReturnToFieldWithOpenMenu);
        DestroyTask(taskId);
    }
}

static void Task_CalculateDamage(u8 taskId) {
    u8 i, j, k;

    for (i = 0; i < MAX_BATTLERS_COUNT; i++) {
        for (j = 0; j < MAX_TARGETS * 2; j++) {
            for (k = 0; k < MAX_MON_MOVES; k++) {
                if (IsBattlerAlive(i) && IsBattlerAlive(j) && GetBattlerSide(i) != GetBattlerSide(j) && gBattleMons[i].moves[k] != MOVE_NONE &&
                    gBattleMoves[gBattleMons[i].moves[k]].split != SPLIT_STATUS) {
                    CalculateDamage(i, j, k);
                }
            }
        }
    }
    DestroyTask(taskId);
}

// This is our main initialization function if you want to call the menu from elsewhere
void UI_Battle_Menu_Init(MainCallback callback) {
    u8 i, j;
    bool8 isExtraInfoShown;

    if ((sMenuDataPtr = AllocZeroed(sizeof(struct MenuResources))) == NULL) {
        SetMainCallback2(callback);
        return;
    }

    // initialize stuff
    if (IsDoubleBattle() || FlagGet(FLAG_TAG_BATTLE))
        sMenuDataPtr->isDoubleBattle = TRUE;
    else
        sMenuDataPtr->isDoubleBattle = FALSE;

    sMenuDataPtr->gfxLoadState = 0;
    sMenuDataPtr->moveModeId = 0;
    sMenuDataPtr->tabId = 0;
    sMenuDataPtr->modeId = MODE_FIELD;
    sMenuDataPtr->fieldTabId = TAB_PARTY;
    sMenuDataPtr->dmgCalculatorTarget = 0;

    sMenuDataPtr->currentFieldInfo = 0;
    sMenuDataPtr->numFields = 0;

    sMenuDataPtr->currentSideInfoPlayer = 0;
    sMenuDataPtr->numSideInfoPlayer = 0;

    sMenuDataPtr->currentSideInfoEnemy = 0;
    sMenuDataPtr->numSideInfoEnemy = 0;
    sMenuDataPtr->partyMenuSelectorID_X = 0;
    sMenuDataPtr->partyMenuSelectorID_Y = 0;
    sMenuDataPtr->partySelectorMode = FALSE;

    sMenuDataPtr->numStatusInfo[B_POSITION_PLAYER_LEFT] = 0;
    sMenuDataPtr->numStatusInfo[B_POSITION_OPPONENT_LEFT] = 0;
    sMenuDataPtr->numStatusInfo[B_POSITION_PLAYER_RIGHT] = 0;
    sMenuDataPtr->numStatusInfo[B_POSITION_OPPONENT_RIGHT] = 0;

    setBattler();
    for (i = 0; i < NUM_SPRITES; i++) sMenuDataPtr->spriteIds[i] = SPRITE_NONE;
    sMenuDataPtr->savedCallback = callback;

    for (i = 0; i < NUM_FIELD_INFO; i++) sMenuDataPtr->fieldInfo[i] = NUM_FIELD_INFO;

    // Field Info
    for (i = 0; i < NUM_FIELD_INFO; i++) {
        isExtraInfoShown = FALSE;
        switch (i) {
            case FIELD_INFO_WEATHER:
                if ((gBattleWeather & WEATHER_ANY)) isExtraInfoShown = TRUE;
                break;
            case FIELD_INFO_TERRAIN:
                if (gFieldStatuses & STATUS_FIELD_TERRAIN_ANY) isExtraInfoShown = TRUE;
                break;
            case FIELD_INFO_INVERSE_ROOM:
                if (gFieldStatuses & STATUS_FIELD_INVERSE_ROOM) isExtraInfoShown = TRUE;
                break;
            case FIELD_INFO_TRICK_ROOM:
                if (gFieldStatuses & STATUS_FIELD_TRICK_ROOM) isExtraInfoShown = TRUE;
                break;
            case FIELD_INFO_GRAVITY:
                if ((gFieldStatuses & STATUS_FIELD_GRAVITY)) isExtraInfoShown = TRUE;
                break;
            case FIELD_INFO_WONDER_ROOM:
                if (gFieldStatuses & STATUS_FIELD_WONDER_ROOM) isExtraInfoShown = TRUE;
                break;
            case FIELD_INFO_MAGIC_ROOM:
                if (gFieldStatuses & STATUS_FIELD_MAGIC_ROOM) isExtraInfoShown = TRUE;
                break;
            case FIELD_INFO_FAIRY_LOCK:
                if (gFieldStatuses & STATUS_FIELD_FAIRY_LOCK) isExtraInfoShown = TRUE;
                break;
            case FIELD_INFO_MUD_SPORT:
                if (gFieldStatuses & STATUS_FIELD_MUDSPORT) isExtraInfoShown = TRUE;
                break;
            case FIELD_INFO_WATER_SPORT:
                if (gFieldStatuses & STATUS_FIELD_WATERSPORT) isExtraInfoShown = TRUE;
                break;
            case FIELD_INFO_ION_DELUGE:
                if (gFieldStatuses & STATUS_FIELD_ION_DELUGE) isExtraInfoShown = TRUE;
                break;
            case FIELD_INFO_QUASH:
                if (gFieldTimers.quashTimer) isExtraInfoShown = TRUE;
                break;
            case FIELD_INFO_CLEAR_SKIES:
                if (gFieldTimers.clearSkiesTimer) isExtraInfoShown = TRUE;
                break;
        }

        if (isExtraInfoShown) {
            sMenuDataPtr->fieldInfo[sMenuDataPtr->numFields] = i;
            sMenuDataPtr->numFields++;
        }
    }

    // Player Side Info
    for (i = 0; i < NUM_SIDE_INFO; i++) {
        isExtraInfoShown = FALSE;
        switch (i) {
            case SIDE_INFO_AURORA_VEIL:
                if (gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_AURORA_VEIL) isExtraInfoShown = TRUE;
                break;
            case SIDE_INFO_REFLECT:
                if (gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_REFLECT) isExtraInfoShown = TRUE;
                break;
            case SIDE_INFO_LIGHT_SCREEN:
                if (gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_LIGHTSCREEN) isExtraInfoShown = TRUE;
                break;
            case SIDE_INFO_TAILWIND:
                if (gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND) isExtraInfoShown = TRUE;
                break;
            case SIDE_INFO_SPIKES:
                if (gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_SPIKES) isExtraInfoShown = TRUE;
                break;
            case SIDE_INFO_TOXIC_SPIKES:
                if (gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TOXIC_SPIKES) isExtraInfoShown = TRUE;
                break;
            case SIDE_INFO_STEALTH_ROCK:
                if (gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_STEALTH_ROCK && gSideTimers[B_SIDE_PLAYER].stealthRockType == TYPE_ROCK) isExtraInfoShown = TRUE;
                break;
            case SIDE_INFO_CREEPING_THORNS:
                if (gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_STEALTH_ROCK && gSideTimers[B_SIDE_PLAYER].stealthRockType == TYPE_GRASS)
                    isExtraInfoShown = TRUE;
                break;
            case SIDE_INFO_STICKY_WEB:
                if (gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_STICKY_WEB) isExtraInfoShown = TRUE;
                break;
            case SIDE_INFO_SAFEGUARD:
                if (gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_SAFEGUARD) isExtraInfoShown = TRUE;
                break;
            case SIDE_INFO_MIST:
                if (gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_MIST) isExtraInfoShown = TRUE;
                break;
            case SIDE_INFO_RAINBOW:
                if (gSideTimers[B_SIDE_PLAYER].rainbowTimer) isExtraInfoShown = TRUE;
                break;
            case SIDE_INFO_SEA_OF_FIRE:
                if (gSideTimers[B_SIDE_PLAYER].fireSeaTimer) isExtraInfoShown = TRUE;
                break;
            case SIDE_INFO_SWAMP:
                if (gSideTimers[B_SIDE_PLAYER].swampTimer) isExtraInfoShown = TRUE;
                break;
            case SIDE_INFO_SMOKESCREEN:
                if (gSideTimers[B_SIDE_PLAYER].smokescreenTimer) isExtraInfoShown = TRUE;
                break;
            case SIDE_INFO_HOT_COALS:
                if (gSideTimers[B_SIDE_PLAYER].hotCoals) isExtraInfoShown = TRUE;
                break;
            case SIDE_INFO_CALTROPS:
                if (gSideTimers[B_SIDE_PLAYER].caltrops) isExtraInfoShown = TRUE;
                break;
            case SIDE_INFO_QUICK_GUARD:
                if (gSideTimers[B_SIDE_PLAYER].quickGuardTimer) isExtraInfoShown = TRUE;
                break;
        }

        if (isExtraInfoShown) {
            sMenuDataPtr->SideInfoPlayer[sMenuDataPtr->numSideInfoPlayer] = i;
            sMenuDataPtr->numSideInfoPlayer++;
        }
    }

    // Enemy Side Info
    for (i = 0; i < NUM_SIDE_INFO; i++) {
        isExtraInfoShown = FALSE;
        switch (i) {
            case SIDE_INFO_AURORA_VEIL:
                if (gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_AURORA_VEIL) isExtraInfoShown = TRUE;
                break;
            case SIDE_INFO_REFLECT:
                if (gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_REFLECT) isExtraInfoShown = TRUE;
                break;
            case SIDE_INFO_LIGHT_SCREEN:
                if (gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_LIGHTSCREEN) isExtraInfoShown = TRUE;
                break;
            case SIDE_INFO_TAILWIND:
                if (gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_TAILWIND) isExtraInfoShown = TRUE;
                break;
            case SIDE_INFO_SPIKES:
                if (gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_SPIKES) isExtraInfoShown = TRUE;
                break;
            case SIDE_INFO_TOXIC_SPIKES:
                if (gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_TOXIC_SPIKES) isExtraInfoShown = TRUE;
                break;
            case SIDE_INFO_STEALTH_ROCK:
                if (gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_STEALTH_ROCK && gSideTimers[B_SIDE_OPPONENT].stealthRockType == TYPE_ROCK)
                    isExtraInfoShown = TRUE;
                break;
            case SIDE_INFO_CREEPING_THORNS:
                if (gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_STEALTH_ROCK && gSideTimers[B_SIDE_OPPONENT].stealthRockType == TYPE_GRASS)
                    isExtraInfoShown = TRUE;
                break;
            case SIDE_INFO_STICKY_WEB:
                if (gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_STICKY_WEB) isExtraInfoShown = TRUE;
                break;
            case SIDE_INFO_SAFEGUARD:
                if (gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_SAFEGUARD) isExtraInfoShown = TRUE;
                break;
            case SIDE_INFO_MIST:
                if (gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_MIST) isExtraInfoShown = TRUE;
                break;
            case SIDE_INFO_RAINBOW:
                if (gSideTimers[B_SIDE_OPPONENT].rainbowTimer) isExtraInfoShown = TRUE;
                break;
            case SIDE_INFO_SEA_OF_FIRE:
                if (gSideTimers[B_SIDE_OPPONENT].fireSeaTimer) isExtraInfoShown = TRUE;
                break;
            case SIDE_INFO_SWAMP:
                if (gSideTimers[B_SIDE_OPPONENT].swampTimer) isExtraInfoShown = TRUE;
                break;
            case SIDE_INFO_SMOKESCREEN:
                if (gSideTimers[B_SIDE_OPPONENT].smokescreenTimer) isExtraInfoShown = TRUE;
                break;
            case SIDE_INFO_HOT_COALS:
                if (gSideTimers[B_SIDE_OPPONENT].hotCoals) isExtraInfoShown = TRUE;
                break;
            case SIDE_INFO_CALTROPS:
                if (gSideTimers[B_SIDE_OPPONENT].caltrops) isExtraInfoShown = TRUE;
                break;
            case SIDE_INFO_QUICK_GUARD:
                if (gSideTimers[B_SIDE_OPPONENT].quickGuardTimer) isExtraInfoShown = TRUE;
                break;
        }

        if (isExtraInfoShown) {
            sMenuDataPtr->SideInfoEnemy[sMenuDataPtr->numSideInfoEnemy] = i;
            sMenuDataPtr->numSideInfoEnemy++;
        }
    }

    // Battler Status Info
    for (i = 0; i < NUM_STATUS_INFO; i++) {
        for (j = 0; j < MAX_BATTLERS_COUNT; j++) {
            isExtraInfoShown = FALSE;
            switch (i) {
                case STATUS_INFO_EXTRA_ABILITIES: {
                    u8 k;
                    for (k = 0; k < HELL_MODE_EXTRA_ABILITIES; k++) {
                        if (GetBattlerAbilityInSlot(j, TOTAL_ABILITY_COUNT + k) != ABILITY_NONE) isExtraInfoShown = TRUE;
                    }
                    break;
                }
                case STATUS_INFO_PRIMARY:
                    if (gBattleMons[j].status1 & STATUS1_ANY) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_CONFUSION:
                    if (gBattleMons[j].status2 & STATUS2_CONFUSION) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_ENRAGED:
                    if (gBattleMons[j].status2 & STATUS2_ENRAGED) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_FUTURE_SIGHT:
                    if (gWishFutureKnock.futureSightCounter[j] != 0) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_UPROAR:
                    if (gBattleMons[j].status2 & STATUS2_UPROAR) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_BIDE:
                    if (gBattleMons[j].status2 & STATUS2_BIDE) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_INFATUATION:
                    if (gBattleMons[j].status2 & STATUS2_INFATUATION) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_FOCUS_ENERGY:
                    if (gBattleMons[j].status2 & STATUS2_FOCUS_ENERGY) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_DRAGON_CHEER:
                    if (gStatuses4[j] & STATUS4_DRAGON_CHEER) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_TRANSFORMED:
                    if (gBattleMons[j].status2 & STATUS2_TRANSFORMED) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_ESCAPE_PREVENTION:
                    if (gBattleMons[j].status2 & STATUS2_ESCAPE_PREVENTION) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_CURSED:
                    if (gBattleMons[j].status2 & STATUS2_CURSED) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_FORESIGHT:
                    if (gStatuses4[j] & STATUS4_FORESIGHT) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_DEFENSE_CURL:
                    if (gBattleMons[j].status2 & STATUS2_DEFENSE_CURL) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_TORMENT:
                    if (gBattleMons[j].status2 & STATUS2_TORMENT) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_LEECHSEED:
                    if (gStatuses3[j] & STATUS3_LEECHSEED) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_PERISH_SONG:
                    if (gStatuses3[j] & STATUS3_PERISH_SONG) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_MINIMIZED:
                    if (gStatuses3[j] & STATUS3_MINIMIZED) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_CHARGED_UP:
                    if (gStatuses3[j] & STATUS3_CHARGED_UP) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_ROOTED:
                    if (gStatuses3[j] & STATUS3_ROOTED) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_YAWN:
                    if (gStatuses3[j] & STATUS3_YAWN) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_GRUDGE:
                    if (gStatuses3[j] & STATUS3_GRUDGE) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_CANT_CRIT:
                    if (gStatuses3[j] & STATUS3_CANT_SCORE_A_CRIT) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_GASTRO_ACID:
                    if (gStatuses3[j] & STATUS3_GASTRO_ACID) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_EMBARGO:
                    if (gStatuses3[j] & STATUS3_EMBARGO) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_SMACKED_DOWN:
                    if (gStatuses3[j] & STATUS3_SMACKED_DOWN) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_MIRACLE_EYED:
                    if (gStatuses3[j] & STATUS3_MIRACLE_EYED) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_HEAL_BLOCKED:
                    if (gStatuses3[j] & STATUS3_HEAL_BLOCK) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_AQUA_RING:
                    if (gStatuses3[j] & STATUS3_AQUA_RING) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_MAGNET_RISE:
                    if (gStatuses3[j] & STATUS3_MAGNET_RISE) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_SEMI_INVULNERABLE:
                    if (gStatuses3[j] & STATUS3_SEMI_INVULNERABLE) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_ELECTRIFIED:
                    if (gStatuses4[j] & STATUS4_ELECTRIFIED) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_PLASMA_FIST:
                    if (gStatuses4[j] & STATUS4_PLASMA_FISTS) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_COILED:
                    if (gStatuses4[j] & STATUS4_COILED) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_CUTTHROAT:
                    if (gStatuses4[j] & STATUS4_CUTTHROAT) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_PROTOSYNTHESIS:
                    if (GetAbilityState(j, ABILITY_PROTOSYNTHESIS)) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_QUARK_DRIVE:
                    if (GetAbilityState(j, ABILITY_QUARK_DRIVE)) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_GHASTLY_ECHO:
                    if (gStatuses4[j] & STATUS4_GHASTLY_ECHO) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_COMMANDED:
                    if (gStatuses4[j] & STATUS4_COMMANDED) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_VIOLENT_RUSH:
                    if (gVolatileStructs[j].violentRush) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_RAPID_RESPONSE:
                    if (gVolatileStructs[j].rapidResponse) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_READIED_ACTION:
                    if (gVolatileStructs[j].readiedAction) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_SHOWDOWN_MODE:
                    if (gVolatileStructs[j].showdownMode) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_WRAPPED:
                    if (gBattleMons[j].status2 & STATUS2_WRAPPED) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_PARASITIC_SPORES:
                    if (gVolatileStructs[j].parasiticSpores) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_FEAR:
                    if (gVolatileStructs[j].fear) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_ON_THE_PROWL:
                    if (gVolatileStructs[j].onTheProwl) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_DAZED:
                    if (gVolatileStructs[j].dazed) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_TREPIDATION:
                    if (gVolatileStructs[j].trepidation) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_STOCKPILE:
                    if (gVolatileStructs[j].stockpileCounter) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_SOOTHSAYER:
                    if (GetAbilityState(j, ABILITY_SOOTHSAYER)) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_ICE_STATUE:
                    if (gVolatileStructs[j].iceStatue) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_DRENCHED:
                    if (gVolatileStructs[j].drenched) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_EXTRA_ATTACK:
                    if (gVolatileStructs[j].extraAttackLevel) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_EXTRA_DEFENSE:
                    if (gVolatileStructs[j].extraDefenseLevel) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_EXTRA_SPATTACK:
                    if (gVolatileStructs[j].extraSpAttackLevel) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_EXTRA_SPDEFENSE:
                    if (gVolatileStructs[j].extraSpDefenseLevel) isExtraInfoShown = TRUE;
                    break;
                case STATUS_INFO_EXTRA_SPEED:
                    if (gVolatileStructs[j].extraSpeedLevel) isExtraInfoShown = TRUE;
                    break;
            }

            if (isExtraInfoShown && IsBattlerAlive(j)) {
                sMenuDataPtr->BattlerStatus[sMenuDataPtr->numStatusInfo[j]][j] = i;
                sMenuDataPtr->numStatusInfo[j]++;
            }
        }
    }

    if (FlagGet(FLAG_BATTLE_MENU_COMING_FROM_SUMMARY_SCREEN)) {
        if (VarGet(VAR_BATTLE_MENU_MON_ID_X) >= 0xFF) {
            sMenuDataPtr->modeId = VarGet(VAR_BATTLE_MENU_MON_ID_Y);
            sMenuDataPtr->tabId = VarGet(VAR_BATTLE_MENU_MON_ID_X) - 0xFF;
        } else {
            sMenuDataPtr->partyMenuSelectorID_X = VarGet(VAR_BATTLE_MENU_MON_ID_X);
            sMenuDataPtr->partyMenuSelectorID_Y = VarGet(VAR_BATTLE_MENU_MON_ID_Y);
            sMenuDataPtr->partySelectorMode = TRUE;
        }
    }

    // Reset the flags
    VarSet(VAR_BATTLE_MENU_MON_ID_X, 0);
    VarSet(VAR_BATTLE_MENU_MON_ID_Y, 0);
    FlagClear(FLAG_BATTLE_MENU_COMING_FROM_SUMMARY_SCREEN);

    SetMainCallback2(Battle_Menu_RunSetup);
}

static void Battle_Menu_RunSetup(void) {
    while (1) {
        if (Menu_DoGfxSetup() == TRUE) break;
    }
}

static void Menu_MainCB(void) {
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void Menu_VBlankCB(void) {
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

#define POKEMON_ICON_X (1 * 8) + (2 * 8)

#define POKEMON_ICON_1_Y (4 * 8) + (2 * 8)
#define POKEMON_ICON_2_Y (12 * 8) + (2 * 8)
#define POKEMON_ICON_3_Y (0 * 8) + (2 * 8)
#define POKEMON_ICON_4_Y (16 * 8) + (2 * 8)

static bool8 Menu_DoGfxSetup(void) {
    switch (gMain.state) {
        case 0:
            DmaClearLarge16(3, (void*)VRAM, VRAM_SIZE, 0x1000) SetVBlankHBlankCallbacksToNull();
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
            if (Menu_InitBgs()) {
                sMenuDataPtr->gfxLoadState = 0;
                gMain.state++;
            } else {
                Menu_FadeAndBail();
                return TRUE;
            }
            break;
        case 3:
            if (Menu_LoadGraphics() == TRUE) gMain.state++;
            break;
        case 4:
            LoadMessageBoxAndBorderGfx();
            Menu_InitWindows();
            gMain.state++;
            break;
        case 5:
            CreateTask(Task_MenuWaitFadeIn, 0);
            BlendPalettes(0xFFFFFFFF, 16, RGB_BLACK);
            gMain.state++;
            break;
        case 6:
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB_BLACK);
            // Start
            ShowSpeciesIcon(0);
            ShowSpeciesIcon(1);
            if (sMenuDataPtr->isDoubleBattle) {
                ShowSpeciesIcon(2);
                ShowSpeciesIcon(3);
            }
            PrintPage();
            // CreateTask(Task_CalculateDamage, 1);
            CreateSelectorSprite();
            ShowFieldIcon();
            gMain.state++;
            break;
        default:
            SetVBlankCallback(Menu_VBlankCB);
            SetMainCallback2(Menu_MainCB);
            return TRUE;
    }
    return FALSE;
}

#define try_free(ptr)                     \
    ({                                    \
        void** ptr__ = (void**)&(ptr);    \
        if (*ptr__ != NULL) Free(*ptr__); \
    })

static void Menu_FreeResources(void) {
    try_free(sMenuDataPtr);
    try_free(sBg1TilemapBuffer);
    FreeAllWindowBuffers();
}

void LoadTilemapFromMode(void) {
    try_free(sBg1TilemapBuffer);

    sBg1TilemapBuffer = Alloc(0x800);

    memset(sBg1TilemapBuffer, 0, 0x800);
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sMenuBgTemplates, NELEMS(sMenuBgTemplates));
    SetBgTilemapBuffer(1, sBg1TilemapBuffer);
    ScheduleBgCopyTilemapToVram(1);
    ShowBg(0);
    ShowBg(1);
    ShowBg(2);

    // ResetTempTileDataBuffers();
    // DecompressAndCopyTileDataToVram(1, sMenuTiles, 0, 0, 0);
    // FreeTempTileDataBuffersIfPossible();

    if (sMenuDataPtr->modeId != MODE_FIELD) {
        switch (sMenuDataPtr->tabId) {
            case TAB_STATS:
                if (sMenuDataPtr->isDoubleBattle)
                    LZDecompressWram(sMenu_Tilemap_Doubles_Battler_Status, sBg1TilemapBuffer);
                else
                    LZDecompressWram(sMenu_Tilemap_Singles_Battler_Status, sBg1TilemapBuffer);
                break;
            case TAB_ABILITIES:
            case TAB_MOVES:
            case TAB_DAMAGE_CALCULATOR:
                if (sMenuDataPtr->isDoubleBattle)
                    LZDecompressWram(sMenu_Tilemap_Doubles_Battler_Abilities, sBg1TilemapBuffer);
                else
                    LZDecompressWram(sMenu_Tilemap_Singles_Battler_Abilities, sBg1TilemapBuffer);
                break;
            default:
                if (sMenuDataPtr->isDoubleBattle)
                    LZDecompressWram(sMenu_Tilemap_Doubles_Field, sBg1TilemapBuffer);
                else
                    LZDecompressWram(sMenu_Tilemap_Singles_Field, sBg1TilemapBuffer);
                break;
        }
    } else {
        switch (sMenuDataPtr->fieldTabId) {
            case TAB_PARTY:
                if (sMenuDataPtr->isDoubleBattle)
                    LZDecompressWram(sMenu_Tilemap_Doubles_Party_Info, sBg1TilemapBuffer);
                else
                    LZDecompressWram(sMenu_Tilemap_Party_Info, sBg1TilemapBuffer);
                break;
            case TAB_FIELD:
            case TAB_PLAYER_SIDE:
            case TAB_ENEMY_SIDE:
                if (sMenuDataPtr->isDoubleBattle)
                    LZDecompressWram(sMenu_Tilemap_Doubles_Field, sBg1TilemapBuffer);
                else
                    LZDecompressWram(sMenu_Tilemap_Singles_Field, sBg1TilemapBuffer);
                break;
            case TAB_SPEED:
                if (sMenuDataPtr->isDoubleBattle)
                    LZDecompressWram(sMenu_Tilemap_Doubles_Speed, sBg1TilemapBuffer);
                else
                    LZDecompressWram(sMenu_Tilemap_Singles_Speed, sBg1TilemapBuffer);
                break;
        }
    }
}

static void Task_MenuWaitFadeAndBail(u8 taskId) {
    if (!gPaletteFade.active) {
        FreeFieldSprite();
        FreeSelectorSprite();
        FreeItemIconSprite();
        SetMainCallback2(sMenuDataPtr->savedCallback);
        Menu_FreeResources();
        DestroyTask(taskId);
    }
}

static void Menu_FadeAndBail(void) {
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB_BLACK);
    CreateTask(Task_MenuWaitFadeAndBail, 0);
    SetVBlankCallback(Menu_VBlankCB);
    SetMainCallback2(Menu_MainCB);
}

static bool8 Menu_InitBgs(void) {
    ResetAllBgsCoordinates();
    sBg1TilemapBuffer = Alloc(0x800);
    if (sBg1TilemapBuffer == NULL) return FALSE;

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

static bool8 Menu_LoadGraphics(void) {
    switch (sMenuDataPtr->gfxLoadState) {
        case 0:
            ResetTempTileDataBuffers();
            DecompressAndCopyTileDataToVram(1, sMenuTiles, 0, 0, 0);
            sMenuDataPtr->gfxLoadState++;
            break;
        case 1:
            if (FreeTempTileDataBuffersIfPossible() != TRUE) {
                if (sMenuDataPtr->isDoubleBattle)
                    LZDecompressWram(sMenu_Tilemap_Doubles_Battler_Status, sBg1TilemapBuffer);
                else
                    LZDecompressWram(sMenu_Tilemap_Singles_Battler_Status, sBg1TilemapBuffer);
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

static void Menu_InitWindows(void) {
    INIT_WINDOWS(sMenuWindowTemplates);
    DeactivateAllTextPrinters();
    ScheduleBgCopyTilemapToVram(0);

    // First Window
    FillWindowPixelBuffer(WINDOW_1, 0);
    LoadUserWindowBorderGfx(WINDOW_1, 720, 14 * 16);
    PutWindowTilemap(WINDOW_1);
    CopyWindowToVram(WINDOW_1, 3);

    ScheduleBgCopyTilemapToVram(2);
}

static const u8 sText_MyMenu[] = _("HP：{STR_VAR_1}/{STR_VAR_2}");
static const u8 sText_HP[] = _("{STR_VAR_1}/{STR_VAR_2}");
static const u8 sText_PP[] = _("PP：{STR_VAR_1}/{STR_VAR_2}");

static const u8 sText_Ability[] = _("特性：");
static const u8 sText_Innate[] = _("先天：");
static const u8 sText_Held_Item[] = _("持有道具：\n{STR_VAR_1}");
static const u8 sText_None[] = _("全无");
static const u8 sYourOpponentPokemonData[] = _("对手宝可梦的战斗数据");
static const u8 sYourPokemonData[] = _("我方宝可梦战斗数据");

static const u8 sText_StatHP[] = _("HP");
static const u8 sText_Attack[] = _("攻");
static const u8 sText_Defense[] = _("防");
static const u8 sText_SpecialAttack[] = _("特");
static const u8 sText_SpecialDefense[] = _("特防");
static const u8 sText_Speed[] = _("速");
static const u8 sText_Accuracy[] = _("命中");
static const u8 sText_Evasion[] = _("闪");
static const u8 sText_Critical[] = _("会心");

static const u8 statorder[NUM_BATTLE_STATS] = {
    STAT_HP,
    STAT_ATK,
    STAT_DEF,
    STAT_SPATK,
    STAT_SPDEF,
    STAT_SPEED,
    STAT_ACC,
    STAT_EVASION,
};

#define PAL_UI_SPRITES 14
#define PAL_FIELD_ICON 15

#define TAG_ITEM_ICON 4133
#define TAG_BATTLER_SPEED 4134
#define TAG_ICON_PARTY_PLAYER 4135
#define TAG_ICON_PARTY_ENEMY TAG_ICON_PARTY_PLAYER + PARTY_SIZE

static const u32 gBattleFieldIconForest_Gfx[] = INCBIN_U32("graphics/ui_menus/battle_menu/fields/forest.4bpp.lz");
static const u16 gBattleFieldIconForest_Pal[] = INCBIN_U16("graphics/ui_menus/battle_menu/fields/forest.gbapal");

static const struct SpritePalette sBattleMenuFieldIconSpritePalette_Forest[] = {gBattleFieldIconForest_Pal, PAL_FIELD_ICON};

// Field Icon
void FreeFieldSprite(void) {
    u8* spriteId = &sMenuDataPtr->spriteIds[SPRITE_ARR_ID_FIELD_ICON];
    if (*spriteId != SPRITE_NONE) {
        FreeSpriteTilesByTag(SPRITE_ARR_ID_FIELD_ICON);
        FreeSpritePaletteByTag(SPRITE_ARR_ID_FIELD_ICON);
        FreeSpriteOamMatrix(&gSprites[*spriteId]);
        DestroySprite(&gSprites[*spriteId]);
        *spriteId = SPRITE_NONE;
        sMenuDataPtr->spriteIds[SPRITE_ARR_ID_FIELD_ICON] = SPRITE_NONE;
    }
}

static void ShowFieldIcon(void) {
    u8 spriteId;
    u8 SpriteTag = SPRITE_ARR_ID_FIELD_ICON;
    struct CompressedSpriteSheet sSpriteSheet_FieldIcon = {gBattleFieldIconForest_Gfx, 0x0800, SpriteTag};
    struct SpriteTemplate TempSpriteTemplate = gDummySpriteTemplate;

    TempSpriteTemplate.tileTag = SpriteTag;
    TempSpriteTemplate.callback = SpriteCallbackDummy;

    LoadCompressedSpriteSheet(&sSpriteSheet_FieldIcon);
    LoadSpritePalette(sBattleMenuFieldIconSpritePalette_Forest);
    TempSpriteTemplate.paletteTag = PAL_FIELD_ICON;
    spriteId = CreateSprite(&TempSpriteTemplate, 4, 68, 0);
    sMenuDataPtr->spriteIds[SPRITE_ARR_ID_FIELD_ICON] = spriteId;

    gSprites[sMenuDataPtr->spriteIds[SPRITE_ARR_ID_FIELD_ICON]].oam.shape = SPRITE_SHAPE(64x32);
    gSprites[sMenuDataPtr->spriteIds[SPRITE_ARR_ID_FIELD_ICON]].oam.size = SPRITE_SIZE(64x32);
    gSprites[sMenuDataPtr->spriteIds[SPRITE_ARR_ID_FIELD_ICON]].oam.priority = 1;
}

static void SpriteCB_Selector(struct Sprite* sprite) {
    u8 val = sprite->data[0];
    sprite->y2 = (gSineTable[val] / 128) + ((sMenuDataPtr->modeId * 4) * 8);
    sprite->data[0] += 8;
}

static void SpriteCB_PartyMons(struct Sprite* sprite) {
    if (sMenuDataPtr->modeId != MODE_FIELD || sMenuDataPtr->fieldTabId != TAB_PARTY)
        sprite->invisible = TRUE;
    else
        sprite->invisible = FALSE;
}

static const u32 gBattleSelector_Gfx[] = INCBIN_U32("graphics/ui_menus/battle_menu/fields/selector.4bpp.lz");
static const u16 gBattleSelector_Pal[] = INCBIN_U16("graphics/ui_menus/battle_menu/fields/selector.gbapal");

static const struct SpritePalette sBattleMenuSelectorSpritePalette[] = {gBattleSelector_Pal, PAL_UI_SPRITES};

// Selector
void FreeSelectorSprite(void) {
    u8* spriteId = &sMenuDataPtr->spriteIds[SPRITE_ARR_ID_SELECTOR];
    if (*spriteId != SPRITE_NONE) {
        FreeSpriteTilesByTag(SPRITE_ARR_ID_SELECTOR);
        FreeSpritePaletteByTag(SPRITE_ARR_ID_SELECTOR);
        FreeSpriteOamMatrix(&gSprites[*spriteId]);
        DestroySprite(&gSprites[*spriteId]);
        *spriteId = SPRITE_NONE;
        sMenuDataPtr->spriteIds[SPRITE_ARR_ID_SELECTOR] = SPRITE_NONE;
    }
}

static void CreateSelectorSprite(void) {
    u8 spriteId;
    u8 SpriteTag = SPRITE_ARR_ID_SELECTOR;
    struct CompressedSpriteSheet sSpriteSheet_Selector = {gBattleSelector_Gfx, 0x0801, SpriteTag};
    struct SpriteTemplate TempSpriteTemplate = gDummySpriteTemplate;

    TempSpriteTemplate.tileTag = SpriteTag;
    TempSpriteTemplate.callback = SpriteCB_Selector;

    LoadCompressedSpriteSheet(&sSpriteSheet_Selector);
    LoadSpritePalette(sBattleMenuSelectorSpritePalette);
    TempSpriteTemplate.paletteTag = PAL_UI_SPRITES;
    spriteId = CreateSprite(&TempSpriteTemplate, 4, 4, 0);
    sMenuDataPtr->spriteIds[SPRITE_ARR_ID_SELECTOR] = spriteId;

    gSprites[sMenuDataPtr->spriteIds[SPRITE_ARR_ID_SELECTOR]].oam.shape = SPRITE_SHAPE(64x32);
    gSprites[sMenuDataPtr->spriteIds[SPRITE_ARR_ID_SELECTOR]].oam.size = SPRITE_SIZE(64x32);
    gSprites[sMenuDataPtr->spriteIds[SPRITE_ARR_ID_SELECTOR]].oam.priority = 0;
}

static void FreeItemIconSprite(void) {
    u8* spriteId = &sMenuDataPtr->spriteIds[SPRITE_ARR_HELD_ITEM];
    if (*spriteId != SPRITE_NONE) {
        FreeSpriteTilesByTag(TAG_ITEM_ICON);
        FreeSpritePaletteByTag(TAG_ITEM_ICON);
        FreeSpriteOamMatrix(&gSprites[*spriteId]);
        DestroySprite(&gSprites[*spriteId]);
        *spriteId = SPRITE_NONE;
        sMenuDataPtr->spriteIds[SPRITE_ARR_HELD_ITEM] = SPRITE_NONE;
    }
}

static void ShowItemIcon(u16 itemId, u8 x, u8 y) {
    u8 itemSpriteId;
    u8* spriteId = &sMenuDataPtr->spriteIds[SPRITE_ARR_HELD_ITEM];

    FreeItemIconSprite();

    if (*spriteId == SPRITE_NONE && itemId != ITEM_NONE) {
        FreeSpriteTilesByTag(TAG_ITEM_ICON);
        FreeSpritePaletteByTag(TAG_ITEM_ICON);
        itemSpriteId = AddItemIconSprite(TAG_ITEM_ICON, TAG_ITEM_ICON, itemId);
        sMenuDataPtr->spriteIds[SPRITE_ARR_HELD_ITEM] = itemSpriteId;

        if (itemSpriteId != MAX_SPRITES) {
            *spriteId = itemSpriteId;
            gSprites[itemSpriteId].x2 = x;
            gSprites[itemSpriteId].y2 = y;
        }
    }
}
const u8 sText_Title_Nothing[] = _("");

const u8 sText_Title_Battler_Stats[] = _("宝可梦能力");
const u8 sText_Title_Battler_Ability[] = _("特性信息");
const u8 sText_Title_Battler_Moves[] = _("招式信息");
const u8 sText_Title_Battler_Status[] = _("宝可梦状态");
const u8 sText_Title_Battler_Damage[] = _("伤害计算");
const u8 sText_Title_Field_Party[] = _("队伍信息");
const u8 sText_Title_Field_Stats[] = _("场地信息");
const u8 sText_Title_Enemy_Side[] = _("敌方信息");
const u8 sText_Title_Player_Side[] = _("我方信息");
const u8 sText_Title_Speed_Order[] = _("速度顺序");
const u8 sText_Title_FieldControllers[] = _("{A_BUTTON}滚动 {DPAD_UPDOWN}切换 {DPAD_LEFTRIGHT}翻页");
const u8 sText_Title_FieldControllers2[] = _("{A_BUTTON}滚动 {DPAD_UPDOWN}切换");
const u8 sText_Title_PartyInfoSelect[] = _("{A_BUTTON}选择 {DPAD_UPDOWN}切换 {DPAD_LEFTRIGHT}翻页");
const u8 sText_Title_PartyInfo[] = _("{A_BUTTON}信息 {B_BUTTON}返回 {DPAD_NONE}切换");
const u8 sText_Title_Controllers[] = _("{DPAD_UPDOWN}切换 {DPAD_LEFTRIGHT}翻页");

const u8 sText_Title_Type_One[] = _("属性：\n{STR_VAR_1}");
const u8 sText_Title_Type_Two[] = _("属性：\n{STR_VAR_1}/ {STR_VAR_2}");
const u8 sText_Title_Type_Three[] = _("属性：\n{STR_VAR_1}/ {STR_VAR_2}/ {STR_VAR_3}");

const u8 sText_Title_Nature[] = _("性格：{STR_VAR_1}\n(+{STR_VAR_2}, -{STR_VAR_3})");
const u8 sText_Title_Nature_NoStat[] = _("性格：\n{STR_VAR_1}");

const u8 sText_Title_Held_Item[] = _("持有道具：{STR_VAR_1}");
const u8 gText_NewLevelSymbol[] = _("{LV}{STR_VAR_1}");
#define SPACE_BETWEEN_TYPES (5 * 8)

static void PrintStatsTab() {
    u8 i, j;
    u8 x, y, x2, y2;
    u8 windowId = WINDOW_1;
    u8 colorIdx = FONT_BLACK;
    SpeciesEnum species = gBattleMons[sMenuDataPtr->battlerId].species;
    u8 gender = GetGenderFromSpeciesAndPersonality(gBattleMons[sMenuDataPtr->battlerId].species, gBattleMons[sMenuDataPtr->battlerId].personality);
    u8 statStage;
    bool8 statStageUp = FALSE;
    u8 numtypes = 1;
    struct Pokemon* party;
    u8 nature;
    const s8* natureMod;

    FillWindowPixelBuffer(windowId, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));

    // Title
    x = 9;
    y = 0;
    x2 = 0;
    y2 = -4;
    AddTextPrinterParameterized4(windowId, FONT_SMALL, (x * 8), (y * 8), 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, sText_Title_Battler_Stats);
    x = 19;
    AddTextPrinterParameterized4(windowId, FONT_SMALL, (x * 8), (y * 8), 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, sText_Title_Controllers);

    // Pokemon Name
    x = 9;
    y = 4;
    x2 = 0;
    y2 = -4;
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gSpeciesNames[species]);
    // Pokemon Gender
    x = x + 8;
    switch (gender) {
        case MON_MALE:
            AddTextPrinterParameterized4(
                windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLUE], 0xFF, gText_MaleSymbol);
            break;
        case MON_FEMALE:
            AddTextPrinterParameterized4(
                windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_RED], 0xFF, gText_FemaleSymbol);
            break;
    }
    // Pokemon Level
    x++;
    ConvertIntToDecimalStringN(gStringVar1, gBattleMons[sMenuDataPtr->battlerId].level, STR_CONV_MODE_LEFT_ALIGN, 3);
    StringExpandPlaceholders(gStringVar4, gText_NewLevelSymbol);
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar4);
    // Pokemon Types
    y++;
    x = 9;
    StringCopy(gStringVar1, gTypeNames[gBattleMons[sMenuDataPtr->battlerId].type1]);
    // Check if there is a second type
    if (gBattleMons[sMenuDataPtr->battlerId].type1 != gBattleMons[sMenuDataPtr->battlerId].type2) {
        numtypes++;
        StringCopy(gStringVar2, gTypeNames[gBattleMons[sMenuDataPtr->battlerId].type2]);
    }
    // Check if there is a third type
    if (gBattleMons[sMenuDataPtr->battlerId].type3 != TYPE_MYSTERY &&
        gBattleMons[sMenuDataPtr->battlerId].type3 != gBattleMons[sMenuDataPtr->battlerId].type1 &&
        gBattleMons[sMenuDataPtr->battlerId].type3 != gBattleMons[sMenuDataPtr->battlerId].type2) {
        numtypes++;
        if (numtypes == 2)
            StringCopy(gStringVar2, gTypeNames[gBattleMons[sMenuDataPtr->battlerId].type3]);
        else
            StringCopy(gStringVar3, gTypeNames[gBattleMons[sMenuDataPtr->battlerId].type3]);
    }

    switch (numtypes) {
        case 1:
            StringExpandPlaceholders(gStringVar4, sText_Title_Type_One);
            break;
        case 2:
            StringExpandPlaceholders(gStringVar4, sText_Title_Type_Two);
            break;
        case 3:
            StringExpandPlaceholders(gStringVar4, sText_Title_Type_Three);
            break;
    }
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar4);

    // Held Item
    y = y + 2;
    if (gBattleMons[sMenuDataPtr->battlerId].item != ITEM_NONE)
        CopyItemName(gBattleMons[sMenuDataPtr->battlerId].item, gStringVar1);
    else
        StringCopy(gStringVar1, sText_None);
    StringExpandPlaceholders(gStringVar4, sText_Title_Held_Item);
    // StringCopy(gStringVar1, ItemId_GetDescription(gBattleMons[sMenuDataPtr->battlerId].item));
    StringExpandPlaceholders(gStringVar4, sText_Title_Held_Item);
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar4);

    // Item Icon
    x = 26 + 1;
    x2 = 4;
    y = 4 + 3;
    ShowItemIcon(gBattleMons[sMenuDataPtr->battlerId].item, (x * 8) + x2, (y * 8) + y2);
    x2 = 0;

    // Stat Drops & Ups
    x = 9;
    y = 9;
    // Attack
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, sText_Attack);
    y++;
    // Defense
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, sText_Defense);
    y++;
    // Special Attack
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, sText_SpecialAttack);
    y++;
    // Special Defense
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, sText_SpecialDefense);
    y++;
    // Speed
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, sText_Speed);
    y += 2;
    // Accuracy
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, sText_Accuracy);
    // Evasion
    y++;
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, sText_Evasion);
    // Critical
    y++;
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, sText_Critical);

    x = 11;
    y = 9;
    for (i = 0; i < NUM_BATTLE_STATS - 1; i++) {
        statStage = gBattleMons[sMenuDataPtr->battlerId].statStages[statorder[i + 1]];  // HP is not taken into account
        if (statStage != DEFAULT_STAT_STAGE) {
            if (statStage > DEFAULT_STAT_STAGE) {
                statStageUp = TRUE;
                statStage = statStage - DEFAULT_STAT_STAGE;
            } else {
                statStageUp = FALSE;
                statStage = DEFAULT_STAT_STAGE - statStage;
            }

            for (j = 0; j < statStage; j++) {
                if (statStageUp)
                    BlitBitmapToWindow(windowId, sStatUpArrow, ((x + j) * 8) + x2, (y * 8), 8, 8);
                else
                    BlitBitmapToWindow(windowId, sStatDownArrow, ((x + j) * 8) + x2, (y * 8), 8, 8);
            }
        }
        if (statorder[i + 1] == STAT_SPEED)
            y += 2;
        else
            y++;
    }

    for (i = 0; i < gVolatileStructs[sMenuDataPtr->battlerId].critBoost; i++) {
        BlitBitmapToWindow(windowId, sStatUpArrow, ((x + i) * 8) + x2, (y * 8), 8, 8);
    }

    // Stat names
    x = 20;
    y = 9;
    // HP
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, sText_StatHP);
    ConvertIntToDecimalStringN(gStringVar1, gBattleMons[sMenuDataPtr->battlerId].hp, STR_CONV_MODE_LEFT_ALIGN, 3);
    ConvertIntToDecimalStringN(gStringVar2, gBattleMons[sMenuDataPtr->battlerId].maxHP, STR_CONV_MODE_LEFT_ALIGN, 3);
    StringExpandPlaceholders(gStringVar4, sText_HP);
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, ((x + 3) * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar4);
    y++;
    // Attack
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, sText_Attack);
    ConvertIntToDecimalStringN(gStringVar1, CalculateStat(sMenuDataPtr->battlerId, STAT_ATK, 0, 0, 0, 0, 0, 0), STR_CONV_MODE_LEFT_ALIGN, 5);
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, ((x + 3) * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar1);
    y++;
    // Defense
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, sText_Defense);
    ConvertIntToDecimalStringN(gStringVar1, CalculateStat(sMenuDataPtr->battlerId, STAT_DEF, 0, 0, 0, 0, 0, 0), STR_CONV_MODE_LEFT_ALIGN, 5);
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, ((x + 3) * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar1);
    y++;
    // Special Attack
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, sText_SpecialAttack);
    ConvertIntToDecimalStringN(gStringVar1, CalculateStat(sMenuDataPtr->battlerId, STAT_SPATK, 0, 0, 0, 0, 0, 0), STR_CONV_MODE_LEFT_ALIGN, 5);
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, ((x + 3) * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar1);
    y++;
    // Special Defense
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, sText_SpecialDefense);
    ConvertIntToDecimalStringN(gStringVar1, CalculateStat(sMenuDataPtr->battlerId, STAT_SPDEF, 0, 0, 0, 0, 0, 0), STR_CONV_MODE_LEFT_ALIGN, 5);
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, ((x + 3) * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar1);
    y++;
    // Speed
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, sText_Speed);
    ConvertIntToDecimalStringN(gStringVar1, GetBattlerTotalSpeedStat(sMenuDataPtr->battlerId, TOTAL_SPEED_FULL, MOVE_NONE), STR_CONV_MODE_LEFT_ALIGN, 5);
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, ((x + 3) * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar1);

    // Nature
    x = 20;
    y = 16;
    if (GetBattlerSide(sMenuDataPtr->battlerId) == B_SIDE_PLAYER)
        party = gPlayerParty;
    else
        party = gEnemyParty;

    nature = GetMonData(&party[gBattlerPartyIndexes[sMenuDataPtr->battlerId]], MON_DATA_NATURE, NULL);

    CopyNatureName(nature);
    if (nature == NATURE_HARDY || nature == NATURE_DOCILE || nature == NATURE_SERIOUS || nature == NATURE_BASHFUL || nature == NATURE_QUIRKY) {
        // No Stat Up or Down
        StringExpandPlaceholders(gStringVar4, sText_Title_Nature_NoStat);
    } else {
        natureMod = gNatureStatTable[nature];
        for (i = 0; i < NUM_NATURE_STATS; i++) {
            if (natureMod[i] > 0) {
                // Nature Up
                switch (i + 1) {
                    case STAT_ATK:
                        StringCopy(gStringVar2, sText_Attack);
                        break;
                    case STAT_DEF:
                        StringCopy(gStringVar2, sText_Defense);
                        break;
                    case STAT_SPATK:
                        StringCopy(gStringVar2, sText_SpecialAttack);
                        break;
                    case STAT_SPDEF:
                        StringCopy(gStringVar2, sText_SpecialDefense);
                        break;
                    case STAT_SPEED:
                        StringCopy(gStringVar2, sText_Speed);
                        break;
                }
            } else if (natureMod[i] < 0) {
                // Nature Down
                switch (i + 1) {
                    case STAT_ATK:
                        StringCopy(gStringVar3, sText_Attack);
                        break;
                    case STAT_DEF:
                        StringCopy(gStringVar3, sText_Defense);
                        break;
                    case STAT_SPATK:
                        StringCopy(gStringVar3, sText_SpecialAttack);
                        break;
                    case STAT_SPDEF:
                        StringCopy(gStringVar3, sText_SpecialDefense);
                        break;
                    case STAT_SPEED:
                        StringCopy(gStringVar3, sText_Speed);
                        break;
                }
            }
        }
        StringExpandPlaceholders(gStringVar4, sText_Title_Nature);
    }

    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar4);

    PutWindowTilemap(windowId);
    CopyWindowToVram(windowId, 3);
}

const u8 sText_PrintAbilityTab_Ability[] = _("特性");
const u8 sText_PrintAbilityTab_Innate[] = _("天性");
#define SPACE_BETWEEN_ABILITY_AND_NAME (8 * 8)
#define SPACE_BETWEEN_ABILITIES 3
static void GetBattlerDisplayedAbilities(u8 battlerId, AbilityEnum *abilities) {
    SpeciesEnum species = gBattleMons[battlerId].species;
    u32 personality = gBattleMons[battlerId].personality;
    bool8 isEnemyMon = GetBattlerSide(battlerId) == B_SIDE_OPPONENT;
    bool8 shouldShowRandomizedWildEnemy = isEnemyMon && !(gBattleTypeFlags & BATTLE_TYPE_TRAINER);

    if (shouldShowRandomizedWildEnemy) {
        u8 partyIndex = gBattlerPartyIndexes[battlerId];
        struct Pokemon *enemyMon = &gEnemyParty[partyIndex];
        species = GetMonData(enemyMon, MON_DATA_SPECIES, NULL);
        personality = GetMonData(enemyMon, MON_DATA_PERSONALITY, NULL);
        abilities[0] = GetAbilityBySpecies(species, GetMonData(enemyMon, MON_DATA_ABILITY_NUM, NULL));
        abilities[0] = RandomizeAbility(abilities[0], species, personality);
    } else {
        abilities[0] = gBattleMons[battlerId].abilities[0];
    }

    if (shouldShowRandomizedWildEnemy || !isEnemyMon) {
        abilities[1] = RandomizeInnate(gBaseStats[species].innates[0], species, personality);
        abilities[2] = RandomizeInnate(gBaseStats[species].innates[1], species, personality);
        abilities[3] = RandomizeInnate(gBaseStats[species].innates[2], species, personality);
    } else {
        abilities[1] = gBaseStats[species].innates[0];
        abilities[2] = gBaseStats[species].innates[1];
        abilities[3] = gBaseStats[species].innates[2];
    }
}

static void PrintAbilityTab() {
    u8 x, y, x2, y2;
    u8 windowId = WINDOW_1;
    u8 colorIdx = FONT_BLACK;
    AbilityEnum abilities[4];

    GetBattlerDisplayedAbilities(sMenuDataPtr->battlerId, abilities);

    FillWindowPixelBuffer(windowId, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));

    // Title
    x = 9;
    y = 0;
    x2 = 0;
    y2 = 0;
    AddTextPrinterParameterized4(windowId, FONT_SMALL, (x * 8), (y * 8), 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, sText_Title_Battler_Ability);
    x = 19;
    AddTextPrinterParameterized4(windowId, FONT_SMALL, (x * 8), (y * 8), 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, sText_Title_Controllers);

    // Main Ability
    x = 9;
    y = 3;
    x2 = 0;
    y2 = -4;
    // Title
    AddTextPrinterParameterized4(
        windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, sText_PrintAbilityTab_Ability);
    // Name ---------------------------------------------------------------------------------------------------
    AddTextPrinterParameterized4(windowId,
                                 FONT_SMALL_NARROW,
                                 (x * 8) + SPACE_BETWEEN_ABILITY_AND_NAME,
                                 (y * 8) + y2,
                                 0,
                                 0,
                                 sMenuWindowFontColors[FONT_WHITE],
                                 0xFF,
                                 gAbilities[abilities[0]].name);
    // Description ---------------------------------------------------------------------------------------------------
    y++;
    AddTextPrinterParameterized4(
        windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2 + 4, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gAbilities[abilities[0]].description);

    // Innate 1
    y = y + SPACE_BETWEEN_ABILITIES;
    // Title
    AddTextPrinterParameterized4(
        windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, sText_PrintAbilityTab_Innate);
    // Name ---------------------------------------------------------------------------------------------------
    AddTextPrinterParameterized4(windowId,
                                 FONT_SMALL_NARROW,
                                 (x * 8) + SPACE_BETWEEN_ABILITY_AND_NAME,
                                 (y * 8) + y2,
                                 0,
                                 0,
                                 sMenuWindowFontColors[FONT_WHITE],
                                 0xFF,
                                 gAbilities[abilities[1]].name);
    // Description ---------------------------------------------------------------------------------------------------
    y++;
    AddTextPrinterParameterized4(
        windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2 + 4, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gAbilities[abilities[1]].description);

    // Innate 2
    y = y + SPACE_BETWEEN_ABILITIES;
    // Title
    AddTextPrinterParameterized4(
        windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, sText_PrintAbilityTab_Innate);
    // Name ---------------------------------------------------------------------------------------------------
    AddTextPrinterParameterized4(windowId,
                                 FONT_SMALL_NARROW,
                                 (x * 8) + SPACE_BETWEEN_ABILITY_AND_NAME,
                                 (y * 8) + y2,
                                 0,
                                 0,
                                 sMenuWindowFontColors[FONT_WHITE],
                                 0xFF,
                                 gAbilities[abilities[2]].name);
    // Description ---------------------------------------------------------------------------------------------------
    y++;
    AddTextPrinterParameterized4(
        windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2 + 4, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gAbilities[abilities[2]].description);

    // Innate 3
    y = y + SPACE_BETWEEN_ABILITIES;
    // Title
    AddTextPrinterParameterized4(
        windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, sText_PrintAbilityTab_Innate);
    // Name ---------------------------------------------------------------------------------------------------
    AddTextPrinterParameterized4(windowId,
                                 FONT_SMALL_NARROW,
                                 (x * 8) + SPACE_BETWEEN_ABILITY_AND_NAME,
                                 (y * 8) + y2,
                                 0,
                                 0,
                                 sMenuWindowFontColors[FONT_WHITE],
                                 0xFF,
                                 gAbilities[abilities[3]].name);
    // Description ---------------------------------------------------------------------------------------------------
    y++;
    AddTextPrinterParameterized4(
        windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2 + 4, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gAbilities[abilities[3]].description);

    PutWindowTilemap(windowId);
    CopyWindowToVram(windowId, 3);
}

const u8 gText_CurrentPP[] = _("PP：{STR_VAR_1}/{STR_VAR_2}");
const u8 gText_MoveInfo_Power[] = _("威力：{STR_VAR_1}");
const u8 gText_MoveInfo_Accuracy[] = _("命中：{STR_VAR_1}");
const u8 gText_MoveInfo_Priority[] = _("优先度：{STR_VAR_1}");
const u8 gText_MoveInfo_Split[] = _("分类：{STR_VAR_1}");
const u8 gText_MoveInfo_STAB[] = _("本系：{STR_VAR_1}");

const u8 gText_Split_Physical[] = _("物理");
const u8 gText_Split_Special[] = _("特殊");
const u8 gText_Split_Status[] = _("状态");

const u8 gText_MoveInfo_Target1[] = _("目标1：{STR_VAR_1}");
const u8 gText_MoveInfo_Target2[] = _("目标2：{STR_VAR_1}");

const u8 gText_MoveInfo_Damage[] = _("伤害");

const u8 gText_Target_Nothing[] = _("---");
const u8 sText_Title_Controllers_Move[] = _("{DPAD_UPDOWN}切换 {DPAD_LEFTRIGHT}翻页 {A_BUTTON}模式");
#define SPACE_BETWEEN_MOVES 4
#define SPACE_BETWEEN_DAMAGE (4 * 8)
#define MIN_MOVE_DAMAGE 4
#define ENABLE_DAMAGE_CALCULATION FALSE
static void PrintMoveTab(void) {
    u8 x, y;
    u8 windowId = WINDOW_1;
    u16 move1 = gBattleMons[sMenuDataPtr->battlerId].moves[0];
    u16 move2 = gBattleMons[sMenuDataPtr->battlerId].moves[1];
    u16 move3 = gBattleMons[sMenuDataPtr->battlerId].moves[2];
    u16 move4 = gBattleMons[sMenuDataPtr->battlerId].moves[3];

    FillWindowPixelBuffer(windowId, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));

    // Title
    x = 9;
    y = 0;
    AddTextPrinterParameterized4(windowId, FONT_SMALL, (x * 8), (y * 8), 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, sText_Title_Battler_Moves);
    x = 16;
    AddTextPrinterParameterized4(windowId, FONT_SMALL, (x * 8), (y * 8), 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, sText_Title_Controllers_Move);

    // First Move
    x = 9;
    y = 3;
    PrintMoveInfo(move1, x, y, 0);

    // Second Move
    y = y + SPACE_BETWEEN_MOVES;
    PrintMoveInfo(move2, x, y, 1);

    // Third Move
    y = y + SPACE_BETWEEN_MOVES;
    PrintMoveInfo(move3, x, y, 2);

    // Fourth Move
    y = y + SPACE_BETWEEN_MOVES;
    PrintMoveInfo(move4, x, y, 3);

    PutWindowTilemap(windowId);
    CopyWindowToVram(windowId, 3);
}

const u8 gText_Makes_Contact_True[] = _("接触");
const u8 gText_Makes_Contact_False[] = _("不接触");
const u8 gText_Protect_Affected_True[] = _("受守住影响");
const u8 gText_Protect_Affected_False[] = _("不受守住影响");
const u8 gText_Magic_Coat_Affected_True[] = _("受魔法反射影响");
const u8 gText_Magic_Coat_Affected_False[] = _("不受魔法反射影响");
const u8 gText_Snatch_Affected_True[] = _("受抢夺影响");
const u8 gText_Snatch_Affected_False[] = _("不受抢夺影响");
const u8 gText_Mirror_Move_Affected_True[] = _("受鹦鹉学舌影响");
const u8 gText_Mirror_Move_Affected_False[] = _("不受鹦鹉学舌影响");
const u8 gText_Kings_Rock_Affected_True[] = _("受王者之证影响");
const u8 gText_Kings_Rock_Affected_False[] = _("不受王者之证影响");

const u8 gText_Boost_True[] = _("是");
const u8 gText_Boost_False[] = _("否");

const u8 gText_Move_Type_TwoTypedMoves[] = _("{STR_VAR_1}/{STR_VAR_2}");

static void PrintMoveInfo(MoveEnum move, u8 x, u8 y, u8 moveIdx) {
    u8 windowId = WINDOW_1;
    u8 colorIdx = FONT_BLACK;
    bool8 isEnemyMon = GetBattlerSide(sMenuDataPtr->battlerId) == B_SIDE_OPPONENT;
    u8 mode = sMenuDataPtr->moveModeId;
    u8 x2 = 0;
    u8 y2 = -4;
    u8 target = BATTLE_OPPOSITE(sMenuDataPtr->battlerId);
    u32 movePower = gBattleMoves[move].power;
    u32 moveAccuracy = GetTotalAccuracy(sMenuDataPtr->battlerId, target, move, NULL);
    u8 moveType = gBattleMoves[move].type;
    bool8 isStatusMove = gBattleMoves[move].split == SPLIT_STATUS;
    u8 stab = 2;
    struct Pokemon* party;

    // Party
    if (!isEnemyMon)
        party = gPlayerParty;
    else
        party = gEnemyParty;

    // Sets move type depending on the mon ability/stats
    SetTypeBeforeUsingMove(move, sMenuDataPtr->battlerId);
    GET_MOVE_TYPE(move, moveType);

    // Check Stab
    stab = StabMultiplierInHalves(sMenuDataPtr->battlerId, moveType, move);

    // Move Name
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gMoveNamesLong[move]);
    // Type
    if (move) {
        StringCopy(gStringVar4, gTypeNames[moveType]);
        AddTextPrinterParameterized4(windowId,
                                     FONT_SMALL_NARROW,
                                     ((x + 1) * 8) + SPACE_BETWEEN_ABILITY_AND_NAME + 16,
                                     (y * 8) + y2,
                                     0,
                                     0,
                                     sMenuWindowFontColors[FONT_WHITE],
                                     0xFF,
                                     gStringVar4);
    } else if (gBattleMoves[move].type2 != TYPE_MYSTERY && gBattleMoves[move].type2 != TYPE_NORMAL) {
        StringCopy(gStringVar1, gTypeNames[moveType]);
        StringCopy(gStringVar2, gTypeNames[gBattleMoves[move].type2]);
        StringExpandPlaceholders(gStringVar4, gText_Move_Type_TwoTypedMoves);
        AddTextPrinterParameterized4(windowId,
                                     FONT_SMALL_NARROW,
                                     ((x + 1) * 8) + SPACE_BETWEEN_ABILITY_AND_NAME + 16,
                                     (y * 8) + y2,
                                     0,
                                     0,
                                     sMenuWindowFontColors[FONT_WHITE],
                                     0xFF,
                                     gStringVar4);
    }
    y++;

    // Stab Boost
    if (stab > 2) movePower = movePower * stab / 2;

    switch (mode) {
        case MOVE_MODE_NORMAL:
            if (!move) {
                StringCopy(gStringVar4, gText_Target_Nothing);
                AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar4);
                break;
            }
            // Move Power ---------------------------------------------------------------------------------------------------
            if (!isStatusMove)
                ConvertIntToDecimalStringN(gStringVar1, movePower, STR_CONV_MODE_LEFT_ALIGN, 3);
            else
                StringCopy(gStringVar1, gText_Target_Nothing);
            StringExpandPlaceholders(gStringVar4, gText_MoveInfo_Power);
            AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar4);
            // Move Accuracy
            if (moveAccuracy <= 100)
                ConvertIntToDecimalStringN(gStringVar1, moveAccuracy, STR_CONV_MODE_LEFT_ALIGN, 3);
            else
                StringCopy(gStringVar1, gText_Target_Nothing);
            StringExpandPlaceholders(gStringVar4, gText_MoveInfo_Accuracy);
            AddTextPrinterParameterized4(
                windowId, FONT_SMALL_NARROW, (x * 8) + SPACE_BETWEEN_ABILITY_AND_NAME, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar4);
            y++;
            // Move Priority ---------------------------------------------------------------------------------------------------
            if (gBattleMoves[move].priority >= 0)
                ConvertIntToDecimalStringN(gStringVar1, gBattleMoves[move].priority, STR_CONV_MODE_LEFT_ALIGN, 3);
            else
                ConvertIntToDecimalStringN(gStringVar1, 0, STR_CONV_MODE_LEFT_ALIGN, 3);
            StringExpandPlaceholders(gStringVar4, gText_MoveInfo_Priority);
            AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar4);
            // Move Split
            switch (gBattleMoves[move].split) {
                case SPLIT_PHYSICAL:
                    StringCopy(gStringVar1, gText_Split_Physical);
                    break;
                case SPLIT_SPECIAL:
                    StringCopy(gStringVar1, gText_Split_Special);
                    break;
                case SPLIT_STATUS:
                default:
                    StringCopy(gStringVar1, gText_Split_Status);
                    break;
            }
            StringExpandPlaceholders(gStringVar4, gText_MoveInfo_Split);
            AddTextPrinterParameterized4(
                windowId, FONT_SMALL_NARROW, (x * 8) + SPACE_BETWEEN_ABILITY_AND_NAME, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar4);
            y++;
            // PP
            ConvertIntToDecimalStringN(gStringVar1, gBattleMons[sMenuDataPtr->battlerId].pp[moveIdx], STR_CONV_MODE_LEFT_ALIGN, 2);  // Current PP
            if (GetBattlerSide(sMenuDataPtr->battlerId) == B_SIDE_PLAYER) {
                ConvertIntToDecimalStringN(
                    gStringVar2,
                    CalculatePPWithBonusPlayer(move, GetMonData(&party[gBattlerPartyIndexes[sMenuDataPtr->battlerId]], MON_DATA_PP_BONUSES, NULL), moveIdx),
                    STR_CONV_MODE_LEFT_ALIGN,
                    2);  // Max PP
            } else {
                ConvertIntToDecimalStringN(
                    gStringVar2,
                    CalculatePPWithBonus(move, GetMonData(&party[gBattlerPartyIndexes[sMenuDataPtr->battlerId]], MON_DATA_PP_BONUSES, NULL), moveIdx),
                    STR_CONV_MODE_LEFT_ALIGN,
                    2);  // Max PP
            }
            StringExpandPlaceholders(gStringVar4, gText_CurrentPP);
            AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar4);
            // Stab
            if (!isStatusMove) {  // No stab boost for status moves
                if (stab > 2)
                    StringCopy(gStringVar1, gText_Boost_True);
                else
                    StringCopy(gStringVar1, gText_Boost_False);
                StringExpandPlaceholders(gStringVar4, gText_MoveInfo_STAB);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + SPACE_BETWEEN_ABILITY_AND_NAME,
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[colorIdx],
                                             0xFF,
                                             gStringVar4);
            }
            break;
        case MOVE_MODE_DESCRIPTION:
            y2 = y2 + 4;
            StringCopy(gStringVar4, gMoveDescriptionPointers[move - 1]);
            AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar4);
            break;
        /*case MOVE_MODE_DAMAGE_CALCULATOR:
            if (gBattleMoves[move].split == SPLIT_STATUS || movePower == 0) {
                StringCopy(gStringVar4, gText_Target_Nothing);
            }
            else {
                PrintDamageCalculation(sMenuDataPtr->battlerId, target, move);
            }
            AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar4);
        break;*/
        case MOVE_MODE_AFFECTED_1:
            // Makes Contact
            if (gBattleMoves[move].contact)
                StringCopy(gStringVar4, gText_Makes_Contact_True);
            else
                StringCopy(gStringVar4, gText_Makes_Contact_False);
            AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar4);

            // Protect Affected
            y++;
            if (gBattleMoves[move].flags & FLAG_PROTECT_AFFECTED)
                StringCopy(gStringVar4, gText_Protect_Affected_True);
            else
                StringCopy(gStringVar4, gText_Protect_Affected_False);
            AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar4);
            // Magic Coat Affected
            y++;
            if (gBattleMoves[move].flags & FLAG_MAGIC_COAT_AFFECTED)
                StringCopy(gStringVar4, gText_Magic_Coat_Affected_True);
            else
                StringCopy(gStringVar4, gText_Magic_Coat_Affected_False);
            AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar4);
            break;
        case MOVE_MODE_AFFECTED_2:
            // Snatch Affected
            if (gBattleMoves[move].flags & FLAG_SNATCH_AFFECTED)
                StringCopy(gStringVar4, gText_Snatch_Affected_True);
            else
                StringCopy(gStringVar4, gText_Snatch_Affected_False);
            AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar4);

            // Mirror Move Affected
            y++;
            if (gBattleMoves[move].flags & FLAG_PROTECT_AFFECTED)
                StringCopy(gStringVar4, gText_Mirror_Move_Affected_True);
            else
                StringCopy(gStringVar4, gText_Mirror_Move_Affected_False);
            AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar4);
            // King's Rock Affected
            y++;
            if (gBattleMoves[move].flags & FLAG_MAGIC_COAT_AFFECTED)
                StringCopy(gStringVar4, gText_Kings_Rock_Affected_True);
            else
                StringCopy(gStringVar4, gText_Kings_Rock_Affected_False);
            AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar4);
            break;
    }
}

// Misc
const u8 sText_Title_Field_Turns_Left[] = _("剩余回合：");
const u8 sText_Title_Field_Turns[] = _("回合：");
const u8 sText_Title_Field_Layers[] = _("层数：");
const u8 sText_Title_Field_Charges[] = _("蓄力：");
const u8 sText_Title_Field_Nature_Power[] = _("自然之力：");
const u8 sText_Title_Field_Secret_Power[] = _("秘密之力：");
const u8 sText_Title_Field_Paralysis[] = _("造成麻痹");
const u8 sText_Title_Field_Active[] = _("生效");
const u8 sText_Title_Field_Not_Active[] = _("未生效");
const u8 sText_Title_Field_None[] = _("全无");
// Extra Abilities
const u8 sText_Title_Extra_Abilities[] = _("额外特性");
const u8 sText_Title_Extra_Abilities_Description[] =
    _("这只宝可梦还有以下\n额外特性：{STR_VAR_3}。");
const u8 sText_Title_Extra_Abilities_Description_2[] = _("{STR_VAR_1}、{STR_VAR_2}");
const u8 sText_Title_Extra_Abilities_Description_3[] = _("{STR_VAR_1}\n{STR_VAR_2}");
const u8 sText_Title_Extra_Abilities_Description_4[] = _("{STR_VAR_1}和{STR_VAR_2}");
// Primary Status
const u8 sText_Title_Status_Paralysis[] = _("麻痹");
const u8 sText_Title_Status_Paralysis_Description[] =
    _("使这只宝可梦的速度降低50%，\n并有25%的几率因完全麻痹\n而失去行动机会。");
const u8 sText_Title_Status_Burn[] = _("灼伤");
const u8 sText_Title_Status_Burn_Description[] =
    _("使这只宝可梦的物攻降低50%，\n并在每回合结束时损失\n最大HP的1/16。");
const u8 sText_Title_Status_Sleep[] = _("睡眠");
const u8 sText_Title_Status_Sleep_Description[] =
    _("使这只宝可梦无法行动，\n易受食梦、恶梦与\n梦魇影响。");
const u8 sText_Title_Status_Frostbite[] = _("冻伤");
const u8 sText_Title_Status_Frostbite_Description[] =
    _("使这只宝可梦的特攻降低50%，\n并在每回合结束时损失\n最大HP的1/16。");
const u8 sText_Title_Status_Freeze[] = _("冰冻");
const u8 sText_Title_Status_Freeze_Description[] =
    _("使这只宝可梦数回合无法行动，\n受到某些火属性招式攻击时\n可以解除。");
const u8 sText_Title_Status_Poison[] = _("中毒");
const u8 sText_Title_Status_Poison_Description[] =
    _("每回合损失最大HP的1/8，\n受到祸不单行和\n毒液的伤害翻倍。");
const u8 sText_Title_Status_Bad_Poison[] = _("剧毒");
const u8 sText_Title_Status_Bad_Poison_Description[] =
    _("首回合损失最大HP的1/16，\n此后每次受到毒伤，\n损失量再增加1/16。");
const u8 sText_Title_Status_Bleed[] = _("流血");
const u8 sText_Title_Status_Bleed_Description[] =
    _("无法回复，无视能力提升，\n并损失最大HP的1/16。\n可被回复招式治愈。");

// Secondary Status
const u8 sText_Title_Status_Confusion[] = _("混乱");
const u8 sText_Title_Status_Confusion_Description[] =
    _("这只宝可梦有33%的几率\n以40威力攻击自己\n而不行动。");
const u8 sText_Title_Status_Enraged[] = _("愤怒");
const u8 sText_Title_Status_Enraged_Description[] =
    _("这只宝可梦会承受\n所造成伤害33%的\n反作用力。");
const u8 sText_Title_Status_IncomingAttack[] = _("受到攻击");
const u8 sText_Title_Status_IncomingAttack_Description[] =
    _("数回合后会受到{STR_VAR_1}的\n{STR_VAR_2}攻击，\n威力为{STR_VAR_3}。");
const u8 sText_Title_Status_Uproar[] = _("正在吵闹");
const u8 sText_Title_Status_Uproar_Description[] =
    _("数回合内持续吵闹，\n在此期间除少数例外\n无人能入睡。");
const u8 sText_Title_Status_Bide[] = _("忍耐");
const u8 sText_Title_Status_Bide_Description[] =
    _("无法攻击，\n会将受到的伤害翻倍\n返还给对手。");
const u8 sText_Title_Status_Infuation[] = _("着迷于{B_EFF_NAME_WITH_PREFIX}");
const u8 sText_Title_Status_Infuation_Description[] =
    _("降低对目标造成的伤害，\n任一方被替换时\n效果结束。");
const u8 sText_Title_Status_Focus_Energy[] = _("聚气");
const u8 sText_Title_Status_Focus_Energy_Description[] =
    _("会心率提高两级。\n该效果可通过\n接棒传递。");
const u8 sText_Title_Status_Dragon_Cheer[] = _("龙声鼓舞");
const u8 sText_Title_Status_Dragon_Cheer_Description[] =
    _("会心率提高一级。\n若为龙属性则\n提高两级。");
const u8 sText_Title_Status_Transformed[] = _("变成了{STR_VAR_1}");
const u8 sText_Title_Status_Transformed_Description[] =
    _("完全复制了目标宝可梦，\nPP较少，部分\n特性可能无效。");
const u8 sText_Title_Status_Escape_Prevention[] = _("无法逃走");
const u8 sText_Title_Status_Escape_Prevention_Description[] =
    _("这只宝可梦无法逃走或替换，\n只有战斗结束或倒下\n才能离场。");
const u8 sText_Title_Status_Cursed[] = _("诅咒");
const u8 sText_Title_Status_Cursed_Description[] =
    _("每回合结束时损失\n最大HP的1/4，直到\n离开场地为止。");
const u8 sText_Title_Status_Foresight[] = _("预知");
const u8 sText_Title_Status_Foresight_Description[] =
    _("若为幽灵属性，可被一般\n或格斗招式击中，\n无视闪避率。");
const u8 sText_Title_Status_Defense_Curl[] = _("变圆");
const u8 sText_Title_Status_Defense_Curl_Description[] =
    _("只要使用者仍在场上，\n滚动与冰球威力\n翻倍。");
const u8 sText_Title_Status_Torment[] = _("折磨");
const u8 sText_Title_Status_Torment_Description[] =
    _("无法连续使用\n同一招式，直到\n离开场地为止。");
const u8 sText_Title_Status_Leech_Seed_Target[] = _("被{STR_VAR_1}种下种子");
const u8 sText_Title_Status_Leech_Seed_Target_Description[] =
    _("每回合被吸取部分HP，\n直到离开\n场地为止。");
const u8 sText_Title_Status_Leech_Seed_User[] = _("给{STR_VAR_1}种下种子");
const u8 sText_Title_Status_Leech_Seed_User_Description[] =
    _("每回合从目标获得HP，\n直到离开\n场地为止。");
const u8 sText_Title_Status_Perish_Song[] = _("濒死");
const u8 sText_Title_Status_Perish_Song_Description[] =
    _("灭亡计数每回合减少1，\n减到0时\n就会倒下。");
const u8 sText_Title_Status_Minimize[] = _("缩小");
const u8 sText_Title_Status_Minimize_Description[] =
    _("特定招式的伤害翻倍，\n并跳过命中与\n闪避判定。");
const u8 sText_Title_Status_Charge[] = _("蓄力完毕");
const u8 sText_Title_Status_Charge_Description[] =
    _("使用者下一招电属性\n招式的伤害\n变为两倍。");
const u8 sText_Title_Status_Rooted[] = _("扎根");
const u8 sText_Title_Status_Rooted_Description[] =
    _("每回合结束时回复\n最大HP的1/16，\n但无法逃走或替换。");
const u8 sText_Title_Status_Yawn[] = _("困倦");
const u8 sText_Title_Status_Yawn_Description[] =
    _("若没有任何阻止因素，\n这只宝可梦会在下回合\n结束时睡着。");
const u8 sText_Title_Status_Grudge[] = _("怨念");
const u8 sText_Title_Status_Grudge_Description[] =
    _("若使用者因攻击直接倒下，\n导致其倒下的招式\n将失去全部PP。");
const u8 sText_Title_Status_Cant_Crit[] = _("无法击中要害");
const u8 sText_Title_Status_Cant_Crit_Description[] =
    _("这只宝可梦无法击中\n任何要害，直到\n离开场地为止。");
const u8 sText_Title_Status_Gastro_Acid[] = _("特性被压制");
#if B_NEUTRALIZING_GAS_WORKS_ON_INNATES
const u8 sText_Title_Status_Gastro_Acid_Description[] =
    _("这只宝可梦的特性与\n先天特性被禁用，直到\n离开场地为止。");
#else
const u8 sText_Title_Status_Gastro_Acid_Description[] =
    _("这只宝可梦的特性\n被禁用。直到\n离开场地为止。");
#endif
const u8 sText_Title_Status_Embargo[] = _("携带道具效果被无效");
const u8 sText_Title_Status_Embargo_Description[] =
    _("这只宝可梦数回合内\n无法使用\n持有道具。");
const u8 sText_Title_Status_Smack_Down[] = _("接地");
const u8 sText_Title_Status_Smack_Down_Description[] =
    _("地面招式会命中这只\n宝可梦，无视属性、\n特性、状态与道具。");
const u8 sText_Title_Status_Miracle_Eye[] = _("奇迹之眼");
const u8 sText_Title_Status_Miracle_Eye_Description[] =
    _("反转这只宝可梦的属性\n相性。超能属性\n免疫恶属性。");
const u8 sText_Title_Status_Heal_Block[] = _("无法被治愈");
const u8 sText_Title_Status_Heal_Block_Description[] =
    _("这只宝可梦无法以任何\n方式被治愈，持续\n数回合。");
const u8 sText_Title_Status_Aqua_Ring[] = _("水流环");
const u8 sText_Title_Status_Aqua_Ring_Description[] =
    _("这只宝可梦每回合结束时\n回复最大HP的\n1/16。");
const u8 sText_Title_Status_Magnet_Rise[] = _("电磁飘浮");
const u8 sText_Title_Status_Magnet_Rise_Description[] =
    _("这只宝可梦不会被\n地面属性招式命中，\n持续数回合。");
const u8 sText_Title_Status_Semi_Invulnerable[] = _("半无敌");
const u8 sText_Title_Status_Semi_Invulnerable_Description[] =
    _("这只宝可梦一回合内\n能避开大部分攻击，\n特定招式仍能命中。");
const u8 sText_Title_Status_Electrified[] = _("电气化");
const u8 sText_Title_Status_Electrified_Description[] =
    _("这只宝可梦的招式本回合\n将变为电属性。");
const u8 sText_Title_Status_Plasma_Fist[] = _("等离子拳");
const u8 sText_Title_Status_Plasma_Fist_Description[] =
    _("这只宝可梦的一般属性\n招式本回合将变为\n电属性。");
const u8 sText_Title_Status_Coiled_Up[] = _("盘卷");
const u8 sText_Title_Status_Coiled_Up_Description[] =
    _("这只宝可梦下一招\n咬类招式的\n优先度提高。");
const u8 sText_Title_Status_Cutthroat[] = _("凶手");
const u8 sText_Title_Status_Cutthroat_Description[] =
    _("这只宝可梦下一招\n斩击类招式的\n优先度提高。");
const u8 sText_Title_Status_Quark_Drive[] = _("夸克充能");
const u8 sText_Title_Status_Protosynthesis[] = _("古代活性");
const u8 sText_Title_Status_Paradox_Boost_Description[] =
    _("这只宝可梦的{STR_VAR_2}\n提升了{STR_VAR_3}%。");
const u8 sText_Paradox_Speed_Value[] = _("50");
const u8 sText_Paradox_Other_Value[] = _("30");
const u8 sText_Title_Status_Ghastly_Echo[] = _("幽灵回声");
const u8 sText_Title_Status_Ghastly_Echo_Description[] =
    _("这只宝可梦的下一击\n威力提升50%。");
const u8 sText_Title_Status_Commanded[] = _("被命令");
const u8 sText_Title_Status_Commanded_Description[] =
    _("这只宝可梦无法替换，\n也不会被强制替换。");
const u8 sText_Title_Status_Violent_Rush[] = _("暴力冲刺");
const u8 sText_Title_Status_Violent_Rush_Description[] =
    _("一回合内，这只宝可梦的\n速度提升50%，\n攻击提升20%。");
const u8 sText_Title_Status_Rapid_Response[] = _("快速响应");
const u8 sText_Title_Status_Rapid_Response_Description[] =
    _("一回合内，这只宝可梦的\n速度提升50%，\n特攻提升20%。");
const u8 sText_Title_Status_Readied_Action[] = _("准备就绪");
const u8 sText_Title_Status_Readied_Action_Description[] =
    _("一回合内，这只宝可梦的\n攻击翻倍。");
const u8 sText_Title_Status_Showdown_Mode[] = _("决胜模式");
const u8 sText_Title_Status_Showdown_Mode_Description[] =
    _("一回合内，这只宝可梦速度\n+50%、攻击+20%，\n且必定击中要害。");
const u8 sText_Title_Status_Trapped[] = _("无法逃");
const u8 sText_Title_Status_Trapped_Description[] =
    _("这只宝可梦数回合内\n无法替换，并损失\n最大HP的1/8。");
const u8 sText_Title_Status_Parasitic_Spores[] = _("寄生孢子");
const u8 sText_Title_Status_Parasitic_Spores_Description[] =
    _("若非幽灵属性，这只\n宝可梦损失最大HP的1/8。\n接触时会蔓延。");
const u8 sText_Title_Status_Fear[] = _("恐惧");
const u8 sText_Title_Status_Fear_Description[] =
    _("一回合内这只宝可梦\n无法替换，受到的伤害\n增加50%。");
const u8 sText_Title_Status_Dazed[] = _("晕眩");
const u8 sText_Title_Status_Dazed_Description[] = _("这只宝可梦总是最后行动。");
const u8 sText_Title_Status_Trepidation[] = _("战栗不安");
const u8 sText_Title_Status_Trepidation_Description[] =
    _("这只宝可梦使用的超能\n属性招式全部落空。");
const u8 sText_Title_Status_OnTheProwl[] = _("潜行");
const u8 sText_Title_Status_OnTheProwl_Description[] =
    _("一回合内这只宝可梦的招式\n优先度+1。对负优先度\n招式无效。");
const u8 sText_Title_Status_Stockpile[] = _("蓄力");
const u8 sText_Title_Status_Stockpile_Description[] =
    _("这只宝可梦积蓄了力量，\n可以吞下或吐出。");
const u8 sText_Title_Status_Soothsayer[] = _("预言家");
const u8 sText_Title_Status_Soothsayer_Description[] =
    _("所有针对这只宝可梦的\n攻击都变为效果\n不理想。");
const u8 sText_Title_Status_ExtraStat[] = _("额外{STR_VAR_2}。");
const u8 sText_Title_Status_ExtraStat_Description[] =
    _("这只宝可梦原本的{STR_VAR_2}\n提升了{STR_VAR_3}%");
const u8 sText_Title_Status_IceStatue[] = _("冰雕");
const u8 sText_Title_Status_IceStatue_Description[] =
    _("这只宝可梦为冰属性，\n不抵抗冰，也不会\n免疫冻伤。");
const u8 sText_Title_Status_Drenched[] = _("湿透");
const u8 sText_Title_Status_Drenched_Description[] =
    _("这只宝可梦在相同优先度\n内最后行动。");

#define SPACE_BETWEEN_LINES_FIELD ((6 * 8) + 4)
#define MAX_DESCRIPTION_LINES 3
#define LINES_BETWEEN_STUFF 1

#define SHOW_SLEEPING_TURNS FALSE

static void PrintStatusTab(void) {
    u8 i, j;
    u8 x, y, x2, y2;
    u8 windowId = WINDOW_1;
    u8 maxLines = 3;
    bool8 printedInfo = FALSE;
    u8 turnsLeft = 0;
    u8 currentStatus = 0;

    FillWindowPixelBuffer(windowId, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));

    // Title
    x = 9;
    y = 0;
    x2 = 0;
    y2 = 0;
    AddTextPrinterParameterized4(windowId, FONT_SMALL, (x * 8), (y * 8), 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, sText_Title_Battler_Status);
    x = 19;
    AddTextPrinterParameterized4(windowId, FONT_SMALL, (x * 8), (y * 8), 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, sText_Title_Controllers);

    // Weather
    x = 9;
    y = 3;
    x2 = 0;
    y2 = -4;

    if (sMenuDataPtr->numStatusInfo[sMenuDataPtr->battlerId] < maxLines) maxLines = sMenuDataPtr->numStatusInfo[sMenuDataPtr->battlerId];

    for (i = 0; i < maxLines; i++) {
        printedInfo = FALSE;
        currentStatus = (i + sMenuDataPtr->CurrentStatusInfo[sMenuDataPtr->battlerId]) % sMenuDataPtr->numStatusInfo[sMenuDataPtr->battlerId];
        j = sMenuDataPtr->BattlerStatus[currentStatus][sMenuDataPtr->battlerId];

        switch (j) {
            case STATUS_INFO_EXTRA_ABILITIES: {
                u8 k;
                AbilityEnum extraAbilities[HELL_MODE_EXTRA_ABILITIES];

                StringCopy(gStringVar1, sText_Title_Extra_Abilities);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                for (k = 0; k < HELL_MODE_EXTRA_ABILITIES; k++) extraAbilities[k] = GetBattlerAbilityInSlot(sMenuDataPtr->battlerId, TOTAL_ABILITY_COUNT + k);

                StringCopy(gStringVar1, gAbilities[extraAbilities[0]].name);
                StringCopy(gStringVar2, gAbilities[extraAbilities[1]].name);

                StringExpandPlaceholders(gStringVar3, sText_Title_Extra_Abilities_Description_2);

                StringCopy(gStringVar1, gStringVar3);
                StringCopy(gStringVar2, gAbilities[extraAbilities[2]].name);
                StringExpandPlaceholders(gStringVar3, sText_Title_Extra_Abilities_Description_3);

                StringCopy(gStringVar1, gStringVar3);
                StringCopy(gStringVar2, gAbilities[extraAbilities[2]].name);  // Placeholder since 4th Hell Mode ability slot is not implemented
                StringExpandPlaceholders(gStringVar3, sText_Title_Extra_Abilities_Description_4);

                // Description
                StringExpandPlaceholders(gStringVar4, sText_Title_Extra_Abilities_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar4);

                printedInfo = TRUE;
            } break;
            case STATUS_INFO_PRIMARY:
                printedInfo = TRUE;
                if (gBattleMons[sMenuDataPtr->battlerId].status1 & STATUS1_SLEEP) {
                    // Sleep
                    StringCopy(gStringVar1, sText_Title_Status_Sleep);
                    AddTextPrinterParameterized4(
                        windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                    // Turns Left
                    if (SHOW_SLEEPING_TURNS) {
                        StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                        AddTextPrinterParameterized4(windowId,
                                                     FONT_SMALL_NARROW,
                                                     (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                                     (y * 8) + y2,
                                                     0,
                                                     0,
                                                     sMenuWindowFontColors[FONT_WHITE],
                                                     0xFF,
                                                     gStringVar1);
                        turnsLeft = gBattleMons[sMenuDataPtr->battlerId].status1;
                        ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                        AddTextPrinterParameterized4(windowId,
                                                     FONT_SMALL_NARROW,
                                                     (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                                     (y * 8) + y2,
                                                     0,
                                                     0,
                                                     sMenuWindowFontColors[FONT_WHITE],
                                                     0xFF,
                                                     gStringVar1);
                    }
                    // Description
                    StringCopy(gStringVar1, sText_Title_Status_Sleep_Description);
                    AddTextPrinterParameterized4(
                        windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                } else if (gBattleMons[sMenuDataPtr->battlerId].status1 & STATUS1_POISON) {
                    // Poison
                    StringCopy(gStringVar1, sText_Title_Status_Poison);
                    AddTextPrinterParameterized4(
                        windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                    // Description
                    StringCopy(gStringVar1, sText_Title_Status_Poison_Description);
                    AddTextPrinterParameterized4(
                        windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                } else if (gBattleMons[sMenuDataPtr->battlerId].status1 & STATUS1_BURN) {
                    // Burn
                    StringCopy(gStringVar1, sText_Title_Status_Burn);
                    AddTextPrinterParameterized4(
                        windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                    // Description
                    StringCopy(gStringVar1, sText_Title_Status_Burn_Description);
                    AddTextPrinterParameterized4(
                        windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                } else if (gBattleMons[sMenuDataPtr->battlerId].status1 & STATUS1_FREEZE) {
                    // Freeze - Unused rn
                    StringCopy(gStringVar1, sText_Title_Status_Freeze);
                    AddTextPrinterParameterized4(
                        windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                    // Description
                    StringCopy(gStringVar1, sText_Title_Status_Freeze_Description);
                    AddTextPrinterParameterized4(
                        windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                } else if (gBattleMons[sMenuDataPtr->battlerId].status1 & STATUS1_PARALYSIS) {
                    // Paralysis
                    StringCopy(gStringVar1, sText_Title_Status_Paralysis);
                    AddTextPrinterParameterized4(
                        windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                    // Description
                    StringCopy(gStringVar1, sText_Title_Status_Paralysis_Description);
                    AddTextPrinterParameterized4(
                        windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                } else if (gBattleMons[sMenuDataPtr->battlerId].status1 & STATUS1_TOXIC_POISON) {
                    // Toxic Poison
                    StringCopy(gStringVar1, sText_Title_Status_Bad_Poison);
                    AddTextPrinterParameterized4(
                        windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                    // Turns
                    StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                    AddTextPrinterParameterized4(windowId,
                                                 FONT_SMALL_NARROW,
                                                 (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                                 (y * 8) + y2,
                                                 0,
                                                 0,
                                                 sMenuWindowFontColors[FONT_WHITE],
                                                 0xFF,
                                                 gStringVar1);
                    turnsLeft = gBattleMons[sMenuDataPtr->battlerId].status1 - STATUS1_TOXIC_COUNTER;
                    ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                    AddTextPrinterParameterized4(windowId,
                                                 FONT_SMALL_NARROW,
                                                 (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                                 (y * 8) + y2,
                                                 0,
                                                 0,
                                                 sMenuWindowFontColors[FONT_WHITE],
                                                 0xFF,
                                                 gStringVar1);

                    // Description
                    StringCopy(gStringVar1, sText_Title_Status_Bad_Poison_Description);
                    AddTextPrinterParameterized4(
                        windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                    // STATUS1_TOXIC_COUNTER
                } else if (gBattleMons[sMenuDataPtr->battlerId].status1 & STATUS1_FROSTBITE) {
                    // Paralysis
                    StringCopy(gStringVar1, sText_Title_Status_Frostbite);
                    AddTextPrinterParameterized4(
                        windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                    // Description
                    StringCopy(gStringVar1, sText_Title_Status_Frostbite_Description);
                    AddTextPrinterParameterized4(
                        windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                } else if (gBattleMons[sMenuDataPtr->battlerId].status1 & STATUS1_BLEED) {
                    // Bleed
                    StringCopy(gStringVar1, sText_Title_Status_Bleed);
                    AddTextPrinterParameterized4(
                        windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                    // Description
                    StringCopy(gStringVar1, sText_Title_Status_Bleed_Description);
                    AddTextPrinterParameterized4(
                        windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                } else {
                    printedInfo = FALSE;
                }
                break;
            case STATUS_INFO_CONFUSION:
                StringCopy(gStringVar1, sText_Title_Status_Confusion);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Confusion_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_ENRAGED:
                StringCopy(gStringVar1, sText_Title_Status_Enraged);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Enraged_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_FUTURE_SIGHT:
                StringCopy(gStringVar1, sText_Title_Status_IncomingAttack);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Turns Left
                StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);
                turnsLeft = gWishFutureKnock.futureSightCounter[sMenuDataPtr->battlerId];
                ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);

                // Description
                StringCopy(gStringVar1, gSpeciesNames[gBattleMons[gWishFutureKnock.futureSightAttacker[sMenuDataPtr->battlerId]].species]);
                StringCopy(gStringVar2, gMoveNames[gWishFutureKnock.futureSightMove[sMenuDataPtr->battlerId]]);
                ConvertIntToDecimalStringN(gStringVar3, gWishFutureKnock.futureSightPower[sMenuDataPtr->battlerId], STR_CONV_MODE_LEFT_ALIGN, 4);
                StringExpandPlaceholders(gStringVar4, sText_Title_Status_IncomingAttack_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar4);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_UPROAR:
                StringCopy(gStringVar1, sText_Title_Status_Uproar);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                /*//Turns Left
                StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2), (y * 8) + y2, 0, 0,
                sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1); turnsLeft = gBattleMons[sMenuDataPtr->battlerId].status2 - STATUS2_UPROAR;
                ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3), (y * 8) + y2, 0, 0,
                sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);*/

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Uproar_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_BIDE:
                StringCopy(gStringVar1, sText_Title_Status_Bide);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Turns Left
                StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);
                turnsLeft = gBattleMons[sMenuDataPtr->battlerId].status2 - STATUS2_BIDE;
                ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Bide_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_INFATUATION: {
                for (i = 0; i < gBattlersCount; i++) {
                    if ((gBattleMons[sMenuDataPtr->battlerId].status2 & STATUS2_INFATUATION) &&
                        (gBattleMons[sMenuDataPtr->battlerId].status2 & STATUS2_INFATUATED_WITH(i)) && i != sMenuDataPtr->battlerId) {
                        gBattleScripting.battler = i;
                        break;
                    }
                }

                BattleStringExpandPlaceholders(sText_Title_Status_Infuation, gStringVar4);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar4);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Infuation_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
            } break;
            case STATUS_INFO_FOCUS_ENERGY:
                StringCopy(gStringVar1, sText_Title_Status_Focus_Energy);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Focus_Energy_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_DRAGON_CHEER:
                StringCopy(gStringVar1, sText_Title_Status_Dragon_Cheer);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Dragon_Cheer_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_TRANSFORMED: {
                SpeciesEnum species = gBattleMons[sMenuDataPtr->battlerId].species;

                StringCopy(gStringVar1, gSpeciesNames[species]);
                StringExpandPlaceholders(gStringVar4, sText_Title_Status_Transformed);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar4);
                // Description
                StringCopy(gStringVar1, sText_Title_Status_Transformed_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
            } break;
            case STATUS_INFO_ESCAPE_PREVENTION:
                StringCopy(gStringVar1, sText_Title_Status_Escape_Prevention);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Escape_Prevention_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_CURSED:
                StringCopy(gStringVar1, sText_Title_Status_Cursed);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Cursed_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_FORESIGHT:
                StringCopy(gStringVar1, sText_Title_Status_Foresight);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Foresight_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_DEFENSE_CURL:
                StringCopy(gStringVar1, sText_Title_Status_Defense_Curl);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Defense_Curl_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_TORMENT:
                StringCopy(gStringVar1, sText_Title_Status_Torment);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Torment_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_LEECHSEED: {
                u8 seedUser = gStatuses3[sMenuDataPtr->battlerId] & STATUS3_LEECHSEED_BATTLER;
                SpeciesEnum species = gBattleMons[seedUser].species;

                StringCopy(gStringVar1, gSpeciesNames[species]);
                StringExpandPlaceholders(gStringVar4, sText_Title_Status_Leech_Seed_Target);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar4);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Leech_Seed_Target_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
            } break;
            case STATUS_INFO_PERISH_SONG:
                StringCopy(gStringVar1, sText_Title_Status_Perish_Song);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Turns Left
                StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);
                turnsLeft = gVolatileStructs[sMenuDataPtr->battlerId].perishSongTimer;
                ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Perish_Song_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_MINIMIZED:
                StringCopy(gStringVar1, sText_Title_Status_Minimize);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Minimize_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_CHARGED_UP:
                StringCopy(gStringVar1, sText_Title_Status_Charge);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Turns Left
                StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);
                turnsLeft = gVolatileStructs[sMenuDataPtr->battlerId].perishSongTimer;
                ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Charge_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_ROOTED:
                StringCopy(gStringVar1, sText_Title_Status_Rooted);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Rooted_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_YAWN:
                StringCopy(gStringVar1, sText_Title_Status_Yawn);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Turns Left
                StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);
                turnsLeft = 1 + (gStatuses3[sMenuDataPtr->battlerId] & STATUS3_YAWN_TURN(0));
                ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Yawn_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_GRUDGE:
                StringCopy(gStringVar1, sText_Title_Status_Grudge);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Grudge_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_CANT_CRIT:
                StringCopy(gStringVar1, sText_Title_Status_Cant_Crit);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Cant_Crit_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_GASTRO_ACID:
                StringCopy(gStringVar1, sText_Title_Status_Gastro_Acid);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Gastro_Acid_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_EMBARGO:
                StringCopy(gStringVar1, sText_Title_Status_Embargo);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Turns Left
                StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);
                turnsLeft = gVolatileStructs[sMenuDataPtr->battlerId].embargoTimer;
                ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Embargo_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_SMACKED_DOWN:
                StringCopy(gStringVar1, sText_Title_Status_Smack_Down);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Smack_Down_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_MIRACLE_EYED:
                StringCopy(gStringVar1, sText_Title_Status_Miracle_Eye);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Miracle_Eye_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_HEAL_BLOCKED:
                StringCopy(gStringVar1, sText_Title_Status_Heal_Block);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Heal_Block_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_AQUA_RING:
                StringCopy(gStringVar1, sText_Title_Status_Aqua_Ring);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Aqua_Ring_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_MAGNET_RISE:
                StringCopy(gStringVar1, sText_Title_Status_Magnet_Rise);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Turns Left
                StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);
                turnsLeft = gVolatileStructs[sMenuDataPtr->battlerId].magnetRiseTimer;
                ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Magnet_Rise_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_DAZED:
                StringCopy(gStringVar1, sText_Title_Status_Dazed);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Turns Left
                StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);
                turnsLeft = gVolatileStructs[sMenuDataPtr->battlerId].dazed;
                ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Dazed_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_TREPIDATION:
                StringCopy(gStringVar1, sText_Title_Status_Trepidation);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Turns Left
                StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);
                turnsLeft = gVolatileStructs[sMenuDataPtr->battlerId].trepidation;
                ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Trepidation_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_STOCKPILE:
                StringCopy(gStringVar1, sText_Title_Status_Stockpile);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Turns Left
                StringCopy(gStringVar1, sText_Title_Field_Charges);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);
                turnsLeft = gVolatileStructs[sMenuDataPtr->battlerId].stockpileCounter;
                ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Stockpile_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_SOOTHSAYER:
                StringCopy(gStringVar1, sText_Title_Status_Soothsayer);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Turns Left
                StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);
                turnsLeft = GetAbilityState(sMenuDataPtr->battlerId, ABILITY_SOOTHSAYER);
                ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Soothsayer_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_SEMI_INVULNERABLE:
                StringCopy(gStringVar1, sText_Title_Status_Semi_Invulnerable);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Semi_Invulnerable_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_ELECTRIFIED:
                StringCopy(gStringVar1, sText_Title_Status_Electrified);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Electrified_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_PLASMA_FIST:
                StringCopy(gStringVar1, sText_Title_Status_Plasma_Fist);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Plasma_Fist_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_COILED:
                StringCopy(gStringVar1, sText_Title_Status_Coiled_Up);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Coiled_Up_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_CUTTHROAT:
                StringCopy(gStringVar1, sText_Title_Status_Cutthroat);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Cutthroat_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_PROTOSYNTHESIS:
                StringCopy(gStringVar1, sText_Title_Status_Protosynthesis);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                {
                    u8 stat = GetAbilityStateAs(sMenuDataPtr->battlerId, ABILITY_PROTOSYNTHESIS).paradoxBoost.statId;
                    StringCopy(gStringVar2, gStatNamesTable[stat]);
                    StringCopy(gStringVar3, stat == STAT_SPEED ? sText_Paradox_Speed_Value : sText_Paradox_Other_Value);
                }
                StringExpandPlaceholders(gStringVar1, sText_Title_Status_Paradox_Boost_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_QUARK_DRIVE:
                StringCopy(gStringVar1, sText_Title_Status_Quark_Drive);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                {
                    u8 stat = GetAbilityStateAs(sMenuDataPtr->battlerId, ABILITY_QUARK_DRIVE).paradoxBoost.statId;
                    StringCopy(gStringVar2, gStatNamesTable[stat]);
                    StringCopy(gStringVar3, stat == STAT_SPEED ? sText_Paradox_Speed_Value : sText_Paradox_Other_Value);
                }
                StringExpandPlaceholders(gStringVar1, sText_Title_Status_Paradox_Boost_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_GHASTLY_ECHO:
                StringCopy(gStringVar1, sText_Title_Status_Ghastly_Echo);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Ghastly_Echo_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_COMMANDED:
                StringCopy(gStringVar1, sText_Title_Status_Commanded);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Commanded_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_VIOLENT_RUSH:
                StringCopy(gStringVar1, sText_Title_Status_Violent_Rush);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Violent_Rush_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_RAPID_RESPONSE:
                StringCopy(gStringVar1, sText_Title_Status_Rapid_Response);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Rapid_Response_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_READIED_ACTION:
                StringCopy(gStringVar1, sText_Title_Status_Readied_Action);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Readied_Action_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_SHOWDOWN_MODE:
                StringCopy(gStringVar1, sText_Title_Status_Showdown_Mode);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Showdown_Mode_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_WRAPPED:
                StringCopy(gStringVar1, sText_Title_Status_Trapped);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Trapped_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_PARASITIC_SPORES:
                StringCopy(gStringVar1, sText_Title_Status_Parasitic_Spores);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Parasitic_Spores_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_FEAR:
                StringCopy(gStringVar1, sText_Title_Status_Fear);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Fear_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_ON_THE_PROWL:
                StringCopy(gStringVar1, sText_Title_Status_OnTheProwl);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_OnTheProwl_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_DRENCHED:
                StringCopy(gStringVar1, sText_Title_Status_Drenched);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_Drenched_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_ICE_STATUE:
                StringCopy(gStringVar1, sText_Title_Status_IceStatue);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Status_IceStatue_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_EXTRA_ATTACK:
                StringCopy(gStringVar2, gStatNamesTable[STAT_ATK]);
                StringExpandPlaceholders(gStringVar1, sText_Title_Status_ExtraStat);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                {
                    ConvertIntToDecimalStringN(gStringVar3, gVolatileStructs[sMenuDataPtr->battlerId].extraAttackLevel * 20, STR_CONV_MODE_LEFT_ALIGN, 3);
                }
                StringExpandPlaceholders(gStringVar1, sText_Title_Status_ExtraStat_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_EXTRA_DEFENSE:
                StringCopy(gStringVar2, gStatNamesTable[STAT_DEF]);
                StringExpandPlaceholders(gStringVar1, sText_Title_Status_ExtraStat);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                {
                    ConvertIntToDecimalStringN(gStringVar3, gVolatileStructs[sMenuDataPtr->battlerId].extraDefenseLevel * 20, STR_CONV_MODE_LEFT_ALIGN, 3);
                }
                StringExpandPlaceholders(gStringVar1, sText_Title_Status_ExtraStat_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_EXTRA_SPATTACK:
                StringCopy(gStringVar2, gStatNamesTable[STAT_SPATK]);
                StringExpandPlaceholders(gStringVar1, sText_Title_Status_ExtraStat);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                {
                    ConvertIntToDecimalStringN(gStringVar3, gVolatileStructs[sMenuDataPtr->battlerId].extraSpAttackLevel * 20, STR_CONV_MODE_LEFT_ALIGN, 3);
                }
                StringExpandPlaceholders(gStringVar1, sText_Title_Status_ExtraStat_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_EXTRA_SPDEFENSE:
                StringCopy(gStringVar2, gStatNamesTable[STAT_SPDEF]);
                StringExpandPlaceholders(gStringVar1, sText_Title_Status_ExtraStat);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                {
                    ConvertIntToDecimalStringN(gStringVar3, gVolatileStructs[sMenuDataPtr->battlerId].extraSpDefenseLevel * 20, STR_CONV_MODE_LEFT_ALIGN, 3);
                }
                StringExpandPlaceholders(gStringVar1, sText_Title_Status_ExtraStat_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
            case STATUS_INFO_EXTRA_SPEED:
                StringCopy(gStringVar2, gStatNamesTable[STAT_SPEED]);
                StringExpandPlaceholders(gStringVar1, sText_Title_Status_ExtraStat);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                {
                    ConvertIntToDecimalStringN(gStringVar3, gVolatileStructs[sMenuDataPtr->battlerId].extraSpeedLevel * 20, STR_CONV_MODE_LEFT_ALIGN, 3);
                }
                StringExpandPlaceholders(gStringVar1, sText_Title_Status_ExtraStat_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                printedInfo = TRUE;
                break;
        }
        if (printedInfo) y = y + MAX_DESCRIPTION_LINES + 2;
    }

    PutWindowTilemap(windowId);
    CopyWindowToVram(windowId, 3);
}

static void PrintDamageCalulatorTab(void) {
    u8 i;
    u8 x, y, x2, y2;
    u8 windowId = WINDOW_1;
    u8 colorIdx = FONT_BLACK;
    u16 moves[MAX_MON_MOVES] = {gBattleMons[sMenuDataPtr->battlerId].moves[0],
                                gBattleMons[sMenuDataPtr->battlerId].moves[1],
                                gBattleMons[sMenuDataPtr->battlerId].moves[2],
                                gBattleMons[sMenuDataPtr->battlerId].moves[3]};
    u8 target = BATTLE_OPPOSITE(sMenuDataPtr->battlerId);

    if (sMenuDataPtr->dmgCalculatorTarget != 0 && sMenuDataPtr->isDoubleBattle) target = BATTLE_OPPOSITE(BATTLE_PARTNER(sMenuDataPtr->battlerId));

    FillWindowPixelBuffer(windowId, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));

    // Title
    x = 9;
    y = 0;
    x2 = 0;
    y2 = 0;
    AddTextPrinterParameterized4(windowId, FONT_SMALL, (x * 8), (y * 8), 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, sText_Title_Battler_Damage);
    x = 20;
    AddTextPrinterParameterized4(windowId, FONT_SMALL, (x * 8), (y * 8), 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, sText_Title_Controllers);

    // First Move
    x = 9;
    y = 3;
    x2 = 0;
    y2 = -4;
    for (i = 0; i < MAX_MON_MOVES; i++) {
        // Move Name
        AddTextPrinterParameterized4(
            windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gMoveNamesLong[moves[i]]);

        // Move Damage ---------------------------------------------------------------------------------------------------
        y++;
        if (gBattleMoves[moves[i]].split == SPLIT_STATUS || gBattleMoves[moves[i]].power <= 2) {
            StringCopy(gStringVar4, gText_Target_Nothing);
        } else {
            PrintDamageCalculation(sMenuDataPtr->battlerId, target, i);
        }
        AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar4);
        y = y + 3;
    }

    PutWindowTilemap(windowId);
    CopyWindowToVram(windowId, 3);
}

#define HP_EV_INDEX 0
#define ATK_EV_INDEX 1
#define DEF_EV_INDEX 2
#define SPEED_EV_INDEX 3
#define SPATK_EV_INDEX 4
#define SPDEF_EV_INDEX 5

const u8 gText_SmogonDamageCalculator_FirstPartText_SpecialDefense[] = _("{STR_VAR_1} 特攻 {STR_VAR_2} {STR_VAR_3} vs.");
const u8 gText_SmogonDamageCalculator_FirstPartText_SpecialDefense_NatureUp[] = _("{STR_VAR_1} 特攻+ {STR_VAR_2} {STR_VAR_3} vs.");
const u8 gText_SmogonDamageCalculator_FirstPartText_SpecialDefense_NatureDown[] = _("{STR_VAR_1} 特攻- {STR_VAR_2} {STR_VAR_3} vs.");
const u8 gText_SmogonDamageCalculator_FirstPartText_PhysicalDefense[] = _("{STR_VAR_1} 攻击 {STR_VAR_2} {STR_VAR_3} vs.");
const u8 gText_SmogonDamageCalculator_FirstPartText_PhysicalDefense_NatureUp[] = _("{STR_VAR_1} 攻击+ {STR_VAR_2} {STR_VAR_3} vs.");
const u8 gText_SmogonDamageCalculator_FirstPartText_PhysicalDefense_NatureDown[] = _("{STR_VAR_1} 攻击- {STR_VAR_2} {STR_VAR_3} vs.");
//{STR_VAR_1}+ = Special Attack EV, {STR_VAR_2} = Pokemon Name, {STR_VAR_3} = Move Name
const u8 gText_SmogonDamageCalculator_SecondPartText_SpecialDefense[] = _("{STR_VAR_1}\n{STR_VAR_2} HP/ {STR_VAR_3} 特防");
const u8 gText_SmogonDamageCalculator_SecondPartText_SpecialDefense_NatureUp[] = _("{STR_VAR_1}\n{STR_VAR_2} HP/ {STR_VAR_3} 特防+");
const u8 gText_SmogonDamageCalculator_SecondPartText_SpecialDefense_NatureDown[] = _("{STR_VAR_1}\n{STR_VAR_2} HP/ {STR_VAR_3} 特防-");
const u8 gText_SmogonDamageCalculator_SecondPartText_PhysicalDefense[] = _("{STR_VAR_1}\n{STR_VAR_2} HP/ {STR_VAR_3} 防御");
const u8 gText_SmogonDamageCalculator_SecondPartText_PhysicalDefense_NatureUp[] = _("{STR_VAR_1}\n{STR_VAR_2} HP/ {STR_VAR_3} 防御+");
const u8 gText_SmogonDamageCalculator_SecondPartText_PhysicalDefense_NatureDown[] = _("{STR_VAR_1}\n{STR_VAR_2} HP/ {STR_VAR_3} 防御-");
//{STR_VAR_1} = First Part {STR_VAR_2} = HP EV, {STR_VAR_3} = Special Defense EV
const u8 gText_SmogonDamageCalculator_ThirdPart[] = _("{STR_VAR_1} {STR_VAR_2}：{STR_VAR_3}");
//{STR_VAR_1} = Second Part {STR_VAR_2} = Target Name, {STR_VAR_3} = Min Damage
const u8 gText_SmogonDamageCalculator_FourthPart[] = _("{STR_VAR_1}-{STR_VAR_2}\n({STR_VAR_3} -");
//{STR_VAR_1} = Third Part {STR_VAR_2} = Max Damage, {STR_VAR_3} = Min Percent
const u8 gText_SmogonDamageCalculator_FifthPart[] = _("{STR_VAR_1} {STR_VAR_2}%) -- {STR_VAR_3}%几率");
const u8 gText_SmogonDamageCalculator_FifthPart_Guaranteed[] = _("{STR_VAR_1} {STR_VAR_2}%) -- 必定");
//{STR_VAR_1} = Foruth Part {STR_VAR_2} = Max Percent, {STR_VAR_3} = Chance
const u8 gText_SmogonDamageCalculator_SixthPart[] = _("{STR_VAR_1} {STR_VAR_2}HKO");
const u8 gText_SmogonDamageCalculator_SixthPart_OHKO[] = _("{STR_VAR_1} 一击必杀");
const u8 gText_SmogonDamageCalculator_SixthPart_OHKO_Focus_Sash[] = _("{STR_VAR_1} 两击必杀-");
//{STR_VAR_1} = Fifth Part {STR_VAR_2} = 2HKO or 3HKO
const u8 gText_SmogonDamageCalculator_FastPart[] = _("{STR_VAR_1}% 几率{STR_VAR_2}击必杀");
const u8 gText_SmogonDamageCalculator_FastPart_Guaranteed[] = _("必定{STR_VAR_2}击必杀");

#define MAX_DAMAGE_FACTOR 0
#define MIN_DAMAGE_FACTOR 16
#define MAX_PERCENT 100
#define MAX_PERCENT_2 10000

// Rough probability of an N hit KO needing an average roll
static const u8 sConvolutionTable[][MIN_DAMAGE_FACTOR] = {
    {100, 97, 92, 86, 79, 70, 59, 47, 36, 26, 18, 11, 6, 3, 1, 1},  // N = 2
    {100, 99, 96, 92, 84, 73, 60, 46, 32, 20, 12, 6, 3, 1, 1, 1},
    {100, 100, 98, 95, 87, 76, 61, 44, 29, 16, 8, 3, 1, 1, 1, 1},
    {100, 100, 99, 96, 90, 78, 62, 43, 26, 13, 6, 2, 1, 1, 1, 1},
};

static void CalculateDamage(u8 battler, u8 target, u8 moveIndex) {
    u32 minDamage, maxDamage, tempdamage;
    s32 minHits2KOChance;
    u32 hits2KO, percentage;
    u8 moveType;
    u8 natureAtk, natureDef;
    u8 i;
    struct Pokemon *party, *targetParty;
    u16 targetCurrentHp = gBattleMons[target].hp;
    MoveEnum move = gBattleMons[battler].moves[moveIndex];
    const s8* natureMod;
    int ignored, immune;

    if (sMenuDataPtr->damageCalculation[battler][target][moveIndex].calculated) return;

    sMenuDataPtr->damageCalculation[battler][target][moveIndex].calculated = TRUE;

    SetMoldBreaker(battler, move);

    // Sets move type depending on the mon ability/stats
    SetTypeBeforeUsingMove(move, battler);
    GET_MOVE_TYPE(move, moveType);

    // Max and Min Damage
    minDamage = DoMoveDamageCalcBattleMenu(move, battler, target, &moveType, CRIT_ROLL_ONLY_IF_GUARANTEED, MIN_DAMAGE_FACTOR, (u16*)&ignored);
    maxDamage = DoMoveDamageCalcBattleMenu(move, battler, target, &moveType, CRIT_ROLL_ONLY_IF_GUARANTEED, MAX_DAMAGE_FACTOR, (u16*)&ignored);

    immune = TestImmunityAbilitiesOnly(target, battler, move, moveType);
    if (!immune) immune = TestAbsorbingAbilitiesOnly(target, battler, move, moveType);
    if (immune) minDamage = maxDamage = 0;

    sMenuDataPtr->damageCalculation[battler][target][moveIndex].minDamage = minDamage;
    sMenuDataPtr->damageCalculation[battler][target][moveIndex].maxDamage = maxDamage;

    if (!IsBattlerAlive(battler) || !IsBattlerAlive(target) || gBattleMoves[move].split == SPLIT_STATUS || gBattleMoves[move].power <= 2 ||
        sMenuDataPtr->damageCalculation[battler][target][moveIndex].maxDamage == 0) {
        sMenuDataPtr->damageCalculation[battler][target][moveIndex].noDamage = TRUE;
        return;
    }

    // Min Damage Percentage
    percentage = (minDamage * MAX_PERCENT_2) / targetCurrentHp;
    percentage = (percentage / MAX_PERCENT);
    if (percentage >= MAX_PERCENT) percentage = MAX_PERCENT;
    sMenuDataPtr->damageCalculation[battler][target][moveIndex].minDamagePercentage = percentage;

    // Max Damage Percentage
    percentage = (maxDamage * MAX_PERCENT_2) / targetCurrentHp;
    percentage = (percentage / MAX_PERCENT);
    if (percentage > MAX_PERCENT) percentage = MAX_PERCENT;
    sMenuDataPtr->damageCalculation[battler][target][moveIndex].maxDamagePercentage = percentage;

    // Chances To KO
    hits2KO = sMenuDataPtr->damageCalculation[battler][target][moveIndex].hits2KO =
        (targetCurrentHp / sMenuDataPtr->damageCalculation[battler][target][moveIndex].maxDamage);
    hits2KO++;

    for (i = 0; i < MIN_DAMAGE_FACTOR; i++) {
        tempdamage = DoMoveDamageCalcBattleMenu(move, battler, target, &moveType, CRIT_ROLL_ONLY_IF_GUARANTEED, MIN_DAMAGE_FACTOR - i, (u16*)&ignored);

        if (tempdamage * hits2KO >= targetCurrentHp) {
            break;
        }
    }

    if (hits2KO <= 1) {
        minHits2KOChance = (MIN_DAMAGE_FACTOR - i) * MAX_PERCENT_2 / MIN_DAMAGE_FACTOR / MAX_PERCENT;
    } else {
        if (hits2KO > ARRAY_COUNT(sConvolutionTable) + 1) hits2KO = ARRAY_COUNT(sConvolutionTable) + 1;
        minHits2KOChance = sConvolutionTable[hits2KO - 2][i];
    }
    sMenuDataPtr->damageCalculation[battler][target][moveIndex].chance2KO = minHits2KOChance;

    // Nature Stuff
    if (GetBattlerSide(battler) == B_SIDE_PLAYER)
        party = gPlayerParty;
    else
        party = gEnemyParty;

    if (GetBattlerSide(target) == B_SIDE_PLAYER)
        targetParty = gPlayerParty;
    else
        targetParty = gEnemyParty;

    natureAtk = GetMonData(&party[gBattlerPartyIndexes[battler]], MON_DATA_NATURE, NULL);
    natureDef = GetMonData(&targetParty[gBattlerPartyIndexes[target]], MON_DATA_NATURE, NULL);

    // Attacker
    if (natureAtk == NATURE_HARDY || natureAtk == NATURE_DOCILE || natureAtk == NATURE_SERIOUS || natureAtk == NATURE_BASHFUL || natureAtk == NATURE_QUIRKY) {
        sMenuDataPtr->damageCalculation[battler][target][moveIndex].attackerNatureUp = NUM_NATURE_STATS;
        sMenuDataPtr->damageCalculation[battler][target][moveIndex].attackerNatureDown = NUM_NATURE_STATS;
    } else {
        natureMod = gNatureStatTable[natureAtk];
        for (i = 0; i < NUM_NATURE_STATS; i++) {
            if (natureMod[i] > 0)
                sMenuDataPtr->damageCalculation[battler][target][moveIndex].attackerNatureUp = i + 1;  // Nature Up
            else if (natureMod[i] < 0)
                sMenuDataPtr->damageCalculation[battler][target][moveIndex].attackerNatureDown = i + 1;  // Nature Down
        }
    }

    // Target
    if (natureDef == NATURE_HARDY || natureDef == NATURE_DOCILE || natureDef == NATURE_SERIOUS || natureDef == NATURE_BASHFUL || natureDef == NATURE_QUIRKY) {
        sMenuDataPtr->damageCalculation[battler][target][moveIndex].targetNatureUp = NUM_NATURE_STATS;
        sMenuDataPtr->damageCalculation[battler][target][moveIndex].targetNatureDown = NUM_NATURE_STATS;
    } else {
        natureMod = gNatureStatTable[natureDef];
        for (i = 0; i < NUM_NATURE_STATS; i++) {
            if (natureMod[i] > 0)
                sMenuDataPtr->damageCalculation[battler][target][moveIndex].targetNatureUp = i + 1;  // Nature Up
            else if (natureMod[i] < 0)
                sMenuDataPtr->damageCalculation[battler][target][moveIndex].targetNatureDown = i + 1;  // Nature Down
        }
    }

    // Evs
    if (GetBattlerSide(battler) == B_SIDE_PLAYER)
        party = gPlayerParty;
    else
        party = gEnemyParty;

    if (GetBattlerSide(target) == B_SIDE_PLAYER)
        targetParty = gPlayerParty;
    else
        targetParty = gEnemyParty;

    for (i = 0; i < NUM_STATS; i++) {
        sMenuDataPtr->damageCalculation[battler][target][moveIndex].battlerEvs[i] = GetMonData(&party[gBattlerPartyIndexes[battler]], MON_DATA_HP_EV + i, NULL);
        sMenuDataPtr->damageCalculation[battler][target][moveIndex].targetEvs[i] =
            GetMonData(&targetParty[gBattlerPartyIndexes[target]], MON_DATA_HP_EV + i, NULL);
    }
}

void PrintDamageCalculation(u8 battler, u8 target, u8 moveIdx) {
    u32 minDamage, maxDamage;
    u16 targetMaxHp = gBattleMons[target].maxHP;
    u16 targetCurrentHp = gBattleMons[target].hp;
    u16 targetHeldItem = gBattleMons[target].item;
    MoveEnum move = gBattleMons[battler].moves[moveIdx];
    struct DamageCalculation* damageCalculation;

    // Sets move type depending on the mon ability/stats
    SetTypeBeforeUsingMove(move, battler);

    if (sMenuDataPtr->damageCalculation[battler][target][moveIdx].noDamage) {
        StringCopy(gStringVar4, gText_Target_Nothing);
        return;
    }

    CalculateDamage(battler, target, moveIdx);
    damageCalculation = &sMenuDataPtr->damageCalculation[battler][target][moveIdx];

    // 252+ SpA Abomasnow Blizzard vs. 168 HP / 0 SpD Abomasnow: 178-211 (49 - 58.1%) -- 97.7% chance to 2HKO

    // Damage Calculation
    minDamage = damageCalculation->minDamage;
    maxDamage = damageCalculation->maxDamage;

    // First Part
    StringCopy(gStringVar2, gSpeciesNames[gBattleMons[battler].species]);
    StringCopy(gStringVar3, gMoveNames[move]);
    if (gBattleMoves[move].split == SPLIT_SPECIAL) {
        ConvertIntToDecimalStringN(gStringVar1, damageCalculation->battlerEvs[SPATK_EV_INDEX], STR_CONV_MODE_LEFT_ALIGN, 3);
        if (damageCalculation->attackerNatureDown == STAT_SPATK)
            StringExpandPlaceholders(gStringVar4, gText_SmogonDamageCalculator_FirstPartText_SpecialDefense_NatureDown);
        else if (damageCalculation->attackerNatureUp == STAT_SPATK)
            StringExpandPlaceholders(gStringVar4, gText_SmogonDamageCalculator_FirstPartText_SpecialDefense_NatureUp);
        else
            StringExpandPlaceholders(gStringVar4, gText_SmogonDamageCalculator_FirstPartText_SpecialDefense);
    } else {
        ConvertIntToDecimalStringN(gStringVar1, damageCalculation->battlerEvs[ATK_EV_INDEX], STR_CONV_MODE_LEFT_ALIGN, 3);
        if (damageCalculation->attackerNatureDown == STAT_ATK)
            StringExpandPlaceholders(gStringVar4, gText_SmogonDamageCalculator_FirstPartText_PhysicalDefense_NatureDown);
        else if (damageCalculation->attackerNatureUp == STAT_ATK)
            StringExpandPlaceholders(gStringVar4, gText_SmogonDamageCalculator_FirstPartText_PhysicalDefense_NatureUp);
        else
            StringExpandPlaceholders(gStringVar4, gText_SmogonDamageCalculator_FirstPartText_PhysicalDefense);
    }

    // Second Part
    StringCopy(gStringVar1, gStringVar4);
    ConvertIntToDecimalStringN(gStringVar2, damageCalculation->targetEvs[HP_EV_INDEX], STR_CONV_MODE_LEFT_ALIGN, 3);
    if (gBattleMoves[move].split == SPLIT_SPECIAL) {
        ConvertIntToDecimalStringN(gStringVar3, damageCalculation->targetEvs[SPDEF_EV_INDEX], STR_CONV_MODE_LEFT_ALIGN, 3);
        if (damageCalculation->targetNatureDown == STAT_SPDEF)
            StringExpandPlaceholders(gStringVar4, gText_SmogonDamageCalculator_SecondPartText_SpecialDefense_NatureDown);
        else if (damageCalculation->targetNatureUp == STAT_SPDEF)
            StringExpandPlaceholders(gStringVar4, gText_SmogonDamageCalculator_SecondPartText_SpecialDefense_NatureUp);
        else
            StringExpandPlaceholders(gStringVar4, gText_SmogonDamageCalculator_SecondPartText_SpecialDefense);
    } else {
        ConvertIntToDecimalStringN(gStringVar3, damageCalculation->targetEvs[DEF_EV_INDEX], STR_CONV_MODE_LEFT_ALIGN, 3);
        if (damageCalculation->targetNatureDown == STAT_DEF)
            StringExpandPlaceholders(gStringVar4, gText_SmogonDamageCalculator_SecondPartText_PhysicalDefense_NatureDown);
        else if (damageCalculation->targetNatureUp == STAT_DEF)
            StringExpandPlaceholders(gStringVar4, gText_SmogonDamageCalculator_SecondPartText_PhysicalDefense_NatureUp);
        else
            StringExpandPlaceholders(gStringVar4, gText_SmogonDamageCalculator_SecondPartText_PhysicalDefense);
    }

    // Third Part
    StringCopy(gStringVar1, gStringVar4);
    StringCopy(gStringVar2, gSpeciesNames[gBattleMons[target].species]);
    ConvertIntToDecimalStringN(gStringVar3, minDamage, STR_CONV_MODE_LEFT_ALIGN, 4);
    StringExpandPlaceholders(gStringVar4, gText_SmogonDamageCalculator_ThirdPart);

    // Fourth Part gText_SmogonDamageCalculator_FourthPart
    StringCopy(gStringVar1, gStringVar4);
    ConvertIntToDecimalStringN(gStringVar2, maxDamage, STR_CONV_MODE_LEFT_ALIGN, 4);
    ConvertIntToDecimalStringN(gStringVar3, damageCalculation->minDamagePercentage, STR_CONV_MODE_LEFT_ALIGN, 3);
    StringExpandPlaceholders(gStringVar4, gText_SmogonDamageCalculator_FourthPart);

    // Fifth Part
    StringCopy(gStringVar1, gStringVar4);
    ConvertIntToDecimalStringN(gStringVar2, damageCalculation->maxDamagePercentage, STR_CONV_MODE_LEFT_ALIGN, 3);
    ConvertIntToDecimalStringN(gStringVar3, damageCalculation->chance2KO, STR_CONV_MODE_LEFT_ALIGN, 3);
    if (targetCurrentHp > minDamage)
        StringExpandPlaceholders(gStringVar4, gText_SmogonDamageCalculator_FifthPart);
    else
        StringExpandPlaceholders(gStringVar4, gText_SmogonDamageCalculator_FifthPart_Guaranteed);

    // Sixth Part
    StringCopy(gStringVar1, gStringVar4);
    if (damageCalculation->hits2KO > 0) {
        ConvertIntToDecimalStringN(gStringVar2, (damageCalculation->hits2KO + 1), STR_CONV_MODE_LEFT_ALIGN, 3);
        StringExpandPlaceholders(gStringVar4, gText_SmogonDamageCalculator_SixthPart);
    } else if (targetHeldItem == ITEM_FOCUS_SASH && targetCurrentHp == targetMaxHp)
        StringExpandPlaceholders(gStringVar4, gText_SmogonDamageCalculator_SixthPart_OHKO_Focus_Sash);
    else
        StringExpandPlaceholders(gStringVar4, gText_SmogonDamageCalculator_SixthPart_OHKO);
}

void PrintDamageCalculationExported(u8 battler, u8 target, u8 moveIdx) {
    u32 minDamage, maxDamage;
    MoveEnum move = gBattleMons[battler].moves[moveIdx];
    struct DamageCalculation* damageCalculation;

    // Sets move type depending on the mon ability/stats
    SetTypeBeforeUsingMove(move, battler);

    CalculateDamage(battler, target, moveIdx);
    damageCalculation = &sMenuDataPtr->damageCalculation[battler][target][moveIdx];

    // Damage Calculation
    minDamage = damageCalculation->minDamagePercentage;
    maxDamage = damageCalculation->maxDamagePercentage;

    ConvertIntToDecimalStringN(gStringVar1, minDamage, STR_CONV_MODE_LEFT_ALIGN, 3);
    ConvertIntToDecimalStringN(gStringVar2, maxDamage, STR_CONV_MODE_LEFT_ALIGN, 3);
}

#define NUM_PARTY_ICONS_SHOWN 6
#define PARTY_POKEMON_ICON_X (11 * 8) + 4
#define PARTY_POKEMON_ICON_Y (4 * 8)
#define PARTY_POKEMON_ICON_Y_2 (4 * 8) + (10 * 8)
#define PARTY_POKEMON_SPACE_X (7 * 8)
#define PARTY_POKEMON_SPACE_Y (3 * 8)

static void PrintPartyTab() {
    u8 i;
    u8 x, y;
    u8 windowId = WINDOW_1;
    u8 partyIndex = sMenuDataPtr->partyMenuSelectorID_X + (sMenuDataPtr->partyMenuSelectorID_Y * 3);

    FillWindowPixelBuffer(windowId, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));

    // Title
    x = 9;
    y = 0;
    AddTextPrinterParameterized4(windowId, FONT_SMALL, (x * 8), (y * 8), 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, sText_Title_Field_Party);
    x = 15;
    if (sMenuDataPtr->partySelectorMode)
        AddTextPrinterParameterized4(windowId, FONT_SMALL, (x * 8), (y * 8), 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, sText_Title_PartyInfo);
    else
        AddTextPrinterParameterized4(windowId, FONT_SMALL, (x * 8), (y * 8), 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, sText_Title_PartyInfoSelect);

    if (!sMenuDataPtr->partyIconsCreated) {
        // Player Mon Icons
        for (i = 0; i < NUM_PARTY_ICONS_SHOWN; i++) {
            ShowSpeciesIconParty(i, FALSE, PARTY_POKEMON_ICON_X + ((i % 3) * PARTY_POKEMON_SPACE_X), PARTY_POKEMON_ICON_Y + ((i / 3) * PARTY_POKEMON_SPACE_Y));
        }

        // Enemy Mon Icons
        for (i = 0; i < NUM_PARTY_ICONS_SHOWN; i++) {
            ShowSpeciesIconParty(i, TRUE, PARTY_POKEMON_ICON_X + ((i % 3) * PARTY_POKEMON_SPACE_X), PARTY_POKEMON_ICON_Y_2 + ((i / 3) * PARTY_POKEMON_SPACE_Y));
        }

        sMenuDataPtr->partyIconsCreated = TRUE;
    }

    // Selector
    if (sMenuDataPtr->partySelectorMode) {
        x = 9 + ((partyIndex % 3) * 7);  // PARTY_POKEMON_ICON_X / 8

        if (partyIndex >= 6) {
            u8 enemyIndex = partyIndex - 6;
            y = 13 + ((enemyIndex / 3) * 3);  // PARTY_POKEMON_ICON_Y / 8
        } else
            y = 3 + ((partyIndex / 3) * 3);  // PARTY_POKEMON_ICON_Y / 8

        BlitBitmapToWindow(windowId, sSelector2, (x * 8), (y * 8), 40, 24);
    }

    PutWindowTilemap(windowId);
    CopyWindowToVram(windowId, 3);
}

// Weathers
const u8 sText_Title_Field_Weather[] = _("天气：{STR_VAR_1}");
const u8 sText_Title_Field_Weather_Rain[] = _("下雨");
const u8 sText_Title_Field_Weather_Sun[] = _("晴");
const u8 sText_Title_Field_Weather_Sandstorm[] = _("沙暴");
const u8 sText_Title_Field_Weather_Hail[] = _("冰雹");
const u8 sText_Title_Field_Weather_Snow[] = _("雪");
const u8 sText_Title_Field_Weather_Strong_Winds[] = _("强风");
const u8 sText_Title_Field_Weather_Rain_Primal[] = _("原始之雨");
const u8 sText_Title_Field_Weather_Sun_Primal[] = _("原始之阳");
const u8 sText_Title_Field_Weather_EerieFog[] = _("诡异迷雾");

const u8 sText_Title_Field_Weather_Description_Rain[] =
    _("水属性招式威力提升50%，\n同时火属性招式\n威力降低50%。");
const u8 sText_Title_Field_Weather_Description_Sun[] =
    _("火属性招式威力提升50%，\n同时水属性招式\n威力降低50%。");
const u8 sText_Title_Field_Weather_Description_Sandstorm[] =
    _("非岩石、地面、钢属性的\n宝可梦每回合损失\n最大HP的1/16。");
const u8 sText_Title_Field_Weather_Description_Hail[] =
    _("非冰属性的宝可梦\n每回合受损，冻伤几率\n提高一倍。");
const u8 sText_Title_Field_Weather_Description_Strong_Winds[] =
    _("使电、冰、岩石属性招式\n对飞行属性宝可梦\n造成普通伤害。");
const u8 sText_Title_Field_Weather_Description_Primal_Rain[] =
    _("提升水属性招式威力，\n并保护宝可梦免受\n火属性招式影响。");
const u8 sText_Title_Field_Weather_Description_Primal_Sun[] =
    _("提升火属性招式威力，\n并保护宝可梦免受\n水属性招式影响。");
const u8 sText_Title_Field_Weather_Description_EerieFog[] =
    _("幽灵属性宝可梦受到的伤害\n降低20%，非幽灵与超能\n属性的能力提升减少。");
// Terrains
const u8 sText_Title_Field_Terrain[] = _("{STR_VAR_1}场地");
const u8 sText_Title_Field_Terrain_Electric[] = _("电");
const u8 sText_Title_Field_Terrain_Psychic[] = _("超能力");
const u8 sText_Title_Field_Terrain_Misty[] = _("薄雾");
const u8 sText_Title_Field_Terrain_Grassy[] = _("青草");
const u8 sText_Title_Field_Terrain_Toxic[] = _("剧毒");

const u8 sText_Title_Field_Terrain_Description_Electric[] =
    _("地面上的宝可梦不会睡着。\n电属性招式\n威力提升。");
const u8 sText_Title_Field_Terrain_Description_Psychic[] =
    _("地面上的宝可梦不会被\n先制招式命中。超能属性\n招式威力提升。");
const u8 sText_Title_Field_Terrain_Description_Misty[] =
    _("地面上的宝可梦不会陷入\n异常状态。妖精属性\n招式威力提升。");
const u8 sText_Title_Field_Terrain_Description_Grassy[] =
    _("提升草属性招式威力，\n每回合为地面上所有宝可梦\n回复1/16 HP。");
const u8 sText_Title_Field_Terrain_Description_Toxic[] =
    _("提升毒属性招式威力，\n每回合对地面上易中毒的\n宝可梦造成1/16 HP伤害。");

// Rooms
const u8 sText_Title_Field_Trick_Room[] = _("戏法空间");
const u8 sText_Title_Field_Room_Description_Trick[] =
    _("行动顺序反转，较慢的\n宝可梦会先于较快的\n宝可梦行动。");
const u8 sText_Title_Field_Wonder_Room[] = _("奇异空间：");
const u8 sText_Title_Field_Room_Description_Wonder[] =
    _("交换所有宝可梦的攻击\n与特攻，并压制这些\n能力的变化。");
const u8 sText_Title_Field_Magic_Room[] = _("魔法空间：");
const u8 sText_Title_Field_Room_Description_Magic[] =
    _("阻止一切间接伤害，\n并禁用所有\n进化石。");
const u8 sText_Title_Field_Gravity[] = _("重力");
const u8 sText_Title_Field_Gravity_Description[] =
    _("所有招式的命中率\n乘以1.67，场上所有\n宝可梦变为接地状态。");
const u8 sText_Title_Field_Inverse_Room[] = _("反转空间：");
const u8 sText_Title_Field_Inverse_Description[] =
    _("属性相性反转。原本抵抗\n或免疫某属性的属性\n会变得被克制。");
// Other
const u8 sText_Title_Field_Fairy_Lock[] = _("妖精之锁");
const u8 sText_Title_Field_Fairy_Lock_Description[] =
    _("使所有宝可梦（除幽灵\n属性）在下一回合无法\n替换或逃走。");
const u8 sText_Title_Field_Mud_Sport[] = _("玩泥巴");
const u8 sText_Title_Field_Mud_Sport_Description[] =
    _("双方电属性招式的\n基础威力\n降低67%。");
const u8 sText_Title_Field_Water_Sport[] = _("玩水");
const u8 sText_Title_Field_Water_Sport_Description[] =
    _("双方火属性招式的\n基础威力\n降低67%。");
const u8 sText_Title_Field_Ion_Deluge[] = _("等离子浴");
const u8 sText_Title_Field_Ion_Deluge_Description[] =
    _("使所有一般属性招式\n变为电属性，\n包括变化招式。");
const u8 sText_Title_Field_Quash[] = _("延后");
const u8 sText_Title_Field_Quash_Description[] =
    _("在判定行动顺序时\n压制优先度、能力提升、\n特性与场地效果。");
const u8 sText_Title_Field_ClearSkies[] = _("万里晴空");
const u8 sText_Title_Field_ClearSkies_Description[] =
    _("无法设置标准天气，原始天气无效");
const u8 sText_Title_Field_No_Effect[] = _("场地无效果");
const u8 sText_Title_Field_No_Effect_Description[] = _("场地没有特殊效果。");
static void PrintFieldTab(void) {
    u8 i;
    u8 x, y, x2, y2;
    u8 windowId = WINDOW_1;
    u8 turnsLeft = 5;
    bool8 printedInfo = FALSE;
    u8 maxLines = 3;

    FillWindowPixelBuffer(windowId, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));

    // Title
    x = 9;
    y = 0;
    x2 = 0;
    y2 = -4;
    AddTextPrinterParameterized4(windowId, FONT_SMALL, (x * 8), (y * 8), 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, sText_Title_Field_Stats);
    x = 15;
    AddTextPrinterParameterized4(windowId, FONT_SMALL, (x * 8), (y * 8), 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, sText_Title_FieldControllers);

    // Weather
    x = 9;
    y = 3;
    x2 = 0;
    y2 = -4;

    if (sMenuDataPtr->numFields < maxLines) maxLines = sMenuDataPtr->numFields;

    for (i = 0; i < maxLines; i++) {
        printedInfo = FALSE;
        // switch (sMenuDataPtr->fieldInfo[(i ) % NUM_FIELD_INFO]) {
        switch (sMenuDataPtr->fieldInfo[(i + sMenuDataPtr->currentFieldInfo) % sMenuDataPtr->numFields]) {
            case FIELD_INFO_WEATHER:
                // Title
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, sText_Title_Field_Weather);
                if ((gBattleWeather & WEATHER_RAIN_ANY) && !(gBattleWeather & WEATHER_RAIN_PRIMAL))
                    StringCopy(gStringVar1, sText_Title_Field_Weather_Rain);
                else if ((gBattleWeather & WEATHER_SUN_ANY) && !(gBattleWeather & WEATHER_SUN_PRIMAL))
                    StringCopy(gStringVar1, sText_Title_Field_Weather_Sun);
                else if ((gBattleWeather & WEATHER_SANDSTORM_ANY))
                    StringCopy(gStringVar1, sText_Title_Field_Weather_Sandstorm);
                else if ((gBattleWeather & WEATHER_HAIL_ANY))
                    StringCopy(gStringVar1, sText_Title_Field_Weather_Hail);
                else if ((gBattleWeather & WEATHER_STRONG_WINDS))
                    StringCopy(gStringVar1, sText_Title_Field_Weather_Strong_Winds);
                else if ((gBattleWeather & WEATHER_RAIN_PRIMAL))
                    StringCopy(gStringVar1, sText_Title_Field_Weather_Rain_Primal);
                else if ((gBattleWeather & WEATHER_SUN_PRIMAL))
                    StringCopy(gStringVar1, sText_Title_Field_Weather_Sun_Primal);
                else if (gBattleWeather & WEATHER_FOG_ANY)
                    StringCopy(gStringVar1, sText_Title_Field_Weather_EerieFog);
                else
                    StringCopy(gStringVar1, sText_Title_Field_None);

                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + SPACE_BETWEEN_LINES_FIELD,
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);

                if ((gBattleWeather & WEATHER_ANY)) {
                    // Turns Left
                    if (!(gBattleWeather & WEATHER_PERMANENT) && !FlagGet(FLAG_PERMANENT_UNCHANGEABLE_WEATHER)) {
                        AddTextPrinterParameterized4(windowId,
                                                     FONT_SMALL_NARROW,
                                                     (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                                     (y * 8) + y2,
                                                     0,
                                                     0,
                                                     sMenuWindowFontColors[FONT_WHITE],
                                                     0xFF,
                                                     sText_Title_Field_Turns_Left);
                        ConvertIntToDecimalStringN(gStringVar1, gWishFutureKnock.weatherDuration, STR_CONV_MODE_LEFT_ALIGN, 4);
                        AddTextPrinterParameterized4(windowId,
                                                     FONT_SMALL_NARROW,
                                                     (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                                     (y * 8) + y2,
                                                     0,
                                                     0,
                                                     sMenuWindowFontColors[FONT_WHITE],
                                                     0xFF,
                                                     gStringVar1);
                    }

                    // Weather Description
                    if ((gBattleWeather & WEATHER_STRONG_WINDS))
                        StringCopy(gStringVar1, sText_Title_Field_Weather_Description_Strong_Winds);
                    else if ((gBattleWeather & WEATHER_RAIN_PRIMAL))
                        StringCopy(gStringVar1, sText_Title_Field_Weather_Description_Primal_Rain);
                    else if ((gBattleWeather & WEATHER_SUN_PRIMAL))
                        StringCopy(gStringVar1, sText_Title_Field_Weather_Description_Primal_Sun);
                    else if ((gBattleWeather & WEATHER_RAIN_ANY))
                        StringCopy(gStringVar1, sText_Title_Field_Weather_Description_Rain);
                    else if ((gBattleWeather & WEATHER_SUN_ANY))
                        StringCopy(gStringVar1, sText_Title_Field_Weather_Description_Sun);
                    else if ((gBattleWeather & WEATHER_SANDSTORM_ANY))
                        StringCopy(gStringVar1, sText_Title_Field_Weather_Description_Sandstorm);
                    else if ((gBattleWeather & WEATHER_HAIL_ANY))
                        StringCopy(gStringVar1, sText_Title_Field_Weather_Description_Hail);
                    else if (gBattleWeather & WEATHER_FOG_ANY)
                        StringCopy(gStringVar1, sText_Title_Field_Weather_Description_EerieFog);

                    AddTextPrinterParameterized4(
                        windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                }

                printedInfo = TRUE;
                break;
            case FIELD_INFO_TERRAIN:
                // Terrain
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, sText_Title_Field_Terrain);
                if (gFieldStatuses & STATUS_FIELD_ELECTRIC_TERRAIN)
                    StringCopy(gStringVar1, sText_Title_Field_Terrain_Electric);
                else if (gFieldStatuses & STATUS_FIELD_GRASSY_TERRAIN)
                    StringCopy(gStringVar1, sText_Title_Field_Terrain_Grassy);
                else if (gFieldStatuses & STATUS_FIELD_MISTY_TERRAIN)
                    StringCopy(gStringVar1, sText_Title_Field_Terrain_Misty);
                else if (gFieldStatuses & STATUS_FIELD_PSYCHIC_TERRAIN)
                    StringCopy(gStringVar1, sText_Title_Field_Terrain_Psychic);
                else if (gFieldStatuses & STATUS_FIELD_TOXIC_TERRAIN)
                    StringCopy(gStringVar1, sText_Title_Field_Terrain_Toxic);
                else
                    StringCopy(gStringVar1, sText_Title_Field_None);

                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + SPACE_BETWEEN_LINES_FIELD,
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);
                // Turns Left
                if (gFieldStatuses & STATUS_FIELD_TERRAIN_ANY) {
                    AddTextPrinterParameterized4(windowId,
                                                 FONT_SMALL_NARROW,
                                                 (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                                 (y * 8) + y2,
                                                 0,
                                                 0,
                                                 sMenuWindowFontColors[FONT_WHITE],
                                                 0xFF,
                                                 sText_Title_Field_Turns_Left);
                    ConvertIntToDecimalStringN(gStringVar1, gFieldTimers.terrainTimer, STR_CONV_MODE_LEFT_ALIGN, 4);
                    AddTextPrinterParameterized4(windowId,
                                                 FONT_SMALL_NARROW,
                                                 (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                                 (y * 8) + y2,
                                                 0,
                                                 0,
                                                 sMenuWindowFontColors[FONT_WHITE],
                                                 0xFF,
                                                 gStringVar1);
                    // Terrain Description
                    if ((gFieldStatuses & STATUS_FIELD_ELECTRIC_TERRAIN))
                        StringCopy(gStringVar1, sText_Title_Field_Terrain_Description_Electric);
                    else if ((gFieldStatuses & STATUS_FIELD_GRASSY_TERRAIN))
                        StringCopy(gStringVar1, sText_Title_Field_Terrain_Description_Grassy);
                    else if ((gFieldStatuses & STATUS_FIELD_MISTY_TERRAIN))
                        StringCopy(gStringVar1, sText_Title_Field_Terrain_Description_Misty);
                    else if ((gFieldStatuses & STATUS_FIELD_PSYCHIC_TERRAIN))
                        StringCopy(gStringVar1, sText_Title_Field_Terrain_Description_Psychic);
                    else if ((gFieldStatuses & STATUS_FIELD_TOXIC_TERRAIN))
                        StringCopy(gStringVar1, sText_Title_Field_Terrain_Description_Toxic);

                    AddTextPrinterParameterized4(
                        windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
                }

                printedInfo = TRUE;
                break;
            case FIELD_INFO_INVERSE_ROOM:
                StringCopy(gStringVar1, sText_Title_Field_Inverse_Room);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Turns Left
                StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);
                turnsLeft = gFieldTimers.inverseRoomTimer;
                ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Field_Inverse_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);

                printedInfo = TRUE;
                break;
            case FIELD_INFO_GRAVITY:
                StringCopy(gStringVar1, sText_Title_Field_Gravity);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Turns Left
                StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);
                turnsLeft = gFieldTimers.gravityTimer;
                ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Field_Gravity_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);

                printedInfo = TRUE;
                break;
            case FIELD_INFO_TRICK_ROOM:
                StringCopy(gStringVar1, sText_Title_Field_Trick_Room);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Turns Left
                turnsLeft = gFieldTimers.trickRoomTimer;
                if (turnsLeft < 10) {
                    StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                    AddTextPrinterParameterized4(windowId,
                                                 FONT_SMALL_NARROW,
                                                 (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                                 (y * 8) + y2,
                                                 0,
                                                 0,
                                                 sMenuWindowFontColors[FONT_WHITE],
                                                 0xFF,
                                                 gStringVar1);
                    ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                    AddTextPrinterParameterized4(windowId,
                                                 FONT_SMALL_NARROW,
                                                 (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                                 (y * 8) + y2,
                                                 0,
                                                 0,
                                                 sMenuWindowFontColors[FONT_WHITE],
                                                 0xFF,
                                                 gStringVar1);
                }

                // Description
                StringCopy(gStringVar1, sText_Title_Field_Room_Description_Trick);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);

                printedInfo = TRUE;
                break;
            case FIELD_INFO_WONDER_ROOM:
                StringCopy(gStringVar1, sText_Title_Field_Wonder_Room);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Turns Left
                StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);
                turnsLeft = gFieldTimers.wonderRoomTimer;
                ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Field_Room_Description_Wonder);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);

                printedInfo = TRUE;
                break;
            case FIELD_INFO_MAGIC_ROOM:
                StringCopy(gStringVar1, sText_Title_Field_Magic_Room);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Turns Left
                StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);
                turnsLeft = gFieldTimers.magicRoomTimer;
                ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Field_Room_Description_Magic);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);

                printedInfo = TRUE;
                break;
            case FIELD_INFO_FAIRY_LOCK:
                StringCopy(gStringVar1, sText_Title_Field_Fairy_Lock);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Turns Left
                StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);
                turnsLeft = gFieldTimers.fairyLockTimer;
                ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Field_Fairy_Lock_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);

                printedInfo = TRUE;
                break;
            case FIELD_INFO_MUD_SPORT:
                StringCopy(gStringVar1, sText_Title_Field_Mud_Sport);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Turns Left
                StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);
                turnsLeft = gFieldTimers.mudSportTimer;
                ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Field_Mud_Sport_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);

                printedInfo = TRUE;
                break;
            case FIELD_INFO_WATER_SPORT:
                StringCopy(gStringVar1, sText_Title_Field_Water_Sport);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Turns Left
                StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);
                turnsLeft = gFieldTimers.waterSportTimer;
                ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Field_Water_Sport_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);

                printedInfo = TRUE;
                break;
            case FIELD_INFO_ION_DELUGE:
                StringCopy(gStringVar1, sText_Title_Field_Ion_Deluge);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Turns Left
                /*StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2), (y * 8) + y2, 0, 0,
                sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1); turnsLeft = gFieldTimers.magicRoomTimer; ConvertIntToDecimalStringN(gStringVar1,
                turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4); AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD *
                3), (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);*/

                // Description
                StringCopy(gStringVar1, sText_Title_Field_Ion_Deluge_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);

                printedInfo = TRUE;
                break;
            case FIELD_INFO_QUASH:
                StringCopy(gStringVar1, sText_Title_Field_Quash);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Turns Left
                StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);
                turnsLeft = gFieldTimers.quashTimer;
                ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Field_Quash_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);

                printedInfo = TRUE;
                break;
            case FIELD_INFO_CLEAR_SKIES:
                StringCopy(gStringVar1, sText_Title_Field_ClearSkies);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Turns Left
                StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);
                turnsLeft = gFieldTimers.clearSkiesTimer;
                ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Field_ClearSkies_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);

                printedInfo = TRUE;
                break;
        }
        if (printedInfo) y = y + MAX_DESCRIPTION_LINES + 2;
    }

    if (sMenuDataPtr->numFields == 0) {
        StringCopy(gStringVar1, sText_Title_Field_No_Effect);
        AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

        // Description
        StringCopy(gStringVar1, sText_Title_Field_No_Effect_Description);
        AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
    }

    /*//Nature Power
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF,
    sText_Title_Field_Nature_Power); StringCopy(gStringVar1, gMoveNamesLong[MOVE_THUNDERBOLT]); AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x *
    8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 1.5), (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

    //Secret Power
    y++;
    StringCopy(gStringVar1, sText_Title_Field_Secret_Power);
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);
    StringCopy(gStringVar1, sText_Title_Field_Paralysis);
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 1.5), (y * 8) + y2, 0, 0,
    sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);
    */

    PutWindowTilemap(windowId);
    CopyWindowToVram(windowId, 3);
}

const u8 sText_Title_Side_Aurora_Veil[] = _("极光幕");
const u8 sText_Title_Side_Aurora_Veil_Description[] =
    _("使用者受到的物理与特殊\n招式伤害\n降低50%。");
const u8 sText_Title_Side_Reflect[] = _("反射壁");
const u8 sText_Title_Side_Reflect_Description[] =
    _("使用者受到的物理招式\n伤害降低50%，可被\n黑雾等招式清除。");
const u8 sText_Title_Side_Light_Screen[] = _("光墙");
const u8 sText_Title_Side_Light_Screen_Description[] =
    _("使用者受到的特殊招式\n伤害降低50%，可被\n除雾等招式清除。");
const u8 sText_Title_Side_Tailwind[] = _("顺风");
const u8 sText_Title_Side_Tailwind_Description[] =
    _("使用者及己方所有\n宝可梦的速度\n翻倍。");
const u8 sText_Title_Side_Spikes[] = _("撒菱");
const u8 sText_Title_Side_Spikes_1_Description[] =
    _("替换上场的宝可梦会损失\n最大HP的12.5%\n作为伤害。");
const u8 sText_Title_Side_Spikes_2_Description[] =
    _("替换上场的宝可梦会损失\n最大HP的16.67%\n作为伤害。");
const u8 sText_Title_Side_Spikes_3_Description[] =
    _("替换上场的宝可梦会损失\n最大HP的25%\n作为伤害。");
const u8 sText_Title_Side_Toxic_Spikes[] = _("毒菱");
const u8 sText_Title_Side_Toxic_Spikes_1_Description[] =
    _("替换上场的宝可梦陷入中毒状态");
const u8 sText_Title_Side_Toxic_Spikes_2_Description[] =
    _("替换上场的宝可梦陷入剧毒状态");
const u8 sText_Title_Side_Stealth_Rock[] = _("隐形岩");
const u8 sText_Title_Side_Stealth_Rock_Description[] =
    _("替换上场的宝可梦会受到伤害，\n数值取决于岩石属性\n对它的相性。");
const u8 sText_Title_Side_Creeping_Thorns[] = _("蔓延荆棘");
const u8 sText_Title_Side_Creeping_Thorns_Description[] =
    _("替换上场的宝可梦会受到伤害，\n数值取决于草属性\n对它的相性。");
const u8 sText_Title_Side_Sticky_Web[] = _("黏黏网");
const u8 sText_Title_Side_Sticky_Web_Description[] =
    _("替换上场的宝可梦\n速度\n降低一级。");
const u8 sText_Title_Side_Foamy_Web[] = _("泡沫网");
const u8 sText_Title_Side_Foamy_Web_Description[] =
    _("替换上场的宝可梦\n速度降低一级。\n无法被效果清除。");
const u8 sText_Title_Side_Safeguard[] = _("神秘守护");
const u8 sText_Title_Side_Safeguard_Description[] =
    _("防止中毒、麻痹、灼伤\n与睡眠等异常状态的\n保护场。");
const u8 sText_Title_Side_Mist[] = _("神秘守护");
const u8 sText_Title_Side_Mist_Description[] =
    _("白色迷雾，防止己方\n任何宝可梦的\n能力被降低。");
const u8 sText_Title_Side_Rainbow[] = _("彩虹");
const u8 sText_Title_Side_Rainbow_Description[] =
    _("绚丽的彩虹提升己方\n所有宝可梦的\n追加效果几率。");
const u8 sText_Title_Side_FireSea[] = _("火海");
const u8 sText_Title_Side_FireSea_Description[] =
    _("火海每回合对己方非火属性\n宝可梦造成最大HP的12.5%\n伤害。");
const u8 sText_Title_Side_Swamp[] = _("沼泽");
const u8 sText_Title_Side_Swamp_Description[] =
    _("深沼泽使己方地面上\n宝可梦的速度\n降低33%。");
const u8 sText_Title_Side_Smokescreen[] = _("烟幕");
const u8 sText_Title_Side_Smokescreen_Description[] =
    _("被命中的几率降低25%。\n可被除雾等\n招式清除。");
const u8 sText_Title_Side_HotCoals[] = _("热煤");
const u8 sText_Title_Side_HotCoals_Description[] =
    _("使下一只上场的宝可梦\n灼伤，随后消失。");
const u8 sText_Title_Side_Caltrops[] = _("铁蒺藜");
const u8 sText_Title_Side_Caltrops_Description[] =
    _("使下一只上场的宝可梦\n流血，随后消失。");
const u8 sText_Title_Side_QuickGuard[] = _("快速防守");
const u8 sText_Title_Side_QuickGuard_Description[] =
    _("保护己方所有宝可梦\n免受先制\n招式影响。");
const u8 sText_Title_Side_No_Effect[] = _("无效果");
const u8 sText_Title_Side_No_Effect_Description[] = _("己方场地没有特殊效果。");

static void PrintSideTab(u8 side) {
    u8 i, j;
    u8 x, y, x2, y2;
    u8 windowId = WINDOW_1;
    u8 turnsLeft = 5;
    bool8 printedInfo = FALSE;
    u8 maxLines = 3;

    FillWindowPixelBuffer(windowId, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));

    // Title
    x = 9;
    y = 0;
    x2 = 0;
    y2 = -4;
    switch (side) {
        case B_SIDE_PLAYER:
            AddTextPrinterParameterized4(windowId, FONT_SMALL, (x * 8), (y * 8), 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, sText_Title_Player_Side);
            break;
        case B_SIDE_OPPONENT:
            AddTextPrinterParameterized4(windowId, FONT_SMALL, (x * 8), (y * 8), 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, sText_Title_Enemy_Side);
            break;
    }
    x = 19;
    AddTextPrinterParameterized4(windowId, FONT_SMALL, (x * 8), (y * 8), 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, sText_Title_FieldControllers2);

    // Weather
    x = 9;
    y = 3;
    x2 = 0;
    y2 = -4;

    switch (side) {
        case B_SIDE_PLAYER:
            if (sMenuDataPtr->numSideInfoPlayer < maxLines) maxLines = sMenuDataPtr->numSideInfoPlayer;
            // sMenuDataPtr->currentSideInfoPlayer
            break;
        case B_SIDE_OPPONENT:
            if (sMenuDataPtr->numSideInfoEnemy < maxLines) maxLines = sMenuDataPtr->numSideInfoEnemy;
            // sMenuDataPtr->currentSideInfoEnemy
            break;
    }

    for (i = 0; i < maxLines; i++) {
        printedInfo = FALSE;

        if (side == B_SIDE_PLAYER) {
            j = sMenuDataPtr->SideInfoPlayer[(i + sMenuDataPtr->currentSideInfoPlayer) % sMenuDataPtr->numSideInfoPlayer];
        } else {
            j = sMenuDataPtr->SideInfoEnemy[(i + sMenuDataPtr->currentSideInfoEnemy) % sMenuDataPtr->numSideInfoEnemy];
        }

        switch (j) {
            case SIDE_INFO_AURORA_VEIL:
                StringCopy(gStringVar1, sText_Title_Side_Aurora_Veil);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Turns Left
                StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);
                turnsLeft = gSideTimers[GetBattlerSide(side)].auroraVeilTimer;
                ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Side_Aurora_Veil_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);

                printedInfo = TRUE;
                break;
            case SIDE_INFO_REFLECT:
                StringCopy(gStringVar1, sText_Title_Side_Reflect);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Turns Left
                StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);
                turnsLeft = gSideTimers[GetBattlerSide(side)].reflectTimer;
                ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Side_Reflect_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);

                printedInfo = TRUE;
                break;
            case SIDE_INFO_LIGHT_SCREEN:
                StringCopy(gStringVar1, sText_Title_Side_Light_Screen);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Turns Left
                StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);
                turnsLeft = gSideTimers[GetBattlerSide(side)].lightscreenTimer;
                ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Side_Light_Screen_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);

                printedInfo = TRUE;
                break;
            case SIDE_INFO_TAILWIND:
                StringCopy(gStringVar1, sText_Title_Side_Tailwind);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Turns Left
                StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);
                turnsLeft = gSideTimers[GetBattlerSide(side)].tailwindTimer;
                ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Side_Tailwind_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);

                printedInfo = TRUE;
                break;
            case SIDE_INFO_SPIKES:
                StringCopy(gStringVar1, sText_Title_Side_Spikes);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Layers
                StringCopy(gStringVar1, sText_Title_Field_Layers);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);
                turnsLeft = gSideTimers[GetBattlerSide(side)].spikesAmount;
                ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);

                // Description
                if (turnsLeft == 1)
                    StringCopy(gStringVar1, sText_Title_Side_Spikes_1_Description);
                else if (turnsLeft == 2)
                    StringCopy(gStringVar1, sText_Title_Side_Spikes_2_Description);
                else
                    StringCopy(gStringVar1, sText_Title_Side_Spikes_3_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);

                printedInfo = TRUE;
                break;
            case SIDE_INFO_TOXIC_SPIKES:
                StringCopy(gStringVar1, sText_Title_Side_Toxic_Spikes);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Layers
                StringCopy(gStringVar1, sText_Title_Field_Layers);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);
                turnsLeft = gSideTimers[GetBattlerSide(side)].toxicSpikesAmount;
                ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);

                // Description
                if (turnsLeft == 1)
                    StringCopy(gStringVar1, sText_Title_Side_Toxic_Spikes_1_Description);
                else
                    StringCopy(gStringVar1, sText_Title_Side_Toxic_Spikes_2_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);

                printedInfo = TRUE;
                break;
            case SIDE_INFO_STEALTH_ROCK:
                StringCopy(gStringVar1, sText_Title_Side_Stealth_Rock);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Side_Stealth_Rock_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);

                printedInfo = TRUE;
                break;
            case SIDE_INFO_CREEPING_THORNS:
                StringCopy(gStringVar1, sText_Title_Side_Creeping_Thorns);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Side_Creeping_Thorns_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);

                printedInfo = TRUE;
                break;
            case SIDE_INFO_STICKY_WEB:
                int foamyWeb = gSideTimers[GetBattlerSide(side)].foamyWeb;
                StringCopy(gStringVar1, foamyWeb ? sText_Title_Side_Foamy_Web : sText_Title_Side_Sticky_Web);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                if ((turnsLeft = gSideTimers[GetBattlerSide(side)].stickyWebTimer)) {
                    // Turns Left
                    StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                    AddTextPrinterParameterized4(windowId,
                                                 FONT_SMALL_NARROW,
                                                 (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                                 (y * 8) + y2,
                                                 0,
                                                 0,
                                                 sMenuWindowFontColors[FONT_WHITE],
                                                 0xFF,
                                                 gStringVar1);
                    ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                    AddTextPrinterParameterized4(windowId,
                                                 FONT_SMALL_NARROW,
                                                 (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                                 (y * 8) + y2,
                                                 0,
                                                 0,
                                                 sMenuWindowFontColors[FONT_WHITE],
                                                 0xFF,
                                                 gStringVar1);
                }

                // Description
                StringCopy(gStringVar1, foamyWeb ? sText_Title_Side_Foamy_Web_Description : sText_Title_Side_Sticky_Web_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);

                printedInfo = TRUE;
                break;
            case SIDE_INFO_SAFEGUARD:
                StringCopy(gStringVar1, sText_Title_Side_Safeguard);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Turns Left
                StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);
                turnsLeft = gSideTimers[GetBattlerSide(side)].safeguardTimer;
                ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Side_Safeguard_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);

                printedInfo = TRUE;
                break;
            case SIDE_INFO_MIST:
                StringCopy(gStringVar1, sText_Title_Side_Mist);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Turns Left
                StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);
                turnsLeft = gSideTimers[GetBattlerSide(side)].mistTimer;
                ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Side_Mist_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);

                printedInfo = TRUE;
                break;
            case SIDE_INFO_RAINBOW:
                StringCopy(gStringVar1, sText_Title_Side_Rainbow);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Turns Left
                StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);
                turnsLeft = gSideTimers[GetBattlerSide(side)].rainbowTimer;
                ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Side_Rainbow_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);

                printedInfo = TRUE;
                break;
            case SIDE_INFO_SEA_OF_FIRE:
                StringCopy(gStringVar1, sText_Title_Side_FireSea);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Turns Left
                StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);
                turnsLeft = gSideTimers[GetBattlerSide(side)].fireSeaTimer;
                ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Side_FireSea_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);

                printedInfo = TRUE;
                break;
            case SIDE_INFO_SWAMP:
                StringCopy(gStringVar1, sText_Title_Side_Swamp);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Turns Left
                StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);
                turnsLeft = gSideTimers[GetBattlerSide(side)].swampTimer;
                ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Side_Swamp_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);

                printedInfo = TRUE;
                break;
            case SIDE_INFO_SMOKESCREEN:
                StringCopy(gStringVar1, sText_Title_Side_Smokescreen);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Turns Left
                StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);
                turnsLeft = gSideTimers[GetBattlerSide(side)].smokescreenTimer;
                ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Side_Smokescreen_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);

                printedInfo = TRUE;
                break;
            case SIDE_INFO_HOT_COALS:
                StringCopy(gStringVar1, sText_Title_Side_HotCoals);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Side_HotCoals_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);

                printedInfo = TRUE;
                break;
            case SIDE_INFO_CALTROPS:
                StringCopy(gStringVar1, sText_Title_Side_Caltrops);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Side_Caltrops_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);

                printedInfo = TRUE;
                break;
            case SIDE_INFO_QUICK_GUARD:
                StringCopy(gStringVar1, sText_Title_Side_QuickGuard);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

                // Turns Left
                StringCopy(gStringVar1, sText_Title_Field_Turns_Left);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 2),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);
                turnsLeft = gSideTimers[GetBattlerSide(side)].quickGuardTimer;
                ConvertIntToDecimalStringN(gStringVar1, turnsLeft, STR_CONV_MODE_LEFT_ALIGN, 4);
                AddTextPrinterParameterized4(windowId,
                                             FONT_SMALL_NARROW,
                                             (x * 8) + x2 + (SPACE_BETWEEN_LINES_FIELD * 3),
                                             (y * 8) + y2,
                                             0,
                                             0,
                                             sMenuWindowFontColors[FONT_WHITE],
                                             0xFF,
                                             gStringVar1);

                // Description
                StringCopy(gStringVar1, sText_Title_Side_QuickGuard_Description);
                AddTextPrinterParameterized4(
                    windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);

                printedInfo = TRUE;
                break;
        }

        if (printedInfo) y = y + MAX_DESCRIPTION_LINES + 2;
    }

    if (maxLines == 0) {
        StringCopy(gStringVar1, sText_Title_Side_No_Effect);
        AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, gStringVar1);

        // Description
        StringCopy(gStringVar1, sText_Title_Side_No_Effect_Description);
        AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, ((y + 1) * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLACK], 0xFF, gStringVar1);
    }

    PutWindowTilemap(windowId);
    CopyWindowToVram(windowId, 3);
}

// gText_MoveInfo_Priority
const u8 gText_SpeedInfo_CanKOThisTurn[] = _("本回合可击倒{STR_VAR_1}：\n{STR_VAR_2}");
#define SPEED_POKEMON_ICON_X (9 * 8) + 4
#define SPEED_POKEMON_ICON_Y (6 * 8) + 1
#define SPEED_POKEMON_SPACE (8 * 8)

const u8 gText_BattlerLevelString[] = _("{LV}{STR_VAR_1}");
const u8 gText_PrintSpeedTabStats[] = _("总速度：{STR_VAR_1}");

#define MAX_SPEED_MONS_SHOWN 2

static void PrintSpeedTab(void) {
    // Single Battle Only
    u16 targetCurrentHp, move, moveDamage, species;
    u8 i, j, target, x, y, x2, y2;
    u8 windowId = WINDOW_1;
    u8 colorIdx = FONT_BLACK;
    u8 sBattlerByTurnOrder[gBattlersCount];
    u8 firstMon = sMenuDataPtr->speedModeId;
    u8 battlertoCheck = 0;

    FillWindowPixelBuffer(windowId, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));

    // Calculate Order
    SortBattlersBySpeed(sBattlerByTurnOrder, FALSE);

    // Mon Icon
    for (i = 0; i < MAX_SPEED_MONS_SHOWN; i++) {
        battlertoCheck = sBattlerByTurnOrder[firstMon + i];
        if (IsBattlerAlive(battlertoCheck)) {
            ShowSpeciesIconSpeed(battlertoCheck, SPEED_POKEMON_ICON_X, SPEED_POKEMON_ICON_Y + (i * SPEED_POKEMON_SPACE));
        }
    }

    // Title
    x = 9;
    y = 0;
    x2 = 0;
    y2 = -4;
    AddTextPrinterParameterized4(windowId, FONT_SMALL, (x * 8), (y * 8), 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, sText_Title_Speed_Order);
    x = 19;
    AddTextPrinterParameterized4(windowId, FONT_SMALL, (x * 8), (y * 8), 0, 0, sMenuWindowFontColors[FONT_WHITE], 0xFF, sText_Title_Controllers);

    // Speed Order
    x = 13;
    y = 4;
    x2 = 0;
    y2 = -4;

    for (i = 0; i < MAX_SPEED_MONS_SHOWN; i++) {
        battlertoCheck = sBattlerByTurnOrder[firstMon + i];
        if (IsBattlerAlive(battlertoCheck)) {
            u8 gender = GetGenderFromSpeciesAndPersonality(gBattleMons[battlertoCheck].species, gBattleMons[battlertoCheck].personality);
            u16 speed = GetBattlerTotalSpeedStat(battlertoCheck, TOTAL_SPEED_FULL, MOVE_NONE);
            species = gBattleMons[battlertoCheck].species;
            target = BATTLE_OPPOSITE(battlertoCheck);
            targetCurrentHp = gBattleMons[target].hp;

            // Species Name
            StringCopy(gStringVar1, gSpeciesNames[species]);
            AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar1);
            x = x + 8;

            // Level
            ConvertIntToDecimalStringN(gStringVar1, gBattleMons[battlertoCheck].level, STR_CONV_MODE_LEFT_ALIGN, 3);
            StringExpandPlaceholders(gStringVar4, gText_BattlerLevelString);
            AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar4);
            x = x + 4;

            switch (gender) {
                case MON_MALE:
                    AddTextPrinterParameterized4(
                        windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLUE], 0xFF, gText_MaleSymbol);
                    break;
                case MON_FEMALE:
                    AddTextPrinterParameterized4(
                        windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_RED], 0xFF, gText_FemaleSymbol);
                    break;
            }
            x = 13;
            y++;
            // Total Stats 9 x 8
            ConvertIntToDecimalStringN(gStringVar1, speed, STR_CONV_MODE_LEFT_ALIGN, 4);
            AddTextPrinterParameterized4(
                windowId, FONT_SMALL_NARROW, (8 * 8) + 4, (10 * 8) - 4 + (SPEED_POKEMON_SPACE * i), 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar1);
            y = y + 2;

            // Move Names
            for (j = 0; j < MAX_MON_MOVES; j++) {
                move = gBattleMons[battlertoCheck].moves[j];
                if (move != MOVE_NONE) {
                    StringCopy(gStringVar1, gMoveNames[move]);
                    AddTextPrinterParameterized4(
                        windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar1);
                    x = x + 9;
                    // Priority
                    ConvertIntToDecimalStringN(gStringVar1, gBattleMoves[move].priority, STR_CONV_MODE_LEFT_ALIGN, 3);
                    AddTextPrinterParameterized4(
                        windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar1);
                    x = x + 3;
                    x2 = 4;
                    // Can KO - Todo: Check calculation
                    if (gBattleMoves[move].split != SPLIT_STATUS && gBattleMoves[move].power > 0) {
                        u8 moveType = gBattleMoves[move].type;
                        SetTypeBeforeUsingMove(move, battlertoCheck);
                        GET_MOVE_TYPE(move, moveType);
                        moveDamage = CalculateMoveDamage(move, battlertoCheck, target, &moveType, 0, CRIT_ROLL_ONLY_IF_GUARANTEED, FALSE, FALSE);
                        gSwapDamageCategory = FALSE;
                        if (targetCurrentHp <= moveDamage) BlitBitmapToWindow(windowId, sCheck, (x * 8) + x2, (y * 8), 8, 8);
                    }
                    x = x - 12;
                    x2 = 0;
                }
                y++;
            }
            y++;
        }
    }

    PutWindowTilemap(windowId);
    CopyWindowToVram(windowId, 3);
}

static void PrintToWindow(u8 windowId, u8 colorIdx) {
    u8 i, j;
    u8 x, y, x2, y2;
    SpeciesEnum species = gBattleMons[sMenuDataPtr->battlerId].species;
    AbilityEnum abilities[4];
    u8 gender = GetGenderFromSpeciesAndPersonality(gBattleMons[sMenuDataPtr->battlerId].species, gBattleMons[sMenuDataPtr->battlerId].personality);
    u8 statStage;
    bool8 statStageUp = FALSE;
    bool8 isEnemyMon = GetBattlerSide(sMenuDataPtr->battlerId) == B_SIDE_OPPONENT;
    MoveEnum move = MOVE_NONE;

    FillWindowPixelBuffer(windowId, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    GetBattlerDisplayedAbilities(sMenuDataPtr->battlerId, abilities);

    // Title
    x = 7;
    y = 2;
    x2 = 0;
    y2 = -4;
    if (isEnemyMon)
        AddTextPrinterParameterized4(
            windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, sYourOpponentPokemonData);
    else
        AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, sYourPokemonData);

    // Pokemon Name
    y++;
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gSpeciesNames[species]);
    // Pokemon HP
    y++;
    ConvertIntToDecimalStringN(gStringVar1, gBattleMons[sMenuDataPtr->battlerId].hp, STR_CONV_MODE_LEFT_ALIGN, 3);
    ConvertIntToDecimalStringN(gStringVar2, gBattleMons[sMenuDataPtr->battlerId].maxHP, STR_CONV_MODE_LEFT_ALIGN, 3);
    StringExpandPlaceholders(gStringVar4, sText_HP);
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar4);
    // Pokemon Gender
    x = 2;
    y = 6;
    switch (gender) {
        case MON_MALE:
            AddTextPrinterParameterized4(
                windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_BLUE], 0xFF, gText_MaleSymbol);
            break;
        case MON_FEMALE:
            AddTextPrinterParameterized4(
                windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[FONT_RED], 0xFF, gText_FemaleSymbol);
            break;
    }

    // Pokemon Level
    x++;
    ConvertIntToDecimalStringN(gStringVar1, gBattleMons[sMenuDataPtr->battlerId].level, STR_CONV_MODE_LEFT_ALIGN, 3);
    StringExpandPlaceholders(gStringVar4, gText_NewLevelSymbol);
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar4);

    // Held Item
    x = 22;
    y = 3;
    if (gBattleMons[sMenuDataPtr->battlerId].item == ITEM_NONE)
        StringCopy(gStringVar1, sText_None);
    else
        StringCopy(gStringVar1, ItemId_GetName(gBattleMons[sMenuDataPtr->battlerId].item));
    StringExpandPlaceholders(gStringVar4, sText_Held_Item);
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar4);

    // Stats
    x = 3;
    y = 7;
    // Attack
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, sText_Attack);
    ConvertIntToDecimalStringN(gStringVar1, gBattleMons[sMenuDataPtr->battlerId].attack, STR_CONV_MODE_LEFT_ALIGN, 3);
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, ((x + 3) * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar1);
    y++;
    // Defense
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, sText_Defense);
    ConvertIntToDecimalStringN(gStringVar1, gBattleMons[sMenuDataPtr->battlerId].defense, STR_CONV_MODE_LEFT_ALIGN, 3);
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, ((x + 3) * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar1);
    y++;
    // Special Attack
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, sText_SpecialAttack);
    ConvertIntToDecimalStringN(gStringVar1, gBattleMons[sMenuDataPtr->battlerId].spAttack, STR_CONV_MODE_LEFT_ALIGN, 3);
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, ((x + 3) * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar1);
    y++;
    // Special Defense
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, sText_SpecialDefense);
    ConvertIntToDecimalStringN(gStringVar1, gBattleMons[sMenuDataPtr->battlerId].spDefense, STR_CONV_MODE_LEFT_ALIGN, 3);
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, ((x + 3) * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar1);
    y++;
    // Speed
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, sText_Speed);
    ConvertIntToDecimalStringN(gStringVar1, gBattleMons[sMenuDataPtr->battlerId].speed, STR_CONV_MODE_LEFT_ALIGN, 3);
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, ((x + 3) * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar1);

    // Accuracy
    y = y + 2;
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, sText_Accuracy);
    // Evasion
    y++;
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, sText_Evasion);

    // Abilitites
    x = 15;
    y = 7;
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, sText_Ability);
    AddTextPrinterParameterized4(windowId,
                                 FONT_SMALL_NARROW,
                                 ((x + 5) * 8) + x2,
                                 (y * 8) + y2,
                                 0,
                                 0,
                                 sMenuWindowFontColors[colorIdx],
                                 0xFF,
                                 gAbilities[abilities[0]].name);
    y++;
    // Innate 1
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, sText_Innate);
    AddTextPrinterParameterized4(
        windowId, FONT_SMALL_NARROW, ((x + 5) * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gAbilities[abilities[1]].name);
    y++;
    // Innate 2
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, sText_Innate);
    AddTextPrinterParameterized4(
        windowId, FONT_SMALL_NARROW, ((x + 5) * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gAbilities[abilities[2]].name);
    y++;
    // Innate 3
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, sText_Innate);
    AddTextPrinterParameterized4(
        windowId, FONT_SMALL_NARROW, ((x + 5) * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gAbilities[abilities[3]].name);

    // Moves
    y = y + 2;
    for (i = 0; i < MAX_MON_MOVES; i++) {
        move = gBattleMons[sMenuDataPtr->battlerId].moves[i];
        if (move != MOVE_NONE) {
            AddTextPrinterParameterized4(
                windowId, FONT_SMALL_NARROW, (x * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gMoveNames[move]);
            // Current PP
            ConvertIntToDecimalStringN(gStringVar1, gBattleMons[sMenuDataPtr->battlerId].pp[i], STR_CONV_MODE_LEFT_ALIGN, 3);  // Current PP
            ConvertIntToDecimalStringN(gStringVar2, gBattleMoves[move].pp, STR_CONV_MODE_LEFT_ALIGN, 3);                       // Max PP, ToFix
            StringExpandPlaceholders(gStringVar4, sText_PP);
            AddTextPrinterParameterized4(
                windowId, FONT_SMALL_NARROW, ((x + 8) * 8) + x2, (y * 8) + y2, 0, 0, sMenuWindowFontColors[colorIdx], 0xFF, gStringVar4);
            y++;
        }
    }

    // Stat Drops & Ups
    x = 8;
    y = 7;
    for (i = 0; i < NUM_BATTLE_STATS - 1; i++) {
        statStage = gBattleMons[sMenuDataPtr->battlerId].statStages[statorder[i + 1]];  // HP is not taken into account
        if (statStage != DEFAULT_STAT_STAGE) {
            if (statStage > DEFAULT_STAT_STAGE) {
                statStageUp = TRUE;
                statStage = statStage - DEFAULT_STAT_STAGE;
            } else {
                statStageUp = FALSE;
                statStage = DEFAULT_STAT_STAGE - statStage;
            }

            for (j = 0; j < statStage; j++) {
                if (statStageUp)
                    BlitBitmapToWindow(windowId, sStatUpArrow, ((x + j) * 8) + x2, (y * 8), 8, 8);
                else
                    BlitBitmapToWindow(windowId, sStatDownArrow, ((x + j) * 8) + x2, (y * 8), 8, 8);
            }
        }
        if (statorder[i + 1] == STAT_SPEED)
            y = y + 2;
        else
            y++;
    }

    PutWindowTilemap(windowId);
    CopyWindowToVram(windowId, 3);
}

static void Task_MenuWaitFadeIn(u8 taskId) {
    if (!gPaletteFade.active) gTasks[taskId].func = Task_MenuMain;
}

static void Task_MenuTurnOff(u8 taskId) {
    if (!gPaletteFade.active) {
        SetMainCallback2(sMenuDataPtr->savedCallback);
        Menu_FreeResources();
        try_free(tempsavedCallback);
        DestroyTask(taskId);
    }
}
// Status

static const union AnimCmd sSpriteAnim_StatusPoison[] = {ANIMCMD_FRAME(0, 0, FALSE, FALSE), ANIMCMD_END};
static const union AnimCmd sSpriteAnim_StatusParalyzed[] = {ANIMCMD_FRAME(4, 0, FALSE, FALSE), ANIMCMD_END};
static const union AnimCmd sSpriteAnim_StatusSleep[] = {ANIMCMD_FRAME(8, 0, FALSE, FALSE), ANIMCMD_END};
static const union AnimCmd sSpriteAnim_StatusFrozen[] = {ANIMCMD_FRAME(12, 0, FALSE, FALSE), ANIMCMD_END};
static const union AnimCmd sSpriteAnim_StatusBurn[] = {ANIMCMD_FRAME(16, 0, FALSE, FALSE), ANIMCMD_END};
static const union AnimCmd sSpriteAnim_StatusPokerus[] = {ANIMCMD_FRAME(20, 0, FALSE, FALSE), ANIMCMD_END};
static const union AnimCmd sSpriteAnim_StatusFaint[] = {ANIMCMD_FRAME(24, 0, FALSE, FALSE), ANIMCMD_END};
static const union AnimCmd sSpriteAnim_StatusFrostbite[] = {ANIMCMD_FRAME(28, 0, FALSE, FALSE), ANIMCMD_END};
static const union AnimCmd sSpriteAnim_StatusBleed[] = {ANIMCMD_FRAME(32, 0, FALSE, FALSE), ANIMCMD_END};
static const union AnimCmd* const sSpriteAnimTable_StatusCondition[] = {
    sSpriteAnim_StatusPoison,
    sSpriteAnim_StatusParalyzed,
    sSpriteAnim_StatusSleep,
    sSpriteAnim_StatusFrozen,
    sSpriteAnim_StatusBurn,
    sSpriteAnim_StatusPokerus,
    sSpriteAnim_StatusFaint,
    sSpriteAnim_StatusFrostbite,
    sSpriteAnim_StatusBleed,
};
static const struct OamData sOamData_StatusCondition = {
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = 0,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x8),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(32x8),
    .tileNum = 0,
    .priority = 3,
    .paletteNum = 0,
    .affineParam = 0,
};
#define TAG_MON_STATUS 30001
static const struct SpriteTemplate sSpriteTemplate_StatusCondition = {.tileTag = TAG_MON_STATUS,
                                                                      .paletteTag = TAG_MON_STATUS,
                                                                      .oam = &sOamData_StatusCondition,
                                                                      .anims = sSpriteAnimTable_StatusCondition,
                                                                      .images = NULL,
                                                                      .affineAnims = gDummySpriteAffineAnimTable,
                                                                      .callback = SpriteCallbackDummy};

#define STATUS_ICON_X (7 * 8)
#define STATUS_ICON_Y (5 * 8)
static void CreateSetStatusSprite(void) {
    u8* spriteId = &sMenuDataPtr->spriteIds[SPRITE_ARR_ID_STATUS];
    u8 statusAnim;

    if (*spriteId == SPRITE_NONE) *spriteId = CreateSprite(&sSpriteTemplate_StatusCondition, STATUS_ICON_X, STATUS_ICON_Y, 0);

    // statusAnim = gBattleMons[sMenuDataPtr->battlerId].status1;
    statusAnim = sMenuDataPtr->battlerId;
    if (statusAnim != 0) {
        StartSpriteAnim(&gSprites[*spriteId], statusAnim - 1);
        SetSpriteInvisibility(SPRITE_ARR_ID_STATUS, FALSE);
    } else {
        SetSpriteInvisibility(SPRITE_ARR_ID_STATUS, TRUE);
    }
}

static void DestroyBattleMenuSprite(u8 spriteArrayId) {
    struct Sprite* sprite = &gSprites[sMenuDataPtr->spriteIds[spriteArrayId]];
    sMenuDataPtr->spriteIds[spriteArrayId] = SPRITE_NONE;
    DestroySpriteAndFreeResources(sprite);
}

static u8 ShowSpeciesIcon(u8 num) {
    SpeciesEnum species = gBattleMons[num].species;
    u16 personality = gBattleMons[num].personality;
    LoadMonIconPalette(species);

    switch (num) {
        case 0:
            sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_1] = CreateMonIcon(species, SpriteCallbackDummy, POKEMON_ICON_X, POKEMON_ICON_1_Y, 0, personality);

            gSprites[sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_1]].invisible = FALSE;
            return sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_1];
            break;
        case 1:
            sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_2] = CreateMonIcon(species, SpriteCallbackDummy, POKEMON_ICON_X, POKEMON_ICON_2_Y, 0, personality);

            gSprites[sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_2]].invisible = FALSE;
            return sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_2];
            break;
        case 2:
            if (sMenuDataPtr->isDoubleBattle) {
                sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_3] =
                    CreateMonIcon(species, SpriteCallbackDummy, POKEMON_ICON_X, POKEMON_ICON_3_Y, 0, personality);

                gSprites[sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_3]].invisible = FALSE;
            }
            return sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_3];
            break;
        case 3:
            if (sMenuDataPtr->isDoubleBattle) {
                sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_4] =
                    CreateMonIcon(species, SpriteCallbackDummy, POKEMON_ICON_X, POKEMON_ICON_4_Y, 0, personality);

                gSprites[sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_4]].invisible = FALSE;
            }
            return sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_4];
            break;
    }
    return 0;
}

static void FreeEveryMonIconSprite(void) {
    u8 i;
    for (i = 0; i < gBattlersCount; i++) FreeSpeciesIconSprite(i);
}

static void FreeSpeciesIconSprite(u8 battler) {
    u8* spriteId = &sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_1_SPEED + battler];
    if (*spriteId != SPRITE_NONE) {
        FreeSpriteTilesByTag(TAG_BATTLER_SPEED + battler - 1);
        FreeSpritePaletteByTag(TAG_BATTLER_SPEED + battler - 1);
        FreeSpriteOamMatrix(&gSprites[*spriteId]);
        DestroySprite(&gSprites[*spriteId]);
        *spriteId = SPRITE_NONE;
        sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_1_SPEED + battler] = SPRITE_NONE;
    }
}

static u8 ShowSpeciesIconSpeed(u8 battler, u8 x, u8 y) {
    SpeciesEnum species = gBattleMons[battler].species;
    u32 personality = gBattleMons[battler].personality;
    LoadMonIconPalette(species);

    switch (battler) {
        case 0:
            sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_1_SPEED] = CreateMonIcon(species, SpriteCallbackDummy, x, y, 0, personality);

            gSprites[sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_1_SPEED]].invisible = FALSE;
            return sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_1_SPEED];
            break;
        case 1:
            sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_2_SPEED] = CreateMonIcon(species, SpriteCallbackDummy, x, y, 0, personality);

            gSprites[sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_2_SPEED]].invisible = FALSE;
            return sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_2_SPEED];
            break;
        case 2:
            if (sMenuDataPtr->isDoubleBattle) {
                sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_3_SPEED] = CreateMonIcon(species, SpriteCallbackDummy, x, y, 0, personality);

                gSprites[sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_3_SPEED]].invisible = FALSE;
            }
            return sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_3_SPEED];
            break;
        case 3:
            if (sMenuDataPtr->isDoubleBattle) {
                sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_4_SPEED] = CreateMonIcon(species, SpriteCallbackDummy, x, y, 0, personality);

                gSprites[sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_4_SPEED]].invisible = FALSE;
            }
            return sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_4_SPEED];
            break;
    }
    return 0;
}

static u8 ShowSpeciesIconParty(u8 num, bool8 isEnemyParty, u8 x, u8 y) {
    struct Pokemon* party = NULL;
    SpeciesEnum species;
    u32 personality;
    u8 spriteId;

    if (isEnemyParty)
        party = gEnemyParty;
    else
        party = gPlayerParty;

    species = GetMonData(&party[num], MON_DATA_SPECIES, NULL);
    personality = GetMonData(&party[num], MON_DATA_PERSONALITY, NULL);

    LoadMonIconPalette(species);

    if (species == SPECIES_NONE) return 0;

    if (isEnemyParty) {
        if (sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_1_PARTY_ENEMY + num] != SPRITE_NONE)  // Already created
            return sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_1_PARTY_ENEMY + num];

        sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_1_PARTY_ENEMY + num] = CreateMonIcon(species, SpriteCB_PartyMons, x, y, 0, personality);

        gSprites[sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_1_PARTY_ENEMY] + num].invisible = FALSE;
        spriteId = sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_1_PARTY_ENEMY + num];
    } else {
        if (sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_1_PARTY_PLAYER + num] != SPRITE_NONE)  // Already created
            return sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_1_PARTY_PLAYER + num];

        sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_1_PARTY_PLAYER + num] = CreateMonIcon(species, SpriteCB_PartyMons, x, y, 0, personality);

        gSprites[sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_1_PARTY_PLAYER + num]].invisible = FALSE;
        spriteId = sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_1_PARTY_PLAYER + num];
    }

    return spriteId;
}

static void FreeEveryPartyMonIconSprite(void) {
    u8 i;
    for (i = 0; i < PARTY_SIZE; i++) FreePartySpeciesIconSprite(i, FALSE);

    for (i = 0; i < PARTY_SIZE; i++) FreePartySpeciesIconSprite(i, TRUE);
}

static void FreePartySpeciesIconSprite(u8 num, bool8 isEnemyMon) {
    u8* spriteId;

    if (isEnemyMon)
        spriteId = &sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_1_PARTY_ENEMY + num];
    else
        spriteId = &sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_1_PARTY_PLAYER + num];

    if (*spriteId != SPRITE_NONE) {
        if (isEnemyMon) {
            FreeSpriteTilesByTag(TAG_ICON_PARTY_ENEMY + num);
            FreeSpritePaletteByTag(TAG_ICON_PARTY_ENEMY + num);
        } else {
            FreeSpriteTilesByTag(TAG_ICON_PARTY_PLAYER + num);
            FreeSpritePaletteByTag(TAG_ICON_PARTY_PLAYER + num);
        }
        FreeSpriteOamMatrix(&gSprites[*spriteId]);
        DestroySprite(&gSprites[*spriteId]);
        *spriteId = SPRITE_NONE;

        if (isEnemyMon)
            sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_1_PARTY_ENEMY + num] = SPRITE_NONE;
        else
            sMenuDataPtr->spriteIds[SPRITE_ARR_ID_MON_ICON_1_PARTY_PLAYER + num] = SPRITE_NONE;
    }
}

// different from pokemon_summary_screen
#define TYPE_ICON_PAL_NUM_0 13
#define TYPE_ICON_PAL_NUM_1 14
#define TYPE_ICON_PAL_NUM_2 15
static const u8 sMoveTypeToOamPaletteNum[NUMBER_OF_MON_TYPES] = {
    [TYPE_NORMAL] = TYPE_ICON_PAL_NUM_0, [TYPE_FIGHTING] = TYPE_ICON_PAL_NUM_0, [TYPE_FLYING] = TYPE_ICON_PAL_NUM_1,  [TYPE_POISON] = TYPE_ICON_PAL_NUM_1,
    [TYPE_GROUND] = TYPE_ICON_PAL_NUM_0, [TYPE_ROCK] = TYPE_ICON_PAL_NUM_0,     [TYPE_BUG] = TYPE_ICON_PAL_NUM_2,     [TYPE_GHOST] = TYPE_ICON_PAL_NUM_1,
    [TYPE_STEEL] = TYPE_ICON_PAL_NUM_0,  [TYPE_MYSTERY] = TYPE_ICON_PAL_NUM_2,  [TYPE_FIRE] = TYPE_ICON_PAL_NUM_0,    [TYPE_WATER] = TYPE_ICON_PAL_NUM_1,
    [TYPE_GRASS] = TYPE_ICON_PAL_NUM_2,  [TYPE_ELECTRIC] = TYPE_ICON_PAL_NUM_0, [TYPE_PSYCHIC] = TYPE_ICON_PAL_NUM_1, [TYPE_ICE] = TYPE_ICON_PAL_NUM_1,
    [TYPE_DRAGON] = TYPE_ICON_PAL_NUM_2, [TYPE_DARK] = TYPE_ICON_PAL_NUM_0,     [TYPE_FAIRY] = TYPE_ICON_PAL_NUM_1,   [TYPE_STELLAR] = TYPE_ICON_PAL_NUM_2,
};

static void SetSpriteInvisibility(u8 spriteArrayId, bool8 invisible) { gSprites[sMenuDataPtr->spriteIds[spriteArrayId]].invisible = invisible; }

static void SetTypeIconSpritePosAndPal(u8 typeId, u8 x, u8 y, u8 spriteArrayId) {
    struct Sprite* sprite = &gSprites[sMenuDataPtr->spriteIds[spriteArrayId]];
    StartSpriteAnim(sprite, typeId);
    sprite->oam.paletteNum = sMoveTypeToOamPaletteNum[typeId];
    sprite->x = x + 16;
    sprite->y = y + 8;
    SetSpriteInvisibility(spriteArrayId, FALSE);
}
/*
#define TYPE_ICON_Y (5 * 8)
#define TYPE_ICON_1_X (7 * 8)
#define TYPE_ICON_2_X TYPE_ICON_1_X + (4 * 8)
#define TYPE_ICON_3_X TYPE_ICON_2_X + (4 * 8)
static void SetMonTypeIcons(void)
{
    u8 type1 = gBattleMons[sMenuDataPtr->battlerId].type1;
    u8 type2 = gBattleMons[sMenuDataPtr->battlerId].type2;
    u8 type3 = gBattleMons[sMenuDataPtr->battlerId].type3;

    if (type1 != type2)
    {
        SetTypeIconSpritePosAndPal(type1, TYPE_ICON_1_X, TYPE_ICON_Y, SPRITE_ARR_ID_TYPE_1);
        SetTypeIconSpritePosAndPal(type2, TYPE_ICON_2_X, TYPE_ICON_Y, SPRITE_ARR_ID_TYPE_2);
        SetSpriteInvisibility(SPRITE_ARR_ID_TYPE_2, FALSE);
    }
    else
    {
        SetTypeIconSpritePosAndPal(type1, TYPE_ICON_1_X, TYPE_ICON_Y, SPRITE_ARR_ID_TYPE_1);
        SetSpriteInvisibility(SPRITE_ARR_ID_TYPE_2, TRUE);
    }
}*/

static const u8 tabColors[NUM_TABS] = {
    [TAB_STATS] = MENU_COLOR_BLUE,
    [TAB_ABILITIES] = MENU_COLOR_RED,
    [TAB_MOVES] = MENU_COLOR_GREEN,
    [TAB_STATUS] = MENU_COLOR_YELLOW,
    [TAB_DAMAGE_CALCULATOR] = MENU_COLOR_RED,
};

static const u8 tabColorsField[NUM_FIELD_TABS + 2] = {
    [TAB_PARTY] = MENU_COLOR_YELLOW,
    [TAB_FIELD] = MENU_COLOR_GREEN,
    [TAB_SPEED] = MENU_COLOR_BLUE,
    [TAB_PLAYER_SIDE] = MENU_COLOR_RED,
    [TAB_ENEMY_SIDE] = MENU_COLOR_YELLOW,
};

static void setBattler(void) {
    switch (sMenuDataPtr->modeId) {
        case MODE_BATTLER0:
            sMenuDataPtr->battlerId = 0;
            break;
        case MODE_BATTLER1:
            sMenuDataPtr->battlerId = 1;
            break;
        case MODE_BATTLER2:
            sMenuDataPtr->battlerId = 2;
            break;
        case MODE_BATTLER3:
            sMenuDataPtr->battlerId = 3;
            break;
        case MODE_FIELD:
            sMenuDataPtr->battlerId = 0;
            break;
    }
}

static void LoadTabPalette(void) {
    if (sMenuDataPtr->modeId == MODE_FIELD) {
        switch (tabColorsField[sMenuDataPtr->fieldTabId]) {
            case MENU_COLOR_BLUE:
                LoadPalette(sMenuPalette_Blue, 0, 32);
                break;
            case MENU_COLOR_YELLOW:
                LoadPalette(sMenuPalette_Yellow, 0, 32);
                break;
            case MENU_COLOR_RED:
                LoadPalette(sMenuPalette_Red, 0, 32);
                break;
            case MENU_COLOR_GREEN:
                LoadPalette(sMenuPalette_Green, 0, 32);
                break;
            default:
                LoadPalette(sMenuPalette, 0, 32);
                break;
        }
    } else {
        switch (tabColors[sMenuDataPtr->tabId]) {
            case MENU_COLOR_BLUE:
                LoadPalette(sMenuPalette_Blue, 0, 32);
                break;
            case MENU_COLOR_YELLOW:
                LoadPalette(sMenuPalette_Yellow, 0, 32);
                break;
            case MENU_COLOR_RED:
                LoadPalette(sMenuPalette_Red, 0, 32);
                break;
            case MENU_COLOR_GREEN:
                LoadPalette(sMenuPalette_Green, 0, 32);
                break;
            default:
                LoadPalette(sMenuPalette, 0, 32);
                break;
        }
    }
}

static void PrintPage(void) {
    LoadTilemapFromMode();
    LoadTabPalette();
    FreeItemIconSprite();
    FreeEveryMonIconSprite();
    // FreeEveryPartyMonIconSprite();
    if (sMenuDataPtr->modeId != MODE_FIELD) switch (sMenuDataPtr->tabId) {
            case TAB_STATS:
                PrintStatsTab();
                break;
            case TAB_ABILITIES:
                PrintAbilityTab();
                break;
            case TAB_MOVES:
                PrintMoveTab();
                break;
            case TAB_STATUS:
                PrintStatusTab();
                break;
            case TAB_DAMAGE_CALCULATOR:
                PrintDamageCalulatorTab();
                break;
            default:
                PrintStatsTab();
                break;
        }
    else {
        switch (sMenuDataPtr->fieldTabId) {
            case TAB_PARTY:
                PrintPartyTab();
                break;
            case TAB_FIELD:
                PrintFieldTab();
                break;
            case TAB_SPEED:
                PrintSpeedTab();
                break;
            case TAB_PLAYER_SIDE:
                PrintSideTab(B_SIDE_PLAYER);
                break;
            case TAB_ENEMY_SIDE:
                PrintSideTab(B_SIDE_OPPONENT);
                break;
        }
    }
}

#define PARTY_TAB_NUM_MONS_X 3
#define PARTY_TAB_NUM_MONS_Y 4
#define PARTY_MENU_SUMMARY_LOCK_MONS FALSE

static void StartSummaryScreen(u8 taskId) {
    u8 currMonId = sMenuDataPtr->partyMenuSelectorID_X + (sMenuDataPtr->partyMenuSelectorID_Y * PARTY_TAB_NUM_MONS_X);
    SpeciesEnum species;
    u8 partyCount;
    u8 value = 2;
    bool8 isEnemyMon = FALSE;

    // Save State
    VarSet(VAR_BATTLE_MENU_MON_ID_X, sMenuDataPtr->partyMenuSelectorID_X);
    VarSet(VAR_BATTLE_MENU_MON_ID_Y, sMenuDataPtr->partyMenuSelectorID_Y);
    FlagSet(FLAG_BATTLE_MENU_COMING_FROM_SUMMARY_SCREEN);

    // Check if the mon is from the enemy party or the player party
    if (currMonId < PARTY_SIZE) {
        // Player Party
        species = GetMonData(&gPlayerParty[currMonId], MON_DATA_SPECIES, NULL);
        partyCount = gPlayerPartyCount - 1;
    } else {
        // Enemy Party
        species = GetMonData(&gEnemyParty[currMonId - PARTY_SIZE], MON_DATA_SPECIES, NULL);
        partyCount = CalculateEnemyPartyCount() - 1;
        currMonId = currMonId - PARTY_SIZE;
        isEnemyMon = TRUE;
    }

    if (PARTY_MENU_SUMMARY_LOCK_MONS) partyCount = 0;  // To remove being able to change mons in the summary screen

    if (species != SPECIES_NONE) {
        VarSet(VAR_BATTLE_CONTROLLER_PLAYER_F, value);
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 0x10, RGB_BLACK);
        FreeAllWindowBuffers();
        DestroyTask(taskId);
        tempsavedCallback = sMenuDataPtr->savedCallback;
        Menu_FreeResources();
        if (!isEnemyMon)
            ShowPokemonSummaryScreen(SUMMARY_MODE_LOCK_MOVES, gPlayerParty, currMonId, partyCount, CB2_SetUpReshowBattleMenuAfterSummaryScreen);
        else
            ShowPokemonSummaryScreen(SUMMARY_MODE_LOCK_MOVES, gEnemyParty, currMonId, partyCount, CB2_SetUpReshowBattleMenuAfterSummaryScreen);
    } else {
        // There is no pokemon in that slot
        PlaySE(SE_BOO);
        FlagClear(FLAG_BATTLE_MENU_COMING_FROM_SUMMARY_SCREEN);
        VarSet(VAR_BATTLE_MENU_MON_ID_X, 0);
        VarSet(VAR_BATTLE_MENU_MON_ID_Y, 0);
    }
}

static void StartSummaryScreenForSpecificMon(u8 taskId) {
    u8 battler = sMenuDataPtr->battlerId;
    u8 currMonId = gBattlerPartyIndexes[battler];
    SpeciesEnum species;
    u8 partyCount = 0;
    u8 value = 2;
    bool8 isEnemyMon = FALSE;

    // Save State
    VarSet(VAR_BATTLE_MENU_MON_ID_X, 0xFF + sMenuDataPtr->tabId);
    VarSet(VAR_BATTLE_MENU_MON_ID_Y, sMenuDataPtr->modeId);
    FlagSet(FLAG_BATTLE_MENU_COMING_FROM_SUMMARY_SCREEN);

    // Check if the mon is from the enemy party or the player party
    if (GetBattlerSide(sMenuDataPtr->battlerId) == B_SIDE_PLAYER) {
        // Player Party
        species = GetMonData(&gPlayerParty[currMonId], MON_DATA_SPECIES, NULL);
    } else {
        // Enemy Party
        species = GetMonData(&gEnemyParty[currMonId], MON_DATA_SPECIES, NULL);
        currMonId = currMonId;
        isEnemyMon = TRUE;
    }

    if (species != SPECIES_NONE) {
        VarSet(VAR_BATTLE_CONTROLLER_PLAYER_F, value);
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 0x10, RGB_BLACK);
        FreeAllWindowBuffers();
        DestroyTask(taskId);
        tempsavedCallback = sMenuDataPtr->savedCallback;
        Menu_FreeResources();
        if (!isEnemyMon)
            ShowPokemonSummaryScreen(SUMMARY_MODE_LOCK_MOVES, gPlayerParty, currMonId, partyCount, CB2_SetUpReshowBattleMenuAfterSummaryScreen);
        else
            ShowPokemonSummaryScreen(SUMMARY_MODE_LOCK_MOVES, gEnemyParty, currMonId, partyCount, CB2_SetUpReshowBattleMenuAfterSummaryScreen);
    } else {
        // There is no pokemon in that slot
        PlaySE(SE_BOO);
        FlagClear(FLAG_BATTLE_MENU_COMING_FROM_SUMMARY_SCREEN);
        VarSet(VAR_BATTLE_MENU_MON_ID_X, 0);
        VarSet(VAR_BATTLE_MENU_MON_ID_Y, 0);
    }
}

void ReshowBattleMenuAfterSummaryScreen(void) { UI_Battle_Menu_Init(tempsavedCallback); }

void CB2_SetUpReshowBattleMenuAfterSummaryScreen(void) { SetMainCallback2(ReshowBattleMenuAfterSummaryScreen); }

/* This is the meat of the UI. This is where you wait for player inputs and can branch to other tasks accordingly */
static void Task_MenuMain(u8 taskId) {
    u8 value = 3;

    if (JOY_NEW(B_BUTTON)) {
        if (sMenuDataPtr->modeId == MODE_FIELD && sMenuDataPtr->fieldTabId == TAB_PARTY && sMenuDataPtr->partySelectorMode) {
            sMenuDataPtr->partySelectorMode = FALSE;
            sMenuDataPtr->partyMenuSelectorID_X = 0;
            sMenuDataPtr->partyMenuSelectorID_Y = 0;

            PrintPartyTab();
        } else {
            VarSet(VAR_BATTLE_CONTROLLER_PLAYER_F, value);
            PlaySE(SE_PC_OFF);
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB_BLACK);
            gTasks[taskId].func = Task_MenuTurnOff;
        }
    } else if (JOY_NEW(A_BUTTON)) {
        if (sMenuDataPtr->modeId != MODE_FIELD) {
            switch (sMenuDataPtr->tabId) {
                case TAB_MOVES:
                    if (sMenuDataPtr->moveModeId != NUM_MOVE_MODES - 1)
                        sMenuDataPtr->moveModeId++;
                    else
                        sMenuDataPtr->moveModeId = 0;
                    PrintMoveTab();
                    break;
                case TAB_DAMAGE_CALCULATOR:
                    if (sMenuDataPtr->isDoubleBattle) {
                        if (sMenuDataPtr->dmgCalculatorTarget != MAX_TARGETS - 1)
                            sMenuDataPtr->dmgCalculatorTarget++;
                        else
                            sMenuDataPtr->dmgCalculatorTarget = 0;
                        PrintDamageCalulatorTab();
                    }
                    break;
                case TAB_STATUS:
                    sMenuDataPtr->CurrentStatusInfo[sMenuDataPtr->battlerId] =
                        (sMenuDataPtr->CurrentStatusInfo[sMenuDataPtr->battlerId] + 1) % sMenuDataPtr->numStatusInfo[sMenuDataPtr->battlerId];
                    PrintStatusTab();
                    break;
                default:
                    StartSummaryScreenForSpecificMon(taskId);
                    break;
            }
        } else {
            switch (sMenuDataPtr->fieldTabId) {
                case TAB_PARTY:
                    if (!sMenuDataPtr->partySelectorMode) {
                        sMenuDataPtr->partySelectorMode = TRUE;
                        PrintPartyTab();
                    } else {
                        StartSummaryScreen(taskId);
                    }
                    break;
                case TAB_FIELD:
                    sMenuDataPtr->currentFieldInfo = (sMenuDataPtr->currentFieldInfo + 1) % sMenuDataPtr->numFields;
                    PrintFieldTab();
                    break;
                case TAB_PLAYER_SIDE:
                    sMenuDataPtr->currentSideInfoPlayer = (sMenuDataPtr->currentSideInfoPlayer + 1) % sMenuDataPtr->numSideInfoPlayer;
                    PrintSideTab(B_SIDE_PLAYER);
                    break;
                case TAB_ENEMY_SIDE:
                    sMenuDataPtr->currentSideInfoEnemy = (sMenuDataPtr->currentSideInfoEnemy + 1) % sMenuDataPtr->numSideInfoEnemy;
                    PrintSideTab(B_SIDE_OPPONENT);
                    break;
                case TAB_SPEED:
                    // Scrolling since we only have 2 spaces, will be changed in the future, lacks ability to change targets
                    if (sMenuDataPtr->isDoubleBattle) {
                        if (sMenuDataPtr->speedModeId != gBattlersCount - 2)
                            sMenuDataPtr->speedModeId++;
                        else
                            sMenuDataPtr->speedModeId = 0;
                        FreeEveryMonIconSprite();
                        PrintSpeedTab();
                    } else {
                        PlaySE(SE_PC_OFF);
                        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB_BLACK);
                        gTasks[taskId].func = Task_MenuTurnOff;
                    }
                    break;
                default:
                    PlaySE(SE_PC_OFF);
                    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB_BLACK);
                    gTasks[taskId].func = Task_MenuTurnOff;
                    break;
            }
        }
    } else if (JOY_NEW(DPAD_DOWN)) {
        if (sMenuDataPtr->modeId == MODE_FIELD && sMenuDataPtr->fieldTabId == TAB_PARTY && sMenuDataPtr->partySelectorMode) {
            if ((sMenuDataPtr->partyMenuSelectorID_Y) < PARTY_TAB_NUM_MONS_Y - 1)
                sMenuDataPtr->partyMenuSelectorID_Y++;
            else
                sMenuDataPtr->partyMenuSelectorID_Y = 0;
            PrintPartyTab();
        } else {
            if (sMenuDataPtr->isDoubleBattle) {
                if (sMenuDataPtr->modeId != NUM_MODES - 1)
                    sMenuDataPtr->modeId++;
                else
                    sMenuDataPtr->modeId = 0;
            } else {
                if (sMenuDataPtr->modeId != MODE_BATTLER1)
                    sMenuDataPtr->modeId++;
                else
                    sMenuDataPtr->modeId = MODE_BATTLER0;
            }
            setBattler();
            PrintPage();
        }
    } else if (JOY_NEW(DPAD_UP)) {
        if (sMenuDataPtr->modeId == MODE_FIELD && sMenuDataPtr->fieldTabId == TAB_PARTY && sMenuDataPtr->partySelectorMode) {
            if (sMenuDataPtr->partyMenuSelectorID_Y != 0)
                sMenuDataPtr->partyMenuSelectorID_Y--;
            else
                sMenuDataPtr->partyMenuSelectorID_Y = PARTY_TAB_NUM_MONS_Y - 1;
            PrintPartyTab();
        } else {
            if (sMenuDataPtr->isDoubleBattle) {
                if (sMenuDataPtr->modeId != 0)
                    sMenuDataPtr->modeId--;
                else
                    sMenuDataPtr->modeId = NUM_MODES - 1;
            } else {
                if (sMenuDataPtr->modeId != MODE_BATTLER0)
                    sMenuDataPtr->modeId--;
                else
                    sMenuDataPtr->modeId = MODE_BATTLER1;
            }
            setBattler();
            PrintPage();
        }
    } else if (JOY_NEW(DPAD_LEFT)) {
        switch (sMenuDataPtr->modeId) {
            case MODE_FIELD:
                if (sMenuDataPtr->fieldTabId == TAB_PARTY && sMenuDataPtr->partySelectorMode) {
                    if (sMenuDataPtr->partyMenuSelectorID_X != 0)
                        sMenuDataPtr->partyMenuSelectorID_X--;
                    else
                        sMenuDataPtr->partyMenuSelectorID_X = PARTY_TAB_NUM_MONS_X - 1;
                    PrintPartyTab();
                } else {
                    if (sMenuDataPtr->fieldTabId != 0)
                        sMenuDataPtr->fieldTabId--;
                    else
                        sMenuDataPtr->fieldTabId = NUM_FIELD_TABS - 1;
                    PrintPage();
                }
                break;
            default:  // Battlers
                if (sMenuDataPtr->tabId != 0)
                    sMenuDataPtr->tabId--;
                else
                    sMenuDataPtr->tabId = NUM_TABS - 1;
                setBattler();
                PrintPage();
                break;
        }
    } else if (JOY_NEW(DPAD_RIGHT)) {
        switch (sMenuDataPtr->modeId) {
            case MODE_FIELD:
                if (sMenuDataPtr->fieldTabId == TAB_PARTY && sMenuDataPtr->partySelectorMode) {
                    if (sMenuDataPtr->partyMenuSelectorID_X < PARTY_TAB_NUM_MONS_X - 1)
                        sMenuDataPtr->partyMenuSelectorID_X++;
                    else
                        sMenuDataPtr->partyMenuSelectorID_X = 0;
                    PrintPartyTab();
                } else {
                    if (sMenuDataPtr->fieldTabId != NUM_FIELD_TABS - 1)
                        sMenuDataPtr->fieldTabId++;
                    else
                        sMenuDataPtr->fieldTabId = 0;
                    PrintPage();
                }
                break;
            default:  // Battlers
                if (sMenuDataPtr->tabId != NUM_TABS - 1)
                    sMenuDataPtr->tabId++;
                else
                    sMenuDataPtr->tabId = 0;
                setBattler();
                PrintPage();
                break;
        }
    }
}
