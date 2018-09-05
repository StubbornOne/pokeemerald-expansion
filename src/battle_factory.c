#include "global.h"
#include "sprite.h"
#include "event_data.h"
#include "overworld.h"
#include "random.h"
#include "battle_tower.h"
#include "text.h"
#include "palette.h"
#include "task.h"
#include "main.h"
#include "malloc.h"
#include "bg.h"
#include "gpu_regs.h"
#include "string_util.h"
#include "international_string_util.h"
#include "window.h"
#include "data2.h"
#include "decompress.h"
#include "pokemon_summary_screen.h"
#include "sound.h"
#include "pokedex.h"
#include "blend_palette.h"
#include "trainer_pokemon_sprites.h"
#include "constants/battle_frontier.h"
#include "constants/songs.h"

// Select_ refers to the first Pokemon selection screen where you choose 3 Pokemon.
// Swap_   refers to the consecutive selection screen where you can keep your Pokemon or swap one with beaten trainer's.

#define MENU_SUMMARY 0
#define MENU_RENT 1
#define MENU_DESELECT 1
#define MENU_OTHERS 2
#define MENU_OPTIONS_COUNT 3

#define SELECTABLE_MONS_COUNT 6
#define TAG_PAL_BALL_GREY 0x64
#define TAG_PAL_BALL_SELECTED 0x65

struct FactorySelecteableMon
{
    u16 monSetId;
    u16 spriteId;
    u8 selectedId; // 0 - not selected, 1 - first pokemon, 2 - second pokemon, 3 - third pokemon
    struct Pokemon monData;
};

struct UnkFactoryStruct
{
    u8 field0;
    u8 field1;
};

struct FactorySelectMonsStruct
{
    u8 menuCursorPos;
    u8 menuCursor1SpriteId;
    u8 menuCursor2SpriteId;
    u8 cursorPos;
    u8 cursorSpriteId;
    u8 selectingMonsState;
    bool8 fromSummaryScreen;
    u8 yesNoCursorPos;
    u8 unused8;
    struct FactorySelecteableMon mons[SELECTABLE_MONS_COUNT];
    struct UnkFactoryStruct unk294[3];
    u8 unk2A0;
    u8 palBlendTaskId;
    bool8 unk2A2;
    u16 unk2A4;
    bool8 unk2A6;
    u8 unk2A7;
    u8 unk2A8;
    u8 unk2A9;
};

extern u8 (*gUnknown_030062E8)(void);
extern u8 gUnknown_0203CF20;

extern const u16 gBattleFrontierHeldItems[];
extern const struct FacilityMon gBattleFrontierMons[];
extern const struct FacilityMon gSlateportBattleTentMons[];
extern const struct BattleFrontierTrainer gBattleFrontierTrainers[];
extern const u8 gUnknown_085B18AC[];

extern void SetMonMoveAvoidReturn(struct Pokemon *mon, u16 move, u8 moveSlot);

// This file's functions.
static void CB2_InitSelectScreen(void);
static void Select_SetWinRegs(s16 mWin0H, s16 nWin0H, s16 mWin0V, s16 nWin0V);
static void Select_InitMonsData(void);
static void Select_InitAllSprites(void);
static void Select_ShowCheckedMonSprite(void);
static void Select_PrintSelectMonString(void);
static void Select_PrintMonSpecies(void);
static void Select_PrintMonCategory(void);
static void Select_PrintRentalPkmnString(void);
static void Select_CopyMonsToPlayerParty(void);
static void sub_819C4B4(void);
static void Select_ShowYesNoOptions(void);
static void sub_819C568(void);
static void Select_ShowMenuOptions(void);
static void Select_PrintMenuOptions(void);
static void Select_PrintYesNoOptions(void);
static void Task_SelectBlendPalette(u8 taskId);
static void sub_819C1D0(u8 taskId);
static void Task_HandleSelectionScreenChooseMons(u8 taskId);
static void Task_HandleSelectionScreenMenu(u8 taskId);
static void CreateFrontierFactorySelectableMons(u8 firstMonId);
static void CreateTentFactorySelectableMons(u8 firstMonId);
static void Select_SetBallSpritePaletteNum(u8 id);
void sub_819F444(struct UnkFactoryStruct arg0, u8 *arg1);
static void sub_819B958(u8 windowId);
void sub_819F2B4(u8 *arg0, u8 *arg1, u8 arg2);
void sub_819F3F8(struct UnkFactoryStruct arg0, u8 *arg1, u8 arg2);
static u8 Select_RunMenuOptionFunc(void);
static u8 sub_819BC9C(void);
static u8 Select_OptionSummary(void);
static u8 Select_OptionOthers(void);
static u8 Select_OptionRentDeselect(void);
u8 sub_81A6F70(u8 battleMode, u8 lvlMode);
u8 sub_81A6CA8(u8 arg0, u8 arg1);
static bool32 Select_AreSpeciesValid(u16 monSetId);

// Ewram variables
EWRAM_DATA u8 *gUnknown_0203CE2C = NULL;
EWRAM_DATA u8 *gUnknown_0203CE30 = NULL;
EWRAM_DATA u8 *gUnknown_0203CE34 = NULL;
EWRAM_DATA u8 *gUnknown_0203CE38 = NULL;
static EWRAM_DATA struct Pokemon *sFactorySelectMons = NULL;

// IWRAM bss
static IWRAM_DATA struct FactorySelectMonsStruct *sFactorySelectScreen;

// Const rom data.
const u16 gUnknown_0860F13C[] = INCBIN_U16("graphics/unknown/unknown_60F13C.gbapal");
const u16 gUnknown_0860F15C[] = INCBIN_U16("graphics/unknown/unknown_60F15C.gbapal");
const u16 gUnknown_0860F17C[] = INCBIN_U16("graphics/unknown/unknown_60F17C.gbapal");
const u8 gUnknown_0860F1BC[] = INCBIN_U8("graphics/unknown/unknown_60F1BC.4bpp");
const u8 gUnknown_0860F3BC[] = INCBIN_U8("graphics/unknown/unknown_60F3BC.4bpp");
const u8 gUnknown_0860F43C[] = INCBIN_U8("graphics/unknown/unknown_60F43C.4bpp");
const u8 gUnknown_0860F53C[] = INCBIN_U8("graphics/unknown/unknown_60F53C.4bpp");
const u8 gUnknown_0860F63C[] = INCBIN_U8("graphics/unknown/unknown_60F63C.4bpp");
const u8 gUnknown_0860F6BC[] = INCBIN_U8("graphics/unknown/unknown_60F6BC.4bpp");
const u8 gUnknown_0860F7BC[] = INCBIN_U8("graphics/unknown/unknown_60F7BC.4bpp");
const u8 gUnknown_0860F83C[] = INCBIN_U8("graphics/unknown/unknown_60F83C.4bpp");
const u8 gUnknown_0860F93C[] = INCBIN_U8("graphics/unknown/unknown_60F93C.4bpp");
const u8 gUnknown_0860FA3C[] = INCBIN_U8("graphics/unknown/unknown_60FA3C.4bpp");
const u8 gUnknown_0861023C[] = INCBIN_U8("graphics/unknown/unknown_61023C.bin");
const u8 gUnknown_0861033C[] = INCBIN_U8("graphics/unknown/unknown_61033C.4bpp");
const u16 gUnknown_0861039C[] = INCBIN_U16("graphics/unknown/unknown_61039C.gbapal");

const struct SpriteSheet gUnknown_086103BC[] =
{
    {gUnknown_0860F3BC, sizeof(gUnknown_0860F3BC), 0x65},
    {gUnknown_0860F43C, sizeof(gUnknown_0860F43C), 0x66},
    {gUnknown_0860F53C, sizeof(gUnknown_0860F53C), 0x67},
    {gUnknown_0860FA3C, sizeof(gUnknown_0860FA3C), 0x6D},
    {},
};

const struct CompressedSpriteSheet gUnknown_086103E4[] =
{
    {gUnknown_085B18AC, 0x800, 0x64},
    {},
};

const struct SpritePalette gUnknown_086103F4[] =
{
    {gUnknown_0860F13C, 0x64},
    {gUnknown_0860F15C, 0x65},
    {gUnknown_0860F17C, 0x66},
    {gUnknown_0861039C, 0x67},
    {},
};

u8 (* const sSelect_MenuOptionFuncs[])(void) =
{
    [MENU_SUMMARY] = Select_OptionSummary,
    [MENU_RENT] /*Or Deselect*/ = Select_OptionRentDeselect,
    [MENU_OTHERS] = Select_OptionOthers
};

extern const struct BgTemplate gUnknown_08610428[3];
extern const struct WindowTemplate gUnknown_08610434[];
extern const u16 gUnknown_0861046C[];
extern const struct SpriteTemplate gUnknown_086105D8;
extern const struct SpriteTemplate gUnknown_086105F0;
extern const struct SpriteTemplate gUnknown_08610608;
extern const struct SpriteTemplate gUnknown_08610620;
extern const struct SpriteTemplate gUnknown_08610638;
extern const u8 gUnknown_08610479[];
extern const u8 gUnknown_08610476[];

// gfx
extern const u8 gFrontierFactorySelectMenu_Gfx[];
extern const u8 gFrontierFactorySelectMenu_Tilemap[];
extern const u16 gFrontierFactorySelectMenu_Pal[];

// text
extern const u8 gText_RentalPkmn2[];
extern const u8 gText_SelectFirstPkmn[];
extern const u8 gText_SelectSecondPkmn[];
extern const u8 gText_SelectThirdPkmn[];
extern const u8 gText_TheseThreePkmnOkay[];
extern const u8 gText_CantSelectSamePkmn[];
extern const u8 gText_Summary[];
extern const u8 gText_Deselect[];
extern const u8 gText_Rent[];
extern const u8 gText_Others2[];
extern const u8 gText_Yes2[];
extern const u8 gText_No2[];

// code
void sub_819A44C(struct Sprite *sprite)
{
    if (sprite->oam.paletteNum == IndexOfSpritePaletteTag(TAG_PAL_BALL_SELECTED))
    {
        if (sprite->animEnded)
        {
            if (sprite->data[0] != 0)
            {
                sprite->data[0]--;
            }
            else if (Random() % 5 == 0)
            {
                StartSpriteAnim(sprite, 0);
                sprite->data[0] = 32;
            }
            else
            {
                StartSpriteAnim(sprite, 1);
            }
        }
        else
        {
            StartSpriteAnimIfDifferent(sprite, 1);
        }
    }
    else
    {
        StartSpriteAnimIfDifferent(sprite, 0);
    }
}

static void Select_CB2(void)
{
    AnimateSprites();
    BuildOamBuffer();
    RunTextPrinters();
    UpdatePaletteFade();
    RunTasks();
}

static void Select_VblankCb(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

void sub_819A4F8(void)
{
    sFactorySelectScreen = NULL;
    SetMainCallback2(CB2_InitSelectScreen);
}

static void CB2_InitSelectScreen(void)
{
    u8 taskId;

    switch (gMain.state)
    {
    case 0:
        if (sFactorySelectMons != NULL)
            FREE_AND_SET_NULL(sFactorySelectMons);
        SetHBlankCallback(NULL);
        SetVBlankCallback(NULL);
        CpuFill32(0, (void *)VRAM, VRAM_SIZE);
        ResetBgsAndClearDma3BusyFlags(0);
        InitBgsFromTemplates(0, gUnknown_08610428, ARRAY_COUNT(gUnknown_08610428));
        InitWindows(gUnknown_08610434);
        DeactivateAllTextPrinters();
        gMain.state++;
        break;
    case 1:
        gUnknown_0203CE2C = Alloc(0x440);
        gUnknown_0203CE30 = AllocZeroed(0x440);
        gUnknown_0203CE34 = Alloc(0x800);
        gUnknown_0203CE38 = AllocZeroed(0x800);
        ChangeBgX(0, 0, 0);
        ChangeBgY(0, 0, 0);
        ChangeBgX(1, 0, 0);
        ChangeBgY(1, 0, 0);
        ChangeBgX(3, 0, 0);
        ChangeBgY(3, 0, 0);
        SetGpuReg(REG_OFFSET_BLDCNT, 0);
        SetGpuReg(REG_OFFSET_BLDALPHA, 0);
        SetGpuReg(REG_OFFSET_BLDY, 0);
        SetGpuReg(REG_OFFSET_MOSAIC, 0);
        SetGpuReg(REG_OFFSET_WIN0H, 0);
        SetGpuReg(REG_OFFSET_WIN0V, 0);
        SetGpuReg(REG_OFFSET_WIN1H, 0);
        SetGpuReg(REG_OFFSET_WIN1V, 0);
        SetGpuReg(REG_OFFSET_WININ, 0);
        SetGpuReg(REG_OFFSET_WINOUT, 0);
        gMain.state++;
        break;
    case 2:
        ResetPaletteFade();
        ResetSpriteData();
        ResetTasks();
        FreeAllSpritePalettes();
        CpuCopy16(gFrontierFactorySelectMenu_Gfx, gUnknown_0203CE2C, 0x440);
        CpuCopy16(gUnknown_0861033C, gUnknown_0203CE30, 0x60);
        LoadBgTiles(1, gUnknown_0203CE2C, 0x440, 0);
        LoadBgTiles(3, gUnknown_0203CE30, 0x60, 0);
        CpuCopy16(gFrontierFactorySelectMenu_Tilemap, gUnknown_0203CE34, 0x800);
        LoadBgTilemap(1, gUnknown_0203CE34, 0x800, 0);
        LoadPalette(gFrontierFactorySelectMenu_Pal, 0, 0x40);
        LoadPalette(gUnknown_0861046C, 0xF0, 8);
        LoadPalette(gUnknown_0861046C, 0xE0, 10);
        if (sFactorySelectScreen->fromSummaryScreen == TRUE)
            gPlttBufferUnfaded[228] = sFactorySelectScreen->unk2A4;
        LoadPalette(gUnknown_0861039C, 0x20, 4);
        gMain.state++;
        break;
    case 3:
        SetBgTilemapBuffer(3, gUnknown_0203CE38);
        CopyToBgTilemapBufferRect(3, gUnknown_0861023C, 11, 4, 8, 8);
        CopyToBgTilemapBufferRect(3, gUnknown_0861023C,  2, 4, 8, 8);
        CopyToBgTilemapBufferRect(3, gUnknown_0861023C, 20, 4, 8, 8);
        CopyBgTilemapBufferToVram(3);
        gMain.state++;
        break;
    case 4:
        LoadSpritePalettes(gUnknown_086103F4);
        LoadSpriteSheets(gUnknown_086103BC);
        LoadCompressedObjectPic(gUnknown_086103E4);
        ShowBg(0);
        ShowBg(1);
        SetVBlankCallback(Select_VblankCb);
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0x10, 0, 0);
        SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON | DISPCNT_BG0_ON | DISPCNT_BG1_ON | DISPCNT_OBJ_1D_MAP);
        if (sFactorySelectScreen->fromSummaryScreen == TRUE)
        {
            Select_SetWinRegs(88, 152, 32, 96);
            ShowBg(3);
            SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT1_BG3 | BLDCNT_EFFECT_BLEND | BLDCNT_TGT2_BG1 | BLDCNT_TGT2_OBJ);
            SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(11, 4));
        }
        else
        {
            HideBg(3);
        }
        gMain.state++;
        break;
    case 5:
        if (sFactorySelectScreen->fromSummaryScreen == TRUE)
            sFactorySelectScreen->cursorPos = gUnknown_0203CF20;
        Select_InitMonsData();
        Select_InitAllSprites();
        if (sFactorySelectScreen->fromSummaryScreen == TRUE)
            Select_ShowCheckedMonSprite();
        gMain.state++;
        break;
    case 6:
        Select_PrintSelectMonString();
        PutWindowTilemap(2);
        gMain.state++;
        break;
    case 7:
        Select_PrintMonCategory();
        PutWindowTilemap(5);
        gMain.state++;
        break;
    case 8:
        Select_PrintMonSpecies();
        PutWindowTilemap(1);
        gMain.state++;
        break;
    case 9:
        Select_PrintRentalPkmnString();
        PutWindowTilemap(0);
        gMain.state++;
        break;
    case 10:
        sFactorySelectScreen->palBlendTaskId = CreateTask(Task_SelectBlendPalette, 0);
        if (!sFactorySelectScreen->fromSummaryScreen)
        {
            gTasks[sFactorySelectScreen->palBlendTaskId].data[0] = 0;
            taskId = CreateTask(Task_HandleSelectionScreenChooseMons, 0);
            gTasks[taskId].data[0] = 0;
        }
        else
        {
            gTasks[sFactorySelectScreen->palBlendTaskId].data[0] = 1;
            sFactorySelectScreen->unk2A2 = FALSE;
            taskId = CreateTask(Task_HandleSelectionScreenMenu, 0);
            gTasks[taskId].data[0] = 13;
        }
        SetMainCallback2(Select_CB2);
        break;
    }
}

static void Select_InitMonsData(void)
{
    u8 i;

    if (sFactorySelectScreen != NULL)
        return;

    sFactorySelectScreen = AllocZeroed(sizeof(*sFactorySelectScreen));
    sFactorySelectScreen->cursorPos = 0;
    sFactorySelectScreen->selectingMonsState = 1;
    sFactorySelectScreen->fromSummaryScreen = FALSE;
    for (i = 0; i < SELECTABLE_MONS_COUNT; i++)
        sFactorySelectScreen->mons[i].selectedId = 0;

    if (gSaveBlock2Ptr->frontier.lvlMode != FRONTIER_LVL_TENT)
        CreateFrontierFactorySelectableMons(0);
    else
        CreateTentFactorySelectableMons(0);
}

static void Select_InitAllSprites(void)
{
    u8 i, cursorPos;
    s16 x;

    for (i = 0; i < SELECTABLE_MONS_COUNT; i++)
    {
        sFactorySelectScreen->mons[i].spriteId = CreateSprite(&gUnknown_086105D8, (35 * i) + 32, 64, 1);
        gSprites[sFactorySelectScreen->mons[i].spriteId].data[0] = 0;
        Select_SetBallSpritePaletteNum(i);
    }
    cursorPos = sFactorySelectScreen->cursorPos;
    x = gSprites[sFactorySelectScreen->mons[cursorPos].spriteId].pos1.x;
    sFactorySelectScreen->cursorSpriteId = CreateSprite(&gUnknown_086105F0, x, 88, 0);
    sFactorySelectScreen->menuCursor1SpriteId = CreateSprite(&gUnknown_08610608, 176, 112, 0);
    sFactorySelectScreen->menuCursor2SpriteId = CreateSprite(&gUnknown_08610620, 176, 144, 0);

    gSprites[sFactorySelectScreen->menuCursor1SpriteId].invisible = 1;
    gSprites[sFactorySelectScreen->menuCursor2SpriteId].invisible = 1;

    gSprites[sFactorySelectScreen->menuCursor1SpriteId].centerToCornerVecX = 0;
    gSprites[sFactorySelectScreen->menuCursor1SpriteId].centerToCornerVecY = 0;
    gSprites[sFactorySelectScreen->menuCursor2SpriteId].centerToCornerVecX = 0;
    gSprites[sFactorySelectScreen->menuCursor2SpriteId].centerToCornerVecY = 0;
}

static void Select_DestroyAllSprites(void)
{
    u8 i;

    for (i = 0; i < SELECTABLE_MONS_COUNT; i++)
        DestroySprite(&gSprites[sFactorySelectScreen->mons[i].spriteId]);

    DestroySprite(&gSprites[sFactorySelectScreen->cursorSpriteId]);
    DestroySprite(&gSprites[sFactorySelectScreen->menuCursor1SpriteId]);
    DestroySprite(&gSprites[sFactorySelectScreen->menuCursor2SpriteId]);
}

static void Select_UpdateBallCursorPosition(s8 direction)
{
    u8 cursorPos;
    if (direction > 0) // Move cursor right.
    {
        if (sFactorySelectScreen->cursorPos != SELECTABLE_MONS_COUNT - 1)
            sFactorySelectScreen->cursorPos++;
        else
            sFactorySelectScreen->cursorPos = 0;
    }
    else // Move cursor left.
    {
        if (sFactorySelectScreen->cursorPos != 0)
            sFactorySelectScreen->cursorPos--;
        else
            sFactorySelectScreen->cursorPos = SELECTABLE_MONS_COUNT - 1;
    }

    cursorPos = sFactorySelectScreen->cursorPos;
    gSprites[sFactorySelectScreen->cursorSpriteId].pos1.x = gSprites[sFactorySelectScreen->mons[cursorPos].spriteId].pos1.x;
}

static void Select_UpdateMenuCursorPosition(s8 direction)
{
    if (direction > 0) // Move cursor down.
    {
        if (sFactorySelectScreen->menuCursorPos != MENU_OPTIONS_COUNT - 1)
            sFactorySelectScreen->menuCursorPos++;
        else
            sFactorySelectScreen->menuCursorPos = 0;
    }
    else // Move cursor up.
    {
        if (sFactorySelectScreen->menuCursorPos != 0)
            sFactorySelectScreen->menuCursorPos--;
        else
            sFactorySelectScreen->menuCursorPos = MENU_OPTIONS_COUNT - 1;
    }

    gSprites[sFactorySelectScreen->menuCursor1SpriteId].pos1.y = (sFactorySelectScreen->menuCursorPos * 16) + 112;
    gSprites[sFactorySelectScreen->menuCursor2SpriteId].pos1.y = (sFactorySelectScreen->menuCursorPos * 16) + 112;
}

static void Select_UpdateYesNoCursorPosition(s8 direction)
{
    if (direction > 0) // Move cursor down.
    {
        if (sFactorySelectScreen->yesNoCursorPos != 1)
            sFactorySelectScreen->yesNoCursorPos++;
        else
            sFactorySelectScreen->yesNoCursorPos = 0;
    }
    else // Move cursor up.
    {
        if (sFactorySelectScreen->yesNoCursorPos != 0)
            sFactorySelectScreen->yesNoCursorPos--;
        else
            sFactorySelectScreen->yesNoCursorPos = 1;
    }

    gSprites[sFactorySelectScreen->menuCursor1SpriteId].pos1.y = (sFactorySelectScreen->yesNoCursorPos * 16) + 112;
    gSprites[sFactorySelectScreen->menuCursor2SpriteId].pos1.y = (sFactorySelectScreen->yesNoCursorPos * 16) + 112;
}

static void Select_HandleMonSelectionChange(void)
{
    u8 i, paletteNum;
    u8 cursorPos = sFactorySelectScreen->cursorPos;
    if (sFactorySelectScreen->mons[cursorPos].selectedId) // Deselect a mon.
    {
        paletteNum = IndexOfSpritePaletteTag(TAG_PAL_BALL_GREY);
        if (sFactorySelectScreen->selectingMonsState == 3 && sFactorySelectScreen->mons[cursorPos].selectedId == 1)
        {
            for (i = 0; i < SELECTABLE_MONS_COUNT; i++)
            {
                if (sFactorySelectScreen->mons[i].selectedId == 2)
                    break;
            }
            if (i == SELECTABLE_MONS_COUNT)
                return;
            else
                sFactorySelectScreen->mons[i].selectedId = 1;
        }
        sFactorySelectScreen->mons[cursorPos].selectedId = 0;
        sFactorySelectScreen->selectingMonsState--;
    }
    else // Select a mon.
    {
        paletteNum = IndexOfSpritePaletteTag(TAG_PAL_BALL_SELECTED);
        sFactorySelectScreen->mons[cursorPos].selectedId = sFactorySelectScreen->selectingMonsState;
        sFactorySelectScreen->selectingMonsState++;
    }

    gSprites[sFactorySelectScreen->mons[cursorPos].spriteId].oam.paletteNum = paletteNum;
}

static void Select_SetBallSpritePaletteNum(u8 id)
{
    u8 palNum;

    if (sFactorySelectScreen->mons[id].selectedId)
        palNum = IndexOfSpritePaletteTag(TAG_PAL_BALL_SELECTED);
    else
        palNum = IndexOfSpritePaletteTag(TAG_PAL_BALL_GREY);

    gSprites[sFactorySelectScreen->mons[id].spriteId].oam.paletteNum = palNum;
}

static void Task_FromSelectScreenToSummaryScreen(u8 taskId)
{
    u8 i;
    u8 currMonId;

    switch (gTasks[taskId].data[0])
    {
    case 6:
        gPlttBufferUnfaded[228] = gPlttBufferFaded[228];
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 0x10, 0);
        gTasks[taskId].data[0] = 7;
        break;
    case 7:
        if (!gPaletteFade.active)
        {
            DestroyTask(sFactorySelectScreen->palBlendTaskId);
            sub_819F444(sFactorySelectScreen->unk294[1], &sFactorySelectScreen->unk2A0);
            Select_DestroyAllSprites();
            FREE_AND_SET_NULL(gUnknown_0203CE2C);
            FREE_AND_SET_NULL(gUnknown_0203CE30);
            FREE_AND_SET_NULL(gUnknown_0203CE34);
            FREE_AND_SET_NULL(gUnknown_0203CE38);
            FreeAllWindowBuffers();
            gTasks[taskId].data[0] = 8;
        }
        break;
    case 8:
        sFactorySelectScreen->unk2A4 = gPlttBufferUnfaded[228];
        DestroyTask(taskId);
        sFactorySelectScreen->fromSummaryScreen = TRUE;
        currMonId = sFactorySelectScreen->cursorPos;
        sFactorySelectMons = AllocZeroed(sizeof(struct Pokemon) * SELECTABLE_MONS_COUNT);
        for (i = 0; i < SELECTABLE_MONS_COUNT; i++)
            sFactorySelectMons[i] = sFactorySelectScreen->mons[i].monData;
        ShowPokemonSummaryScreen(1, sFactorySelectMons, currMonId, SELECTABLE_MONS_COUNT - 1, CB2_InitSelectScreen);
        break;
    }
}

static void Task_CloseSelectionScreen(u8 taskId)
{
    if (sFactorySelectScreen->unk2A0 != 1)
    {
        switch (gTasks[taskId].data[0])
        {
        case 0:
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 0x10, 0);
            gTasks[taskId].data[0]++;
            break;
        case 1:
            if (!UpdatePaletteFade())
            {
                Select_CopyMonsToPlayerParty();
                DestroyTask(sFactorySelectScreen->palBlendTaskId);
                Select_DestroyAllSprites();
                FREE_AND_SET_NULL(gUnknown_0203CE2C);
                FREE_AND_SET_NULL(gUnknown_0203CE34);
                FREE_AND_SET_NULL(gUnknown_0203CE38);
                FREE_AND_SET_NULL(sFactorySelectScreen);
                FreeAllWindowBuffers();
                SetMainCallback2(CB2_ReturnToFieldContinueScript);
                DestroyTask(taskId);
            }
            break;
        }
    }
}

static void Task_HandleSelectionScreenYesNo(u8 taskId)
{
    if (sFactorySelectScreen->unk2A0 != 1)
    {
        switch (gTasks[taskId].data[0])
        {
        case 10:
            sub_819C4B4();
            gTasks[taskId].data[0] = 4;
            break;
        case 4:
            Select_ShowYesNoOptions();
            gTasks[taskId].data[0] = 5;
            break;
        case 5:
            if (gMain.newKeys & A_BUTTON)
            {
                PlaySE(SE_SELECT);
                if (sFactorySelectScreen->yesNoCursorPos == 0)
                {
                    sub_819C568();
                    gTasks[taskId].data[0] = 0;
                    gTasks[taskId].func = Task_CloseSelectionScreen;
                }
                else
                {
                    sub_819B958(4);
                    sub_819BC9C();
                    sFactorySelectScreen->unk2A2 = TRUE;
                    gTasks[taskId].data[0] = 1;
                    gTasks[taskId].func = Task_HandleSelectionScreenChooseMons;
                }
            }
            else if (gMain.newKeys & B_BUTTON)
            {
                PlaySE(SE_SELECT);
                sub_819B958(4);
                sub_819BC9C();
                sFactorySelectScreen->unk2A2 = TRUE;
                gTasks[taskId].data[0] = 1;
                gTasks[taskId].func = Task_HandleSelectionScreenChooseMons;
            }
            else if (gMain.newAndRepeatedKeys & DPAD_UP)
            {
                PlaySE(SE_SELECT);
                Select_UpdateYesNoCursorPosition(-1);
            }
            else if (gMain.newAndRepeatedKeys & DPAD_DOWN)
            {
                PlaySE(SE_SELECT);
                Select_UpdateYesNoCursorPosition(1);
            }
            break;
        }
    }
}

static void Task_HandleSelectionScreenMenu(u8 taskId)
{
    switch (gTasks[taskId].data[0])
    {
    case 2:
        if (!sFactorySelectScreen->fromSummaryScreen)
            sub_819F2B4(&sFactorySelectScreen->unk294[1].field1, &sFactorySelectScreen->unk2A0, 0);
        gTasks[taskId].data[0] = 9;
        break;
    case 9:
        if (sFactorySelectScreen->unk2A0 != 1)
        {
            Select_ShowMenuOptions();
            sFactorySelectScreen->fromSummaryScreen = FALSE;
            gTasks[taskId].data[0] = 3;
        }
        break;
    case 3:
        if (gMain.newKeys & A_BUTTON)
        {
            u8 retVal;
            PlaySE(SE_SELECT);
            retVal = Select_RunMenuOptionFunc();
            if (retVal == 1)
            {
                sFactorySelectScreen->unk2A2 = TRUE;
                gTasks[taskId].data[0] = 1;
                gTasks[taskId].func = Task_HandleSelectionScreenChooseMons;
            }
            else if (retVal == 2)
            {
                gTasks[taskId].data[0] = 10;
                gTasks[taskId].func = Task_HandleSelectionScreenYesNo;
            }
            else if (retVal == 3)
            {
                gTasks[taskId].data[0] = 11;
                gTasks[taskId].func = Task_HandleSelectionScreenChooseMons;
            }
            else
            {
                gTasks[taskId].data[0] = 6;
                gTasks[taskId].func = Task_FromSelectScreenToSummaryScreen;
            }
        }
        else if (gMain.newKeys & B_BUTTON)
        {
            PlaySE(SE_SELECT);
            sub_819F3F8(sFactorySelectScreen->unk294[1], &sFactorySelectScreen->unk2A0, 0);
            sub_819B958(3);
            sFactorySelectScreen->unk2A2 = TRUE;
            gTasks[taskId].data[0] = 1;
            gTasks[taskId].func = Task_HandleSelectionScreenChooseMons;
        }
        else if (gMain.newAndRepeatedKeys & DPAD_UP)
        {
            PlaySE(SE_SELECT);
            Select_UpdateMenuCursorPosition(-1);
        }
        else if (gMain.newAndRepeatedKeys & DPAD_DOWN)
        {
            PlaySE(SE_SELECT);
            Select_UpdateMenuCursorPosition(1);
        }
        break;
    case 12:
        if (!gPaletteFade.active)
        {
            if (sFactorySelectScreen->fromSummaryScreen == TRUE)
            {
                gPlttBufferFaded[228] = sFactorySelectScreen->unk2A4;
                gPlttBufferUnfaded[228] = gPlttBufferUnfaded[244];
            }
            sFactorySelectScreen->fromSummaryScreen = FALSE;
            gTasks[taskId].data[0] = 3;
        }
        break;
    case 13:
        Select_ShowMenuOptions();
        gTasks[taskId].data[0] = 12;
        break;
    }
}

static void Task_HandleSelectionScreenChooseMons(u8 taskId)
{
    if (sFactorySelectScreen->unk2A0 != 1)
    {
        switch (gTasks[taskId].data[0])
        {
        case 0:
            if (!gPaletteFade.active)
            {
                gTasks[taskId].data[0] = 1;
                sFactorySelectScreen->unk2A2 = TRUE;
            }
            break;
        case 1:
            if (gMain.newKeys & A_BUTTON)
            {
                PlaySE(SE_SELECT);
                sFactorySelectScreen->unk2A2 = FALSE;
                gTasks[taskId].data[0] = 2;
                gTasks[taskId].func = Task_HandleSelectionScreenMenu;
            }
            else if (gMain.newAndRepeatedKeys & DPAD_LEFT)
            {
                PlaySE(SE_SELECT);
                Select_UpdateBallCursorPosition(-1);
                Select_PrintMonCategory();
                Select_PrintMonSpecies();
            }
            else if (gMain.newAndRepeatedKeys & DPAD_RIGHT)
            {
                PlaySE(SE_SELECT);
                Select_UpdateBallCursorPosition(1);
                Select_PrintMonCategory();
                Select_PrintMonSpecies();
            }
            break;
        case 11:
            if (gMain.newKeys & A_BUTTON)
            {
                PlaySE(SE_SELECT);
                sub_819F3F8(sFactorySelectScreen->unk294[1], &sFactorySelectScreen->unk2A0, 0);
                Select_PrintSelectMonString();
                sFactorySelectScreen->unk2A2 = TRUE;
                gTasks[taskId].data[0] = 1;
            }
            break;
        }
    }
}

static void CreateFrontierFactorySelectableMons(u8 firstMonId)
{
    u8 i, j = 0;
    u8 ivs = 0;
    u8 level = 0;
    u8 happiness = 0;
    u32 otId = 0;
    u8 battleMode = VarGet(VAR_FRONTIER_BATTLE_MODE);
    u8 lvlMode = gSaveBlock2Ptr->frontier.lvlMode;
    u8 var_2C = gSaveBlock2Ptr->frontier.field_DE2[battleMode][lvlMode] / 7;
    u8 var_28 = 0;

    gFacilityTrainerMons = gBattleFrontierMons;
    if (gSaveBlock2Ptr->frontier.lvlMode != FRONTIER_LVL_50)
        level = 100;
    else
        level = 50;

    var_28 = sub_81A6F70(battleMode, lvlMode);
    otId = T1_READ_32(gSaveBlock2Ptr->playerTrainerId);

    for (i = 0; i < SELECTABLE_MONS_COUNT; i++)
    {
        u16 monSetId = gSaveBlock2Ptr->frontier.field_E70[i].monId;
        sFactorySelectScreen->mons[i + firstMonId].monSetId = monSetId;
        if (i < var_28)
            ivs = sub_81A6CA8(var_2C + 1, 0);
        else
            ivs = sub_81A6CA8(var_2C, 0);
        CreateMonWithEVSpreadPersonalityOTID(&sFactorySelectScreen->mons[i + firstMonId].monData,
                                             gFacilityTrainerMons[monSetId].species,
                                             level,
                                             gFacilityTrainerMons[monSetId].nature,
                                             ivs,
                                             gFacilityTrainerMons[monSetId].evSpread,
                                             otId);
        happiness = 0;
        for (j = 0; j < 4; j++)
            SetMonMoveAvoidReturn(&sFactorySelectScreen->mons[i + firstMonId].monData, gFacilityTrainerMons[monSetId].moves[j], j);
        SetMonData(&sFactorySelectScreen->mons[i + firstMonId].monData, MON_DATA_FRIENDSHIP, &happiness);
        SetMonData(&sFactorySelectScreen->mons[i + firstMonId].monData, MON_DATA_HELD_ITEM, &gBattleFrontierHeldItems[gFacilityTrainerMons[monSetId].itemTableId]);
    }
}

static void CreateTentFactorySelectableMons(u8 firstMonId)
{
    u8 i, j;
    u8 ivs = 0;
    u8 level = 30;
    u8 happiness = 0;
    u32 otId = 0;

    gFacilityTrainerMons = gSlateportBattleTentMons;
    otId = T1_READ_32(gSaveBlock2Ptr->playerTrainerId);

    for (i = 0; i < SELECTABLE_MONS_COUNT; i++)
    {
        u16 monSetId = gSaveBlock2Ptr->frontier.field_E70[i].monId;
        sFactorySelectScreen->mons[i + firstMonId].monSetId = monSetId;
        CreateMonWithEVSpreadPersonalityOTID(&sFactorySelectScreen->mons[i + firstMonId].monData,
                                             gFacilityTrainerMons[monSetId].species,
                                             level,
                                             gFacilityTrainerMons[monSetId].nature,
                                             ivs,
                                             gFacilityTrainerMons[monSetId].evSpread,
                                             otId);
        happiness = 0;
        for (j = 0; j < 4; j++)
            SetMonMoveAvoidReturn(&sFactorySelectScreen->mons[i + firstMonId].monData, gFacilityTrainerMons[monSetId].moves[j], j);
        SetMonData(&sFactorySelectScreen->mons[i + firstMonId].monData, MON_DATA_FRIENDSHIP, &happiness);
        SetMonData(&sFactorySelectScreen->mons[i + firstMonId].monData, MON_DATA_HELD_ITEM, &gBattleFrontierHeldItems[gFacilityTrainerMons[monSetId].itemTableId]);
    }
}

static void Select_CopyMonsToPlayerParty(void)
{
    u8 i, j;

    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < SELECTABLE_MONS_COUNT; j++)
        {
            if (sFactorySelectScreen->mons[j].selectedId == i + 1)
            {
                gPlayerParty[i] = sFactorySelectScreen->mons[j].monData;
                gSaveBlock2Ptr->frontier.field_E70[i].monId = sFactorySelectScreen->mons[j].monSetId;
                gSaveBlock2Ptr->frontier.field_E70[i].personality = GetMonData(&gPlayerParty[i].box, MON_DATA_PERSONALITY, NULL);
                gSaveBlock2Ptr->frontier.field_E70[i].abilityBit = GetBoxMonData(&gPlayerParty[i].box, MON_DATA_ALT_ABILITY, NULL);
                gSaveBlock2Ptr->frontier.field_E70[i].ivs = GetBoxMonData(&gPlayerParty[i].box, MON_DATA_ATK_IV, NULL);
                break;
            }
        }
    }
    CalculatePlayerPartyCount();
}

static void Select_ShowMenuOptions(void)
{
    if (!sFactorySelectScreen->fromSummaryScreen)
        sFactorySelectScreen->menuCursorPos = 0;

    gSprites[sFactorySelectScreen->menuCursor1SpriteId].pos1.x = 176;
    gSprites[sFactorySelectScreen->menuCursor1SpriteId].pos1.y = (sFactorySelectScreen->menuCursorPos * 16) + 112;
    gSprites[sFactorySelectScreen->menuCursor2SpriteId].pos1.x = 208;
    gSprites[sFactorySelectScreen->menuCursor2SpriteId].pos1.y = (sFactorySelectScreen->menuCursorPos * 16) + 112;

    gSprites[sFactorySelectScreen->menuCursor1SpriteId].invisible = 0;
    gSprites[sFactorySelectScreen->menuCursor2SpriteId].invisible = 0;

    Select_PrintMenuOptions();
}

static void Select_ShowYesNoOptions(void)
{
    sFactorySelectScreen->yesNoCursorPos = 0;

    gSprites[sFactorySelectScreen->menuCursor1SpriteId].pos1.x = 176;
    gSprites[sFactorySelectScreen->menuCursor1SpriteId].pos1.y = 112;
    gSprites[sFactorySelectScreen->menuCursor2SpriteId].pos1.x = 208;
    gSprites[sFactorySelectScreen->menuCursor2SpriteId].pos1.y = 112;

    gSprites[sFactorySelectScreen->menuCursor1SpriteId].invisible = 0;
    gSprites[sFactorySelectScreen->menuCursor2SpriteId].invisible = 0;

    Select_PrintYesNoOptions();
}

static void sub_819B958(u8 windowId)
{
    gSprites[sFactorySelectScreen->menuCursor1SpriteId].invisible = 1;
    gSprites[sFactorySelectScreen->menuCursor2SpriteId].invisible = 1;
    FillWindowPixelBuffer(windowId, 0);
    CopyWindowToVram(windowId, 2);
    ClearWindowTilemap(windowId);
}

static void Select_PrintRentalPkmnString(void)
{
    FillWindowPixelBuffer(0, 0);
    PrintTextOnWindow(0, 1, gText_RentalPkmn2, 2, 1, 0, NULL);
    CopyWindowToVram(0, 3);
}

static void Select_PrintMonSpecies(void)
{
    u16 species;
    u8 x;
    u8 monId = sFactorySelectScreen->cursorPos;

    FillWindowPixelBuffer(1, 0);
    species = GetMonData(&sFactorySelectScreen->mons[monId].monData, MON_DATA_SPECIES, NULL);
    StringCopy(gStringVar4, gSpeciesNames[species]);
    x = GetStringRightAlignXOffset(1, gStringVar4, 86);
    AddTextPrinterParameterized3(1, 1, x, 1, gUnknown_08610479, 0, gStringVar4);
    CopyWindowToVram(1, 2);
}

static void Select_PrintSelectMonString(void)
{
    const u8 *str = NULL;

    FillWindowPixelBuffer(2, 0);
    if (sFactorySelectScreen->selectingMonsState == 1)
        str = gText_SelectFirstPkmn;
    else if (sFactorySelectScreen->selectingMonsState == 2)
        str = gText_SelectSecondPkmn;
    else if (sFactorySelectScreen->selectingMonsState == 3)
        str = gText_SelectThirdPkmn;
    else
        str = gText_TheseThreePkmnOkay;

    PrintTextOnWindow(2, 1, str, 2, 5, 0, NULL);
    CopyWindowToVram(2, 2);
}

static void Select_PrintCantSelectSameMon(void)
{
    FillWindowPixelBuffer(2, 0);
    PrintTextOnWindow(2, 1, gText_CantSelectSamePkmn, 2, 5, 0, NULL);
    CopyWindowToVram(2, 2);
}

static void Select_PrintMenuOptions(void)
{
    u8 selectedId = sFactorySelectScreen->mons[sFactorySelectScreen->cursorPos].selectedId;

    PutWindowTilemap(3);
    FillWindowPixelBuffer(3, 0);
    AddTextPrinterParameterized3(3, 1, 7, 1, gUnknown_08610476, 0, gText_Summary);
    if (selectedId != 0)
        AddTextPrinterParameterized3(3, 1, 7, 17, gUnknown_08610476, 0, gText_Deselect);
    else
        AddTextPrinterParameterized3(3, 1, 7, 17, gUnknown_08610476, 0, gText_Rent);

    AddTextPrinterParameterized3(3, 1, 7, 33, gUnknown_08610476, 0, gText_Others2);
    CopyWindowToVram(3, 3);
}

static void Select_PrintYesNoOptions(void)
{
    PutWindowTilemap(4);
    FillWindowPixelBuffer(4, 0);
    AddTextPrinterParameterized3(4, 1, 7, 1, gUnknown_08610476, 0, gText_Yes2);
    AddTextPrinterParameterized3(4, 1, 7, 17, gUnknown_08610476, 0, gText_No2);
    CopyWindowToVram(4, 3);
}

static u8 Select_RunMenuOptionFunc(void)
{
    gUnknown_030062E8 = sSelect_MenuOptionFuncs[sFactorySelectScreen->menuCursorPos];
    return gUnknown_030062E8();
}

static u8 Select_OptionRentDeselect(void)
{
    u8 selectedId = sFactorySelectScreen->mons[sFactorySelectScreen->cursorPos].selectedId;
    u16 monSetId  = sFactorySelectScreen->mons[sFactorySelectScreen->cursorPos].monSetId;
    if (selectedId == 0 && !Select_AreSpeciesValid(monSetId))
    {
        Select_PrintCantSelectSameMon();
        sub_819B958(3);
        return 3;
    }
    else
    {
        sub_819F3F8(sFactorySelectScreen->unk294[1], &sFactorySelectScreen->unk2A0, 0);
        Select_HandleMonSelectionChange();
        Select_PrintSelectMonString();
        sub_819B958(3);
        if (sFactorySelectScreen->selectingMonsState > 3)
            return 2;
        else
            return 1;
    }
}

static u8 sub_819BC9C(void)
{
    sub_819C568();
    Select_HandleMonSelectionChange();
    Select_PrintSelectMonString();
    sub_819B958(3);
    if (sFactorySelectScreen->selectingMonsState > 3)
        return 2;
    else
        return 1;
}

static u8 Select_OptionSummary(void)
{
    return 0;
}

static u8 Select_OptionOthers(void)
{
    sub_819F3F8(sFactorySelectScreen->unk294[1], &sFactorySelectScreen->unk2A0, 0);
    sub_819B958(3);
    return 1;
}

static void Select_PrintMonCategory(void)
{
    u16 species;
    u8 text[30];
    u8 x;
    u8 monId = sFactorySelectScreen->cursorPos;
    if (monId < SELECTABLE_MONS_COUNT)
    {
        PutWindowTilemap(5);
        FillWindowPixelBuffer(5, 0);
        species = GetMonData(&sFactorySelectScreen->mons[monId].monData, MON_DATA_SPECIES, NULL);
        CopyMonCategoryText(SpeciesToNationalPokedexNum(species), text);
        x = GetStringRightAlignXOffset(1, text, 0x76);
        PrintTextOnWindow(5, 1, text, x, 1, 0, NULL);
        CopyWindowToVram(5, 2);
    }
}

void sub_819BD70(void)
{
    u8 monId = sFactorySelectScreen->cursorPos;
    struct Pokemon *mon = &sFactorySelectScreen->mons[monId].monData;
    u16 species = GetMonData(mon, MON_DATA_SPECIES, NULL);
    u32 personality = GetMonData(mon, MON_DATA_PERSONALITY, NULL);
    u32 otId = GetMonData(mon, MON_DATA_OT_ID, NULL);

    sFactorySelectScreen->unk294[1].field0 = CreateMonPicSprite_HandleDeoxys(species, otId, personality, TRUE, 88, 32, 15, 0xFFFF);
    gSprites[sFactorySelectScreen->unk294[1].field0].centerToCornerVecX = 0;
    gSprites[sFactorySelectScreen->unk294[1].field0].centerToCornerVecY = 0;

    sFactorySelectScreen->unk2A0 = 0;
}

void sub_819BE20(u8 arg0)
{
    sFactorySelectScreen->unk2A0 = arg0;
}

static void Select_ShowCheckedMonSprite(void)
{
    struct Pokemon *mon;
    u16 species;
    u32 personality, otId;

    sFactorySelectScreen->unk294[1].field1 = CreateSprite(&gUnknown_08610638, 120, 64, 1);
    StartSpriteAffineAnim(&gSprites[sFactorySelectScreen->unk294[1].field1], 2);

    mon = &sFactorySelectScreen->mons[sFactorySelectScreen->cursorPos].monData;
    species = GetMonData(mon, MON_DATA_SPECIES, NULL);
    personality = GetMonData(mon, MON_DATA_PERSONALITY, NULL);
    otId = GetMonData(mon, MON_DATA_OT_ID, NULL);

    sFactorySelectScreen->unk294[1].field0 = CreateMonPicSprite_HandleDeoxys(species, otId, personality, TRUE, 88, 32, 15, 0xFFFF);
    gSprites[sFactorySelectScreen->unk294[1].field0].centerToCornerVecX = 0;
    gSprites[sFactorySelectScreen->unk294[1].field0].centerToCornerVecY = 0;

    gSprites[sFactorySelectScreen->unk294[1].field1].invisible = 1;
}

static void Select_ShowChosenMonsSprites(void)
{
    u8 i, j;

    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < SELECTABLE_MONS_COUNT; j++)
        {
            if (sFactorySelectScreen->mons[j].selectedId == i + 1)
            {
                struct Pokemon *mon = &sFactorySelectScreen->mons[j].monData;
                u16 species = GetMonData(mon, MON_DATA_SPECIES, NULL);
                u32 personality = GetMonData(mon, MON_DATA_PERSONALITY, NULL);
                u32 otId = GetMonData(mon, MON_DATA_OT_ID, NULL);

                sFactorySelectScreen->unk294[i].field0 = CreateMonPicSprite_HandleDeoxys(species, otId, personality, TRUE, (i * 72) + 16, 32, i + 13, 0xFFFF);
                gSprites[sFactorySelectScreen->unk294[i].field0].centerToCornerVecX = 0;
                gSprites[sFactorySelectScreen->unk294[i].field0].centerToCornerVecY = 0;
                break;
            }
        }
    }
    sFactorySelectScreen->unk2A0 = 0;
}

static void sub_819C040(struct Sprite *sprite)
{
    u8 taskId;

    if (sprite->affineAnimEnded
        && gSprites[sFactorySelectScreen->unk294[0].field1].affineAnimEnded
        && gSprites[sFactorySelectScreen->unk294[2].field1].affineAnimEnded)
    {
        sprite->invisible = 1;
        gSprites[sFactorySelectScreen->unk294[0].field1].invisible = 1;
        gSprites[sFactorySelectScreen->unk294[2].field1].invisible = 1;

        taskId = CreateTask(sub_819C1D0, 1);
        gTasks[taskId].func(taskId);

        sprite->callback = SpriteCallbackDummy;
    }
}

static void sub_819C100(struct Sprite *sprite)
{
    if (sprite->affineAnimEnded
        && gSprites[sFactorySelectScreen->unk294[0].field1].affineAnimEnded
        && gSprites[sFactorySelectScreen->unk294[2].field1].affineAnimEnded)
    {
        FreeOamMatrix(sprite->oam.matrixNum);
        FreeOamMatrix(gSprites[sFactorySelectScreen->unk294[0].field1].oam.matrixNum);
        FreeOamMatrix(gSprites[sFactorySelectScreen->unk294[2].field1].oam.matrixNum);

        sFactorySelectScreen->unk2A0 = 0;

        DestroySprite(&gSprites[sFactorySelectScreen->unk294[0].field1]);
        DestroySprite(&gSprites[sFactorySelectScreen->unk294[2].field1]);
        DestroySprite(sprite);
    }
}

static void sub_819C1D0(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    switch (task->data[0])
    {
    case 0:
        task->data[3] = 16;
        task->data[24] = 224; // BUG: writing outside the array's bounds.
        task->data[5] = 64;
        task->data[8] = 65;
        SetGpuRegBits(REG_OFFSET_DISPCNT, DISPCNT_WIN0_ON);
        SetGpuReg(REG_OFFSET_WIN0H, WINHV_COORDS(task->data[3], task->data[24]));
        SetGpuReg(REG_OFFSET_WIN0V, WINHV_COORDS(task->data[5], task->data[8]));
        SetGpuReg(REG_OFFSET_WININ, WININ_WIN0_BG_ALL | WININ_WIN0_CLR | WININ_WIN0_OBJ);
        SetGpuReg(REG_OFFSET_WINOUT, WINOUT_WIN01_BG0 | WINOUT_WIN01_BG1 | WINOUT_WIN01_BG2 | WINOUT_WIN01_CLR | WINOUT_WIN01_OBJ);
        break;
    case 1:
        ShowBg(3);
        SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT1_BG3 | BLDCNT_EFFECT_BLEND | BLDCNT_TGT2_BG1 | BLDCNT_TGT2_OBJ);
        SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(11, 4));
        break;
    case 2:
        task->data[5] -= 4;
        task->data[8] += 4;
        if (task->data[5] <= 32 || task->data[8] >= 96)
        {
            task->data[5] = 32;
            task->data[8] = 96;
            ClearGpuRegBits(REG_OFFSET_DISPCNT, DISPCNT_WIN0_ON);
        }
        SetGpuReg(REG_OFFSET_WIN0V, WINHV_COORDS(task->data[5], task->data[8]));
        if (task->data[5] != 32)
            return;
        break;
    default:
        DestroyTask(taskId);
        Select_ShowChosenMonsSprites();
        return;
    }
    task->data[0]++;
}

static void sub_819C2D4(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    switch (task->data[0])
    {
    default:
        HideBg(3);
        gSprites[sFactorySelectScreen->unk294[1].field1].invisible = 0;
        gSprites[sFactorySelectScreen->unk294[1].field1].callback = sub_819C100;
        gSprites[sFactorySelectScreen->unk294[0].field1].invisible = 0;
        gSprites[sFactorySelectScreen->unk294[0].field1].callback = SpriteCallbackDummy;
        gSprites[sFactorySelectScreen->unk294[2].field1].invisible = 0;
        gSprites[sFactorySelectScreen->unk294[2].field1].callback = SpriteCallbackDummy;
        StartSpriteAffineAnim(&gSprites[sFactorySelectScreen->unk294[1].field1], 1);
        StartSpriteAffineAnim(&gSprites[sFactorySelectScreen->unk294[0].field1], 1);
        StartSpriteAffineAnim(&gSprites[sFactorySelectScreen->unk294[2].field1], 1);
        ClearGpuRegBits(REG_OFFSET_DISPCNT, DISPCNT_WIN0_ON);
        DestroyTask(taskId);
        break;
    case 0:
        task->data[3] = 16;
        task->data[24] = 224; // BUG: writing outside the array's bounds.
        task->data[5] = 32;
        task->data[8] = 96;
        SetGpuRegBits(REG_OFFSET_DISPCNT, DISPCNT_WIN0_ON);
        SetGpuReg(REG_OFFSET_WIN0H, WINHV_COORDS(task->data[3], task->data[24]));
        SetGpuReg(REG_OFFSET_WIN0V, WINHV_COORDS(task->data[5], task->data[8]));
        SetGpuReg(REG_OFFSET_WININ, WININ_WIN0_BG_ALL | WININ_WIN0_CLR | WININ_WIN0_OBJ);
        SetGpuReg(REG_OFFSET_WINOUT, WINOUT_WIN01_BG0 | WINOUT_WIN01_BG1 | WINOUT_WIN01_BG2 | WINOUT_WIN01_CLR | WINOUT_WIN01_OBJ);
        task->data[0]++;
        break;
    case 1:
        task->data[5] += 4;
        task->data[8] -= 4;
        if (task->data[5] >= 64 || task->data[8] <= 65)
        {
            task->data[5] = 64;
            task->data[8] = 65;
        }
        SetGpuReg(REG_OFFSET_WIN0V, WINHV_COORDS(task->data[5], task->data[8]));
        if (task->data[5] == 64)
            task->data[0]++;
        break;
    }
}

static void sub_819C4B4(void)
{
    sFactorySelectScreen->unk294[1].field1 = CreateSprite(&gUnknown_08610638, 120, 64, 1);
    sFactorySelectScreen->unk294[0].field1 = CreateSprite(&gUnknown_08610638,  44, 64, 1);
    sFactorySelectScreen->unk294[2].field1 = CreateSprite(&gUnknown_08610638, 196, 64, 1);

    gSprites[sFactorySelectScreen->unk294[1].field1].callback = sub_819C040;
    gSprites[sFactorySelectScreen->unk294[0].field1].callback = SpriteCallbackDummy;
    gSprites[sFactorySelectScreen->unk294[2].field1].callback = SpriteCallbackDummy;

    sFactorySelectScreen->unk2A0 = 1;
}

static void sub_819C568(void)
{
    u8 taskId;

    FreeAndDestroyMonPicSprite(sFactorySelectScreen->unk294[0].field0);
    FreeAndDestroyMonPicSprite(sFactorySelectScreen->unk294[1].field0);
    FreeAndDestroyMonPicSprite(sFactorySelectScreen->unk294[2].field0);

    taskId = CreateTask(sub_819C2D4, 1);
    gTasks[taskId].func(taskId);

    sFactorySelectScreen->unk2A0 = 1;
}

static void Select_SetWinRegs(s16 mWin0H, s16 nWin0H, s16 mWin0V, s16 nWin0V)
{
    SetGpuRegBits(REG_OFFSET_DISPCNT, DISPCNT_WIN0_ON);
    SetGpuReg(REG_OFFSET_WIN0H, WINHV_COORDS(mWin0H, nWin0H));
    SetGpuReg(REG_OFFSET_WIN0V, WINHV_COORDS(mWin0V, nWin0V));
    SetGpuReg(REG_OFFSET_WININ, WININ_WIN0_BG_ALL | WININ_WIN0_CLR | WININ_WIN0_OBJ);
    SetGpuReg(REG_OFFSET_WINOUT, WINOUT_WIN01_BG0 | WINOUT_WIN01_BG1 | WINOUT_WIN01_BG2 | WINOUT_WIN01_CLR | WINOUT_WIN01_OBJ);
}

static bool32 Select_AreSpeciesValid(u16 monSetId)
{
    u8 i, j;
    u32 species = gFacilityTrainerMons[monSetId].species;
    u8 selectState = sFactorySelectScreen->selectingMonsState;

    for (i = 1; i < selectState; i++)
    {
        for (j = 0; j < SELECTABLE_MONS_COUNT; j++)
        {
            if (sFactorySelectScreen->mons[j].selectedId == i)
            {
                if (gFacilityTrainerMons[sFactorySelectScreen->mons[j].monSetId].species == species)
                    return FALSE;

                break;
            }
        }
    }

    return TRUE;
}

static void Task_SelectBlendPalette(u8 taskId)
{
    switch (gTasks[taskId].data[0])
    {
    case 0:
        sFactorySelectScreen->unk2A7 = 0;
        sFactorySelectScreen->unk2A8 = 0;
        sFactorySelectScreen->unk2A6 = TRUE;
        gTasks[taskId].data[0] = 1;
        break;
    case 1:
        if (sFactorySelectScreen->unk2A2)
        {
            if (sFactorySelectScreen->unk2A9)
            {
                gTasks[taskId].data[0] = 2;
            }
            else
            {
                sFactorySelectScreen->unk2A7++;
                if (sFactorySelectScreen->unk2A7 > 6)
                {
                    sFactorySelectScreen->unk2A7 = 0;
                    if (!sFactorySelectScreen->unk2A6)
                        sFactorySelectScreen->unk2A8--;
                    else
                        sFactorySelectScreen->unk2A8++;
                }
                BlendPalettes(0x4000, sFactorySelectScreen->unk2A8, 0);
                if (sFactorySelectScreen->unk2A8 > 5)
                {
                    sFactorySelectScreen->unk2A6 = FALSE;
                }
                else if (sFactorySelectScreen->unk2A8 == 0)
                {
                    gTasks[taskId].data[0] = 2;
                    sFactorySelectScreen->unk2A6 = TRUE;
                }
            }
        }
        break;
    case 2:
        if (sFactorySelectScreen->unk2A9 > 14)
        {
            sFactorySelectScreen->unk2A9 = 0;
            gTasks[taskId].data[0] = 1;
        }
        else
        {
            sFactorySelectScreen->unk2A9++;
        }
        break;
    }
}
