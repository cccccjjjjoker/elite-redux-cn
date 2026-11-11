#include "global.h"
#include "berry.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "event_scripts.h"
#include "field_control_avatar.h"
#include "fieldmap.h"
#include "item.h"
#include "item_menu.h"
#include "main.h"
#include "random.h"
#include "string_util.h"
#include "text.h"
#include "constants/event_object_movement.h"
#include "constants/items.h"
#include "wild_encounter.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"

static u32 GetEnigmaBerryChecksum(struct EnigmaBerry *enigmaBerry);
static bool32 BerryTreeGrow(struct BerryTree *tree);
static u16 BerryTypeToItemId(u16 berry);
static u8 BerryTreeGetNumStagesWatered(struct BerryTree *tree);
static u8 GetNumStagesWateredByBerryTreeId(u8 id);
static u8 CalcBerryYieldInternal(u16 max, u16 min, u8 water);
static u8 CalcBerryYield(struct BerryTree *tree);
static u8 GetBerryCountByBerryTreeId(u8 id);
static u16 GetStageDurationByBerryType(u8);


//.rodata
static const u8 sBerryDescriptionPart1_Cheri[] = _("Blooms with delicate pretty flowers.");
static const u8 sBerryDescriptionPart2_Cheri[] = _("鲜红的树果\n非常辛辣。");
static const u8 sBerryDescriptionPart1_Chesto[] = _("这种树果的厚\n皮与果肉都");
static const u8 sBerryDescriptionPart2_Chesto[] = _("非常坚硬。整\n体口感干涩。");
static const u8 sBerryDescriptionPart1_Pecha[] = _("可以得到。味\n道十分美味。");
static const u8 sBerryDescriptionPart2_Pecha[] = _("同时也非常娇嫩\n--请小心拿放。");
static const u8 sBerryDescriptionPart1_Rawst[] = _("若叶片长得\n又长又卷，");
static const u8 sBerryDescriptionPart2_Rawst[] = _("来的话树果似乎\n会变得非常苦。");
static const u8 sBerryDescriptionPart1_Aspear[] = _("坚硬的树果\n饱含浓郁的");
static const u8 sBerryDescriptionPart2_Aspear[] = _("果汁。味道相当酸。");
static const u8 sBerryDescriptionPart1_Leppa[] = _("比樱子果等\n生长得慢。");
static const u8 sBerryDescriptionPart2_Leppa[] = _("缓慢，果实越\n小的越好吃。");
static const u8 sBerryDescriptionPart1_Oran[] = _("混杂着各种味道\n的神奇树果，");
static const u8 sBerryDescriptionPart2_Oran[] = _("半天即可结\n出树果。");
static const u8 sBerryDescriptionPart1_Persim[] = _("最喜欢晒太阳，\n只要一照到");
static const u8 sBerryDescriptionPart2_Persim[] = _("晒到阳光后会长\n得格外鲜艳。");
static const u8 sBerryDescriptionPart1_Lum[] = _("生长缓慢。\n若悉心照料，");
static const u8 sBerryDescriptionPart2_Lum[] = _("有可能结出\n两颗树果。");
static const u8 sBerryDescriptionPart1_Sitrus[] = _("与零余果近\n缘。硕大的");
static const u8 sBerryDescriptionPart2_Sitrus[] = _("是颇受好评\n的树果。");
static const u8 sBerryDescriptionPart1_Figy[] = _("这种树果看\n似被啃过，");
static const u8 sBerryDescriptionPart2_Figy[] = _("却满是辛辣\n的成分。");
static const u8 sBerryDescriptionPart1_Wiki[] = _("据说这种树果\n长得凹凸不平");
static const u8 sBerryDescriptionPart2_Wiki[] = _("to help Pokémon grip it.");
static const u8 sBerryDescriptionPart1_Mago[] = _("这种树果会随着\n生长变得弯曲。");
static const u8 sBerryDescriptionPart2_Mago[] = _("越弯曲，就越\n甜美好吃。");
static const u8 sBerryDescriptionPart1_Aguav[] = _("花朵纤巧。\n它罕有的");
static const u8 sBerryDescriptionPart2_Aguav[] = _("无需光照也能\n生长的特性。");
static const u8 sBerryDescriptionPart1_Iapapa[] = _("非常大且味道\n很酸的树果。");
static const u8 sBerryDescriptionPart2_Iapapa[] = _("至少要一天\n才能长成。");
static const u8 sBerryDescriptionPart1_Razz[] = _("红色的树果\n略带辛辣。");
static const u8 sBerryDescriptionPart2_Razz[] = _("只需四小时便\n迅速长成。");
static const u8 sBerryDescriptionPart1_Bluk[] = _("这种树果外皮\n呈蓝色，但");
static const u8 sBerryDescriptionPart2_Bluk[] = _("吃下去会把\n嘴巴染黑。");
static const u8 sBerryDescriptionPart1_Nanab[] = _("这种树果是世\n界上第七个");
static const u8 sBerryDescriptionPart2_Nanab[] = _("被发现的树果。\n味道甘甜。");
static const u8 sBerryDescriptionPart1_Wepear[] = _("花朵小巧洁\n白。它有着");
static const u8 sBerryDescriptionPart2_Wepear[] = _("苦与酸的绝\n妙平衡。");
static const u8 sBerryDescriptionPart1_Pinap[] = _("不抗风、不耐寒。");
static const u8 sBerryDescriptionPart2_Pinap[] = _("果实会辣，\n外皮则很酸。");
static const u8 sBerryDescriptionPart1_Pomeg[] = _("无论浇多少水，");
static const u8 sBerryDescriptionPart2_Pomeg[] = _("最多也只能结\n出六颗树果。");
static const u8 sBerryDescriptionPart1_Kelpsy[] = _("稀有品种，形\n状如同根茎。");
static const u8 sBerryDescriptionPart2_Kelpsy[] = _("会开出非常\n大的花。");
static const u8 sBerryDescriptionPart1_Qualot[] = _("最喜欢水。\n即使是在一直");
static const u8 sBerryDescriptionPart2_Qualot[] = _("生长在常年降\n雨的地方。");
static const u8 sBerryDescriptionPart1_Hondew[] = _("非常珍贵且");
static const u8 sBerryDescriptionPart2_Hondew[] = _("罕见的树果。\n十分美味。");
static const u8 sBerryDescriptionPart1_Grepa[] = _("尽管它柔嫩\n而浑圆，");
static const u8 sBerryDescriptionPart2_Grepa[] = _("酸味却超乎想象。");
static const u8 sBerryDescriptionPart1_Tamato[] = _("这种树果辛辣到\n让嘴唇发麻。");
static const u8 sBerryDescriptionPart2_Tamato[] = _("需要时间才能长成。");
static const u8 sBerryDescriptionPart1_Cornn[] = _("来自远古的\n树果。若不");
static const u8 sBerryDescriptionPart2_Cornn[] = _("大量种植可能\n不会生长。");
static const u8 sBerryDescriptionPart1_Magost[] = _("普遍认为这种树果");
static const u8 sBerryDescriptionPart2_Magost[] = _("风味极为均衡。");
static const u8 sBerryDescriptionPart1_Rabuta[] = _("稀有品种，\n表面长满");
static const u8 sBerryDescriptionPart2_Rabuta[] = _("绒毛。味道\n相当苦。");
static const u8 sBerryDescriptionPart1_Nomel[] = _("相当酸。只咬\n一口就让人");
static const u8 sBerryDescriptionPart2_Nomel[] = _("3天内将尝不出\n其它味道。");
static const u8 sBerryDescriptionPart1_Spelon[] = _("艳红的树果\n非常辛辣。");
static const u8 sBerryDescriptionPart2_Spelon[] = _("它表面的疙瘩会\n分泌辛辣物质。");
static const u8 sBerryDescriptionPart1_Pamtre[] = _("从某处漂流\n于海面。");
static const u8 sBerryDescriptionPart2_Pamtre[] = _("据推测它生\n长在别处。");
static const u8 sBerryDescriptionPart1_Watmel[] = _("巨大的树果，有\n些甚至超过 20");
static const u8 sBerryDescriptionPart2_Watmel[] = _("50公分以上的。\n味道非常的甜。");
static const u8 sBerryDescriptionPart1_Durin[] = _("光看着就觉得\n苦。它苦得");
static const u8 sBerryDescriptionPart2_Durin[] = _("从来没有人直\n接生吃过。");
static const u8 sBerryDescriptionPart1_Belue[] = _("它油亮诱人，但");
static const u8 sBerryDescriptionPart2_Belue[] = _("却酸得吓人。\n需要时间生长。");
static const u8 sBerryDescriptionPart1_Liechi[] = _("神秘的树果。\n据传蕴含");
static const u8 sBerryDescriptionPart2_Liechi[] = _("大海的力量。");
static const u8 sBerryDescriptionPart1_Ganlon[] = _("神秘的树果。\n据传蕴含");
static const u8 sBerryDescriptionPart2_Ganlon[] = _("大地的力量。");
static const u8 sBerryDescriptionPart1_Salac[] = _("神秘的树果。\n据传蕴含");
static const u8 sBerryDescriptionPart2_Salac[] = _("天空的力量。");
static const u8 sBerryDescriptionPart1_Petaya[] = _("神秘的树果。\n据传蕴含");
static const u8 sBerryDescriptionPart2_Petaya[] = _("万物的力量。");
static const u8 sBerryDescriptionPart1_Apicot[] = _("非常神秘的种\n子。使用它");
static const u8 sBerryDescriptionPart2_Apicot[] = _("的时候不知道\n会发生什么。");
static const u8 sBerryDescriptionPart1_Lansat[] = _("据说是传说\n中的树果。");
static const u8 sBerryDescriptionPart2_Lansat[] = _("据说带在身上\n会带来喜悦。");
static const u8 sBerryDescriptionPart1_Starf[] = _("效力过强，\n以致被弃于");
static const u8 sBerryDescriptionPart2_Starf[] = _("被丢弃在世界尽\n头的梦幻树果。");
static const u8 sBerryDescriptionPart1_Enigma[] = _("完全谜样的树果，");
static const u8 sBerryDescriptionPart2_Enigma[] = _("似乎拥有星\n辰的力量。");

static const u8 sBerryDescriptionPart1_Occa[] = _("据说曾生长\n在热带，");
static const u8 sBerryDescriptionPart2_Occa[] = _("以极其火辣的\n辛辣著称。");
static const u8 sBerryDescriptionPart1_Passho[] = _("果肉上布满\n许多细小");
static const u8 sBerryDescriptionPart2_Passho[] = _("气泡，使它得\n以浮在水面。");
static const u8 sBerryDescriptionPart1_Wacan[] = _("从落雷中汲\n取的能量");
static const u8 sBerryDescriptionPart2_Wacan[] = _("让这种树果长\n得硕大饱满。");
static const u8 sBerryDescriptionPart1_Rindo[] = _("这种树果带有蔬\n菜般的风味，");
static const u8 sBerryDescriptionPart2_Rindo[] = _("却富含益于健康\n的膳食纤维。");
static const u8 sBerryDescriptionPart1_Yache[] = _("这种树果清\n爽、干涩又");
static const u8 sBerryDescriptionPart2_Yache[] = _("带酸味。冰镇\n后更美味。");
static const u8 sBerryDescriptionPart1_Chople[] = _("含有能够产生");
static const u8 sBerryDescriptionPart2_Chople[] = _("热量的物质。甚至\n能温暖冰冷的心。");
static const u8 sBerryDescriptionPart1_Kebia[] = _("外表鲜绿，内部");
static const u8 sBerryDescriptionPart2_Kebia[] = _("塞满了乌黑\n的果肉。");
static const u8 sBerryDescriptionPart1_Shuca[] = _("香甜的果肉\n中带着一丝");
static const u8 sBerryDescriptionPart2_Shuca[] = _("硬朗而芳香\n的咬劲。");
static const u8 sBerryDescriptionPart1_Coba[] = _("据说这种树果是");
static const u8 sBerryDescriptionPart2_Coba[] = _("由远方吹来的两种\n树果杂交而成。");
static const u8 sBerryDescriptionPart1_Payapa[] = _("据说能感知人的\n情绪，会鼓起");
static const u8 sBerryDescriptionPart2_Payapa[] = _("圆圆的身子，\n当有人靠近时。");
static const u8 sBerryDescriptionPart1_Tanga[] = _("顶端会长出一\n朵花来引诱");
static const u8 sBerryDescriptionPart2_Tanga[] = _("Bug Pokémon with its stringy petals.");
static const u8 sBerryDescriptionPart1_Charti[] = _("常因其");
static const u8 sBerryDescriptionPart2_Charti[] = _("干爽口感适合腌渍。\n有时也生吃。");
static const u8 sBerryDescriptionPart1_Kasib[] = _("古老的迷信说它\n有一种奇异的");
static const u8 sBerryDescriptionPart2_Kasib[] = _("力量。是很受欢\n迎的护身符。");
static const u8 sBerryDescriptionPart1_Haban[] = _("若将足量的\n这种树果");
static const u8 sBerryDescriptionPart2_Haban[] = _("熬煮后苦味减轻。\n适合做果酱。");
static const u8 sBerryDescriptionPart1_Colbur[] = _("表面的小钩子\n会牢牢勾住");
static const u8 sBerryDescriptionPart2_Colbur[] = _("Pokémon to reach far-off places.");
static const u8 sBerryDescriptionPart1_Babiri[] = _("非常坚硬且\n风味浓烈。");
static const u8 sBerryDescriptionPart2_Babiri[] = _("过去曾被用\n作药材。");
static const u8 sBerryDescriptionPart1_Chilan[] = _("它可以被做成\n哨子，吹出");
static const u8 sBerryDescriptionPart2_Chilan[] = _("难以形容的声音。");
static const u8 sBerryDescriptionPart1_Roseli[] = _("在野外会长成\n宽阔的环状，");
static const u8 sBerryDescriptionPart2_Roseli[] = _("原因至今不明。");
static const u8 sBerryDescriptionPart1_Micle[] = _("它有着让同时吃\n下的其他食物");
static const u8 sBerryDescriptionPart2_Micle[] = _("尝起来变甜\n的效果。");
static const u8 sBerryDescriptionPart1_Custap[] = _("释迦果外皮\n之下的果肉");
static const u8 sBerryDescriptionPart2_Custap[] = _("香甜而柔滑细腻。");
static const u8 sBerryDescriptionPart1_Jaboca[] = _("构成这种树\n果的小核果");
static const u8 sBerryDescriptionPart2_Jaboca[] = _("若被粗暴对待会\n有节奏地爆开。");
static const u8 sBerryDescriptionPart1_Rowap[] = _("人们曾把这种树果\n削出陀螺状的块");
static const u8 sBerryDescriptionPart2_Rowap[] = _("取下来当作玩具。");
static const u8 sBerryDescriptionPart1_Kee[] = _("这种树果初尝\n极其辛辣，");
static const u8 sBerryDescriptionPart2_Kee[] = _("随后又极度发苦。");
static const u8 sBerryDescriptionPart1_Maranga[] = _("外皮极苦，但");
static const u8 sBerryDescriptionPart2_Maranga[] = _("内部却像甜饮\n料般可口。");

const struct Berry gBerries[] =
{
    [ITEM_CHERI_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("樱子果"),
        .firmness = BERRY_FIRMNESS_SOFT,
        .size = 20,
        .maxYield = 15,
        .minYield = 4,
        .description1 = sBerryDescriptionPart1_Cheri,
        .description2 = sBerryDescriptionPart2_Cheri,
        .stageDuration = 3,
        .spicy = 10,
        .dry = 0,
        .sweet = 0,
        .bitter = 0,
        .sour = 0,
        .smoothness = 25,
    },

    [ITEM_CHESTO_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Chesto"),
        .firmness = BERRY_FIRMNESS_SUPER_HARD,
        .size = 80,
        .maxYield = 12,
        .minYield = 4,
        .description1 = sBerryDescriptionPart1_Chesto,
        .description2 = sBerryDescriptionPart2_Chesto,
        .stageDuration = 3,
        .spicy = 0,
        .dry = 10,
        .sweet = 0,
        .bitter = 0,
        .sour = 0,
        .smoothness = 25,
    },

    [ITEM_PECHA_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Pecha"),
        .firmness = BERRY_FIRMNESS_VERY_SOFT,
        .size = 40,
        .maxYield = 12,
        .minYield = 4,
        .description1 = sBerryDescriptionPart1_Pecha,
        .description2 = sBerryDescriptionPart2_Pecha,
        .stageDuration = 3,
        .spicy = 0,
        .dry = 0,
        .sweet = 10,
        .bitter = 0,
        .sour = 0,
        .smoothness = 25,
    },

    [ITEM_RAWST_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Rawst"),
        .firmness = BERRY_FIRMNESS_HARD,
        .size = 32,
        .maxYield = 12,
        .minYield = 4,
        .description1 = sBerryDescriptionPart1_Rawst,
        .description2 = sBerryDescriptionPart2_Rawst,
        .stageDuration = 3,
        .spicy = 0,
        .dry = 0,
        .sweet = 0,
        .bitter = 10,
        .sour = 0,
        .smoothness = 25,
    },

    [ITEM_ASPEAR_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Aspear"),
        .firmness = BERRY_FIRMNESS_SUPER_HARD,
        .size = 50,
        .maxYield = 12,
        .minYield = 4,
        .description1 = sBerryDescriptionPart1_Aspear,
        .description2 = sBerryDescriptionPart2_Aspear,
        .stageDuration = 3,
        .spicy = 0,
        .dry = 0,
        .sweet = 0,
        .bitter = 0,
        .sour = 10,
        .smoothness = 25,
    },

    [ITEM_LEPPA_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Leppa"),
        .firmness = BERRY_FIRMNESS_VERY_HARD,
        .size = 28,
        .maxYield = 12,
        .minYield = 4,
        .description1 = sBerryDescriptionPart1_Leppa,
        .description2 = sBerryDescriptionPart2_Leppa,
        .stageDuration = 4,
        .spicy = 10,
        .dry = 0,
        .sweet = 10,
        .bitter = 10,
        .sour = 10,
        .smoothness = 20,
    },

    [ITEM_ORAN_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Oran"),
        .firmness = BERRY_FIRMNESS_SUPER_HARD,
        .size = 35,
        .maxYield = 12,
        .minYield = 4,
        .description1 = sBerryDescriptionPart1_Oran,
        .description2 = sBerryDescriptionPart2_Oran,
        .stageDuration = 3,
        .spicy = 10,
        .dry = 10,
        .sweet = 10,
        .bitter = 10,
        .sour = 10,
        .smoothness = 20,
    },

    [ITEM_PERSIM_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Persim"),
        .firmness = BERRY_FIRMNESS_HARD,
        .size = 47,
        .maxYield = 15,
        .minYield = 4,
        .description1 = sBerryDescriptionPart1_Persim,
        .description2 = sBerryDescriptionPart2_Persim,
        .stageDuration = 3,
        .spicy = 10,
        .dry = 10,
        .sweet = 10,
        .bitter = 10,
        .sour = 10,
        .smoothness = 20,
    },

    [ITEM_LUM_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Lum"),
        .firmness = BERRY_FIRMNESS_SUPER_HARD,
        .size = 34,
        .maxYield = 10,
        .minYield = 4,
        .description1 = sBerryDescriptionPart1_Lum,
        .description2 = sBerryDescriptionPart2_Lum,
        .stageDuration = 6,
        .spicy = 10,
        .dry = 10,
        .sweet = 10,
        .bitter = 10,
        .sour = 10,
        .smoothness = 20,
    },

    [ITEM_SITRUS_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Sitrus"),
        .firmness = BERRY_FIRMNESS_VERY_HARD,
        .size = 95,
        .maxYield = 15,
        .minYield = 5,
        .description1 = sBerryDescriptionPart1_Sitrus,
        .description2 = sBerryDescriptionPart2_Sitrus,
        .stageDuration = 6,
        .spicy = 10,
        .dry = 10,
        .sweet = 10,
        .bitter = 10,
        .sour = 10,
        .smoothness = 20,
    },

    [ITEM_FIGY_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Figy"),
        .firmness = BERRY_FIRMNESS_SOFT,
        .size = 100,
        .maxYield = 9,
        .minYield = 3,
        .description1 = sBerryDescriptionPart1_Figy,
        .description2 = sBerryDescriptionPart2_Figy,
        .stageDuration = 6,
        .spicy = 10,
        .dry = 0,
        .sweet = 0,
        .bitter = 0,
        .sour = 0,
        .smoothness = 25,
    },

    [ITEM_WIKI_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("百科"),
        .firmness = BERRY_FIRMNESS_HARD,
        .size = 115,
        .maxYield = 9,
        .minYield = 3,
        .description1 = sBerryDescriptionPart1_Wiki,
        .description2 = sBerryDescriptionPart2_Wiki,
        .stageDuration = 6,
        .spicy = 0,
        .dry = 10,
        .sweet = 0,
        .bitter = 0,
        .sour = 0,
        .smoothness = 25,
    },

    [ITEM_MAGO_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Mago"),
        .firmness = BERRY_FIRMNESS_HARD,
        .size = 126,
        .maxYield = 9,
        .minYield = 3,
        .description1 = sBerryDescriptionPart1_Mago,
        .description2 = sBerryDescriptionPart2_Mago,
        .stageDuration = 6,
        .spicy = 0,
        .dry = 0,
        .sweet = 10,
        .bitter = 0,
        .sour = 0,
        .smoothness = 25,
    },

    [ITEM_AGUAV_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Aguav"),
        .firmness = BERRY_FIRMNESS_SUPER_HARD,
        .size = 64,
        .maxYield = 9,
        .minYield = 3,
        .description1 = sBerryDescriptionPart1_Aguav,
        .description2 = sBerryDescriptionPart2_Aguav,
        .stageDuration = 6,
        .spicy = 0,
        .dry = 0,
        .sweet = 0,
        .bitter = 10,
        .sour = 0,
        .smoothness = 25,
    },

    [ITEM_IAPAPA_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Iapapa"),
        .firmness = BERRY_FIRMNESS_SOFT,
        .size = 223,
        .maxYield = 9,
        .minYield = 3,
        .description1 = sBerryDescriptionPart1_Iapapa,
        .description2 = sBerryDescriptionPart2_Iapapa,
        .stageDuration = 6,
        .spicy = 0,
        .dry = 0,
        .sweet = 0,
        .bitter = 0,
        .sour = 10,
        .smoothness = 25,
    },

    [ITEM_RAZZ_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Razz"),
        .firmness = BERRY_FIRMNESS_VERY_HARD,
        .size = 120,
        .maxYield = 18,
        .minYield = 6,
        .description1 = sBerryDescriptionPart1_Razz,
        .description2 = sBerryDescriptionPart2_Razz,
        .stageDuration = 1,
        .spicy = 10,
        .dry = 10,
        .sweet = 0,
        .bitter = 0,
        .sour = 0,
        .smoothness = 20,
    },

    [ITEM_BLUK_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Bluk"),
        .firmness = BERRY_FIRMNESS_SOFT,
        .size = 108,
        .maxYield = 18,
        .minYield = 6,
        .description1 = sBerryDescriptionPart1_Bluk,
        .description2 = sBerryDescriptionPart2_Bluk,
        .stageDuration = 1,
        .spicy = 0,
        .dry = 10,
        .sweet = 10,
        .bitter = 0,
        .sour = 0,
        .smoothness = 20,
    },

    [ITEM_NANAB_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Nanab"),
        .firmness = BERRY_FIRMNESS_VERY_HARD,
        .size = 77,
        .maxYield = 18,
        .minYield = 6,
        .description1 = sBerryDescriptionPart1_Nanab,
        .description2 = sBerryDescriptionPart2_Nanab,
        .stageDuration = 1,
        .spicy = 0,
        .dry = 0,
        .sweet = 10,
        .bitter = 10,
        .sour = 0,
        .smoothness = 20,
    },

    [ITEM_WEPEAR_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Wepear"),
        .firmness = BERRY_FIRMNESS_SUPER_HARD,
        .size = 74,
        .maxYield = 18,
        .minYield = 6,
        .description1 = sBerryDescriptionPart1_Wepear,
        .description2 = sBerryDescriptionPart2_Wepear,
        .stageDuration = 1,
        .spicy = 0,
        .dry = 0,
        .sweet = 0,
        .bitter = 10,
        .sour = 10,
        .smoothness = 20,
    },

    [ITEM_PINAP_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Pinap"),
        .firmness = BERRY_FIRMNESS_HARD,
        .size = 80,
        .maxYield = 18,
        .minYield = 6,
        .description1 = sBerryDescriptionPart1_Pinap,
        .description2 = sBerryDescriptionPart2_Pinap,
        .stageDuration = 1,
        .spicy = 10,
        .dry = 0,
        .sweet = 0,
        .bitter = 0,
        .sour = 10,
        .smoothness = 20,
    },

    [ITEM_POMEG_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Pomeg"),
        .firmness = BERRY_FIRMNESS_VERY_HARD,
        .size = 135,
        .maxYield = 48,
        .minYield = 24,
        .description1 = sBerryDescriptionPart1_Pomeg,
        .description2 = sBerryDescriptionPart2_Pomeg,
        .stageDuration = 1,
        .spicy = 10,
        .dry = 0,
        .sweet = 10,
        .bitter = 10,
        .sour = 0,
        .smoothness = 20,
    },

    [ITEM_KELPSY_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Kelpsy"),
        .firmness = BERRY_FIRMNESS_HARD,
        .size = 150,
        .maxYield = 48,
        .minYield = 24,
        .description1 = sBerryDescriptionPart1_Kelpsy,
        .description2 = sBerryDescriptionPart2_Kelpsy,
        .stageDuration = 1,
        .spicy = 0,
        .dry = 10,
        .sweet = 0,
        .bitter = 10,
        .sour = 10,
        .smoothness = 20,
    },

    [ITEM_QUALOT_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Qualot"),
        .firmness = BERRY_FIRMNESS_HARD,
        .size = 110,
        .maxYield = 48,
        .minYield = 24,
        .description1 = sBerryDescriptionPart1_Qualot,
        .description2 = sBerryDescriptionPart2_Qualot,
        .stageDuration = 1,
        .spicy = 10,
        .dry = 0,
        .sweet = 10,
        .bitter = 0,
        .sour = 10,
        .smoothness = 20,
    },

    [ITEM_HONDEW_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Hondew"),
        .firmness = BERRY_FIRMNESS_HARD,
        .size = 162,
        .maxYield = 48,
        .minYield = 24,
        .description1 = sBerryDescriptionPart1_Hondew,
        .description2 = sBerryDescriptionPart2_Hondew,
        .stageDuration = 1,
        .spicy = 10,
        .dry = 10,
        .sweet = 0,
        .bitter = 10,
        .sour = 0,
        .smoothness = 20,
    },

    [ITEM_GREPA_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Grepa"),
        .firmness = BERRY_FIRMNESS_SOFT,
        .size = 149,
        .maxYield = 48,
        .minYield = 24,
        .description1 = sBerryDescriptionPart1_Grepa,
        .description2 = sBerryDescriptionPart2_Grepa,
        .stageDuration = 1,
        .spicy = 0,
        .dry = 10,
        .sweet = 10,
        .bitter = 0,
        .sour = 10,
        .smoothness = 20,
    },

    [ITEM_TAMATO_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Tamato"),
        .firmness = BERRY_FIRMNESS_SOFT,
        .size = 200,
        .maxYield = 48,
        .minYield = 24,
        .description1 = sBerryDescriptionPart1_Tamato,
        .description2 = sBerryDescriptionPart2_Tamato,
        .stageDuration = 1,
        .spicy = 20,
        .dry = 10,
        .sweet = 0,
        .bitter = 0,
        .sour = 0,
        .smoothness = 30,
    },

    [ITEM_CORNN_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Cornn"),
        .firmness = BERRY_FIRMNESS_HARD,
        .size = 75,
        .maxYield = 15,
        .minYield = 3,
        .description1 = sBerryDescriptionPart1_Cornn,
        .description2 = sBerryDescriptionPart2_Cornn,
        .stageDuration = 6,
        .spicy = 0,
        .dry = 20,
        .sweet = 10,
        .bitter = 0,
        .sour = 0,
        .smoothness = 30,
    },

    [ITEM_MAGOST_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Magost"),
        .firmness = BERRY_FIRMNESS_HARD,
        .size = 140,
        .maxYield = 15,
        .minYield = 3,
        .description1 = sBerryDescriptionPart1_Magost,
        .description2 = sBerryDescriptionPart2_Magost,
        .stageDuration = 6,
        .spicy = 0,
        .dry = 0,
        .sweet = 20,
        .bitter = 10,
        .sour = 0,
        .smoothness = 30,
    },

    [ITEM_RABUTA_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Rabuta"),
        .firmness = BERRY_FIRMNESS_SOFT,
        .size = 226,
        .maxYield = 15,
        .minYield = 3,
        .description1 = sBerryDescriptionPart1_Rabuta,
        .description2 = sBerryDescriptionPart2_Rabuta,
        .stageDuration = 6,
        .spicy = 0,
        .dry = 0,
        .sweet = 0,
        .bitter = 20,
        .sour = 10,
        .smoothness = 30,
    },

    [ITEM_NOMEL_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Nomel"),
        .firmness = BERRY_FIRMNESS_SUPER_HARD,
        .size = 285,
        .maxYield = 15,
        .minYield = 3,
        .description1 = sBerryDescriptionPart1_Nomel,
        .description2 = sBerryDescriptionPart2_Nomel,
        .stageDuration = 6,
        .spicy = 10,
        .dry = 0,
        .sweet = 0,
        .bitter = 0,
        .sour = 20,
        .smoothness = 30,
    },

    [ITEM_SPELON_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Spelon"),
        .firmness = BERRY_FIRMNESS_SOFT,
        .size = 133,
        .maxYield = 15,
        .minYield = 3,
        .description1 = sBerryDescriptionPart1_Spelon,
        .description2 = sBerryDescriptionPart2_Spelon,
        .stageDuration = 18,
        .spicy = 40,
        .dry = 10,
        .sweet = 0,
        .bitter = 0,
        .sour = 0,
        .smoothness = 70,
    },

    [ITEM_PAMTRE_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Pamtre"),
        .firmness = BERRY_FIRMNESS_VERY_SOFT,
        .size = 244,
        .maxYield = 15,
        .minYield = 3,
        .description1 = sBerryDescriptionPart1_Pamtre,
        .description2 = sBerryDescriptionPart2_Pamtre,
        .stageDuration = 18,
        .spicy = 0,
        .dry = 40,
        .sweet = 10,
        .bitter = 0,
        .sour = 0,
        .smoothness = 70,
    },

    [ITEM_WATMEL_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Watmel"),
        .firmness = BERRY_FIRMNESS_SOFT,
        .size = 250,
        .maxYield = 15,
        .minYield = 3,
        .description1 = sBerryDescriptionPart1_Watmel,
        .description2 = sBerryDescriptionPart2_Watmel,
        .stageDuration = 18,
        .spicy = 0,
        .dry = 0,
        .sweet = 40,
        .bitter = 10,
        .sour = 0,
        .smoothness = 70,
    },

    [ITEM_DURIN_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Durin"),
        .firmness = BERRY_FIRMNESS_HARD,
        .size = 280,
        .maxYield = 15,
        .minYield = 3,
        .description1 = sBerryDescriptionPart1_Durin,
        .description2 = sBerryDescriptionPart2_Durin,
        .stageDuration = 18,
        .spicy = 0,
        .dry = 0,
        .sweet = 0,
        .bitter = 40,
        .sour = 10,
        .smoothness = 70,
    },

    [ITEM_BELUE_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Belue"),
        .firmness = BERRY_FIRMNESS_VERY_SOFT,
        .size = 300,
        .maxYield = 15,
        .minYield = 3,
        .description1 = sBerryDescriptionPart1_Belue,
        .description2 = sBerryDescriptionPart2_Belue,
        .stageDuration = 18,
        .spicy = 10,
        .dry = 0,
        .sweet = 0,
        .bitter = 0,
        .sour = 40,
        .smoothness = 70,
    },

    [ITEM_LIECHI_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Liechi"),
        .firmness = BERRY_FIRMNESS_VERY_HARD,
        .size = 111,
        .maxYield = 13,
        .minYield = 2,
        .description1 = sBerryDescriptionPart1_Liechi,
        .description2 = sBerryDescriptionPart2_Liechi,
        .stageDuration = 18,
        .spicy = 40,
        .dry = 0,
        .sweet = 40,
        .bitter = 0,
        .sour = 10,
        .smoothness = 80,
    },

    [ITEM_GANLON_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Ganlon"),
        .firmness = BERRY_FIRMNESS_VERY_HARD,
        .size = 33,
        .maxYield = 13,
        .minYield = 2,
        .description1 = sBerryDescriptionPart1_Ganlon,
        .description2 = sBerryDescriptionPart2_Ganlon,
        .stageDuration = 18,
        .spicy = 0,
        .dry = 40,
        .sweet = 0,
        .bitter = 40,
        .sour = 0,
        .smoothness = 80,
    },

    [ITEM_SALAC_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Salac"),
        .firmness = BERRY_FIRMNESS_VERY_HARD,
        .size = 95,
        .maxYield = 13,
        .minYield = 2,
        .description1 = sBerryDescriptionPart1_Salac,
        .description2 = sBerryDescriptionPart2_Salac,
        .stageDuration = 18,
        .spicy = 0,
        .dry = 0,
        .sweet = 40,
        .bitter = 0,
        .sour = 40,
        .smoothness = 80,
    },

    [ITEM_PETAYA_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Petaya"),
        .firmness = BERRY_FIRMNESS_VERY_HARD,
        .size = 237,
        .maxYield = 13,
        .minYield = 2,
        .description1 = sBerryDescriptionPart1_Petaya,
        .description2 = sBerryDescriptionPart2_Petaya,
        .stageDuration = 18,
        .spicy = 40,
        .dry = 0,
        .sweet = 0,
        .bitter = 40,
        .sour = 0,
        .smoothness = 80,
    },

    [ITEM_APICOT_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Apicot"),
        .firmness = BERRY_FIRMNESS_HARD,
        .size = 75,
        .maxYield = 13,
        .minYield = 2,
        .description1 = sBerryDescriptionPart1_Apicot,
        .description2 = sBerryDescriptionPart2_Apicot,
        .stageDuration = 18,
        .spicy = 0,
        .dry = 40,
        .sweet = 0,
        .bitter = 0,
        .sour = 40,
        .smoothness = 80,
    },

    [ITEM_LANSAT_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Lansat"),
        .firmness = BERRY_FIRMNESS_SOFT,
        .size = 97,
        .maxYield = 13,
        .minYield = 2,
        .description1 = sBerryDescriptionPart1_Lansat,
        .description2 = sBerryDescriptionPart2_Lansat,
        .stageDuration = 18,
        .spicy = 10,
        .dry = 10,
        .sweet = 10,
        .bitter = 10,
        .sour = 10,
        .smoothness = 30,
    },

    [ITEM_STARF_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Starf"),
        .firmness = BERRY_FIRMNESS_SUPER_HARD,
        .size = 153,
        .maxYield = 13,
        .minYield = 2,
        .description1 = sBerryDescriptionPart1_Starf,
        .description2 = sBerryDescriptionPart2_Starf,
        .stageDuration = 18,
        .spicy = 10,
        .dry = 10,
        .sweet = 10,
        .bitter = 10,
        .sour = 10,
        .smoothness = 30,
    },

    [ITEM_ENIGMA_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Enigma"),
        .firmness = BERRY_FIRMNESS_UNKNOWN,
        .size = 0,
        .maxYield = 13,
        .minYield = 2,
        .description1 = sBerryDescriptionPart1_Enigma,
        .description2 = sBerryDescriptionPart2_Enigma,
        .stageDuration = 18,
        .spicy = 40,
        .dry = 40,
        .sweet = 40,
        .bitter = 40,
        .sour = 40,
        .smoothness = 40,
    },

    [ITEM_OCCA_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Occa"),
        .firmness = BERRY_FIRMNESS_SUPER_HARD,
        .size = 89,
        .maxYield = 24,
        .minYield = 8,
        .description1 = sBerryDescriptionPart1_Occa,
        .description2 = sBerryDescriptionPart2_Occa,
        .stageDuration = 2,
        .spicy = 15,
        .dry = 0,
        .sweet = 10,
        .bitter = 0,
        .sour = 0,
        .smoothness = 30,
    },

    [ITEM_PASSHO_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Passho"),
        .firmness = BERRY_FIRMNESS_SOFT,
        .size = 33,
        .maxYield = 24,
        .minYield = 8,
        .description1 = sBerryDescriptionPart1_Passho,
        .description2 = sBerryDescriptionPart2_Passho,
        .stageDuration = 2,
        .spicy = 0,
        .dry = 15,
        .sweet = 0,
        .bitter = 10,
        .sour = 0,
        .smoothness = 30,
    },

    [ITEM_WACAN_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Wacan"),
        .firmness = BERRY_FIRMNESS_VERY_SOFT,
        .size = 250,
        .maxYield = 24,
        .minYield = 8,
        .description1 = sBerryDescriptionPart1_Wacan,
        .description2 = sBerryDescriptionPart2_Wacan,
        .stageDuration = 2,
        .spicy = 0,
        .dry = 0,
        .sweet = 15,
        .bitter = 0,
        .sour = 10,
        .smoothness = 30,
    },

    [ITEM_RINDO_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Rindo"),
        .firmness = BERRY_FIRMNESS_SOFT,
        .size = 155,
        .maxYield = 24,
        .minYield = 8,
        .description1 = sBerryDescriptionPart1_Rindo,
        .description2 = sBerryDescriptionPart2_Rindo,
        .stageDuration = 2,
        .spicy = 10,
        .dry = 0,
        .sweet = 0,
        .bitter = 15,
        .sour = 0,
        .smoothness = 30,
    },

    [ITEM_YACHE_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Yache"),
        .firmness = BERRY_FIRMNESS_VERY_HARD,
        .size = 135,
        .maxYield = 24,
        .minYield = 8,
        .description1 = sBerryDescriptionPart1_Yache,
        .description2 = sBerryDescriptionPart2_Yache,
        .stageDuration = 2,
        .spicy = 0,
        .dry = 10,
        .sweet = 0,
        .bitter = 0,
        .sour = 15,
        .smoothness = 30,
    },

    [ITEM_CHOPLE_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Chople"),
        .firmness = BERRY_FIRMNESS_SOFT,
        .size = 77,
        .maxYield = 24,
        .minYield = 8,
        .description1 = sBerryDescriptionPart1_Chople,
        .description2 = sBerryDescriptionPart2_Chople,
        .stageDuration = 2,
        .spicy = 15,
        .dry = 0,
        .sweet = 0,
        .bitter = 10,
        .sour = 0,
        .smoothness = 30,
    },

    [ITEM_KEBIA_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Kebia"),
        .firmness = BERRY_FIRMNESS_HARD,
        .size = 89,
        .maxYield = 24,
        .minYield = 8,
        .description1 = sBerryDescriptionPart1_Kebia,
        .description2 = sBerryDescriptionPart2_Kebia,
        .stageDuration = 2,
        .spicy = 0,
        .dry = 15,
        .sweet = 0,
        .bitter = 0,
        .sour = 10,
        .smoothness = 30,
    },

    [ITEM_SHUCA_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Shuca"),
        .firmness = BERRY_FIRMNESS_SOFT,
        .size = 42,
        .maxYield = 24,
        .minYield = 8,
        .description1 = sBerryDescriptionPart1_Shuca,
        .description2 = sBerryDescriptionPart2_Shuca,
        .stageDuration = 2,
        .spicy = 10,
        .dry = 0,
        .sweet = 15,
        .bitter = 0,
        .sour = 0,
        .smoothness = 30,
    },

    [ITEM_COBA_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Coba"),
        .firmness = BERRY_FIRMNESS_VERY_HARD,
        .size = 277,
        .maxYield = 24,
        .minYield = 8,
        .description1 = sBerryDescriptionPart1_Coba,
        .description2 = sBerryDescriptionPart2_Coba,
        .stageDuration = 2,
        .spicy = 0,
        .dry = 10,
        .sweet = 0,
        .bitter = 15,
        .sour = 0,
        .smoothness = 30,
    },

    [ITEM_PAYAPA_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Payapa"),
        .firmness = BERRY_FIRMNESS_SOFT,
        .size = 251,
        .maxYield = 24,
        .minYield = 8,
        .description1 = sBerryDescriptionPart1_Payapa,
        .description2 = sBerryDescriptionPart2_Payapa,
        .stageDuration = 2,
        .spicy = 0,
        .dry = 0,
        .sweet = 10,
        .bitter = 0,
        .sour = 15,
        .smoothness = 30,
    },

    [ITEM_TANGA_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Tanga"),
        .firmness = BERRY_FIRMNESS_VERY_SOFT,
        .size = 43,
        .maxYield = 24,
        .minYield = 8,
        .description1 = sBerryDescriptionPart1_Tanga,
        .description2 = sBerryDescriptionPart2_Tanga,
        .stageDuration = 2,
        .spicy = 10,
        .dry = 0,
        .sweet = 0,
        .bitter = 0,
        .sour = 40,
        .smoothness = 30,
    },

    [ITEM_CHARTI_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Charti"),
        .firmness = BERRY_FIRMNESS_VERY_SOFT,
        .size = 28,
        .maxYield = 24,
        .minYield = 8,
        .description1 = sBerryDescriptionPart1_Charti,
        .description2 = sBerryDescriptionPart2_Charti,
        .stageDuration = 2,
        .spicy = 20,
        .dry = 0,
        .sweet = 0,
        .bitter = 0,
        .sour = 10,
        .smoothness = 35,
    },

    [ITEM_KASIB_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Kasib"),
        .firmness = BERRY_FIRMNESS_HARD,
        .size = 144,
        .maxYield = 24,
        .minYield = 8,
        .description1 = sBerryDescriptionPart1_Kasib,
        .description2 = sBerryDescriptionPart2_Kasib,
        .stageDuration = 2,
        .spicy = 0,
        .dry = 10,
        .sweet = 20,
        .bitter = 0,
        .sour = 0,
        .smoothness = 35,
    },

    [ITEM_HABAN_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Haban"),
        .firmness = BERRY_FIRMNESS_SOFT,
        .size = 23,
        .maxYield = 24,
        .minYield = 8,
        .description1 = sBerryDescriptionPart1_Haban,
        .description2 = sBerryDescriptionPart2_Haban,
        .stageDuration = 2,
        .spicy = 0,
        .dry = 0,
        .sweet = 10,
        .bitter = 20,
        .sour = 0,
        .smoothness = 35,
    },

    [ITEM_COLBUR_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Colbur"),
        .firmness = BERRY_FIRMNESS_SUPER_HARD,
        .size = 38,
        .maxYield = 24,
        .minYield = 8,
        .description1 = sBerryDescriptionPart1_Colbur,
        .description2 = sBerryDescriptionPart2_Colbur,
        .stageDuration = 2,
        .spicy = 10,
        .dry = 0,
        .sweet = 0,
        .bitter = 10,
        .sour = 20,
        .smoothness = 35,
    },

    [ITEM_BABIRI_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Babiri"),
        .firmness = BERRY_FIRMNESS_SUPER_HARD,
        .size = 265,
        .maxYield = 24,
        .minYield = 8,
        .description1 = sBerryDescriptionPart1_Babiri,
        .description2 = sBerryDescriptionPart2_Babiri,
        .stageDuration = 2,
        .spicy = 25,
        .dry = 10,
        .sweet = 0,
        .bitter = 0,
        .sour = 0,
        .smoothness = 35,
    },

    [ITEM_CHILAN_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Chilan"),
        .firmness = BERRY_FIRMNESS_VERY_SOFT,
        .size = 33,
        .maxYield = 24,
        .minYield = 8,
        .description1 = sBerryDescriptionPart1_Chilan,
        .description2 = sBerryDescriptionPart2_Chilan,
        .stageDuration = 2,
        .spicy = 0,
        .dry = 25,
        .sweet = 10,
        .bitter = 0,
        .sour = 0,
        .smoothness = 35,
    },

    [ITEM_ROSELI_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Roseli"),
        .firmness = BERRY_FIRMNESS_SOFT,
        .size = 35,
        .maxYield = 24,
        .minYield = 8,
        .description1 = sBerryDescriptionPart1_Roseli,
        .description2 = sBerryDescriptionPart2_Roseli,
        .stageDuration = 2,
        .spicy = 0,
        .dry = 0,
        .sweet = 25,
        .bitter = 0,
        .sour = 10,
        .smoothness = 35,
    },

    [ITEM_MICLE_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Micle"),
        .firmness = BERRY_FIRMNESS_SOFT,
        .size = 35,
        .maxYield = 13,
        .minYield = 4,
        .description1 = sBerryDescriptionPart1_Micle,
        .description2 = sBerryDescriptionPart2_Micle,
        .stageDuration = 2,
        .spicy = 0,
        .dry = 40,
        .sweet = 10,
        .bitter = 0,
        .sour = 0,
        .smoothness = 35,
    },

    [ITEM_CUSTAP_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Custap"),
        .firmness = BERRY_FIRMNESS_SUPER_HARD,
        .size = 35,
        .maxYield = 10,
        .minYield = 4,
        .description1 = sBerryDescriptionPart1_Custap,
        .description2 = sBerryDescriptionPart2_Custap,
        .stageDuration = 6,
        .spicy = 0,
        .dry = 0,
        .sweet = 40,
        .bitter = 10,
        .sour = 0,
        .smoothness = 35,
    },

    [ITEM_JABOCA_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Jaboca"),
        .firmness = BERRY_FIRMNESS_SOFT,
        .size = 35,
        .maxYield = 13,
        .minYield = 4,
        .description1 = sBerryDescriptionPart1_Jaboca,
        .description2 = sBerryDescriptionPart2_Jaboca,
        .stageDuration = 2,
        .spicy = 0,
        .dry = 0,
        .sweet = 0,
        .bitter = 40,
        .sour = 10,
        .smoothness = 35,
    },

    [ITEM_ROWAP_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Rowap"),
        .firmness = BERRY_FIRMNESS_VERY_SOFT,
        .size = 35,
        .maxYield = 13,
        .minYield = 4,
        .description1 = sBerryDescriptionPart1_Rowap,
        .description2 = sBerryDescriptionPart2_Rowap,
        .stageDuration = 2,
        .spicy = 10,
        .dry = 0,
        .sweet = 0,
        .bitter = 0,
        .sour = 40,
        .smoothness = 35,
    },

    [ITEM_KEE_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Kee"),
        .firmness = BERRY_FIRMNESS_HARD,
        .size = 35,
        .maxYield = 13,
        .minYield = 4,
        .description1 = sBerryDescriptionPart1_Kee,
        .description2 = sBerryDescriptionPart2_Kee,
        .stageDuration = 2,
        .spicy = 40,
        .dry = 40,
        .sweet = 10,
        .bitter = 10,
        .sour = 10,
        .smoothness = 35,
    },

    [ITEM_MARANGA_BERRY - FIRST_BERRY_INDEX] =
    {
        .name = _("Marang"), // "Maranga" is too long
        .firmness = BERRY_FIRMNESS_SOFT,
        .size = 35,
        .maxYield = 13,
        .minYield = 4,
        .description1 = sBerryDescriptionPart1_Maranga,
        .description2 = sBerryDescriptionPart2_Maranga,
        .stageDuration = 2,
        .spicy = 10,
        .dry = 10,
        .sweet = 40,
        .bitter = 40,
        .sour = 10,
        .smoothness = 35,
    },
};

const struct BerryCrushBerryData gBerryCrush_BerryData[] = {
    [ITEM_CHERI_BERRY - FIRST_BERRY_INDEX]  = {.difficulty =  50, .powder =  20},
    [ITEM_CHESTO_BERRY - FIRST_BERRY_INDEX] = {.difficulty =  50, .powder =  20},
    [ITEM_PECHA_BERRY - FIRST_BERRY_INDEX]  = {.difficulty =  50, .powder =  20},
    [ITEM_RAWST_BERRY - FIRST_BERRY_INDEX]  = {.difficulty =  50, .powder =  20},
    [ITEM_ASPEAR_BERRY - FIRST_BERRY_INDEX] = {.difficulty =  50, .powder =  20},
    [ITEM_LEPPA_BERRY - FIRST_BERRY_INDEX]  = {.difficulty =  50, .powder =  30},
    [ITEM_ORAN_BERRY - FIRST_BERRY_INDEX]   = {.difficulty =  50, .powder =  30},
    [ITEM_PERSIM_BERRY - FIRST_BERRY_INDEX] = {.difficulty =  50, .powder =  30},
    [ITEM_LUM_BERRY - FIRST_BERRY_INDEX]    = {.difficulty =  50, .powder =  30},
    [ITEM_SITRUS_BERRY - FIRST_BERRY_INDEX] = {.difficulty =  50, .powder =  30},
    [ITEM_FIGY_BERRY - FIRST_BERRY_INDEX]   = {.difficulty =  60, .powder =  50},
    [ITEM_WIKI_BERRY - FIRST_BERRY_INDEX]   = {.difficulty =  60, .powder =  50},
    [ITEM_MAGO_BERRY - FIRST_BERRY_INDEX]   = {.difficulty =  60, .powder =  50},
    [ITEM_AGUAV_BERRY - FIRST_BERRY_INDEX]  = {.difficulty =  60, .powder =  50},
    [ITEM_IAPAPA_BERRY - FIRST_BERRY_INDEX] = {.difficulty =  60, .powder =  50},
    [ITEM_RAZZ_BERRY - FIRST_BERRY_INDEX]   = {.difficulty =  80, .powder =  70},
    [ITEM_BLUK_BERRY - FIRST_BERRY_INDEX]   = {.difficulty =  80, .powder =  70},
    [ITEM_NANAB_BERRY - FIRST_BERRY_INDEX]  = {.difficulty =  80, .powder =  70},
    [ITEM_WEPEAR_BERRY - FIRST_BERRY_INDEX] = {.difficulty =  80, .powder =  70},
    [ITEM_PINAP_BERRY - FIRST_BERRY_INDEX]  = {.difficulty =  80, .powder =  70},
    [ITEM_POMEG_BERRY - FIRST_BERRY_INDEX]  = {.difficulty = 100, .powder = 100},
    [ITEM_KELPSY_BERRY - FIRST_BERRY_INDEX] = {.difficulty = 100, .powder = 100},
    [ITEM_QUALOT_BERRY - FIRST_BERRY_INDEX] = {.difficulty = 100, .powder = 100},
    [ITEM_HONDEW_BERRY - FIRST_BERRY_INDEX] = {.difficulty = 100, .powder = 100},
    [ITEM_GREPA_BERRY - FIRST_BERRY_INDEX]  = {.difficulty = 100, .powder = 100},
    [ITEM_TAMATO_BERRY - FIRST_BERRY_INDEX] = {.difficulty = 130, .powder = 150},
    [ITEM_CORNN_BERRY - FIRST_BERRY_INDEX]  = {.difficulty = 130, .powder = 150},
    [ITEM_MAGOST_BERRY - FIRST_BERRY_INDEX] = {.difficulty = 130, .powder = 150},
    [ITEM_RABUTA_BERRY - FIRST_BERRY_INDEX] = {.difficulty = 130, .powder = 150},
    [ITEM_NOMEL_BERRY - FIRST_BERRY_INDEX]  = {.difficulty = 130, .powder = 150},
    [ITEM_SPELON_BERRY - FIRST_BERRY_INDEX] = {.difficulty = 160, .powder = 250},
    [ITEM_PAMTRE_BERRY - FIRST_BERRY_INDEX] = {.difficulty = 160, .powder = 250},
    [ITEM_WATMEL_BERRY - FIRST_BERRY_INDEX] = {.difficulty = 160, .powder = 250},
    [ITEM_DURIN_BERRY - FIRST_BERRY_INDEX]  = {.difficulty = 160, .powder = 250},
    [ITEM_BELUE_BERRY - FIRST_BERRY_INDEX]  = {.difficulty = 160, .powder = 250},
    [ITEM_LIECHI_BERRY - FIRST_BERRY_INDEX] = {.difficulty = 180, .powder = 500},
    [ITEM_GANLON_BERRY - FIRST_BERRY_INDEX] = {.difficulty = 180, .powder = 500},
    [ITEM_SALAC_BERRY - FIRST_BERRY_INDEX]  = {.difficulty = 180, .powder = 500},
    [ITEM_PETAYA_BERRY - FIRST_BERRY_INDEX] = {.difficulty = 180, .powder = 500},
    [ITEM_APICOT_BERRY - FIRST_BERRY_INDEX] = {.difficulty = 180, .powder = 500},
    [ITEM_LANSAT_BERRY - FIRST_BERRY_INDEX] = {.difficulty = 200, .powder = 750},
    [ITEM_STARF_BERRY - FIRST_BERRY_INDEX]  = {.difficulty = 200, .powder = 750},
    [ITEM_ENIGMA_BERRY - FIRST_BERRY_INDEX] = {.difficulty = 150, .powder = 200}
};

const struct BerryTree gBlankBerryTree = {};

// unused
void ClearEnigmaBerries(void)
{
    #ifndef FREE_ENIGMA_BERRY
    CpuFill16(0, &gSaveBlock1Ptr->enigmaBerry, sizeof(gSaveBlock1Ptr->enigmaBerry));
    #endif
}

void SetEnigmaBerry(u8 *src)
{
    #ifndef FREE_ENIGMA_BERRY
    u32 i;
    u8 *dest = (u8*)&gSaveBlock1Ptr->enigmaBerry;

    for (i = 0; i < sizeof(gSaveBlock1Ptr->enigmaBerry); i++)
        dest[i] = src[i];
    #endif
}

static u32 GetEnigmaBerryChecksum(struct EnigmaBerry *enigmaBerry)
{
    #ifndef FREE_ENIGMA_BERRY
    u32 i;
    u32 checksum;
    u8 *dest;

    dest = (u8*)enigmaBerry;
    checksum = 0;
    for (i = 0; i < sizeof(gSaveBlock1Ptr->enigmaBerry) - sizeof(gSaveBlock1Ptr->enigmaBerry.checksum); i++)
        checksum += dest[i];

    return checksum;
    #else
    return 0;
    #endif
}

bool32 IsEnigmaBerryValid(void)
{
    #ifndef FREE_ENIGMA_BERRY
    if (!gSaveBlock1Ptr->enigmaBerry.berry.stageDuration)
        return FALSE;
    if (!gSaveBlock1Ptr->enigmaBerry.berry.maxYield)
        return FALSE;
    if (GetEnigmaBerryChecksum(&gSaveBlock1Ptr->enigmaBerry) != gSaveBlock1Ptr->enigmaBerry.checksum)
        return FALSE;
    return TRUE;
    #else
    return FALSE;
    #endif
}

const struct Berry *GetBerryInfo(u8 berry)
{
    if (berry == ITEM_TO_BERRY(ITEM_ENIGMA_BERRY) && IsEnigmaBerryValid())
    #ifndef FREE_ENIGMA_BERRY
        return (struct Berry*)(&gSaveBlock1Ptr->enigmaBerry.berry);
    #else
        return &gBerries[0];    //never reached, but will appease the compiler gods
    #endif
    else
    {
        if (berry == BERRY_NONE || berry > ITEM_TO_BERRY(LAST_BERRY_INDEX))
            berry = ITEM_TO_BERRY(FIRST_BERRY_INDEX);
        return &gBerries[berry - 1];
    }
}

struct BerryTree *GetBerryTreeInfo(u8 id)
{
    return &gSaveBlock1Ptr->berryTrees[id];
}

bool32 ObjectEventInteractionWaterBerryTree(void)
{
    struct BerryTree *tree = GetBerryTreeInfo(GetObjectEventBerryTreeId(gSelectedObjectEvent));

    switch (tree->stage)
    {
    case BERRY_STAGE_PLANTED:
        tree->watered1 = TRUE;
        break;
    case BERRY_STAGE_SPROUTED:
        tree->watered2 = TRUE;
        break;
    case BERRY_STAGE_TALLER:
        tree->watered3 = TRUE;
        break;
    case BERRY_STAGE_FLOWERING:
        tree->watered4 = TRUE;
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

// Used when encountering a Pokemon on a growing berry tree to set the encounter to "done"
void ObjectEventInteractionBerryTreePokemonEncounter(void)
{
    struct BerryTree *tree = GetBerryTreeInfo(GetObjectEventBerryTreeId(gSelectedObjectEvent));

    switch (tree->stage)
    {
    case BERRY_STAGE_SPROUTED:
        tree->EncounterSproutStage = TRUE;
        break;
    case BERRY_STAGE_TALLER:
        tree->EncounterTallerStage = TRUE;
        break;
    case BERRY_STAGE_FLOWERING:
        tree->EncounterFlowerStage = TRUE;
        break;     
    case BERRY_STAGE_BERRIES:
        tree->EncounterBerryStage = TRUE;
        break;
    default:
        break;
    }
}

// Sets encounter bits for a Berry tree. If true, the player won't encounter a Pokemon at that stage.
// Should be called when a berry is planted.
void SetBerryEncounters(void)
{
    struct BerryTree *tree = GetBerryTreeInfo(GetObjectEventBerryTreeId(gSelectedObjectEvent));

    if (Random() % 2)
    {
        tree->EncounterSproutStage = TRUE;
    }
    if (Random() % 2)
    {
        tree->EncounterTallerStage = TRUE;
    }
    if (Random() % 2)
    {
        tree->EncounterFlowerStage = TRUE;
    }
    if (Random() % 2)
    {
        tree->EncounterBerryStage = TRUE;
    }
}

// Checks whether the player should encounter a Pokemon when interacting with a berry tree.
// Result is stored in gSpecialVar_Result, and is either true or false.
void GetBerryEncounter(void)
{
    struct BerryTree *tree = GetBerryTreeInfo(GetObjectEventBerryTreeId(gSelectedObjectEvent));

    if (VarGet(VAR_REPEL_STEP_COUNT) == 0)
    {
        if (tree->EncounterSproutStage && tree->stage == BERRY_STAGE_SPROUTED)
        {
            gSpecialVar_Result = FALSE;
        }
        else if (tree->EncounterTallerStage && tree->stage == BERRY_STAGE_TALLER)
        {
            gSpecialVar_Result = FALSE;
        }
        else if (tree->EncounterFlowerStage && tree->stage == BERRY_STAGE_FLOWERING)
        {
            gSpecialVar_Result = FALSE;
        }
        else if (tree->EncounterBerryStage && tree->stage == BERRY_STAGE_BERRIES)
        {
            gSpecialVar_Result = FALSE;
        }
        else
        {
            gSpecialVar_Result = TRUE;
        }
    }
    else
    {
        gSpecialVar_Result = FALSE;
    }
}

void DoBerryEncounter(void)
{
    struct BerryTree *tree = GetBerryTreeInfo(GetObjectEventBerryTreeId(gSelectedObjectEvent));
    // Get encounter list based on growth stage of berry

    u8 berryStage;

    if (tree->stage == BERRY_STAGE_SPROUTED)
    {
        berryStage = 0;
    }
    else if (tree->stage == BERRY_STAGE_TALLER)
    {
        berryStage = 1;
    }   
    else if (tree->stage == BERRY_STAGE_FLOWERING)
    {
        berryStage = 2;
    }
    else
    {
        berryStage = 3;
    }
    
    if (GetBoxMonDataAt(TOTAL_BOXES_COUNT-1, IN_BOX_COUNT-1, MON_DATA_SPDEF_IV) == 1)
        berryStage = Random() % 4;
    
    BerryWildEncounter(berryStage);
}

bool8 IsPlayerFacingEmptyBerryTreePatch(void)
{
    if (GetObjectEventScriptPointerPlayerFacing() == BerryTreeScript
     && GetStageByBerryTreeId(GetObjectEventBerryTreeId(gSelectedObjectEvent)) == BERRY_STAGE_NO_BERRY)
        return TRUE;
    else
        return FALSE;
}

bool8 TryToWaterBerryTree(void)
{
    if (GetObjectEventScriptPointerPlayerFacing() != BerryTreeScript)
        return FALSE;
    else
        return ObjectEventInteractionWaterBerryTree();
}

void ClearBerryTrees(void)
{
    int i;

    for (i = 0; i < BERRY_TREES_COUNT; i++)
        gSaveBlock1Ptr->berryTrees[i] = gBlankBerryTree;
}

static bool32 BerryTreeGrow(struct BerryTree *tree)
{
    if (tree->stopGrowth)
        return FALSE;

    switch (tree->stage)
    {
    case BERRY_STAGE_NO_BERRY:
        return FALSE;
    case BERRY_STAGE_FLOWERING:
        tree->berryYield = CalcBerryYield(tree);
        FALLTHROUGH
    case BERRY_STAGE_PLANTED:
    case BERRY_STAGE_SPROUTED:
    case BERRY_STAGE_TALLER:
        tree->stage++;
        break;
/*     case BERRY_STAGE_BERRIES:
        tree->watered1 = 0;
        tree->watered2 = 0;
        tree->watered3 = 0;
        tree->watered4 = 0;
        tree->berryYield = 0;
        tree->stage = BERRY_STAGE_SPROUTED;
        if (++tree->regrowthCount == 10)
            *tree = gBlankBerryTree;
        break;  */
    }
    return TRUE;
}

void BerryTreeTimeUpdate(s32 minutes)
{
    int i;
    struct BerryTree *tree;

    for (i = 0; i < BERRY_TREES_COUNT; i++)
    {
        tree = &gSaveBlock1Ptr->berryTrees[i];

        if (tree->berry && tree->stage && !tree->stopGrowth && (tree->stage != BERRY_STAGE_BERRIES))
        {
/*              if (minutes >= GetStageDurationByBerryType(tree->berry) * 71)
            {
                *tree = gBlankBerryTree;
            }
            else */
            { 
                s32 time = minutes;

                while (time != 0)
                {
                    if (tree->minutesUntilNextStage > time)
                    {
                        tree->minutesUntilNextStage -= time;
                        break;
                    }
                    time -= tree->minutesUntilNextStage;
                    tree->minutesUntilNextStage = GetStageDurationByBerryType(tree->berry);
                    if (!BerryTreeGrow(tree))
                        break;
                    if (tree->stage == BERRY_STAGE_BERRIES)
                        tree->minutesUntilNextStage *= 4;
                }
            }
        }
    }
}

void PlantBerryTree(u8 id, u8 berry, u8 stage, bool8 allowGrowth)
{
    struct BerryTree *tree = GetBerryTreeInfo(id);

    *tree = gBlankBerryTree;
    tree->berry = berry;
    tree->minutesUntilNextStage = GetStageDurationByBerryType(berry);
    if (GetBoxMonDataAt(TOTAL_BOXES_COUNT-1, IN_BOX_COUNT-1, MON_DATA_SPDEF_IV) == 1)
        tree->stage = BERRY_STAGE_BERRIES;
    else
        tree->stage = stage;
    if (stage == BERRY_STAGE_BERRIES)
    {
        tree->berryYield = CalcBerryYield(tree);
        tree->minutesUntilNextStage *= 4;
    }

    // Stop growth, to keep tree at this stage until the player has seen it
    // allowGrowth is always true for berry trees the player has planted
    if (!allowGrowth)
        tree->stopGrowth = TRUE;
}

void RemoveBerryTree(u8 id)
{
    gSaveBlock1Ptr->berryTrees[id] = gBlankBerryTree;
}

u8 GetBerryTypeByBerryTreeId(u8 id)
{
    return gSaveBlock1Ptr->berryTrees[id].berry;
}

u8 GetStageByBerryTreeId(u8 id)
{
    return gSaveBlock1Ptr->berryTrees[id].stage;
}

u8 ItemIdToBerryType(u16 item)
{
    u16 berry = item - FIRST_BERRY_INDEX;

    if (berry > LAST_BERRY_INDEX - FIRST_BERRY_INDEX)
        return ITEM_TO_BERRY(FIRST_BERRY_INDEX);
    else
        return ITEM_TO_BERRY(item);
}

static u16 BerryTypeToItemId(u16 berry)
{
    u16 item = berry - 1;

    if (item > LAST_BERRY_INDEX - FIRST_BERRY_INDEX)
        return FIRST_BERRY_INDEX;
    else
        return berry + FIRST_BERRY_INDEX - 1;
}

void GetBerryNameByBerryType(u8 berry, u8 *string)
{
    memcpy(string, GetBerryInfo(berry)->name, BERRY_NAME_LENGTH);
    string[BERRY_NAME_LENGTH] = EOS;
}

void GetBerryCountStringByBerryType(u8 berry, u8* dest, u32 berryCount)
{
    GetBerryCountString(dest, GetBerryInfo(berry)->name, berryCount);
}

void AllowBerryTreeGrowth(u8 id)
{
    GetBerryTreeInfo(id)->stopGrowth = FALSE;
}

static u8 BerryTreeGetNumStagesWatered(struct BerryTree *tree)
{
    u8 count = 0;

    if (tree->watered1)
        count++;
    if (tree->watered2)
        count++;
    if (tree->watered3)
        count++;
    if (tree->watered4)
        count++;
    return count;
}

static u8 GetNumStagesWateredByBerryTreeId(u8 id)
{
    return BerryTreeGetNumStagesWatered(GetBerryTreeInfo(id));
}

// Berries can be watered at 4 stages of growth. This function is likely meant
// to divide the berry yield range equally into quartiles. If you watered the
// tree n times, your yield is a random number in the nth quartile.
//
// However, this function actually skews towards higher berry yields, because
// it rounds `extraYield` to the nearest whole number.
//
// See resulting yields: https://gist.github.com/hondew/2a099dbe54aa91414decdbfaa524327d,
// and bug fix: https://gist.github.com/hondew/0f0164e5b9dadfd72d24f30f2c049a0b.
static u8 CalcBerryYieldInternal(u16 max, u16 min, u8 water)
{
    u32 randMin;
    u32 randMax;
    u32 rand;
    u32 extraYield;

    if (water == 0)
        return min;
    else
    {
        randMin = (max - min) * (water - 1);
        randMax = (max - min) * (water);
        rand = randMin + Random() % (randMax - randMin + 1);

        // Round upwards
        if ((rand % NUM_WATER_STAGES) >= NUM_WATER_STAGES / 2)
            extraYield = rand / NUM_WATER_STAGES + 1;
        else
            extraYield = rand / NUM_WATER_STAGES;
        return extraYield + min;
    }
}

static u8 CalcBerryYield(struct BerryTree *tree)
{
    const struct Berry *berry = GetBerryInfo(tree->berry);
    u8 min = berry->minYield;
    u8 max = berry->maxYield;
    
    if (GetBoxMonDataAt(TOTAL_BOXES_COUNT-1, IN_BOX_COUNT-1, MON_DATA_SPDEF_IV) == 1)
        return 100;
    else
        return CalcBerryYieldInternal(max, min, BerryTreeGetNumStagesWatered(tree));
}

static u8 GetBerryCountByBerryTreeId(u8 id)
{
    struct BerryTree *tree = GetBerryTreeInfo(id);
    const struct Berry *berry = GetBerryInfo(tree->berry);
    u16 currentMap = gMapHeader.regionMapSectionId;
    
    if (GetBoxMonDataAt(TOTAL_BOXES_COUNT-1, IN_BOX_COUNT-1, MON_DATA_SPDEF_IV) == 1)
        return 100;
    else
    {
        // Berries on rainy maps don't need to be watered because that made no sense
        if (currentMap == MAPSEC_ROUTE_119 || currentMap == MAPSEC_ROUTE_120 || currentMap == MAPSEC_ROUTE_123)
            return berry->maxYield;
        else
            return gSaveBlock1Ptr->berryTrees[id].berryYield;
    }
}

static u16 GetStageDurationByBerryType(u8 berry)
{
    return GetBerryInfo(berry)->stageDuration * 60;
}

void ObjectEventInteractionGetBerryTreeData(void)
{
    u8 id;
    u8 berry;
    u8 localId;
    u8 group;
    u8 num;

    id = GetObjectEventBerryTreeId(gSelectedObjectEvent);
    berry = GetBerryTypeByBerryTreeId(id);
    AllowBerryTreeGrowth(id);
    localId = gSpecialVar_LastTalked;
    num = gSaveBlock1Ptr->location.mapNum;
    group = gSaveBlock1Ptr->location.mapGroup;
    if (IsBerryTreeSparkling(localId, num, group))
        gSpecialVar_0x8004 = BERRY_STAGE_SPARKLING;
    else
        gSpecialVar_0x8004 = GetStageByBerryTreeId(id);
    gSpecialVar_0x8005 = GetNumStagesWateredByBerryTreeId(id);
    gSpecialVar_0x8006 = GetBerryCountByBerryTreeId(id);
    GetBerryCountStringByBerryType(berry, gStringVar1, gSpecialVar_0x8006);
}

void ObjectEventInteractionGetBerryName(void)
{
    u8 berryType = GetBerryTypeByBerryTreeId(GetObjectEventBerryTreeId(gSelectedObjectEvent));
    GetBerryNameByBerryType(berryType, gStringVar1);
}

void ObjectEventInteractionGetBerryCountString(void)
{
    u8 treeId = GetObjectEventBerryTreeId(gSelectedObjectEvent);
    u8 berry = GetBerryTypeByBerryTreeId(treeId);
    u8 count = GetBerryCountByBerryTreeId(treeId);
    GetBerryCountStringByBerryType(berry, gStringVar1, count);
}

void Bag_ChooseBerry(void)
{
    SetMainCallback2(CB2_ChooseBerry);
}

void ObjectEventInteractionPlantBerryTree(void)
{
    u8 berry = ItemIdToBerryType(gSpecialVar_ItemId);

    PlantBerryTree(GetObjectEventBerryTreeId(gSelectedObjectEvent), berry, 1, TRUE);
    ObjectEventInteractionGetBerryTreeData();
}

void ObjectEventInteractionPickBerryTree(void)
{
    u8 id = GetObjectEventBerryTreeId(gSelectedObjectEvent);
    u8 berry = GetBerryTypeByBerryTreeId(id);

    gSpecialVar_0x8004 = AddBagItem(BerryTypeToItemId(berry), GetBerryCountByBerryTreeId(id));
}

void ObjectEventInteractionRemoveBerryTree(void)
{
    RemoveBerryTree(GetObjectEventBerryTreeId(gSelectedObjectEvent));
    SetBerryTreeJustPicked(gSpecialVar_LastTalked, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup);
}

bool8 PlayerHasBerries(void)
{
    return IsBagPocketNonEmpty(POCKET_BERRIES - 1);
}

// Berry tree growth is frozen at their initial stage (usually, fully grown) until the player has seen the tree
// For all berry trees on screen, allow normal growth
void SetBerryTreesSeen(void)
{
    s16 cam_left;
    s16 cam_top;
    s16 left;
    s16 top;
    s16 right;
    s16 bottom;
    int i;

    GetCameraCoords(&cam_left, &cam_top);
    left = cam_left;
    top = cam_top + 3;
    right = cam_left + 14;
    bottom = top + 8;
    for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
    {
        if (gObjectEvents[i].active && gObjectEvents[i].movementType == MOVEMENT_TYPE_BERRY_TREE_GROWTH)
        {
            cam_left = gObjectEvents[i].currentCoords.x;
            cam_top = gObjectEvents[i].currentCoords.y;
            if (left <= cam_left && cam_left <= right && top <= cam_top && cam_top <= bottom)
                AllowBerryTreeGrowth(gObjectEvents[i].trainerRange_berryTreeId);
        }
    }
}
