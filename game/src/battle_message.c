#include "global.h"
#include "battle.h"
#include "bg.h"
#include "battle_anim.h"
#include "battle_controllers.h"
#include "battle_message.h"
#include "battle_setup.h"
#include "battle_tower.h"
#include "data.h"
#include "event_data.h"
#include "frontier_util.h"
#include "international_string_util.h"
#include "item.h"
#include "link.h"
#include "menu.h"
#include "palette.h"
#include "recorded_battle.h"
#include "string_util.h"
#include "strings.h"
#include "text.h"
#include "trainer_hill.h"
#include "window.h"
#include "generated/constants/abilities.h"
#include "constants/battle_dome.h"
#include "constants/battle_string_ids.h"
#include "constants/frontier_util.h"
#include "constants/items.h"
#include "generated/constants/moves.h"
#include "generated/constants/species.h"
#include "constants/trainers.h"
#include "constants/trainer_hill.h"
#include "constants/weather.h"
#include "mgba_printf/mgba.h"
#include "mgba_printf/mini_printf.h"
#include "abilities.hh"
#include "battle_skills.hh"

struct BattleWindowText {
    u8 fillValue;
    u8 fontId;
    u8 x;
    u8 y;
    u8 letterSpacing;
    u8 lineSpacing;
    u8 speed;
    u8 fgColor;
    u8 bgColor;
    u8 shadowColor;
};

extern const u16 gUnknown_08D85620[];

// this file's functions
static void ChooseMoveUsedParticle(u8 *textPtr);
static void ChooseTypeOfMoveUsedString(u8 *dst);
static void ExpandBattleTextBuffPlaceholders(const u8 *src, u8 *dst);

// EWRAM vars
static EWRAM_DATA u16 sBattlerAbilities[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA struct BattleMsgData *gBattleMsgDataPtr = NULL;

// const rom data
// todo: make some of those names less vague: attacker/target vs pkmn, etc.

static const u8 sText_Trainer1LoseText[] = _("{B_TRAINER1_LOSE_TEXT}");
static const u8 sText_PkmnGainedEXP[] = _("你的宝可梦获得了经验值！\p");
static const u8 sText_PkmnGrewToLv[] = _("{B_STACK_1_NAME_WITH_PREFIX}升到了\nLv.{B_BUFF2}！{WAIT_SE}\p");
static const u8 sText_PkmnLearnedMove[] = _("{B_BUFF1}学会了\n{B_BUFF2}！{WAIT_SE}\p");
static const u8 sText_TryToLearnMove1[] = _("{B_BUFF1}想学会\n{B_BUFF2}。\p");
static const u8 sText_TryToLearnMove2[] = _("但是{B_BUFF1}无法再\n学会更多招式了。\p");
static const u8 sText_TryToLearnMove3[] = _("要忘记一个招式\n来学习{B_BUFF2}吗？");
static const u8 sText_PkmnForgotMove[] = _("{B_BUFF1}忘记了\n{B_BUFF2}。\p");
static const u8 sText_StopLearningMove[] = _("{PAUSE 32}要停止学习\n{B_BUFF2}吗？");
static const u8 sText_DidNotLearnMove[] = _("{B_BUFF1}没有学会\n{B_BUFF2}。\p");
static const u8 sText_PkmnLearnedMove2[] = _("{B_ATK_NAME_WITH_PREFIX}学会了\n{B_BUFF1}！");
static const u8 sText_AttackMissed[] = _("{B_ATK_NAME_WITH_PREFIX}的\n攻击没有命中！");
static const u8 sText_PkmnProtectedItself[] = _("{B_DEF_NAME_WITH_PREFIX}\n保护了自己！");
static const u8 sText_StatsWontIncrease2[] = _("{B_ATK_NAME_WITH_PREFIX}的能力\n已经无法再提高了！");
static const u8 sText_AvoidedDamage[] = _("{B_DEF_NAME_WITH_PREFIX}靠特性\n避开了伤害！");
static const u8 sText_ItDoesntAffect[] = _("对{B_DEF_NAME_WITH_PREFIX}\n没有效果…");
static const u8 sText_AttackerFainted[] = _("{B_STACK_1_NAME_WITH_PREFIX}\n倒下了！");
static const u8 sText_TargetFainted[] = _("{B_DEF_NAME_WITH_PREFIX}\n倒下了！\p");
static const u8 sText_PlayerGotMoney[] = _("{B_PLAYER_NAME}获胜，\n得到了¥{B_BUFF1}！\p");
static const u8 sText_PlayerWhiteout[] = _("{B_PLAYER_NAME}已经没有\n能战斗的宝可梦了！\p");
static const u8 sText_PlayerWhiteout2[] = _("{B_PLAYER_NAME}惊慌失措，\n失去了¥{B_BUFF1}…\p… … … …\p{B_PLAYER_NAME}眼前发黑！{PAUSE_UNTIL_PRESS}");
static const u8 sText_PreventsEscape[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}用特性\n阻止了逃走！\p");
static const u8 sText_HitXTimes[] = _("命中了{B_BUFF1}次！");
static const u8 sText_PkmnFellAsleep[] = _("{B_EFF_NAME_WITH_PREFIX}\n睡着了！");
static const u8 sText_PkmnMadeSleep[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的特性\n让{B_EFF_NAME_WITH_PREFIX}睡着了！");
static const u8 sText_PkmnAlreadyAsleep[] = _("{B_DEF_NAME_WITH_PREFIX}\n已经睡着了！");
static const u8 sText_PkmnAlreadyAsleep2[] = _("{B_ATK_NAME_WITH_PREFIX}\n已经睡着了！");
static const u8 sText_PkmnWasntAffected[] = _("{B_DEF_NAME_WITH_PREFIX}\n没有受到影响！");
static const u8 sText_PkmnWasPoisoned[] = _("{B_EFF_NAME_WITH_PREFIX}\n中毒了！");
static const u8 sText_PkmnPoisonedBy[] = _("{B_EFF_NAME_WITH_PREFIX}因\n{B_SCR_ACTIVE_NAME_WITH_PREFIX}的{B_LAST_ABILITY}中毒！");
static const u8 sText_PkmnHurtByPoison[] = _("{B_ATK_NAME_WITH_PREFIX}\n受到了毒的伤害！");
static const u8 sText_PkmnAlreadyPoisoned[] = _("{B_DEF_NAME_WITH_PREFIX}\n已经中毒了。");
static const u8 sText_PkmnBadlyPoisoned[] = _("{B_EFF_NAME_WITH_PREFIX}\n中了剧毒！");
static const u8 sText_PkmnEnergyDrained[] = _("{B_DEF_NAME_WITH_PREFIX}的\n能量被吸取了！");
static const u8 sText_PkmnWasBurned[] = _("{B_EFF_NAME_WITH_PREFIX}灼伤了！");
static const u8 sText_PkmnBurnedBy[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的特性\n灼伤了{B_EFF_NAME_WITH_PREFIX}！");
static const u8 sText_PkmnHurtByBurn[] = _("{B_ATK_NAME_WITH_PREFIX}\n受到了灼伤的伤害！");
static const u8 sText_PkmnWasFrozen[] = _("{B_EFF_NAME_WITH_PREFIX}\n被冻住了！");
static const u8 sText_PkmnFrozenBy[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的特性\n冻住了{B_EFF_NAME_WITH_PREFIX}！");
static const u8 sText_PkmnIsFrozen[] = _("{B_ATK_NAME_WITH_PREFIX}\n被冻住了！");
static const u8 sText_PkmnWasDefrosted[] = _("{B_DEF_NAME_WITH_PREFIX}\n解冻了！");
static const u8 sText_PkmnWasDefrosted2[] = _("{B_ATK_NAME_WITH_PREFIX}\n解冻了！");
static const u8 sText_PkmnWasDefrostedBy[] = _("{B_ATK_NAME_WITH_PREFIX}\n因{B_CURRENT_MOVE}解冻了！");
static const u8 sText_PkmnWasParalyzed[] = _("{B_EFF_NAME_WITH_PREFIX}麻痹了！\n可能无法行动！");
static const u8 sText_PkmnWasParalyzedBy[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的特性\n使{B_EFF_NAME_WITH_PREFIX}麻痹了！\l可能无法行动！");
static const u8 sText_PkmnIsParalyzed[] = _("{B_ATK_NAME_WITH_PREFIX}麻痹了！\n无法行动！");
static const u8 sText_PkmnIsAlreadyParalyzed[] = _("{B_DEF_NAME_WITH_PREFIX}\n已经麻痹了！");
static const u8 sText_PkmnHealedParalysis[] = _("{B_DEF_NAME_WITH_PREFIX}\n的麻痹解除了！");
static const u8 sText_PkmnDreamEaten[] = _("{B_DEF_NAME_WITH_PREFIX}的\n梦被吃掉了！");
static const u8 sText_StatsWontIncrease[] = _("{B_ATK_NAME_WITH_PREFIX}的{B_STAT_CHANGER}\n无法再提高了！");
static const u8 sText_StatsWontDecrease[] = _("{B_DEF_NAME_WITH_PREFIX}的{B_STAT_CHANGER}\n无法再降低了！");
static const u8 sText_TeamStoppedWorking[] = _("我方的{B_BUFF1}\n效果消失了！");
static const u8 sText_FoeStoppedWorking[] = _("对手的{B_BUFF1}\n效果消失了！");
static const u8 sText_PkmnIsConfused[] = _("{B_ATK_NAME_WITH_PREFIX}\n混乱了！");
static const u8 sText_PkmnHealedConfusion[] = _("{B_ATK_NAME_WITH_PREFIX}\n解除了混乱！");
static const u8 sText_PkmnWasConfused[] = _("{B_EFF_NAME_WITH_PREFIX}\n陷入了混乱！");
static const u8 sText_PkmnAlreadyConfused[] = _("{B_DEF_NAME_WITH_PREFIX}\n已经混乱了！");
static const u8 sText_PkmnFellInLove[] = _("{B_DEF_NAME_WITH_PREFIX}\n陷入了着迷！");
static const u8 sText_PkmnInLove[] = _("{B_ATK_NAME_WITH_PREFIX}着迷于\n{B_SCR_ACTIVE_NAME_WITH_PREFIX}！");
static const u8 sText_PkmnImmobilizedByLove[] = _("{B_ATK_NAME_WITH_PREFIX}\n因着迷而无法行动！");
static const u8 sText_PkmnBlownAway[] = _("{B_DEF_NAME_WITH_PREFIX}\n被吹飞了！");
static const u8 sText_PkmnChangedType[] = _("{B_ATK_NAME_WITH_PREFIX}变成了\n{B_BUFF1}属性！");
static const u8 sText_PkmnFlinched[] = _("{B_ATK_NAME_WITH_PREFIX}畏缩了！");
static const u8 sText_PkmnRegainedHealth[] = _("{B_DEF_NAME_WITH_PREFIX}\n恢复了体力！");
static const u8 sText_PkmnHPFull[] = _("{B_DEF_NAME_WITH_PREFIX}的\nHP已满！");
static const u8 sText_PkmnRaisedSpDef[] = _("{B_ATK_PREFIX2}的{B_CURRENT_MOVE}\n提高了特防！");
static const u8 sText_PkmnRaisedDef[] = _("{B_ATK_PREFIX2}的{B_CURRENT_MOVE}\n提高了防御！");
static const u8 sText_PkmnCoveredByVeil[] = _("{B_ATK_PREFIX2}的队伍\n被神秘面纱笼罩了！");
static const u8 sText_PkmnUsedSafeguard[] = _("{B_DEF_NAME_WITH_PREFIX}的队伍\n受到了神秘守护的保护！");
static const u8 sText_PkmnSafeguardExpired[] = _("{B_ATK_PREFIX3}的队伍\n不再受神秘守护保护了！");
static const u8 sText_PkmnWentToSleep[] = _("{B_ATK_NAME_WITH_PREFIX}\n睡着了！");
static const u8 sText_PkmnSleptHealthy[] = _("{B_ATK_NAME_WITH_PREFIX}睡了一觉，\n变得健康了！");
static const u8 sText_PkmnWhippedWhirlwind[] = _("{B_ATK_NAME_WITH_PREFIX}\n刮起了旋风！");
static const u8 sText_PkmnTookSunlight[] = _("{B_ATK_NAME_WITH_PREFIX}\n吸收了阳光！");
static const u8 sText_PkmnLoweredHead[] = _("{B_ATK_NAME_WITH_PREFIX}\n低下了头！");
static const u8 sText_PkmnIsGlowing[] = _("{B_ATK_NAME_WITH_PREFIX}正在发光！");
static const u8 sText_PkmnFlewHigh[] = _("{B_ATK_NAME_WITH_PREFIX}\n飞向了高空！");
static const u8 sText_PkmnDugHole[] = _("{B_ATK_NAME_WITH_PREFIX}挖了个洞！");
static const u8 sText_PkmnSqueezedByBind[] = _("{B_DEF_NAME_WITH_PREFIX}被\n{B_ATK_NAME_WITH_PREFIX}的紧束缠住了！");
static const u8 sText_PkmnTrappedInVortex[] = _("{B_DEF_NAME_WITH_PREFIX}\n被困在漩涡中！");
static const u8 sText_PkmnWrappedBy[] = _("{B_DEF_NAME_WITH_PREFIX}被\n{B_ATK_NAME_WITH_PREFIX}卷紧了！");
static const u8 sText_PkmnClamped[] = _("{B_ATK_NAME_WITH_PREFIX}夹住了\n{B_DEF_NAME_WITH_PREFIX}！");
static const u8 sText_PkmnHurtBy[] = _("{B_ATK_NAME_WITH_PREFIX}\n受到了{B_BUFF1}的伤害！");
static const u8 sText_PkmnFreedFrom[] = _("{B_ATK_NAME_WITH_PREFIX}\n摆脱了{B_BUFF1}！");
static const u8 sText_PkmnCrashed[] = _("{B_ATK_NAME_WITH_PREFIX}冲过头\n撞到了地面！");
const u8 gText_PkmnShroudedInMist[] = _("{B_ATK_PREFIX2}\n被白雾笼罩了！");
static const u8 sText_PkmnProtectedByMist[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}\n受到了白雾保护！");
const u8 gText_PkmnGettingPumped[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}\n力量提升了！");
static const u8 sText_PkmnHitWithRecoil[] = _("{B_ATK_NAME_WITH_PREFIX}\n受到了反作用力伤害！");
static const u8 sText_PkmnProtectedItself2[] = _("{B_ATK_NAME_WITH_PREFIX}\n保护了自己！");
static const u8 sText_PkmnBuffetedBySandstorm[] = _("{B_ATK_NAME_WITH_PREFIX}\n受到了沙暴的伤害！");
static const u8 sText_PkmnPeltedByHail[] = _("{B_ATK_NAME_WITH_PREFIX}\n受到了冰雹的伤害！");
static const u8 sText_PkmnSeeded[] = _("{B_DEF_NAME_WITH_PREFIX}被种下了种子！");
static const u8 sText_PkmnEvadedAttack[] = _("{B_DEF_NAME_WITH_PREFIX}\n躲开了攻击！");
static const u8 sText_PkmnSappedByLeechSeed[] = _("{B_ATK_NAME_WITH_PREFIX}的体力\n被寄生种子吸取了！");
static const u8 sText_PkmnFastAsleep[] = _("{B_ATK_NAME_WITH_PREFIX}\n睡得很香。");
static const u8 sText_PkmnWokeUp[] = _("{B_ATK_NAME_WITH_PREFIX}醒来了！");
static const u8 sText_PkmnUproarKeptAwake[] = _("但{B_SCR_ACTIVE_NAME_WITH_PREFIX}的\n吵闹让它睡不着！");
static const u8 sText_PkmnWokeUpInUproar[] = _("{B_ATK_NAME_WITH_PREFIX}在吵闹声中\n醒来了！");
static const u8 sText_PkmnCausedUproar[] = _("{B_ATK_NAME_WITH_PREFIX}\n开始吵闹了！");
static const u8 sText_PkmnMakingUproar[] = _("{B_ATK_NAME_WITH_PREFIX}\n正在吵闹！");
static const u8 sText_PkmnCalmedDown[] = _("{B_ATK_NAME_WITH_PREFIX}安静下来了。");
static const u8 sText_PkmnCantSleepInUproar[] = _("但{B_DEF_NAME_WITH_PREFIX}\n在吵闹声中睡不着！");
static const u8 sText_PkmnStockpiled[] = _("{B_ATK_NAME_WITH_PREFIX}蓄力了\n{B_BUFF1}次！");
static const u8 sText_PkmnCantStockpile[] = _("{B_ATK_NAME_WITH_PREFIX}\n无法继续蓄力了！");
static const u8 sText_PkmnCantSleepInUproar2[] = _("但{B_DEF_NAME_WITH_PREFIX}\n在吵闹声中睡不着！");
static const u8 sText_UproarKeptPkmnAwake[] = _("但吵闹声让\n{B_DEF_NAME_WITH_PREFIX}睡不着！");
static const u8 sText_PkmnStayedAwakeUsing[] = _("{B_DEF_NAME_WITH_PREFIX}靠特性\n保持了清醒！");
static const u8 sText_PkmnStoringEnergy[] = _("{B_ATK_NAME_WITH_PREFIX}\n正在储存能量！");
static const u8 sText_PkmnUnleashedEnergy[] = _("{B_ATK_NAME_WITH_PREFIX}\n释放了能量！");
static const u8 sText_PkmnFatigueConfusion[] = _("{B_ATK_NAME_WITH_PREFIX}因疲劳\n陷入了混乱！");
static const u8 sText_PlayerPickedUpMoney[] = _("{B_PLAYER_NAME}捡到了\n¥{B_BUFF1}！\p");
static const u8 sText_PkmnUnaffected[] = _("{B_DEF_NAME_WITH_PREFIX}\n不受影响！");
static const u8 sText_PkmnTransformedInto[] = _("{B_ATK_NAME_WITH_PREFIX}变成了\n{B_BUFF1}！");
static const u8 sText_PkmnMadeSubstitute[] = _("{B_ATK_NAME_WITH_PREFIX}制造了\n替身！");
static const u8 sText_PkmnHasSubstitute[] = _("{B_ATK_NAME_WITH_PREFIX}\n已经有替身了！");
static const u8 sText_SubstituteDamaged[] = _("替身代替{B_DEF_NAME_WITH_PREFIX}\n承受了伤害！\p");
static const u8 sText_PkmnSubstituteFaded[] = _("{B_DEF_NAME_WITH_PREFIX}的\n替身消失了！\p");
static const u8 sText_PkmnMustRecharge[] = _("{B_ATK_NAME_WITH_PREFIX}\n必须进行充能！");
static const u8 sText_PkmnRageBuilding[] = _("{B_DEF_NAME_WITH_PREFIX}的愤怒\n正在积蓄！");
static const u8 sText_PkmnMoveWasDisabled[] = _("{B_DEF_NAME_WITH_PREFIX}的{B_BUFF1}\n被禁止使用了！");
static const u8 sText_PkmnMoveIsDisabled[] = _("{B_ACTIVE_NAME_WITH_PREFIX}的{B_CURRENT_MOVE}\n被禁止使用了！\p");
static const u8 sText_PkmnMoveDisabledNoMore[] = _("{B_ATK_NAME_WITH_PREFIX}\n的禁令解除了！");
static const u8 sText_PkmnGotEncore[] = _("{B_DEF_NAME_WITH_PREFIX}受到了\n再来一次！");
static const u8 sText_PkmnEncoreEnded[] = _("{B_ATK_NAME_WITH_PREFIX}的\n再来一次结束了！");
static const u8 sText_PkmnTookAim[] = _("{B_ATK_NAME_WITH_PREFIX}\n瞄准了{B_DEF_NAME_WITH_PREFIX}！");
static const u8 sText_PkmnSketchedMove[] = _("{B_ATK_NAME_WITH_PREFIX}写生了\n{B_BUFF1}！");
static const u8 sText_PkmnTryingToTakeFoe[] = _("{B_ATK_NAME_WITH_PREFIX}想\n拉对手一起倒下！");
static const u8 sText_PkmnTookFoe[] = _("{B_DEF_NAME_WITH_PREFIX}拉上了\n{B_ATK_NAME_WITH_PREFIX}！");
static const u8 sText_PkmnReducedPP[] = _("{B_DEF_NAME_WITH_PREFIX}的{B_BUFF1}\n减少了{B_BUFF2}！");
static const u8 sText_PkmnStoleItem[] = _("{B_ATK_NAME_WITH_PREFIX}偷走了\n{B_DEF_NAME_WITH_PREFIX}的{B_LAST_ITEM}！");
static const u8 sText_TargetCantEscapeNow[] = _("{B_DEF_NAME_WITH_PREFIX}\n已经无法逃走了！");
static const u8 sText_PkmnFellIntoNightmare[] = _("{B_DEF_NAME_WITH_PREFIX}\n陷入了恶梦！");
static const u8 sText_PkmnLockedInNightmare[] = _("{B_ATK_NAME_WITH_PREFIX}\n被恶梦困住了！");
static const u8 sText_PkmnLaidCurse[] = _("{B_ATK_NAME_WITH_PREFIX}削减HP，\n对{B_DEF_NAME_WITH_PREFIX}下了诅咒！");
static const u8 sText_PkmnAfflictedByCurse[] = _("{B_ATK_NAME_WITH_PREFIX}\n受到了诅咒！");
static const u8 sText_SpikesScattered[] = _("对手的场地周围撒满了撒菱！");
static const u8 sText_PkmnHurtBySpikes[] = _("{B_STACK_1_NAME_WITH_PREFIX}\n受到了撒菱的伤害！");
static const u8 sText_PkmnIdentified[] = _("{B_ATK_NAME_WITH_PREFIX}\n识破了{B_DEF_NAME_WITH_PREFIX}！");
static const u8 sText_PkmnPerishCountFell[] = _("{B_ATK_NAME_WITH_PREFIX}的灭亡计数\n变成了{B_BUFF1}！");
static const u8 sText_PkmnBracedItself[] = _("{B_ATK_NAME_WITH_PREFIX}\n做好了准备！");
static const u8 sText_PkmnEnduredHit[] = _("{B_DEF_NAME_WITH_PREFIX}\n撑住了攻击！");
static const u8 sText_MagnitudeStrength[] = _("威力{B_BUFF1}！");
static const u8 sText_PkmnCutHPMaxedAttack[] = _("{B_ATK_NAME_WITH_PREFIX}削减HP，\n攻击提升到最大！");
static const u8 sText_PkmnCopiedStatChanges[] = _("{B_ATK_NAME_WITH_PREFIX}复制了\n{B_DEF_NAME_WITH_PREFIX}的能力变化！");
static const u8 sText_PkmnGotFree[] = _("{B_ATK_NAME_WITH_PREFIX}摆脱了\n{B_DEF_NAME_WITH_PREFIX}的{B_BUFF1}！");
static const u8 sText_PkmnShedLeechSeed[] = _("{B_ATK_NAME_WITH_PREFIX}\n去除了寄生种子！");
static const u8 sText_PkmnBlewAwaySpikes[] = _("{B_ATK_NAME_WITH_PREFIX}\n吹散了撒菱！");
static const u8 sText_PkmnFledFromBattle[] = _("{B_ATK_NAME_WITH_PREFIX}\n从战斗中逃走了！");
static const u8 sText_PkmnForesawAttack[] = _("{B_ATK_NAME_WITH_PREFIX}\n预知了攻击！");
static const u8 sText_PkmnTookAttack[] = _("{B_DEF_NAME_WITH_PREFIX}承受了\n{B_BUFF1}攻击！");
static const u8 sText_PkmnAttack[] = _("{B_BUFF1}攻击！");
static const u8 sText_PkmnCenterAttention[] = _("{B_ATK_NAME_WITH_PREFIX}\n成为了焦点！");
static const u8 sText_PkmnChargingPower[] = _("{B_ATK_NAME_WITH_PREFIX}\n开始积蓄力量！");
static const u8 sText_NaturePowerTurnedInto[] = _("自然之力变成了\n{B_CURRENT_MOVE}！");
static const u8 sText_PkmnStatusNormal[] = _("{B_ATK_NAME_WITH_PREFIX}的状态\n恢复了正常！");
static const u8 sText_PkmnHasNoMovesLeft[] = _("{B_ACTIVE_NAME_WITH_PREFIX}\n没有可用的招式了！\p");
static const u8 sText_PkmnSubjectedToTorment[] = _("{B_DEF_NAME_WITH_PREFIX}受到了\n无理取闹！");
static const u8 sText_PkmnCantUseMoveTorment[] = _("{B_ACTIVE_NAME_WITH_PREFIX}因无理取闹\n无法连续使用同一招式！\p");
static const u8 sText_PkmnTighteningFocus[] = _("{B_ATK_NAME_WITH_PREFIX}\n集中了精神！");
static const u8 sText_PkmnFellForTaunt[] = _("{B_DEF_NAME_WITH_PREFIX}\n中了挑衅！");
static const u8 sText_PkmnCantUseMoveTaunt[] = _("{B_ACTIVE_NAME_WITH_PREFIX}因挑衅\n无法使用{B_CURRENT_MOVE}！\p");
static const u8 sText_PkmnReadyToHelp[] = _("{B_ATK_NAME_WITH_PREFIX}准备\n帮助{B_DEF_NAME_WITH_PREFIX}！");
static const u8 sText_PkmnSwitchedItems[] = _("{B_ATK_NAME_WITH_PREFIX}\n和对手交换了道具！");
static const u8 sText_PkmnCopiedFoe[] = _("{B_ATK_NAME_WITH_PREFIX}复制了\n{B_DEF_NAME_WITH_PREFIX}的{B_DEF_ABILITY}！");
static const u8 sText_PkmnMadeWish[] = _("{B_ATK_NAME_WITH_PREFIX}许下了愿望！");
static const u8 sText_PkmnWishCameTrue[] = _("{B_DEF_NAME_WITH_PREFIX}的愿望\n实现了！");
static const u8 sText_PkmnPlantedRoots[] = _("{B_ATK_NAME_WITH_PREFIX}扎下了根！");
static const u8 sText_PkmnAbsorbedNutrients[] = _("{B_ATK_NAME_WITH_PREFIX}用根\n吸取了养分！");
static const u8 sText_PkmnAnchoredItself[] = _("{B_DEF_NAME_WITH_PREFIX}\n用根固定了身体！");
static const u8 sText_PkmnWasMadeDrowsy[] = _("{B_ATK_NAME_WITH_PREFIX}让\n{B_DEF_NAME_WITH_PREFIX}昏昏欲睡！");
static const u8 sText_PkmnKnockedOff[] = _("{B_ATK_NAME_WITH_PREFIX}打落了\n{B_DEF_NAME_WITH_PREFIX}的{B_LAST_ITEM}！");
static const u8 sText_PkmnSwappedAbilities[] = _("{B_ATK_NAME_WITH_PREFIX}\n和对手交换了特性！");
static const u8 sText_PkmnSealedOpponentMove[] = _("{B_ATK_NAME_WITH_PREFIX}封印了\n对手的招式！");
static const u8 sText_PkmnCantUseMoveSealed[] = _("{B_ACTIVE_NAME_WITH_PREFIX}无法使用\n被封印的{B_CURRENT_MOVE}！\p");
static const u8 sText_PkmnWantsGrudge[] = _("{B_ATK_NAME_WITH_PREFIX}\n让对手心生怨恨！");
static const u8 sText_PkmnLostPPGrudge[] = _("{B_ATK_NAME_WITH_PREFIX}的{B_BUFF1}\n因怨恨失去了全部PP！");
static const u8 sText_PkmnShroudedItself[] = _("{B_ATK_NAME_WITH_PREFIX}\n用{B_CURRENT_MOVE}包裹了自己！");
static const u8 sText_PkmnMoveBounced[] = _("{B_ATK_NAME_WITH_PREFIX}的{B_CURRENT_MOVE}\n被魔法反射弹回了！");
static const u8 sText_PkmnWaitsForTarget[] = _("{B_ATK_NAME_WITH_PREFIX}\n正在等待目标行动！");
static const u8 sText_PkmnSnatchedMove[] = _("{B_DEF_NAME_WITH_PREFIX}抢走了\n{B_SCR_ACTIVE_NAME_WITH_PREFIX}的招式！");
static const u8 sText_PkmnMadeItRain[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的特性\n引发了降雨！");
static const u8 sText_PkmnRaisedSpeed[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的特性\n提高了速度！");
static const u8 sText_PkmnProtectedBy[] = _("{B_DEF_NAME_WITH_PREFIX}\n受到了{B_DEF_ABILITY}的保护！");
static const u8 sText_PkmnPreventsUsage[] = _("{B_DEF_NAME_WITH_PREFIX}的湿气\n阻止了{B_ATK_NAME_WITH_PREFIX}\l使用{B_CURRENT_MOVE}！");
static const u8 sText_PkmnRestoredHPUsing[] = _("{B_DEF_NAME_WITH_PREFIX}用特性\n恢复了HP！");
static const u8 sText_PkmnChangedTypeWith[] = _("{B_DEF_NAME_WITH_PREFIX}的特性\n使它变成了{B_BUFF1}属性！");
static const u8 sText_PkmnPreventsParalysisWith[] = _("{B_EFF_NAME_WITH_PREFIX}的特性\n防止了麻痹！");
static const u8 sText_PkmnPreventsRomanceWith[] = _("{B_DEF_NAME_WITH_PREFIX}的特性\n防止了着迷！");
static const u8 sText_PkmnPreventsPoisoningWith[] = _("{B_EFF_NAME_WITH_PREFIX}的特性\n防止了中毒！");
static const u8 sText_PkmnPreventsConfusionWith[] = _("{B_DEF_NAME_WITH_PREFIX}的特性\n防止了混乱！");
static const u8 sText_PkmnRaisedFirePowerWith[] = _("{B_DEF_NAME_WITH_PREFIX}的特性\n提高了火属性威力！");
static const u8 sText_PkmnAnchorsItselfWith[] = _("{B_DEF_NAME_WITH_PREFIX}靠\n{B_LAST_ABILITY}固定了身体！");
static const u8 sText_PkmnCutsAttackWith[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的特性\n削弱了{B_DEF_NAME_WITH_PREFIX}的攻击！");
static const u8 sText_PkmnPreventsStatLossWith[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的特性\n防止了能力下降！");
static const u8 sText_PkmnHurtsWith[] = _("{B_ATK_NAME_WITH_PREFIX}受到了\n{B_DEF_NAME_WITH_PREFIX}的{B_BUFF1}伤害！");
static const u8 sText_PkmnTraced[] = _("{B_STACK_1_NAME_WITH_PREFIX}复制了\n{B_STACK_2_NAME_WITH_PREFIX}的{B_LAST_ABILITY}！");
const u8 gText_StatSharply[] = _("大幅");
const u8 gText_StatRose[] = _("上升!");
static const u8 sText_StatHarshly[] = _("急剧");
static const u8 sText_StatFell[] = _("下降!");
static const u8 sText_AttackersStatRose[] = _("{B_ATK_NAME_WITH_PREFIX}的{B_STAT_CHANGER}\n{B_BUFF2}");
const u8 gText_DefendersStatRose[] = _("{B_DEF_NAME_WITH_PREFIX}的{B_STAT_CHANGER}\n{B_BUFF2}");
static const u8 sText_AttackersStatFell[] = _("{B_ATK_NAME_WITH_PREFIX}的{B_STAT_CHANGER}\n{B_BUFF2}");
static const u8 sText_DefendersStatFell[] = _("{B_DEF_NAME_WITH_PREFIX}的{B_STAT_CHANGER}\n{B_BUFF2}");
static const u8 sText_CriticalHit[] = _("击中要害！");
static const u8 sText_OneHitKO[] = _("一击必杀！");
static const u8 sText_123Poof[] = _("{PAUSE 32}1、{PAUSE 15}2、然后{PAUSE 15}… {PAUSE 15}… {PAUSE 15}… {PAUSE 15}{PLAY_SE SE_BALL_BOUNCE_1}啪！\p");
static const u8 sText_AndEllipsis[] = _("然后…\p");
static const u8 sText_NotVeryEffective[] = _("效果不理想..。");
static const u8 sText_SuperEffective[] = _("效果绝佳！");
static const u8 sText_GotAwaySafely[] = _("{PLAY_SE SE_FLEE}安全逃走了！\p");
static const u8 sText_WildPkmnFled[] = _("{PLAY_SE SE_FLEE}野生{B_BUFF1}逃走了！");
static const u8 sText_NoRunningFromTrainers[] = _("不行！训练家对战\n不能逃跑！\p");
static const u8 sText_CantEscape[] = _("无法逃走！\p");
static const u8 sText_DontLeaveBirch[] = _("小田卷博士：别丢下我啊！\p");
static const u8 sText_ButNothingHappened[] = _("但是什么也没发生！");
static const u8 sText_ButItFailed[] = _("但是失败了！");
static const u8 sText_ItHurtConfusion[] = _("{B_ATK_NAME_WITH_PREFIX}\n在混乱中伤害了自己！");
static const u8 sText_MirrorMoveFailed[] = _("仿效失败了！");
static const u8 sText_StartedToRain[] = _("开始下雨了！");
static const u8 sText_DownpourStarted[] = _("下起了倾盆大雨！");
static const u8 sText_RainContinues[] = _("雨持续下着。");
static const u8 sText_DownpourContinues[] = _("暴雨持续着。");
static const u8 sText_RainStopped[] = _("雨停了。");
static const u8 sText_SandstormBrewed[] = _("沙暴刮了起来！");
static const u8 sText_SandstormRages[] = _("沙暴肆虐。");
static const u8 sText_SandstormSubsided[] = _("沙暴平息了。");
static const u8 sText_SunlightGotBright[] = _("阳光变强了！");
static const u8 sText_SunlightStrong[] = _("阳光很强烈。");
static const u8 sText_SunlightFaded[] = _("阳光减弱了。");
static const u8 sText_StartedHail[] = _("开始下冰雹了！");
static const u8 sText_HailContinues[] = _("冰雹持续落下。");
static const u8 sText_HailStopped[] = _("冰雹停了。");
static const u8 sText_FailedToSpitUp[] = _("但是没能吐出\n任何东西！");
static const u8 sText_FailedToSwallow[] = _("但是没能吞下\n任何东西！");
static const u8 sText_WindBecameHeatWave[] = _("风变成了热风！");
static const u8 sText_StatChangesGone[] = _("所有能力变化\n都被消除了！");
static const u8 sText_CoinsScattered[] = _("金币散落一地！");
static const u8 sText_TooWeakForSubstitute[] = _("太弱了，\n无法制造替身！");
static const u8 sText_SharedPain[] = _("双方分享了彼此的痛苦！");
static const u8 sText_BellChimed[] = _("钟声响起了！");
static const u8 sText_FaintInThree[] = _("所有受影响宝可梦\n将在三回合后倒下！");
static const u8 sText_NoPPLeft[] = _("这个招式的\nPP用完了！\p");
static const u8 sText_ButNoPPLeft[] = _("但是这个招式\n没有PP了！");
static const u8 sText_PlayerUsedItem[] = _("{B_PLAYER_NAME}使用了\n{B_LAST_ITEM}！");
static const u8 sText_WallyUsedItem[] = _("小胜使用了\n{B_LAST_ITEM}！");
static const u8 sText_TrainerBlockedBall[] = _("训练家挡住了球！");
static const u8 sText_DontBeAThief[] = _("别当小偷！");
static const u8 sText_ItDodgedBall[] = _("球被躲开了！\n无法捕捉这只宝可梦！");
static const u8 sText_YouMissedPkmn[] = _("没有命中宝可梦！");
static const u8 sText_PkmnBrokeFree[] = _("啊呀！\n宝可梦挣脱了！");
static const u8 sText_ItAppearedCaught[] = _("哎呀！\n它差点就被抓住了！");
static const u8 sText_AarghAlmostHadIt[] = _("啊！\n就差一点！");
static const u8 sText_ShootSoClose[] = _("可恶！就差那么\n一点点！");
static const u8 sText_GotchaPkmnCaught[] = _("成功了！\n捕获了{B_DEF_NAME}！{WAIT_SE}{PLAY_BGM MUS_CAUGHT}\p");
static const u8 sText_GotchaPkmnCaught2[] = _("成功了！\n捕获了{B_DEF_NAME}！{WAIT_SE}{PLAY_BGM MUS_CAUGHT}{PAUSE 127}");
static const u8 sText_GiveNicknameCaptured[] = _("要给捕获的{B_DEF_NAME}\n起昵称吗？");
static const u8 sText_PkmnSentToPC[] = _("{B_DEF_NAME}被传送到了\n{B_PC_CREATOR_NAME}的电脑。");
static const u8 sText_PkmnDataAddedToDex[] = _("{B_DEF_NAME}的数据\n已添加到图鉴。\p");
static const u8 sText_ItIsRaining[] = _("正在下雨。");
static const u8 sText_SandstormIsRaging[] = _("沙暴正在肆虐。");
static const u8 sText_CantEscape2[] = _("无法逃走！\p");
static const u8 sText_PkmnIgnoresAsleep[] = _("{B_ATK_NAME_WITH_PREFIX}睡着了，\n无视了命令！");
static const u8 sText_PkmnIgnoredOrders[] = _("{B_ATK_NAME_WITH_PREFIX}\n无视了命令！");
static const u8 sText_PkmnBeganToNap[] = _("{B_ATK_NAME_WITH_PREFIX}\n开始打盹了！");
static const u8 sText_PkmnLoafing[] = _("{B_ATK_NAME_WITH_PREFIX}\n在偷懒！");
static const u8 sText_PkmnWontObey[] = _("{B_ATK_NAME_WITH_PREFIX}\n不听话！");
static const u8 sText_PkmnTurnedAway[] = _("{B_ATK_NAME_WITH_PREFIX}转过了头！");
static const u8 sText_PkmnPretendNotNotice[] = _("{B_ATK_NAME_WITH_PREFIX}\n装作没听见！");
static const u8 sText_EnemyAboutToSwitchPkmn[] = _("{B_TRAINER1_CLASS} {B_TRAINER1_NAME}\n即将派出{B_BUFF2}。\p{B_PLAYER_NAME}要更换\n宝可梦吗？");
static const u8 sText_CreptCloser[] = _("{B_PLAYER_NAME}悄悄靠近了\n{B_OPPONENT_MON1_NAME}！");
static const u8 sText_CantGetCloser[] = _("{B_PLAYER_NAME}无法再靠近了！");
static const u8 sText_PkmnWatchingCarefully[] = _("{B_OPPONENT_MON1_NAME}\n正仔细观察！");
static const u8 sText_PkmnCuriousAboutX[] = _("{B_OPPONENT_MON1_NAME}对\n{B_BUFF1}很感兴趣！");
static const u8 sText_PkmnEnthralledByX[] = _("{B_OPPONENT_MON1_NAME}被\n{B_BUFF1}吸引住了！");
static const u8 sText_PkmnIgnoredX[] = _("{B_OPPONENT_MON1_NAME}完全无视了\n{B_BUFF1}！");
static const u8 sText_ThrewPokeblockAtPkmn[] = _("{B_PLAYER_NAME}向{B_OPPONENT_MON1_NAME}\n投出了{POKEBLOCK}！");
static const u8 sText_OutOfSafariBalls[] = _("{PLAY_SE SE_DING_DONG}主持人：狩猎球\n用完了！游戏结束！\p");
static const u8 sText_PkmnsItemCuredParalysis[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的{B_LAST_ITEM}\n治愈了麻痹！");
static const u8 sText_PkmnsItemCuredPoison[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的{B_LAST_ITEM}\n治愈了中毒！");
static const u8 sText_PkmnsItemHealedBurn[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的{B_LAST_ITEM}\n治愈了灼伤！");
static const u8 sText_PkmnsItemDefrostedIt[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的{B_LAST_ITEM}\n解除了冰冻！");
static const u8 sText_PkmnsItemWokeIt[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的{B_LAST_ITEM}\n唤醒了它！");
static const u8 sText_PkmnsItemSnappedOut[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的{B_LAST_ITEM}\n解除了混乱！");
static const u8 sText_PkmnsItemCuredProblem[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的{B_LAST_ITEM}\n治愈了{B_BUFF1}状态！");
static const u8 sText_PkmnsItemRestoredHealth[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的{B_LAST_ITEM}\n恢复了体力！");
static const u8 sText_PkmnsItemRestoredPP[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的{B_LAST_ITEM}\n恢复了{B_BUFF1}的PP！");
static const u8 sText_PkmnsItemRestoredStatus[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的{B_LAST_ITEM}\n恢复了状态！");
static const u8 sText_PkmnsItemRestoredHPALittle[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的{B_LAST_ITEM}\n少量恢复了HP！");
static const u8 sText_ItemAllowsOnlyYMove[] = _("{B_LAST_ITEM}只允许\n使用{B_CURRENT_MOVE}！\p");
static const u8 sText_PkmnHungOnWithX[] = _("{B_DEF_NAME_WITH_PREFIX}靠\n{B_LAST_ITEM}撑住了！");
const u8 gText_EmptyString3[] = _("");
static const u8 sText_PkmnsXPreventsBurns[] = _("{B_EFF_NAME_WITH_PREFIX}的{B_LAST_ABILITY}\n防止了灼伤！");
static const u8 sText_PkmnsXBlocksY[] = _("{B_DEF_NAME_WITH_PREFIX}的{B_DEF_ABILITY}\n挡住了{B_CURRENT_MOVE}！");
static const u8 sText_PkmnsXRestoredHPALittle2[] = _("{B_ATK_NAME_WITH_PREFIX}的特性\n少量恢复了HP！");
static const u8 sText_PkmnsXWhippedUpSandstorm[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的特性\n刮起了沙暴！");
static const u8 sText_PkmnsXPreventsYLoss[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的特性\n防止{B_STAT_CHANGER}下降！");
static const u8 sText_PkmnsXInfatuatedY[] = _("{B_DEF_NAME_WITH_PREFIX}的特性\n使{B_ATK_NAME_WITH_PREFIX}着迷了！");
static const u8 sText_PkmnsXMadeYIneffective[] = _("{B_DEF_NAME_WITH_PREFIX}的特性\n使{B_CURRENT_MOVE}失效了！");
static const u8 sText_PkmnsXCuredYProblem[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的特性\n治愈了{B_BUFF1}状态！");
static const u8 sText_ItSuckedLiquidOoze[] = _("它吸收了\n污泥浆！");
static const u8 sText_PkmnTransformed[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}变身了！");
static const u8 sText_ElectricityWeakened[] = _("电力的威力\n被削弱了！");
static const u8 sText_FireWeakened[] = _("火属性的力量\n被削弱了！");
static const u8 sText_PkmnHidUnderwater[] = _("{B_ATK_NAME_WITH_PREFIX}\n潜入了水中！");
static const u8 sText_PkmnSprangUp[] = _("{B_ATK_NAME_WITH_PREFIX}\n跳了出来！");
static const u8 sText_HMMovesCantBeForgotten[] = _("现在不能忘记\n秘传招式。\p");
static const u8 sText_XFoundOneY[] = _("{B_ATK_NAME_WITH_PREFIX}\n取回了{B_LAST_ITEM}！");
static const u8 sText_PlayerDefeatedLinkTrainerTrainer1[] = _("玩家击败了\n{B_TRAINER1_CLASS} {B_TRAINER1_NAME}！\p");
static const u8 sText_SoothingAroma[] = _("一股舒缓的香气\n弥漫在周围！");
static const u8 sText_ItemsCantBeUsedNow[] = _("现在不能使用道具。{PAUSE 64}");
static const u8 sText_ForXCommaYZ[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的\n{B_LAST_ITEM} {B_BUFF1}");
static const u8 sText_UsingItemTheStatOfPkmnRose[] = _("使用{B_LAST_ITEM}，\n{B_SCR_ACTIVE_NAME_WITH_PREFIX}的{B_STAT_CHANGER}{B_BUFF2}");
static const u8 sText_PkmnUsedXToGetPumped[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}用\n{B_LAST_ITEM}振作了精神！");
static const u8 sText_PkmnsXMadeYUseless[] = _("{B_DEF_NAME_WITH_PREFIX}的特性\n使{B_CURRENT_MOVE}无效了！");
static const u8 sText_PkmnTrappedBySandTomb[] = _("{B_DEF_NAME_WITH_PREFIX}\n被沙暴困住了！");
static const u8 sText_EmptyString4[] = _("");
static const u8 sText_ABoosted[] = _("获得加成");
static const u8 sText_PkmnsXIntensifiedSun[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的特性\n增强了阳光！");
static const u8 sText_PkmnMakesGroundMiss[] = _("{B_DEF_NAME_WITH_PREFIX}的特性\n使地面招式无法命中！");
static const u8 sText_YouThrowABallNowRight[] = _("你现在要扔球了吧？\n我......我会尽力的！");
static const u8 sText_PkmnsXTookAttack[] = _("{B_DEF_NAME_WITH_PREFIX}的特性\n承受了攻击！");
static const u8 sText_PkmnChoseXAsDestiny[] = _("{B_ATK_NAME_WITH_PREFIX}选择了\n{B_CURRENT_MOVE}作为命运！");
static const u8 sText_PkmnLostFocus[] = _("{B_ATK_NAME_WITH_PREFIX}\n失去了集中！");
static const u8 sText_UseNextPkmn[] = _("要使用下一只宝可梦吗？");
static const u8 sText_PkmnFledUsingIts[] = _("{PLAY_SE SE_FLEE}{B_ATK_NAME_WITH_PREFIX}用\n{B_LAST_ITEM}逃走了！\p");
static const u8 sText_PkmnFledUsing[] = _("{PLAY_SE SE_FLEE}{B_ATK_NAME_WITH_PREFIX}靠\n{B_ATK_ABILITY}逃走了！\p");
static const u8 sText_PkmnWasDraggedOut[] = _("{B_DEF_NAME_WITH_PREFIX}\n被拖了出来！\p");
static const u8 sText_PreventedFromWorking[] = _("{B_DEF_NAME_WITH_PREFIX}的{B_DEF_ABILITY}\n阻止了{B_SCR_ACTIVE_NAME_WITH_PREFIX}的\l{B_BUFF1}生效！");
static const u8 sText_PkmnsItemNormalizedStatus[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的{B_LAST_ITEM}\n使状态恢复正常！");
static const u8 sText_Trainer1UsedItem[] = _("{B_TRAINER1_CLASS} {B_TRAINER1_NAME}\n使用了{B_LAST_ITEM}！");
static const u8 sText_BoxIsFull[] = _("盒子已满！\n无法再捕捉了！\p");
static const u8 sText_PkmnAvoidedAttack[] = _("{B_DEF_NAME_WITH_PREFIX}\n躲开了攻击！");
static const u8 sText_PkmnsXMadeItIneffective[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的特性\n使它无效了！");
static const u8 sText_PkmnsXPreventsFlinching[] = _("{B_EFF_NAME_WITH_PREFIX}的{B_LAST_ABILITY}\n防止了畏缩！");
static const u8 sText_PkmnAlreadyHasBurn[] = _("{B_DEF_NAME_WITH_PREFIX}\n已经灼伤了。");
static const u8 sText_StatsWontDecrease2[] = _("{B_DEF_NAME_WITH_PREFIX}的能力\n已经无法再降低了！");
static const u8 sText_PkmnsXBlocksY2[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的特性\n挡住了{B_CURRENT_MOVE}！");
static const u8 sText_PkmnsXWoreOff[] = _("{B_ATK_PREFIX1}的{B_BUFF1}\n效果消失了！");
static const u8 sText_PkmnRaisedDefALittle[] = _("{B_ATK_PREFIX2}的{B_CURRENT_MOVE}\n少量提高了防御！");
static const u8 sText_PkmnRaisedSpDefALittle[] = _("{B_ATK_PREFIX2}的{B_CURRENT_MOVE}\n少量提高了特防！");
static const u8 sText_TheWallShattered[] = _("墙壁碎裂了！");
static const u8 sText_PkmnsXPreventsYsZ[] = _("{B_ATK_NAME_WITH_PREFIX}的{B_ATK_ABILITY}\n阻止了{B_DEF_NAME_WITH_PREFIX}的\l{B_DEF_ABILITY}生效！");
static const u8 sText_PkmnsXCuredItsYProblem[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的特性\n治愈了{B_BUFF1}状态！");
static const u8 sText_AttackerCantEscape[] = _("{B_ATK_NAME_WITH_PREFIX}无法逃走！");
static const u8 sText_PkmnObtainedX[] = _("{B_ATK_NAME_WITH_PREFIX}获得了\n{B_BUFF1}。");
static const u8 sText_PkmnObtainedX2[] = _("{B_DEF_NAME_WITH_PREFIX}获得了\n{B_BUFF2}。");
static const u8 sText_PkmnObtainedXYObtainedZ[] = _("{B_ATK_NAME_WITH_PREFIX}获得了\n{B_BUFF1}。\p{B_DEF_NAME_WITH_PREFIX}获得了\n{B_BUFF2}。");
static const u8 sText_ButNoEffect[] = _("但是没有效果！");
static const u8 sText_PkmnsXHadNoEffectOnY[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的特性\n对{B_EFF_NAME_WITH_PREFIX}没有效果！");
static const u8 sText_TwoInGameTrainersDefeated[] = _("{B_TRAINER1_CLASS} {B_TRAINER1_NAME}和\n{B_TRAINER2_CLASS} {B_TRAINER2_NAME}\l被打败了！\p");
static const u8 sText_Trainer2LoseText[] = _("{B_TRAINER2_LOSE_TEXT}");
static const u8 sText_PkmnIncapableOfPower[] = _("{B_ATK_NAME_WITH_PREFIX}\n似乎无法发挥力量！");
static const u8 sText_GlintAppearsInEye[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}\n眼中闪过一道光！");
static const u8 sText_PkmnGettingIntoPosition[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}\n摆好了架势！");
static const u8 sText_PkmnBeganGrowlingDeeply[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}低声咆哮起来！");
static const u8 sText_PkmnEagerForMore[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}还想再战！");
static const u8 sText_DefeatedOpponentByReferee[] = _("{B_PLAYER_MON1_NAME}经裁判判定\n战胜了对手的{B_OPPONENT_MON1_NAME}！");
static const u8 sText_LostToOpponentByReferee[] = _("{B_PLAYER_MON1_NAME}经裁判判定\n输给了对手的{B_OPPONENT_MON1_NAME}！");
static const u8 sText_TiedOpponentByReferee[] = _("{B_PLAYER_MON1_NAME}经裁判判定\n与对手的{B_OPPONENT_MON1_NAME}打平！");
static const u8 sText_QuestionForfeitMatch[] = _("要放弃比赛现在退出吗？");
static const u8 sText_ForfeitedMatch[] = _("{B_PLAYER_NAME}放弃了比赛！");
const u8 gText_PkmnTransferredSomeonesPC[] = _("{STR_VAR_2}被传送到了\n某人的电脑。\p它被放进了\n“{STR_VAR_1}”盒子。");
const u8 gText_PkmnTransferredLanettesPC[] = _("{STR_VAR_2}被传送到了\n真由美的电脑。\p它被放进了\n“{STR_VAR_1}”盒子。");
const u8 gText_PkmnTransferredSomeonesPCBoxFull[] = _("某人电脑里的\n“{STR_VAR_3}”盒子已满。\p{STR_VAR_2}被传送到了\n“{STR_VAR_1}”盒子。");
const u8 gText_PkmnTransferredLanettesPCBoxFull[] = _("真由美电脑里的\n“{STR_VAR_3}”盒子已满。\p{STR_VAR_2}被传送到了\n“{STR_VAR_1}”盒子。");
static const u8 sText_Trainer1WinText[] = _("{B_TRAINER1_WIN_TEXT}");
static const u8 sText_Trainer2WinText[] = _("{B_TRAINER2_WIN_TEXT}");
static const u8 sText_EnduredViaSturdy[] = _("{B_DEF_NAME_WITH_PREFIX}靠特性\n撑住了攻击！");
static const u8 sText_PowerHerbActivation[] = _("{B_ATK_NAME_WITH_PREFIX}因{B_LAST_ITEM}\n充满了力量！");
static const u8 sText_HurtByItem[] = _("{B_ATK_NAME_WITH_PREFIX}\n受到了{B_LAST_ITEM}的伤害！");
static const u8 sText_BadlyPoisonedByItem[] = _("{B_EFF_NAME_WITH_PREFIX}因{B_LAST_ITEM}\n中了剧毒！");
static const u8 sText_BurnedByItem[] = _("{B_EFF_NAME_WITH_PREFIX}因{B_LAST_ITEM}\n灼伤了！");
static const u8 sText_TargetAbilityActivates[] = _("{B_DEF_NAME_WITH_PREFIX}的{B_DEF_ABILITY}\n发动了！");
static const u8 sText_GravityIntensified[] = _("重力增强了！");
static const u8 sText_TargetIdentified[] = _("{B_DEF_NAME_WITH_PREFIX}\n被识破了！");
static const u8 sText_TargetWokeUp[] = _("{B_DEF_NAME_WITH_PREFIX}醒来了！");
static const u8 sText_PkmnStoleAndAteItem[] = _("{B_ATK_NAME_WITH_PREFIX}偷吃了\n{B_DEF_NAME_WITH_PREFIX}的{B_LAST_ITEM}！");
static const u8 sText_TailWindBlew[] = _("{B_ATK_TEAM2}队伍后方\n刮起了顺风！");
static const u8 sText_PkmnWentBack[] = _("{B_ATK_NAME_WITH_PREFIX}回到了\n{B_ATK_TRAINER_CLASS} {B_ATK_TRAINER_NAME}身边");
static const u8 sText_PkmnCantUseItemsAnymore[] = _("{B_DEF_NAME_WITH_PREFIX}\n无法再使用道具了！");
static const u8 sText_PkmnFlung[] = _("{B_ATK_NAME_WITH_PREFIX}投掷了\n{B_LAST_ITEM}！");
static const u8 sText_PkmnPreventedFromHealing[] = _("{B_DEF_NAME_WITH_PREFIX}\n被阻止回复了！");
static const u8 sText_PkmnSwitchedAtkAndDef[] = _("{B_ATK_NAME_WITH_PREFIX}\n交换了攻击和防御！");
static const u8 sText_PkmnsAbilitySuppressed[] = _("{B_DEF_NAME_WITH_PREFIX}的特性\n被压制了！");
static const u8 sText_ShieldedFromCriticalHits[] = _("{B_CURRENT_MOVE}保护了{B_ATK_TEAM2}\n队伍免受要害！");
static const u8 sText_SwitchedAtkAndSpAtk[] = _("{B_ATK_NAME_WITH_PREFIX}和对手交换了\n攻击的变化！\p还有特攻的变化！");
static const u8 sText_SwitchedDefAndSpDef[] = _("{B_ATK_NAME_WITH_PREFIX}和对手交换了\n防御的变化！\p还有特防的变化！");
static const u8 sText_PkmnAcquiredAbility[] = _("{B_DEF_NAME_WITH_PREFIX}获得了\n{B_DEF_ABILITY}！");
static const u8 sText_PoisonSpikesScattered[] = _("毒菱散布在了对手全队的脚下！");
static const u8 sText_PkmnSwitchedStatChanges[] = _("{B_ATK_NAME_WITH_PREFIX}\n和对手交换了能力变化！");
static const u8 sText_PkmnSurroundedWithVeilOfWater[] = _("{B_ATK_NAME_WITH_PREFIX}\n被水幕包裹了！");
static const u8 sText_PkmnLevitatedOnElectromagnetism[] = _("{B_ATK_NAME_WITH_PREFIX}\n靠电磁力飘浮起来！");
static const u8 sText_PkmnTwistedDimensions[] = _("{B_ATK_NAME_WITH_PREFIX}\n扭曲了次元！");
static const u8 sText_PointedStonesFloat[] = _("{B_DEF_TEAM2}队伍周围\n漂浮着尖锐的岩石！");
static const u8 sText_CloakedInMysticalMoonlight[] = _("它被神秘的月光笼罩了！");
static const u8 sText_TrappedBySwirlingMagma[] = _("{B_DEF_NAME_WITH_PREFIX}\n被翻涌的岩浆困住了！");
static const u8 sText_VanishedInstantly[] = _("{B_ATK_NAME_WITH_PREFIX}\n瞬间消失了！");
static const u8 sText_ProtectedTeam[] = _("{B_CURRENT_MOVE}保护了\n{B_ATK_TEAM2}队伍！");
static const u8 sText_SharedItsGuard[] = _("{B_ATK_NAME_WITH_PREFIX}\n和对手共享了防御！");
static const u8 sText_SharedItsPower[] = _("{B_ATK_NAME_WITH_PREFIX}\n和对手共享了力量！");
static const u8 sText_SwapsDefAndSpDefOfAllPkmn[] = _("创造了一个奇异的空间，\n防御与特防互换！");
static const u8 sText_BecameNimble[] = _("{B_ATK_NAME_WITH_PREFIX}变得轻盈了！");
static const u8 sText_HurledIntoTheAir[] = _("{B_DEF_NAME_WITH_PREFIX}\n被抛到了空中！");
static const u8 sText_HeldItemsLoseEffects[] = _("制造出奇异的领域，\n使宝可梦的道具失效！");
static const u8 sText_FellStraightDown[] = _("{B_DEF_NAME_WITH_PREFIX}\n直直地摔了下来！");
static const u8 sText_TargetChangedType[] = _("{B_DEF_NAME_WITH_PREFIX}变成了\n{B_BUFF1}属性！");
static const u8 sText_PkmnAcquiredSimple[] = _("{B_DEF_NAME_WITH_PREFIX}\n获得了单纯！");
static const u8 sText_KindOffer[] = _("{B_DEF_NAME_WITH_PREFIX}\n接受了善意！");
static const u8 sText_ResetsTargetsStatLevels[] = _("{B_DEF_NAME_WITH_PREFIX}的能力变化\n被消除了！");
static const u8 sText_AllySwitchPosition[] = _("{B_ATK_NAME_WITH_PREFIX}和\n{B_SCR_ACTIVE_NAME_WITH_PREFIX}交换了位置！");
static const u8 sText_RestoreTargetsHealth[] = _("{B_DEF_NAME_WITH_PREFIX}的HP恢复了！");
static const u8 sText_TookPkmnIntoTheSky[] = _("{B_ATK_NAME_WITH_PREFIX}把\n{B_DEF_NAME_WITH_PREFIX}带上了天空！");
static const u8 sText_FreedFromSkyDrop[] = _("{B_DEF_NAME_WITH_PREFIX}\n从空中掉落中解脱了！");
static const u8 sText_PostponeTargetMove[] = _("{B_DEF_NAME_WITH_PREFIX}的招式\n被延后了！");
static const u8 sText_ReflectTargetsType[] = _("{B_DEF_NAME_WITH_PREFIX}的属性\n变得和{B_ATK_NAME_WITH_PREFIX}一样了！");
static const u8 sText_TransferHeldItem[] = _("{B_DEF_NAME_WITH_PREFIX}从\n{B_ATK_NAME_WITH_PREFIX}那里得到了{B_LAST_ITEM}");
static const u8 sText_EmbargoEnds[] = _("{B_ATK_NAME_WITH_PREFIX}\n可以再次使用道具了！");
static const u8 sText_Electromagnetism[] = _("电磁力");
static const u8 sText_BufferEnds[] = _("{B_ATK_NAME_WITH_PREFIX}的{B_BUFF1}\n效果消失了！");
static const u8 sText_TelekinesisEnds[] = _("{B_ATK_NAME_WITH_PREFIX}\n从念力中解脱了！");
static const u8 sText_TailwindEnds[] = _("{B_ATK_TEAM1}队伍的顺风\n停止了！");
static const u8 sText_LuckyChantEnds[] = _("{B_ATK_TEAM1}队伍的\n幸运咒语效果消失了！");
static const u8 sText_TrickRoomEnds[] = _("扭曲的空间恢复了正常！");
static const u8 sText_WonderRoomEnds[] = _("奇迹空间消失了，\n防御和特防恢复了正常！");
static const u8 sText_MagicRoomEnds[] = _("魔法空间消失了，\n携带道具的效果恢复了正常！");
static const u8 sText_MudSportEnds[] = _("玩泥巴的效果消失了。");
static const u8 sText_WaterSportEnds[] = _("玩水的效果消失了。");
static const u8 sText_GravityEnds[] = _("重力恢复正常了！");
static const u8 sText_AquaRingHeal[] = _("水流环恢复了\n{B_ATK_NAME_WITH_PREFIX}的HP！");
static const u8 sText_AuroraVeilEnds[] = _("{B_DEF_NAME_WITH_PREFIX}的极光\n消失了！");
static const u8 sText_ElectricTerrainEnds[] = _("电流从战场上消失了。");
static const u8 sText_MistyTerrainEnds[] = _("薄雾从战场\n上消失了。");
static const u8 sText_PsychicTerrainEnds[] = _("怪异从战场上消失了。");
static const u8 sText_GrassyTerrainEnds[] = _("青草从战场\n上消失了。");
static const u8 sText_TargetAbilityRaisedStat[] = _("{B_DEF_NAME_WITH_PREFIX}的特性\n提高了{B_STAT_CHANGER}！");
static const u8 sText_TargetsStatWasMaxedOut[] = _("{B_DEF_NAME_WITH_PREFIX}的特性\n使{B_STAT_CHANGER}达到最大！");
static const u8 sText_AttackerAbilityRaisedStat[] = _("{B_ATK_NAME_WITH_PREFIX}的特性\n提高了{B_STAT_CHANGER}！");
static const u8 sText_PoisonHealHpUp[] = _("中毒让{B_ATK_NAME_WITH_PREFIX}\n稍微恢复了一点！");
static const u8 sText_BadDreamsDmg[] = _("{B_DEF_NAME_WITH_PREFIX}\n受到{B_BUFF1}的折磨！");
static const u8 sText_MoldBreakerEnters[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}打破了常规！");
static const u8 sText_TeravoltEnters[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}\n散发着爆裂的气场！");
static const u8 sText_TurboblazeEnters[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}\n散发着炽热的气场！");
static const u8 sText_SlowStartEnters[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}提不起劲！");
static const u8 sText_SlowStartEnd[] = _("{B_ATK_NAME_WITH_PREFIX}终于\n振作起来了！");
static const u8 sText_SolarPowerHpDrop[] = _("{B_ATK_NAME_WITH_PREFIX}的特性\n带来了代价！");
static const u8 sText_AftermathDmg[] = _("{B_ATK_NAME_WITH_PREFIX}受到了伤害！");
static const u8 sText_AnticipationActivates[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}\n因期待而颤抖！");
static const u8 sText_ForewarnActivates[] = _("特性让{B_SCR_ACTIVE_NAME_WITH_PREFIX}\n察觉到了{B_DEF_NAME_WITH_PREFIX}的{B_BUFF1}！");
static const u8 sText_IceBodyHpGain[] = _("{B_ATK_NAME_WITH_PREFIX}的冰冻之躯\n稍微回复了体力！");
static const u8 sText_SnowWarningHail[] = _("开始下冰雹了！");
static const u8 sText_FriskActivates[] = _("{B_ATK_NAME_WITH_PREFIX}搜查了{B_DEF_NAME_WITH_PREFIX}，\n发现了{B_LAST_ITEM}！");
static const u8 sText_UnnerveEnters[] = _("对手队伍太紧张了\n无法使用道具！");
static const u8 sText_HarvestBerry[] = _("{B_ATK_NAME_WITH_PREFIX}\n收获了{B_LAST_ITEM}！");
static const u8 sText_LastAbilityRaisedBuff1[] = _("{B_ATK_NAME_WITH_PREFIX}的特性\n提高了{B_STAT_CHANGER}！");
static const u8 sText_MagicBounceActivates[] = _("{B_DEF_NAME_WITH_PREFIX}\n弹回了{B_ATK_NAME_WITH_PREFIX}！");
static const u8 sText_ProteanTypeChange[] = _("{B_ATK_NAME_WITH_PREFIX}的{B_ATK_ABILITY}\n使它变成了{B_BUFF1}属性！");
static const u8 sText_SymbiosisItemPass[] = _("{B_ATK_NAME_WITH_PREFIX}通过{B_ATK_ABILITY}\n把{B_LAST_ITEM}传给了{B_SCR_ACTIVE_NAME_WITH_PREFIX}！");
static const u8 sText_StealthRockDmg[] = _("尖锐的岩石刺入了\n{B_STACK_1_NAME_WITH_PREFIX}！");
static const u8 sText_ToxicSpikesAbsorbed[] = _("{B_ATK_TEAM2}队伍脚下的\n毒菱消失了！");
static const u8 sText_ToxicSpikesPoisoned[] = _("{B_STACK_1_NAME_WITH_PREFIX}中毒了！");
static const u8 sText_StickyWebSwitchIn[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}\n被黏黏网缠住了！");
static const u8 sText_HealingWishCameTrue[] = _("治愈之愿在\n{B_ATK_NAME_WITH_PREFIX}身上实现了！");
static const u8 sText_HealingWishHealed[] = _("{B_ATK_NAME_WITH_PREFIX}恢复了体力！");
static const u8 sText_LunarDanceCameTrue[] = _("{B_ATK_NAME_WITH_PREFIX}\n被神秘的月光笼罩了！");
static const u8 sText_CursedBodyDisabled[] = _("{B_ATK_NAME_WITH_PREFIX}的{B_BUFF1}\n被{B_DEF_NAME_WITH_PREFIX}的诅咒之躯封印了！");
static const u8 sText_AttackerAquiredAbility[] = _("{B_ATK_NAME_WITH_PREFIX}获得了{B_LAST_ABILITY}！");
static const u8 sText_TargetAbilityLoweredStat[] = _("{B_DEF_NAME_WITH_PREFIX}的特性\n降低了{B_STAT_CHANGER}！");
static const u8 sText_TargetStatWontGoHigher[] = _("{B_DEF_NAME_WITH_PREFIX}的{B_STAT_CHANGER}\n无法再提高了！");
static const u8 sText_PkmnMoveBouncedViaAbility[] = _("{B_ATK_NAME_WITH_PREFIX}的{B_CURRENT_MOVE}\n被{B_DEF_NAME_WITH_PREFIX}的特性\l弹回了！");
static const u8 sText_ImposterTransform[] = _("{B_ATK_NAME_WITH_PREFIX}用{B_LAST_ABILITY}\n变成了{B_DEF_NAME_WITH_PREFIX}！");
static const u8 sText_AssaultVestDoesntAllow[] = _("{B_LAST_ITEM}的效果\n使变化招式无法使用！\p");
static const u8 sText_GravityPreventsUsage[] = _("因重力影响，{B_ATK_NAME_WITH_PREFIX}\n无法使用{B_CURRENT_MOVE}！\p");
static const u8 sText_HealBlockPreventsUsage[] = _("{B_ATK_NAME_WITH_PREFIX}\n被阻止回复了！\p");
static const u8 sText_NotDoneYet[] = _("这个招式的效果\n尚未实装！\p");
static const u8 sText_StickyWebUsed[] = _("{B_DEF_TEAM2}队伍周围的地面\n张开了黏黏网！");
static const u8 sText_QuashSuccess[] = _("时间线已无法再被扭曲！");
static const u8 sText_PkmnBlewAwayToxicSpikes[] = _("{B_ATK_NAME_WITH_PREFIX}\n吹散了毒菱！");
static const u8 sText_PkmnBlewAwayStickyWeb[] = _("{B_ATK_NAME_WITH_PREFIX}\n吹散了黏黏网！");
static const u8 sText_PkmnBlewAwayStealthRock[] = _("{B_ATK_NAME_WITH_PREFIX}\n吹散了隐形岩！");
static const u8 sText_IonDelugeOn[] = _("离子洪流倾泻\n在战场上！");
static const u8 sText_TopsyTurvySwitchedStats[] = _("{B_DEF_NAME_WITH_PREFIX}的能力变化\n全部反转了！");
static const u8 sText_TerrainBecomesMisty[] = _("白雾弥漫在\n战场上！");
static const u8 sText_TerrainBecomesGrassy[] = _("青草覆盖了战场！");
static const u8 sText_TerrainBecomesElectric[] = _("电流在战场上奔流！");
static const u8 sText_TerrainBecomesPsychic[] = _("战场变得诡异了！");
static const u8 sText_TargetElectrified[] = _("{B_DEF_NAME_WITH_PREFIX}的招式\n被电气化了！");
static const u8 sText_MegaEvoReacting[] = _("{B_ATK_NAME_WITH_PREFIX}的{B_LAST_ITEM}\n对{B_ATK_TRAINER_NAME}的Mega环产生了反应！");
static const u8 sText_MegaEvoEvolved[] = _("{B_ATK_NAME_WITH_PREFIX}Mega进化成了\nMega{B_BUFF1}！");
static const u8 sText_drastically[] = _("大幅");
static const u8 sText_severely[] = _("急剧");
static const u8 sText_Infestation[] = _("{B_DEF_NAME_WITH_PREFIX}\n被{B_ATK_NAME_WITH_PREFIX}寄生纠缠了！");
static const u8 sText_NoEffectOnTarget[] = _("对{B_DEF_NAME_WITH_PREFIX}\n没有效果！");
static const u8 sText_BurstingFlames[] = _("爆裂的火焰\n击中了{B_SCR_ACTIVE_NAME_WITH_PREFIX}！");
static const u8 sText_BestowItemGiving[] = _("{B_DEF_NAME_WITH_PREFIX}从\n{B_ATK_NAME_WITH_PREFIX}那里得到了{B_LAST_ITEM}！");
static const u8 sText_ThirdTypeAdded[] = _("{B_DEF_NAME_WITH_PREFIX}\n追加了{B_BUFF1}属性！");
static const u8 sText_FellForFeint[] = _("{B_DEF_NAME_WITH_PREFIX}\n中了佯攻！");
static const u8 sText_PokemonCannotUseMove[] = _("{B_ATK_NAME_WITH_PREFIX}\n无法使用{B_CURRENT_MOVE}！");
static const u8 sText_CoveredInPowder[] = _("{B_DEF_NAME_WITH_PREFIX}\n浑身沾满了粉末！");
static const u8 sText_PowderExplodes[] = _("火焰碰到宝可梦身上的\n粉末时发生了爆炸！");
static const u8 sText_BelchCantUse[] = _("无法使用打嗝！\p");
static const u8 sText_SpectralThiefSteal[] = _("{B_ATK_NAME_WITH_PREFIX}\n夺走了对手提升的能力！");
static const u8 sText_GravityGrounding[] = _("因重力影响，{B_DEF_NAME_WITH_PREFIX}\n无法停留在空中！");
static const u8 sText_MistyTerrainPreventsStatus[] = _("{B_DEF_NAME_WITH_PREFIX}\n用保护之雾包裹了自己！");
static const u8 sText_GrassyTerrainHeals[] = _("{B_ATK_NAME_WITH_PREFIX}\n被青草场地治愈了！");
static const u8 sText_ElectricTerrainPreventsSleep[] = _("{B_DEF_NAME_WITH_PREFIX}\n被电气场地包围了！");
static const u8 sText_PsychicTerrainPreventsPriority[] = _("{B_DEF_NAME_WITH_PREFIX}\n被精神场地包围了！");
static const u8 sText_SafetyGogglesProtected[] = _("多亏了{B_LAST_ITEM}，\n{B_DEF_NAME_WITH_PREFIX}没有受到影响！");
static const u8 sText_FlowerVeilProtected[] = _("{B_DEF_NAME_WITH_PREFIX}\n被花瓣之幕包裹了！");
static const u8 sText_SweetVeilProtected[] = _("{B_DEF_NAME_WITH_PREFIX}\n被甜蜜之幕包裹了！");
static const u8 sText_AromaVeilProtected[] = _("{B_DEF_NAME_WITH_PREFIX}\n受到了芳香之幕的保护！");
static const u8 sText_CelebrateMessage[] = _("恭喜你，{B_PLAYER_NAME}！");
static const u8 sText_UsedInstructedMove[] = _("{B_ATK_NAME_WITH_PREFIX}按照\n{B_BUFF1}的指示使用了招式！");
static const u8 sText_ThroatChopEnds[] = _("{B_ATK_NAME_WITH_PREFIX}\n可以再次使用声音招式了！");
static const u8 sText_PkmnCantUseMoveThroatChop[] = _("{B_ACTIVE_NAME_WITH_PREFIX}因地狱突刺\n无法使用{B_CURRENT_MOVE}！\p");
static const u8 sText_LaserFocusMessage[] = _("{B_ATK_NAME_WITH_PREFIX}\n高度集中了！");
static const u8 sText_GemActivates[] = _("{B_LAST_ITEM}增强了\n{B_ATK_NAME_WITH_PREFIX}的力量！");
static const u8 sText_BerryDmgReducing[] = _("{B_LAST_ITEM}减轻了\n对{B_DEF_NAME_WITH_PREFIX}的伤害！");
static const u8 sText_TargetAteItem[] = _("{B_DEF_NAME_WITH_PREFIX}吃掉了{B_LAST_ITEM}！");
static const u8 sText_AirBalloonFloat[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}靠{B_LAST_ITEM}\n飘浮在空中！");
static const u8 sText_AirBalloonPop[] = _("{B_DEF_NAME_WITH_PREFIX}的{B_LAST_ITEM}\n破裂了！");
static const u8 sText_IncinerateBurn[] = _("{B_EFF_NAME_WITH_PREFIX}的{B_LAST_ITEM}\n被烧掉了！");
static const u8 sText_BugBite[] = _("{B_ATK_NAME_WITH_PREFIX}偷吃了\n{B_EFF_NAME_WITH_PREFIX}的{B_LAST_ITEM}！");
static const u8 sText_IllusionWoreOff[] = _("{B_DEF_NAME_WITH_PREFIX}的幻象解除了！");
static const u8 sText_AttackerCuredTargetStatus[] = _("{B_ATK_NAME_WITH_PREFIX}治愈了\n{B_DEF_NAME_WITH_PREFIX}的异常状态！");
static const u8 sText_AttackerLostFireType[] = _("{B_ATK_NAME_WITH_PREFIX}燃烧殆尽了！");
static const u8 sText_HealerCure[] = _("{B_ATK_NAME_WITH_PREFIX}的治愈之心\n治愈了{B_SCR_ACTIVE_NAME_WITH_PREFIX}！");
static const u8 sText_ScriptingAbilityRaisedStat[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的特性\n提高了{B_STAT_CHANGER}！");
static const u8 sText_ReceiverAbilityTakeOver[] = _("{B_STACK_1_NAME_WITH_PREFIX}的\n{B_LAST_ABILITY}被夺取了！");
static const u8 sText_PkmnAbsorbingPower[] = _("{B_ATK_NAME_WITH_PREFIX}\n正在吸取力量！");
static const u8 sText_NoOneWillBeAbleToRun[] = _("下回合谁都无法逃走！");
static const u8 sText_DestinyKnotActivates[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}因\n{B_LAST_ITEM}而着迷了！");
static const u8 sText_CloakedInAFreezingLight[] = _("{B_ATK_NAME_WITH_PREFIX}\n被冰冻的光包裹了！");
static const u8 sText_StatWasNotLowered[] = _("{B_DEF_NAME_WITH_PREFIX}的能力\n没有被降低！");
static const u8 sText_FerventWishReached[] = _("{B_ATK_TRAINER_NAME}的热切愿望\n传达到了{B_ATK_NAME_WITH_PREFIX}！");
static const u8 sText_AirLockActivates[] = _("天气的效果消失了。");
static const u8 sText_PressureActivates[] = _("{B_ATK_NAME_WITH_PREFIX}\n施加着压力！");
static const u8 sText_DarkAuraActivates[] = _("{B_ATK_NAME_WITH_PREFIX}\n散发着暗黑气场！");
static const u8 sText_FairyAuraActivates[] = _("{B_ATK_NAME_WITH_PREFIX}\n散发着妖精气场！");
static const u8 sText_AuraBreakActivates[] = _("{B_ATK_NAME_WITH_PREFIX}\n反转了其他宝可梦的气场！");
static const u8 sText_ComatoseActivates[] = _("{B_ATK_NAME_WITH_PREFIX}昏昏欲睡！");
static const u8 sText_ScreenCleanerActivates[] = _("场上的所有屏障\n都被清除了！");
static const u8 sText_FetchedPokeBall[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}\n发现了{B_LAST_ITEM}！");
static const u8 sText_BattlerAbilityRaisedStat[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的特性\n提高了{B_STAT_CHANGER}！");
static const u8 sText_ASandstormKickedUp[] = _("沙暴刮起来了！");
static const u8 sText_PkmnsWillPerishIn3Turns[] = _("双方宝可梦都将在\n三回合后倒下！");
static const u8 sText_AbilityRaisedStat[] = _("{B_DEF_NAME_WITH_PREFIX}的特性\n提高了{B_STAT_CHANGER}！");
static const u8 sText_AuraFlaredToLife[] = _("{B_DEF_NAME_WITH_PREFIX}的气场燃起了！");
static const u8 sText_AsOneEnters[] = _("{B_ATK_NAME_WITH_PREFIX}\n拥有两个特性！");
static const u8 sText_CuriousMedicineEnters[] = _("{B_EFF_NAME_WITH_PREFIX}的\n能力变化被重置了！");
static const u8 sText_CanActFaster[] = _("{B_ATK_NAME_WITH_PREFIX}靠特性\n可以更快行动！");
static const u8 sText_MicleBerryActivates[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}用{B_LAST_ITEM}\n提高了下一招的命中率！");
static const u8 sText_PkmnShookOffTheTaunt[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}\n摆脱了挑衅！");
static const u8 sText_PkmnGotOverItsInfatuation[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}\n从着迷中清醒了！");
static const u8 sText_ItemCannotBeRemoved[] = _("{B_ATK_NAME_WITH_PREFIX}的道具\n无法被拿走！");
static const u8 sText_StickyBarbTransfer[] = _("{B_LAST_ITEM}\n附着到了{B_ATK_NAME_WITH_PREFIX}身上！");
static const u8 sText_PkmnBurnHealed[] = _("{B_DEF_NAME_WITH_PREFIX}的\n灼伤被治愈了。");
static const u8 sText_RedCardActivate[] = _("{B_ATK_NAME_WITH_PREFIX}用{B_LAST_ITEM}\n抵挡了{B_DEF_NAME_WITH_PREFIX}！");
static const u8 sText_EjectButtonActivate[] = _("{B_DEF_NAME_WITH_PREFIX}\n因{B_LAST_ITEM}被换下了！");
static const u8 sText_AttackerGotOverInfatuation[] = _("{B_ATK_NAME_WITH_PREFIX}\n从着迷中清醒了！");
static const u8 sText_TormentedNoMore[] = _("{B_ATK_NAME_WITH_PREFIX}\n不再受无理取闹影响！");
static const u8 sText_HealBlockedNoMore[] = _("{B_ATK_NAME_WITH_PREFIX}\n的回复封锁解除了！");
static const u8 sText_AttackerBecameFullyCharged[] = _("{B_STACK_1_NAME_WITH_PREFIX}因与训练家的\n羁绊而充满了力量！");
static const u8 sText_AttackerBecameAshSpecies[] = _("{B_ATK_NAME_WITH_PREFIX}变成了\n{STR_VAR_1}！\p");
static const u8 sText_ExtremelyHarshSunlight[] = _("阳光变得极\n其强烈！");
static const u8 sText_ExtremeSunlightFaded[] = _("强烈的阳光减弱了。{PAUSE 64}");
static const u8 sText_MoveEvaporatedInTheHarshSunlight[] = _("水属性攻击在强烈的\n阳光下蒸发了！");
static const u8 sText_ExtremelyHarshSunlightWasNotLessened[] = _("极其强烈的阳光\n丝毫没有减弱！");
static const u8 sText_HeavyRain[] = _("开始下起了大雨！");
static const u8 sText_HeavyRainLifted[] = _("大雨停止了！{PAUSE 64}");
static const u8 sText_MoveFizzledOutInTheHeavyRain[] = _("火属性攻击在大雨中熄灭了！");
static const u8 sText_NoReliefFromHeavyRain[] = _("这场大雨没有\n停歇的迹象！");
static const u8 sText_MysteriousAirCurrent[] = _("神秘的气流\n正在保护飞行属性宝可梦！");
static const u8 sText_StrongWindsDissipated[] = _("神秘强风消散了！{PAUSE 64}");
static const u8 sText_MysteriousAirCurrentBlowsOn[] = _("神秘气流依旧吹拂不止！");
static const u8 sText_AttackWeakenedByStrongWinds[] = _("神秘强风削弱了攻击！");
static const u8 sText_StuffCheeksCantSelect[] = _("没有树果时无法\n选择大快朵颐！\p");
static const u8 sText_PkmnRevertedToPrimal[] = _("{B_ATK_NAME_WITH_PREFIX}原始回归了！\n恢复了原始形态！");
static const u8 sText_ButPokemonCantUseTheMove[] = _("但是{B_ATK_NAME_WITH_PREFIX}\n无法使用这个招式！");
static const u8 sText_ButHoopaCantUseIt[] = _("但胡帕现在这样\n无法使用它！");
static const u8 sText_BrokeThroughProtection[] = _("它突破了\n{B_DEF_NAME_WITH_PREFIX}的保护！");
static const u8 sText_AbilityAllowsOnlyMove[] = _("{B_ATK_NAME_WITH_PREFIX}\n只能使用{B_CURRENT_MOVE}！\p");
static const u8 sText_SwappedAbilities[] = _("{B_DEF_NAME_WITH_PREFIX}\n和对手交换了特性！");
static const u8 sText_PastelVeilProtected[] = _("{B_DEF_NAME_WITH_PREFIX}\n受到了粉彩色之幕的保护！");
static const u8 sText_PastelVeilEnters[] = _("{B_DEF_NAME_WITH_PREFIX}\n的中毒被治愈了！");
static const u8 sText_BattlerTypeChangedTo[] = _("{B_STACK_1_NAME_WITH_PREFIX}的属性\n变成了{B_BUFF2}！");
static const u8 sText_BothCanNoLongerEscape[] = _("双方宝可梦都无法逃走！");
static const u8 sText_CantEscapeDueToUsedMove[] = _("{B_ATK_NAME_WITH_PREFIX}因使用\n{B_CURRENT_MOVE}而无法逃走！");
static const u8 sText_PkmnBecameWeakerToFire[] = _("{B_DEF_NAME_WITH_PREFIX}\n变得怕火了！");
static const u8 sText_PkmnAboutToBeAttackedByItsItem[] = _("{B_DEF_NAME_WITH_PREFIX}\n即将受到{B_LAST_ITEM}的攻击！");
static const u8 sText_CantEscapeBecauseOfCurrentMove[] = _("{B_DEF_NAME_WITH_PREFIX}因\n{B_CURRENT_MOVE}而无法逃走！");
static const u8 sText_NeutralizingGasEnters[] = _("化学变化气体充满了四周！");
static const u8 sText_NeutralizingGasOver[] = _("化学变化气体的效果消失了！");
static const u8 sText_PlayerLostAgainstEnemyTrainer[] = _("{B_PLAYER_NAME}已经没有\n能战斗的宝可梦了！\p{B_PLAYER_NAME}输给了\n{B_TRAINER1_CLASS} {B_TRAINER1_NAME}！{PAUSE_UNTIL_PRESS}");
static const u8 sText_PlayerPaidPrizeMoney[] = _("{B_PLAYER_NAME}支付了¥{B_BUFF1}\n作为奖金…\p… … … …\p{B_PLAYER_NAME}眼前发黑！{PAUSE_UNTIL_PRESS}");
static const u8 sText_PkmnRaisedAttack[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的特性\n提高了攻击！");
static const u8 sText_InflatablePkmnRaisedDefenses[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的膨胀\n提高了防御！");
static const u8 sText_AngerPointsPkmnRaisedAttack[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的怒火\n提高了攻击！");
static const u8 sText_BattlerInnateRaisedStat[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的特性\n提高了{B_STAT_CHANGER}！");
static const u8 sText_BattlerAddedTheType[] = _("{B_ACTIVE_NAME_WITH_PREFIX}\n追加了{B_BUFF2}属性！");
static const u8 sText_BattlerCoiledUp[] = _("{B_ACTIVE_NAME_WITH_PREFIX}盘起了身体，\n准备咬击！");
static const u8 sText_AttackerTypeChangedTo[] = _("{B_ACTIVE_NAME_WITH_PREFIX}的属性\n变成了{B_BUFF1}！");
static const u8 sText_AbilityRestoredHPALittle[] = _("{B_ACTIVE_NAME_WITH_PREFIX}的特性\n少量恢复了HP！");
static const u8 sText_BattlerEnvelopedItselfInAVeil[] = _("{B_ATK_NAME_WITH_PREFIX}\n被水之幕包裹了。");
static const u8 sText_BattlerAbilitySetUpTailwind[] = _("{B_ACTIVE_NAME_WITH_PREFIX}的送风\n刮起了顺风！");
static const u8 sText_TargetPkmnHurtsWith[] = _("{B_DEF_NAME_WITH_PREFIX}受到了\n{B_ATK_NAME_WITH_PREFIX}的{B_BUFF1}伤害！");
static const u8 sText_PkmnCutsSpecialAttackWith[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的特性\n削弱了{B_DEF_NAME_WITH_PREFIX}的特攻！");
static const u8 sText_AttackerRegainedHealth[] = _("{B_ATK_NAME_WITH_PREFIX}\n恢复了体力！");
static const u8 sText_SweetDreamsHealHpUp[] = _("睡眠让{B_ATK_NAME_WITH_PREFIX}\n稍微恢复了一点！");
static const u8 sText_PkmnBecameCursed[] = _("{B_ATK_NAME_WITH_PREFIX}被诅咒了！");
static const u8 sText_BattlerAbilitySetUpStickyWeb[] = _("{B_ACTIVE_NAME_WITH_PREFIX}的蜘蛛巢穴\n在对面张开了黏黏网！");
static const u8 sText_BattlerAbilitySetUpTrickRoom[] = _("{B_ACTIVE_NAME_WITH_PREFIX}的扭曲次元\n展开了戏法空间！");
static const u8 sText_BattlerAbilitySetUpAuroraVeil[] = _("{B_ACTIVE_NAME_WITH_PREFIX}的北风\n展开了极光幕！");
static const u8 sText_BattlerGripPincerActivated[] = _("{B_DEF_NAME_WITH_PREFIX}被\n{B_ACTIVE_NAME_WITH_PREFIX}的{B_LAST_ABILITY}困住了！");
static const u8 sText_PkmnDidAmmountDamage[] = _("{B_ACTIVE_NAME_WITH_PREFIX}\n造成了{B_BUFF4}点伤害！");
static const u8 sText_HeatingUpBeak[] = _("{B_ATK_NAME_WITH_PREFIX}\n开始加热鸟嘴！");
static const u8 sText_BattlerAbilityLoweredEnemyStat[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的特性\n降低了对手的{B_STAT_CHANGER}！");
static const u8 sText_CanActFasterItem[] = _("{B_ATK_NAME_WITH_PREFIX}靠{B_BUFF1}\n可以更快行动！");
static const u8 sText_PkmnHurtByFrostbite[] = _("{B_ATK_NAME_WITH_PREFIX}\n受到了冻伤的伤害！");
static const u8 sText_PkmnGotFrostbite[] = _("{B_EFF_NAME_WITH_PREFIX}冻伤了！");
static const u8 sText_PkmnsItemHealedFrostbite[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的{B_LAST_ITEM}\n治愈了冻伤！");
static const u8 sText_AttackerHealedItsFrostbite[] = _("{B_ATK_NAME_WITH_PREFIX}靠顽强的意志\n治愈了冻伤！");
static const u8 sText_PkmnFrostbiteHealed[] = _("{B_DEF_NAME_WITH_PREFIX}的\n冻伤被治愈了！");
static const u8 sText_PkmnFrostbiteHealed2[] = _("{B_ATK_NAME_WITH_PREFIX}的\n冻伤被治愈了！");
static const u8 sText_PkmnFrostbiteHealedBy[] = _("{B_ATK_NAME_WITH_PREFIX}的{B_CURRENT_MOVE}\n治愈了冻伤！");
static const u8 sText_RegeneratorExits[] = _("再生力让{B_ATK_NAME_WITH_PREFIX}\n回复了一些体力！");
static const u8 sText_NaturalCureExits[] = _("{B_LAST_ABILITY}治愈了\n{B_STACK_1_NAME_WITH_PREFIX}的状态！");
static const u8 sText_AbilityLetItUseMove[] = _("{B_ATK_NAME_WITH_PREFIX}的特性\n让它使出了{B_CURRENT_MOVE}！");
static const u8 sText_LethargyEnters[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}\n能量充足！");
static const u8 sText_LethargyEnd[] = _("{B_ATK_NAME_WITH_PREFIX}\n能量耗尽了！");
static const u8 sText_PickupActivated[] = _("{B_ATK_NAME_WITH_PREFIX}清除了\n我方场上的场地效果！");
static const u8 sText_PkmnRaisedDefense[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的特性\n提高了防御！");
static const u8 sText_PkmnRaisedSpecialAttack[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的特性\n提高了特攻！");
static const u8 sText_PkmnRaisedSpecialDefense[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的特性\n提高了特防！");
static const u8 sText_TargetyBecameCursed[] = _("{B_DEF_NAME_WITH_PREFIX}被诅咒了！");
static const u8 sText_BattlerHasNoDamageHits[] = _("{B_ATK_NAME_WITH_PREFIX}可以无伤\n承受{B_BUFF4}次攻击！");
static const u8 sText_BattlerCanNoLongerEndureHits[] = _("{B_DEF_NAME_WITH_PREFIX}\n无法再承受攻击了！{PAUSE 32}");
static const u8 sText_BattlerCanStillEndureHits[] = _("{B_ATK_NAME_WITH_PREFIX}仍可无伤\n承受{B_BUFF4}次攻击！{PAUSE 32}");
static const u8 sText_BattlerHasASingleNoDamageHit[] = _("{B_DEF_NAME_WITH_PREFIX}可以无伤\n承受一次攻击！");
static const u8 sText_BattlerCanStillEndureASingleHit[] = _("{B_DEF_NAME_WITH_PREFIX}仍可无伤\n承受一次攻击！{PAUSE 32}");
static const u8 sText_BattlerHasSpilledToxicWaste[] = _("{B_ATK_NAME_WITH_PREFIX}在场上\n撒下了剧毒废料！");
static const u8 sText_TheToxicWasteHasDissappeared[] = _("剧毒废料消失了！");
static const u8 sText_PkmnHurtByToxicWaste[] = _("{B_ATK_NAME_WITH_PREFIX}\n受到了剧毒废料的伤害！");
static const u8 sText_ThePokemonBlockedTheBall[] = _("{B_OPPONENT_MON1_NAME}挡住了球！");
static const u8 sText_AttackerIsReadyToTestYou[] = _("宝可梦{B_OPPONENT_MON1_NAME}\n准备好考验你了！");
static const u8 sText_AttackerStatsWereBoosted[] = _("{B_OPPONENT_MON1_NAME}的\n部分能力提高了！");
static const u8 sText_WildMonMegaEvolved[] = _("神秘的力量包围着\n宝可梦{B_OPPONENT_MON1_NAME}！");
static const u8 sText_TheOpposingAttackerRoarsWildly[] = _("对手的{B_OPPONENT_MON1_NAME}\n疯狂咆哮！");
static const u8 sText_AttackerAttackRose[] = _("{B_ATK_NAME_WITH_PREFIX}的攻击\n提高了！");
static const u8 sText_AttackerDefenseRose[] = _("{B_ATK_NAME_WITH_PREFIX}的防御\n提高了！");
static const u8 sText_AttackerSpecialAttackRose[] = _("{B_ATK_NAME_WITH_PREFIX}的特攻\n提高了！");
static const u8 sText_AttackerSpecialDefenseRose[] = _("{B_ATK_NAME_WITH_PREFIX}的特防\n提高了！");
static const u8 sText_AttackerSpeedRose[] = _("{B_ATK_NAME_WITH_PREFIX}的速度\n提高了！");
static const u8 sText_AttackerAccuracyRose[] = _("{B_ATK_NAME_WITH_PREFIX}的命中率\n提高了！");
static const u8 sText_AttackerEvasivenessRose[] = _("{B_ATK_NAME_WITH_PREFIX}的闪避率\n提高了！");
static const u8 sText_AttackerAttackSharplyRose[] = _("{B_ATK_NAME_WITH_PREFIX}的攻击\n大幅提高了！");
static const u8 sText_AttackerDefenseSharplyRose[] = _("{B_ATK_NAME_WITH_PREFIX}的防御\n大幅提高了！");
static const u8 sText_AttackerSpecialAttackSharplyRose[] = _("{B_ATK_NAME_WITH_PREFIX}的特攻\n大幅提高了！");
static const u8 sText_AttackerSpecialDefenseSharplyRose[] = _("{B_ATK_NAME_WITH_PREFIX}的特防\n大幅提高了！");
static const u8 sText_AttackerSpeedSharplyRose[] = _("{B_ATK_NAME_WITH_PREFIX}的速度\n大幅提高了！");
static const u8 sText_AttackerAccuracySharplyRose[] = _("{B_ATK_NAME_WITH_PREFIX}的命中率\n大幅提高了！");
static const u8 sText_AttackerEvasivenessSharplyRose[] = _("{B_ATK_NAME_WITH_PREFIX}的闪避率\n大幅提高了！");
static const u8 sText_DesertCloakVeilProtected[] = _("{B_DEF_NAME_WITH_PREFIX}\n受到了沙之幕的保护！");
static const u8 sText_AttackerStatsRose[] = _("{B_ATK_NAME_WITH_PREFIX}的能力提高了！");
static const u8 sText_AngelsWrath_TackleEffect[] = _("{B_DEF_NAME_WITH_PREFIX}被再来一次\n并禁止了招式！");
static const u8 sText_AngelsWrath_StringShotEffect[] = _("{B_DEF_NAME_WITH_PREFIX}的场地\n布满了场地效果！");
static const u8 sText_AngelsWrath_HardenEffect[] = _("{B_ATK_NAME_WITH_PREFIX}的能力提高了！");
static const u8 sText_AngelsWrath_IronDefenseEffect[] = _("{B_ATK_NAME_WITH_PREFIX}保护了自己！");
static const u8 sText_AngelsWrath_ElectrowebEffect[] = _("{B_DEF_NAME_WITH_PREFIX}被困住，\n变得非常缓慢！");
static const u8 sText_AngelsWrath_BugBiteEffect[] = _("{B_DEF_NAME_WITH_PREFIX}的道具被吃掉了！");
static const u8 sText_AngelsWrath_ProtectEffect[] = _("{B_DEF_NAME_WITH_PREFIX}的\n全部能力都降低了！");
static const u8 sText_PkmnsDisciplineLockEnds[] = _("{B_ATK_NAME_WITH_PREFIX}\n不再只能使用同一个招式！");
static const u8 sText_SleepClauseDisablesMove[] = _("睡眠条款使这个招式\n无法被选择！\p");
static const u8 sText_ElectromorphosisActivates[] = _("{B_DEF_NAME_WITH_PREFIX}\n充满了力量！");
static const u8 sText_InverseRoomEnds[] = _("反转的空间恢复了正常！");
static const u8 sText_BattlerAbilitySetUpInverseRoom[] = _("{B_ACTIVE_NAME_WITH_PREFIX}的反转空间\n展开了反转空间！");
static const u8 sText_AttackerGotTheType[] = _("{B_ACTIVE_NAME_WITH_PREFIX}变成了\n{B_BUFF1}属性！");
static const u8 sText_DefenderTypeChangedTo[] = _("{B_DEF_NAME_WITH_PREFIX}的属性\n变成了{B_BUFF1}！");
static const u8 sText_AttackerBecameInfected[] = _("{B_ATK_NAME_WITH_PREFIX}\n被感染了！");
static const u8 sText_BattlerAbilitySetUpSafeGuard[] = _("{B_ACTIVE_NAME_WITH_PREFIX}的粉彩之幕\n展开了神秘守护！");
static const u8 sText_PkmnStartBleed[] = _("{B_EFF_NAME_WITH_PREFIX}\n开始流血了！");
static const u8 sText_PkmnHurtByBleed[] = _("{B_ATK_NAME_WITH_PREFIX}\n因流血而受到伤害！");
static const u8 sText_PkmnBleedHealed[] = _("{B_DEF_NAME_WITH_PREFIX}的\n流血止住了！");
static const u8 sText_PkmnBleedHealed2[] = _("{B_ATK_NAME_WITH_PREFIX}的\n流血止住了！");
static const u8 sText_PkmnBleedHealedBy[] = _("{B_ATK_NAME_WITH_PREFIX}的{B_CURRENT_MOVE}\n止住了流血！");
static const u8 sText_PkmnsItemHealedBleed[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的{B_LAST_ITEM}\n止住了流血！");
static const u8 sText_LastAbilityLoweredBuff1[] = _("{B_STACK_1_NAME_WITH_PREFIX}的特性\n降低了{B_STAT_CHANGER}！");
static const u8 sText_TippingPointsPkmnRaisedSpecialAttack[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的临界点\n提高了特攻！");
static const u8 sText_PkmnInverseDimensions[] = _("{B_ATK_NAME_WITH_PREFIX}\n反转了次元！");
static const u8 sText_CrownedKingEnters[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}\n拥有三个特性！");
static const u8 sText_Permanence[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}\n正在阻止对手队伍回复！");
static const u8 sText_BerserkDNA[] = _("{B_ATK_NAME_WITH_PREFIX}\n暴走了！");
static const u8 sText_HurtByAbility[] = _("{B_ATK_NAME_WITH_PREFIX}\n因特性受到伤害！");
static const u8 sText_RetrieverExits[] = _("{B_STACK_1_NAME_WITH_PREFIX}\n取回了{B_LAST_ITEM}！");
static const u8 sText_BlockOnEntry[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}\n阻止了对手队伍逃走！");
static const u8 sText_SupremeOverlord[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}\n从倒下的同伴那里获得了力量！");
static const u8 sText_ParadoxBoost[] = _("{B_ATK_NAME_WITH_PREFIX}的{B_STAT_CHANGER}\n得到了提升！");
static const u8 sText_ParadoxBoostEnd[] = _("{B_ATK_NAME_WITH_PREFIX}的\n提升效果结束了！");
static const u8 sText_ParadoxBoostSun[] = _("{B_ATK_NAME_WITH_PREFIX}\n对强烈阳光产生了反应！");
static const u8 sText_ParadoxBoostItem[] = _("{B_ATK_NAME_WITH_PREFIX}\n使用了驱动能量！");
static const u8 sText_ParadoxBoostElectricTerrain[] = _("{B_ATK_NAME_WITH_PREFIX}\n对电气场地产生了反应！");
static const u8 sText_Costar[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}\n复制了同伴的能力变化！");
static const u8 sText_OverflowingWithSpacePower[] = _("{B_ATK_NAME_WITH_PREFIX}\n充满了宇宙之力！");
static const u8 sText_PlayerGotBP[] = _("{B_PLAYER_NAME}获胜，\n获得了{B_BUFF1}点对战点数！\p");
static const u8 sText_CantUseTwiceInARow[] = _("{B_ACTIVE_NAME_WITH_PREFIX}无法\n连续使用{B_CURRENT_MOVE}！\p");
static const u8 sText_AttackerLostElectricType[] = _("{B_ATK_NAME_WITH_PREFIX}\n用尽了电力！");
static const u8 sText_HeavySpikesScattered[] = _("尖刺降落在对\n手的场地上！");
static const u8 sText_PkmnAlreadyType[] = _("{B_ATK_NAME_WITH_PREFIX}\n无法改变属性！");
static const u8 sText_PkmnCutHPRaisedStats[] = _("{B_ATK_NAME_WITH_PREFIX}削减HP，\n提高了能力！");
static const u8 sText_PkmnAfflictedBySaltCure[] = _("{B_ATK_NAME_WITH_PREFIX}\n受到了盐腌的伤害！");
static const u8 sText_PkmnSwitchedFieldEffects[] = _("{B_ATK_NAME_WITH_PREFIX}\n交换了双方场地的效果！");
static const u8 sText_PkmnTellChillingReceptionJoke[] = _("{B_ATK_NAME_WITH_PREFIX}\n准备讲一个冷到骨子里的笑话！");
static const u8 sText_ShedItsTail[] = _("{B_ATK_NAME_WITH_PREFIX}断尾\n制造了替身！");
static const u8 sText_GhastlyEcho[] = _("{B_ATK_NAME_WITH_PREFIX}\n被萦绕的回声强化了！");
static const u8 sText_PkmnRevivedReadyToFight[] = _("{B_BUFF1}复活了，\n准备再次战斗！");
static const u8 sText_TheSwampDisappeared[] = _("{B_ATK_TEAM2}队伍周围的\n沼泽消失了！");
static const u8 sText_SwampEnvelopedSide[] = _("沼泽包围了\n{B_DEF_TEAM2}队伍！");
static const u8 sText_TheSeaOfFireDisappeared[] = _("{B_ATK_TEAM2}队伍周围的\n火海消失了！");
static const u8 sText_HurtByTheSeaOfFire[] = _("{B_ATK_TEAM1} {B_ATK_NAME_WITH_PREFIX}\n受到了火海的伤害！");
static const u8 sText_SeaOfFireEnvelopedSide[] = _("火海包围了\n{B_DEF_TEAM2}队伍！");
static const u8 sText_WaitingForPartnersMove[] = _("{B_ATK_NAME_WITH_PREFIX}正在等待\n{B_ATK_PARTNER_NAME}的招式…{PAUSE 16}");
static const u8 sText_TheRainbowDisappeared[] = _("{B_ATK_TEAM2}场上的\n彩虹消失了！");
static const u8 sText_ARainbowAppearedOnSide[] = _("{B_ATK_TEAM2}队伍场上\n出现了彩虹！");
static const u8 sText_StatBuffsGone[] = _("所有能力加成被消除了！");
static const u8 sText_BlewAwayCreepingThorns[] = _("{B_ATK_NAME_WITH_PREFIX}\n吹散了蔓延荆棘！");
static const u8 sText_CreepingThornsDmg[] = _("恶意的荆棘刺入了\n{B_STACK_1_NAME_WITH_PREFIX}！");
static const u8 sText_ViciousThornsUsed[] = _("{B_DEF_TEAM2}队伍周围\n长出了恶意荆棘！");
static const u8 sText_IllWillLostPp[] = _("{B_ATK_NAME_WITH_PREFIX}的{B_BUFF1}\n因恶意失去了全部PP！");
static const u8 sText_ChargeAbilityBattler[] = _("{B_EFF_NAME_WITH_PREFIX}\n充满了力量！");
static const u8 sText_AttackerAteItem[] = _("{B_ATK_NAME_WITH_PREFIX}吃掉了{B_LAST_ITEM}！");
static const u8 sText_AfflictedWithSaltCure[] = _("{B_EFF_NAME_WITH_PREFIX}\n正在被盐腌！");
static const u8 sText_AfflictedWithSyrup[] = _("{B_EFF_NAME_WITH_PREFIX}\n被黏黏糖浆包裹了！");
static const u8 sText_ScriptingAcquiredAbility[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}\n获得了{B_DEF_ABILITY}！");
static const u8 sText_TeamAcquiredAbility[] = _("{B_ATK_NAME_WITH_PREFIX}的队伍\n获得了{B_DEF_ABILITY}！");
static const u8 sText_CommanderActivates[] = _("{B_STACK_1_NAME_WITH_PREFIX}跳进了\n{B_STACK_2_NAME_WITH_PREFIX}的嘴里！");
static const u8 sText_CommanderEnds[] = _("{B_STACK_1_NAME_WITH_PREFIX}\n回到了战场上！");
static const u8 sText_CommanderCantSwitch[] = _("{B_DEF_NAME_WITH_PREFIX}\n拒绝逃走！");
static const u8 sText_ViolentRush[] = _("{B_ATK_NAME_WITH_PREFIX}\n冲上了战场！");
static const u8 sText_RapidResponse[] = _("{B_ATK_NAME_WITH_PREFIX}\n登场了！");
static const u8 sText_ReadiedAction[] = _("{B_ATK_NAME_WITH_PREFIX}\n准备发动强力攻击！");
static const u8 sText_ShowdownMode[] = _("{B_ATK_NAME_WITH_PREFIX}\n使出了全力！");
static const u8 sText_TrickRoomStarts[] = _("次元变得扭曲了！");
static const u8 sText_InverseRoomStarts[] = _("属性相性被反转了！");
static const u8 sText_WonderRoomStarts[] = _("一片诡异的区域\n被创造出来！");
static const u8 sText_MagicRoomStarts[] = _("一片诡异的区域\n被创造出来！");
static const u8 sText_OriginReversion[] = _("{B_ATK_NAME_WITH_PREFIX}的起源回归！\n恢复了起源形态！");
static const u8 sText_CrownedReversion[] = _("{B_ATK_NAME_WITH_PREFIX}的复原！\n恢复了昔日的荣光！");
static const u8 sText_FogIsDeep[] = _("雾气很浓..。");
static const u8 sText_FogWasBlownAway[] = _("雾气被清除浓\n雾吹散了！");
static const u8 sText_PkmnFrostbittenBy[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的特性\n使{B_EFF_NAME_WITH_PREFIX}冻伤了！");
static const u8 sText_PkmnBledBy[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的特性\n使{B_EFF_NAME_WITH_PREFIX}流血了！");
static const u8 sText_ThunderCage[] = _("{B_DEF_NAME_WITH_PREFIX}\n被雷电囚笼困住了！");
static const u8 sText_SnapTrap[] = _("{B_DEF_NAME_WITH_PREFIX}\n被捕兽夹困住了！");
static const u8 sText_ElectroShot[] = _("{B_ATK_NAME_WITH_PREFIX}\n吸收了电力！");
static const u8 sText_AnnounceMonotypeBoostNormal[] = _("戏法空间和奇妙空间将在本场\n战斗剩余的每回合交替生效！");
static const u8 sText_AnnounceMonotypeBoostFighting[] = _("效果绝佳的招式变为普通伤害。\n格斗招式增强，变化招式\l被禁用，且无法替换宝可梦！");
static const u8 sText_AnnounceMonotypeBoostFlying[] = _("神秘气流保护飞行属性宝可梦，\n并提升冠军队伍的速度！\l戏法空间无效！");
static const u8 sText_AnnounceMonotypeBoostPoison[] = _("场地布满剧毒废料！\n冠军队伍受到的伤害\l会减少！");
static const u8 sText_AnnounceMonotypeBoostGround[] = _("所有飞行宝可梦无法停留在空中！\n冠军队伍受到的效果绝佳\l伤害会减少！");
static const u8 sText_AnnounceMonotypeBoostRock[] = _("冠军队伍免疫岩石属性招式，\n对岩石属性造成效果绝佳伤害，\l你的队伍会被盐腌！");
static const u8 sText_AnnounceMonotypeBoostBug[] = _("你的场地上似乎有永久的虫害！");
static const u8 sText_AnnounceMonotypeBoostGhost[] = _("你的整个队伍被诅咒了！");
static const u8 sText_AnnounceMonotypeBoostSteel[] = _("冠军的队伍打破了常规，\n双方受到的伤害都减少了！");
static const u8 sText_AnnounceMonotypeBoostFire[] = _("阳光变得极其强烈！");
static const u8 sText_AnnounceMonotypeBoostWater[] = _("一场极端的暴雨开始了，\n你的队伍被淹没了！");
static const u8 sText_AnnounceMonotypeBoostGrass[] = _("场地变得诡异了！，\n冠军的队伍将受到减免的伤害！");
static const u8 sText_AnnounceMonotypeBoostElectric[] = _("场地变得诡异了！，\n冠军的队伍将受到减免的伤害！");
static const u8 sText_AnnounceMonotypeBoostPsychic[] = _("场地变得诡异了！，\n冠军的队伍将受到减免的伤害！");
static const u8 sText_AnnounceMonotypeBoostIce[] = _("冠军的队伍将借此\n提升速度和攻击！");
static const u8 sText_AnnounceMonotypeBoostDragon[] = _("冠军的队伍正在威吓你的整个队伍，\n他们可以无视你的免疫！");
static const u8 sText_AnnounceMonotypeBoostDark[] = _("你的整个队伍睡着了！");
static const u8 sText_AnnounceMonotypeBoostFairy[] = _("场地变得奇异了！\n冠军队伍的妖精招式会对\l毒和钢属性造成效果绝佳伤害！");
static const u8 sText_ParasiticSpores[] = _("{B_ATK_NAME_WITH_PREFIX}\n被寄生孢子覆盖了！");
static const u8 sText_ParasiticSporesSpread[] = _("{B_STACK_1_NAME_WITH_PREFIX}把孢子\n散播给了{B_STACK_2_NAME_WITH_PREFIX}。");
static const u8 sText_HurtByParasiticSpores[] = _("{B_ATK_TEAM1} {B_ATK_NAME_WITH_PREFIX}\n受到了寄生孢子的伤害！");
static const u8 sText_CritCantIncrease[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的会心\n一击率无法再提高了！");
static const u8 sText_CritPlus1[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的会心\n一击率提高了！");
static const u8 sText_CritPlus2[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的会心\n一击率大幅提高了！");
static const u8 sText_CritPlus3[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的会心\n一击率达到最大！");
static const u8 sText_FilledWithFear[] = _("{B_DEF_NAME_WITH_PREFIX}\n充满了恐惧！");
static const u8 sText_PowerOfAlchemy[] = _("{B_STACK_1_NAME_WITH_PREFIX}转化了\n{B_STACK_2_NAME_WITH_PREFIX}失去的道具！");
static const u8 sText_PowerOfAlchemySludge[] = _("它变成了污泥.....。");
static const u8 sText_PowerOfAlchemyGold[] = _("它变成了金子！");
static const u8 sText_QuashEnds[] = _("时间的正常化结束了！");
static const u8 sText_FogEnds[] = _("诡异的雾气消散了！");
static const u8 sText_Smokescreen[] = _("{B_ATK_NAME_WITH_PREFIX}用烟雾\n笼罩了自己队伍！");
static const u8 sText_ExtraSkillTerrain[] = _("某种异常正在改变场地。");
static const u8 sText_ExtraSkillSteadyStatsBoost[] = _("{B_OPPONENT_MON1_NAME}的{STR_VAR_1}持续上升。");
static const u8 sText_ExtraSkillPosture[] = _("{B_ATK_NAME_WITH_PREFIX}摆出了{STR_VAR_2}，\n其{STR_VAR_1}提升了{B_BUFF3}%。");
static const u8 sText_ExtraSkillStatusOnTeam[] = _("你的部分队伍成员\n受到了{STR_VAR_1}的影响。");
static const u8 sText_ExtraSkillLastStand[] = _("{B_OPPONENT_MON1_NAME}不会接受失败！\n能力大幅提升了。");
static const u8 sText_ExtraSkillMatBlock[] = _("{B_OPPONENT_MON1_NAME}捡起了垫子\n作为本回合的掩护。");
static const u8 sText_ExtraSkillWoeUponYe[] = _("{STR_VAR_1}降临于你。");
static const u8 sText_ExtraSkillForesight[] = _("{B_PLAYER_MON1_NAME}被预知了。");
static const u8 sText_ExtraSkillEmbargo[] = _("{B_PLAYER_NAME}的出场宝可梦\n在{B_BUFF1}回合内无法使用道具。");
static const u8 sText_ExtraSkillNoProtect[] = _("{B_PLAYER_NAME}队伍的保护PP被抽干了。");
static const u8 sText_ExtraSkillWindSmackDown[] = _("强风使{B_PLAYER_MON1_NAME}\n被击落到地面。");
static const u8 sText_ExtraSkillReflect[] = _("{B_TRAINER1_NAME}队伍获得了\n{B_BUFF1}回合的反射壁。");
static const u8 sText_ExtraSkillLightscreen[] = _("{B_TRAINER1_NAME}队伍获得了\n{B_BUFF1}回合的光墙。");
static const u8 sText_ExtraSkillLuckyChant[] = _("{B_TRAINER1_NAME}队伍获得了\n{B_BUFF1}回合的幸运咒语。");
static const u8 sText_ExtraSkillPermaWideGuard[] = _("{B_TRAINER1_NAME}队伍被施放了\n永久广域防守。");
static const u8 sText_FogStatDrops[] = _("{B_ATK_NAME_WITH_PREFIX}的斗志\n因诡异之雾消散了！");
static const u8 sText_OnTheProwl[] = _("{B_ATK_NAME_WITH_PREFIX}\n在寻找最佳时机！");
static const u8 sText_FogStarts[] = _("诡异的雾气降临了战场！");
static const u8 sText_FlareBoostIgnites[] = _("{B_ATK_NAME_WITH_PREFIX}的内心\n火焰燃起了！");
static const u8 sText_FogReturns[] = _("诡异的雾气又回来了！");
static const u8 sText_RecurringNightmare[] = _("{B_STACK_1_NAME_WITH_PREFIX}\n消散在雾中！");
static const u8 sText_PkmnHurtByFuneralPyre[] = _("{B_ATK_NAME_WITH_PREFIX}受到了\n葬礼火堆的伤害！");
static const u8 sText_FuneralPyreEntry[] = _("{B_ATK_NAME_WITH_PREFIX}\n点燃了葬礼火堆！");
static const u8 sText_NoTurningBack[] = _("{B_DEF_NAME_WITH_PREFIX}\n不会从战斗中退缩！");
static const u8 sText_BattlerAcquiredAbility[] = _("{B_STACK_1_NAME_WITH_PREFIX}获得了{B_LAST_ABILITY}！");
static const u8 sText_SkyDropEndsEarly[] = _("{B_STACK_1_NAME_WITH_PREFIX}\n落回了地面。");
static const u8 sText_SkyDropCharge[] = _("{B_ATK_NAME_WITH_PREFIX}把\n{B_DEF_NAME_WITH_PREFIX}带上了天空！");
static const u8 sText_SkyDropStuck[] = _("{B_ATK_NAME_WITH_PREFIX}\n卡在空中了！");
static const u8 sText_OpposingStatBuffsGone[] = _("对手的能力加成被消除了！");
static const u8 sText_Hospitality[] = _("{B_ATK_NAME_WITH_PREFIX}给\n{B_DEF_NAME_WITH_PREFIX}递上了一杯茶！");
static const u8 sText_TargetCantHeal[] = _("{B_DEF_NAME_WITH_PREFIX}\n无法被治愈！");
static const u8 sText_RestrainingOrder[] = _("{B_STACK_1_NAME_WITH_PREFIX}\n收到了限制令！");
static const u8 sText_AttackerBreaksProtection[] = _("{B_ATK_NAME_WITH_PREFIX}\n打破了{B_DEF_NAME_WITH_PREFIX}的保护！");
static const u8 sText_AttackerShattersScreens[] = _("{B_ATK_NAME_WITH_PREFIX}\n击碎了{B_DEF_NAME_WITH_PREFIX}的屏障！");
static const u8 sText_AttackerDestroysSubstitute[] = _("{B_ATK_NAME_WITH_PREFIX}\n摧毁了{B_DEF_NAME_WITH_PREFIX}的替身！");
static const u8 sText_WhiteSmoke[] = _("浓烟笼罩了\n{B_ATK_NAME_WITH_PREFIX}的场地！");
static const u8 sText_HotCoalsSet[] = _("{B_ATK_NAME_WITH_PREFIX}向对手场地\n投掷了热煤！");
static const u8 sText_HotCoalsBurn[] = _("{B_STACK_1_NAME_WITH_PREFIX}\n被热煤灼伤了！");
static const u8 sText_HotCoalsExtinguish[] = _("炭火的热量消散了！");
static const u8 sText_DisableAttackerItem[] = _("{B_ATK_NAME_WITH_PREFIX}的{B_LAST_ITEM}\n被禁止使用了！");
static const u8 sText_SuperStrainRecoil[] = _("负担对{B_ATK_NAME_WITH_PREFIX}\n造成了影响！");
static const u8 sText_AttackerLostDarkType[] = _("{B_ATK_NAME_WITH_PREFIX}\n已经没有黑暗了！");
static const u8 sText_ClearSkies[] = _("天空放晴了！");
static const u8 sText_WeatherNormal[] = _("天气恢复了正常！");
static const u8 sText_KnowYourPlace[] = _("{B_DEF_NAME_WITH_PREFIX}\n被这一击打晕了！");
static const u8 sText_RoomsCleared[] = _("扭曲的空间恢复了正常！");
static const u8 sText_Trepidation[] = _("{B_DEF_NAME_WITH_PREFIX}\n变得焦躁不安！");
static const u8 sText_Kinesis[] = _("{B_DEF_NAME_WITH_PREFIX}的{B_LAST_ITEM}\n突然飞走了！");
static const u8 sText_CaltropsSet[] = _("锋利的铁蒺藜散落\n在对手场地周围！");
static const u8 sText_CaltropsDamage[] = _("铁蒺藜撕裂了\n{B_STACK_1_NAME_WITH_PREFIX}！");
static const u8 sText_CaltropsFade[] = _("铁蒺藜被磨钝了！");
static const u8 sText_HotCoalsFree[] = _("{B_ATK_NAME_WITH_PREFIX}\n吹散了热煤！");
static const u8 sText_CaltropsFree[] = _("{B_ATK_NAME_WITH_PREFIX}\n吹散了铁蒺藜！");
static const u8 sText_ItemDestroyed[] = _("{B_ATK_NAME_WITH_PREFIX}\n摧毁了{B_DEF_NAME_WITH_PREFIX}的{B_LAST_ITEM}！");
static const u8 sText_SwapWith[] = _("{B_ATK_NAME_WITH_PREFIX}\n与{B_DEF_NAME_WITH_PREFIX}交换了位置！");
static const u8 sText_QuickGuard[] = _("{B_ATK_NAME_WITH_PREFIX}\n保护队伍免受先制招式影响！");
static const u8 sText_BloodStainAnnounce[] = _("{B_ATK_NAME_WITH_PREFIX}在流血！");
static const u8 sText_Trapped[] = _("{B_DEF_NAME_WITH_PREFIX}被困住了！");
static const u8 sText_HoneyGather[] = _("{B_ATK_NAME_WITH_PREFIX}\n发现了一些{B_LAST_ITEM}！");
static const u8 sText_LifeSteal[] = _("{B_ATK_NAME_WITH_PREFIX}\n吸取了{B_DEF_NAME_WITH_PREFIX}的体力！");
static const u8 sText_RudeAwakening[] = _("{B_ATK_NAME_WITH_PREFIX}愤怒地\n醒来了！");
static const u8 sText_Cutthroat[] = _("{B_ATK_NAME_WITH_PREFIX}\n准备好了刀锋！");
static const u8 sText_ItemStatProtected[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}的{B_LAST_ITEM}\n防止了能力下降！");
static const u8 sText_StackChangedTypeTo[] = _("{B_STACK_1_NAME_WITH_PREFIX}的属性\n变成了{B_BUFF1}！");
static const u8 sText_StackRegainedHealth[] = _("{B_STACK_1_NAME_WITH_PREFIX}\n恢复了体力！");
static const u8 sText_FireCoatDamage[] = _("{B_ATK_NAME_WITH_PREFIX}\n被火焰灼伤了！");
static const u8 sText_FireCoatStart[] = _("火焰吞噬了战场！");
static const u8 sText_Soothsayer[] = _("{B_ATK_NAME_WITH_PREFIX}\n窥视了未来！");
static const u8 sText_RoseGarden[] = _("剧毒尖刺倾泻在\n对手的场地上！");
static const u8 sText_MimicryEnds[] = _("{B_STACK_1_NAME_WITH_PREFIX}的属性\n恢复了正常！");
static const u8 sText_TerrainBecomesToxic[] = _("剧毒污泥覆盖了战场！");
static const u8 sText_ToxicTerrainEnds[] = _("污泥从战场上消失了。");
static const u8 sText_ToxicTerrainDamages[] = _("{B_STACK_1_NAME_WITH_PREFIX}\n被剧毒场地伤害了！");
static const u8 sText_AbsorbStealthRocks[] = _("{B_ATK_NAME_WITH_PREFIX}\n吸收了隐形岩！");
static const u8 sText_EvasionClauseDisablesMove[] = _("闪避条款使这个招式\n无法被选择！\p");
static const u8 sText_CurrentFightDisablesMove[] = _("当前的战斗使这个招式\n无法被选择！\p");
static const u8 sText_WinterThroneEntry[] = _("刺骨的寒意笼\n罩了战场！");
static const u8 sText_PkmnHurtByWinterThrone[] = _("{B_ATK_NAME_WITH_PREFIX}\n受到了严寒的伤害！");
static const u8 sText_BackupPower[] = _("{B_STACK_1_NAME_WITH_PREFIX}\n正在重启！");
static const u8 sText_ExtraSkillAbility1[] = _("{B_TRAINER1_NAME}队伍\n获得了特性{B_BUFF1}。");
static const u8 sText_ExtraSkillAbility2[] = _("{B_TRAINER1_NAME}队伍\n获得了特性{B_BUFF2}。");
static const u8 sText_ExtraSkillAbility3[] = _("{B_TRAINER1_NAME}队伍\n获得了特性{B_BUFF3}。");
static const u8 sText_ChristmasNightmare[] = _("冰雹加剧了！");
static const u8 sText_DrakelpHead[] = _("{B_DEF_NAME_WITH_PREFIX}的海藻\n削弱了攻击！");
static const u8 sText_DrakelpHeadReset[] = _("{B_STACK_1_NAME_WITH_PREFIX}\n的海藻重新长出来了！");
static const u8 sText_MentalPollution[] = _("精神干扰阻止了其他\n宝可梦使用特性！");
static const u8 sText_IceStatue[] = _("{B_DEF_NAME_WITH_PREFIX}\n变成了冰！");
static const u8 sText_ToxicBoostPoisons[] = _("{B_ATK_NAME_WITH_PREFIX}\n吸收了毒素！");
static const u8 sText_PetalShield[] = _("{B_ATK_NAME_WITH_PREFIX}\n用纷飞的花瓣包裹了自己。");
static const u8 sText_PowerOutageEntry[] = _("{B_ATK_NAME_WITH_PREFIX}\n周围的空气噼啪作响");
static const u8 sText_StackLostType[] = _("{B_STACK_1_NAME_WITH_PREFIX}\n失去了{B_BUFF1}属性！");
static const u8 sText_RockClimb[] = _("{B_ATK_NAME_WITH_PREFIX}\n爬到了高处！");
static const u8 sText_IsEnraged[] = _("{B_ATK_NAME_WITH_PREFIX}暴怒了！");
static const u8 sText_MadnessEnhancement[] = _("{B_ATK_NAME_WITH_PREFIX}疯狂地怪笑！");
static const u8 sText_BecomesEnraged[] = _("{B_EFF_NAME_WITH_PREFIX}\n被怒火充满！");
static const u8 sText_EnrageHurt[] = _("{B_ATK_NAME_WITH_PREFIX}\n在暴走中伤害了自己！");
static const u8 sText_MadnessEnrage[] = _("{B_ATK_NAME_WITH_PREFIX}\n陷入了疯狂！");
static const u8 sText_BecomesDrenched[] = _("{B_EFF_NAME_WITH_PREFIX}\n湿透了！");
static const u8 sText_Chuckster[] = _("{B_DEF_NAME_WITH_PREFIX}\n被扔了出去！");
static const u8 sText_Waterlog[] = _("{B_DEF_NAME_WITH_PREFIX}\n被延后了！");
static const u8 sText_Fetch[] = _("去吧，{B_ACTIVE_NAME}！");
static const u8 sText_FetchRetrieve[] = _("{B_ATK_NAME_WITH_PREFIX}\n取回了{B_LAST_ITEM}！");
static const u8 sText_FetchNothing[] = _("但是{B_ATK_NAME_WITH_PREFIX}\n已经拿着道具了…");
static const u8 sText_FetchSearch[] = _("{B_ATK_NAME_WITH_PREFIX}\n离开去寻找{B_LAST_ITEM}！");
static const u8 sText_FetchSwitchNoItem[] = _("{B_ATK_NAME_WITH_PREFIX}\n跑出了场地！");
static const u8 sText_Teatime[] = _("{B_ATK_NAME_WITH_PREFIX}\n举办了茶会！");
const u8 gText_PkmnsXPreventsSwitching[] = _("{STR_VAR_1}的特性\n阻止了替换！{PAUSE_UNTIL_PRESS}");
static const u8 sText_PlayerDefeatedLinkTrainer[] = _("玩家击败了\n{B_LINK_OPPONENT1_NAME}！");
static const u8 sText_TwoLinkTrainersDefeated[] = _("玩家击败了{B_LINK_OPPONENT1_NAME}\n和{B_LINK_OPPONENT2_NAME}！");
static const u8 sText_PlayerLostAgainstLinkTrainer[] = _("玩家输给了\n{B_LINK_OPPONENT1_NAME}！");
static const u8 sText_PlayerLostToTwo[] = _("玩家输给了{B_LINK_OPPONENT1_NAME}\n和{B_LINK_OPPONENT2_NAME}！");
static const u8 sText_PlayerBattledToDrawLinkTrainer[] = _("玩家与\n{B_LINK_OPPONENT1_NAME}打成平局！");
static const u8 sText_PlayerBattledToDrawVsTwo[] = _("玩家与{B_LINK_OPPONENT1_NAME}\n和{B_LINK_OPPONENT2_NAME}打成平局！");
static const u8 sText_WildFled[] = _("{PLAY_SE SE_FLEE}{B_LINK_OPPONENT1_NAME}逃走了！");
static const u8 sText_TwoWildFled[] = _("{PLAY_SE SE_FLEE}{B_LINK_OPPONENT1_NAME}和\n{B_LINK_OPPONENT2_NAME}逃走了！");
static const u8 sText_OpponentMon1Appeared[] = _("{B_OPPONENT_MON1_NAME}出现了！\p");
static const u8 sText_WildPkmnAppeared[] = _("野生的{B_OPPONENT_MON1_NAME}出现了！\p");
static const u8 sText_LegendaryPkmnAppeared[] = _("野生的{B_OPPONENT_MON1_NAME}出现了！\p");
static const u8 sText_WildPkmnAppearedPause[] = _("野生的{B_OPPONENT_MON1_NAME}出现了！{PAUSE 127}");
static const u8 sText_TwoWildPkmnAppeared[] = _("野生的{B_OPPONENT_MON1_NAME}和\n{B_OPPONENT_MON2_NAME}出现了！\p");
static const u8 sText_Trainer1WantsToBattle[] = _("{B_TRAINER1_CLASS} {B_TRAINER1_NAME}\n想要对战！\p");
static const u8 sText_LinkTrainerWantsToBattle[] = _("{B_LINK_OPPONENT1_NAME}\n想要对战！");
static const u8 sText_TwoLinkTrainersWantToBattle[] = _("{B_LINK_OPPONENT1_NAME}和{B_LINK_OPPONENT2_NAME}\n想要对战！");
static const u8 sText_Trainer1SentOutPkmn[] = _("{B_TRAINER1_CLASS} {B_TRAINER1_NAME}派出了\n{B_OPPONENT_MON1_NAME}！");
static const u8 sText_Trainer1SentOutTwoPkmn[] = _("{B_TRAINER1_CLASS} {B_TRAINER1_NAME}派出了\n{B_OPPONENT_MON1_NAME}和{B_OPPONENT_MON2_NAME}！");
static const u8 sText_Trainer1SentOutPkmn2[] = _("{B_TRAINER1_CLASS} {B_TRAINER1_NAME}派出了\n{B_BUFF1}！");
static const u8 sText_LinkTrainerSentOutPkmn[] = _("{B_LINK_OPPONENT1_NAME}派出了\n{B_OPPONENT_MON1_NAME}！");
static const u8 sText_LinkTrainerSentOutTwoPkmn[] = _("{B_LINK_OPPONENT1_NAME}派出了\n{B_OPPONENT_MON1_NAME}和{B_OPPONENT_MON2_NAME}！");
static const u8 sText_TwoLinkTrainersSentOutPkmn[] = _("{B_LINK_OPPONENT1_NAME}派出了{B_LINK_OPPONENT_MON1_NAME}！\n{B_LINK_OPPONENT2_NAME}派出了{B_LINK_OPPONENT_MON2_NAME}！");
static const u8 sText_LinkTrainerSentOutPkmn2[] = _("{B_LINK_OPPONENT1_NAME}派出了\n{B_BUFF1}！");
static const u8 sText_LinkTrainerMultiSentOutPkmn[] = _("{B_LINK_SCR_TRAINER_NAME}派出了\n{B_BUFF1}！");
static const u8 sText_GoPkmn[] = _("上吧！{B_PLAYER_MON1_NAME}！");
static const u8 sText_GoTwoPkmn[] = _("上吧！{B_PLAYER_MON1_NAME}和\n{B_PLAYER_MON2_NAME}！");
static const u8 sText_GoPkmn2[] = _("上吧！{B_BUFF1}！");
static const u8 sText_DoItPkmn[] = _("交给你了！{B_BUFF1}！");
static const u8 sText_GoForItPkmn[] = _("加油，{B_BUFF1}！");
static const u8 sText_YourFoesWeakGetEmPkmn[] = _("对手很弱！\n上吧，{B_BUFF1}！");
static const u8 sText_LinkPartnerSentOutPkmnGoPkmn[] = _("{B_LINK_PARTNER_NAME}派出了{B_LINK_PLAYER_MON2_NAME}！\n上吧！{B_LINK_PLAYER_MON1_NAME}！");
static const u8 sText_PkmnThatsEnough[] = _("{B_BUFF1}，够了！\n回来吧！");
static const u8 sText_PkmnComeBack[] = _("{B_BUFF1}，回来吧！");
static const u8 sText_PkmnOkComeBack[] = _("{B_BUFF1}，好吧！\n回来吧！");
static const u8 sText_PkmnGoodComeBack[] = _("{B_BUFF1}，干得好！\n回来吧！");
static const u8 sText_Trainer1WithdrewPkmn[] = _("{B_TRAINER1_CLASS} {B_TRAINER1_NAME}\n收回了{B_BUFF1}！");
static const u8 sText_LinkTrainer1WithdrewPkmn[] = _("{B_LINK_OPPONENT1_NAME}收回了\n{B_BUFF1}！");
static const u8 sText_LinkTrainer2WithdrewPkmn[] = _("{B_LINK_SCR_TRAINER_NAME}收回了\n{B_BUFF1}！");
static const u8 sText_WildPkmnPrefix[] = _("野生的");
static const u8 sText_FoePkmnPrefix[] = _("对");
static const u8 sText_EmptyString8[] = _("");
static const u8 sText_FoePkmnPrefix2[] = _("对");
static const u8 sText_AllyPkmnPrefix[] = _("盟友");
static const u8 sText_FoePkmnPrefix3[] = _("对");
static const u8 sText_AllyPkmnPrefix2[] = _("盟友");
static const u8 sText_FoePkmnPrefix4[] = _("对");
static const u8 sText_AllyPkmnPrefix3[] = _("盟友");
static const u8 sText_AttackerUsedX[] = _("{B_ATK_NAME_WITH_PREFIX}使用了\n{B_BUFF3}！");
static const u8 sText_ExclamationMark[] = _("！");
static const u8 sText_ExclamationMark2[] = _("！");
static const u8 sText_ExclamationMark3[] = _("！");
static const u8 sText_ExclamationMark4[] = _("！");
static const u8 sText_ExclamationMark5[] = _("！");
static const u8 sText_Accuracy[] = _("命中率");
static const u8 sText_Evasiveness[] = _("闪避率");
static const u8 sText_PokeblockWasTooSpicy[] = _("太辣了！");
static const u8 sText_PokeblockWasTooDry[] = _("太干了！");
static const u8 sText_PokeblockWasTooSweet[] = _("太甜了！");
static const u8 sText_PokeblockWasTooBitter[] = _("太苦了！");
static const u8 sText_PokeblockWasTooSour[] = _("太酸了！");
static const u8 sText_Someones[] = _("某人的");
static const u8 sText_Lanettes[] = _("兰妮特的");
static const u8 sText_EnigmaBerry[] = _("谜芝果");
static const u8 sText_BerrySuffix[] = _("树果");
static const u8 sText_BattlerGotTheType[] = _("{B_ACTIVE_NAME_WITH_PREFIX}追加了{B_BUFF1}属性！");
const u8 gText_PkmnIsEvolving[] = _("咦？\n{STR_VAR_1}开始进化了！");
const u8 gText_PkmnIsDeEvolving[] = _("咦？\n{STR_VAR_1}开始退化了！");
const u8 gText_CongratsPkmnEvolved[] = _("恭喜！你的{STR_VAR_1}\n进化成了{STR_VAR_2}！{WAIT_SE}\p");
const u8 gText_CongratsPkmnDeEvolved[] = _("恭喜！你的{STR_VAR_1}\n退化成了{STR_VAR_2}！{WAIT_SE}\p");
const u8 gText_PkmnStoppedEvolving[] = _("咦？{STR_VAR_1}\n停止了进化！\p");
const u8 gText_PkmnStoppedDeEvolving[] = _("咦？{STR_VAR_1}\n停止了退化！\p");
const u8 gText_EllipsisQuestionMark[] = _("……？\p");
const u8 gText_WhatWillPkmnDo[] = _("{B_ACTIVE_NAME2}\n要做什么？");
const u8 gText_WhatWillPkmnDo2[] = _("{B_PLAYER_NAME}\n要做什么？");
const u8 gText_WhatWillWallyDo[] = _("小胜要\n怎么做？");
const u8 gText_LinkStandby[] = _("{PAUSE 16}连接待机…");
const u8 gText_BattleMenu[] = _("战斗{CLEAR_TO 56}背包\n宝可梦{CLEAR_TO 56}逃跑");
const u8 gText_BattleMenuTrainer[] = _("战斗{CLEAR_TO 56}信息\n宝可梦{CLEAR_TO 56}放弃");
const u8 gText_SafariZoneMenu[] = _("球{CLEAR_TO 56}{POKEBLOCK}\n靠近{CLEAR_TO 56}逃跑");
const u8 gText_MoveInterfacePP[] = _("PP");
const u8 gText_MoveInterfaceType[] = _("属性/");
const u8 gText_MoveInterfacePpType[] = _("{PALETTE 5}{COLOR_HIGHLIGHT_SHADOW DYNAMIC_COLOR4 DYNAMIC_COLOR5 DYNAMIC_COLOR6}PP\n属性/");
const u8 gText_MoveInterfaceDynamicColors[] = _("{PALETTE 5}{COLOR_HIGHLIGHT_SHADOW DYNAMIC_COLOR4 DYNAMIC_COLOR5 DYNAMIC_COLOR6}");
const u8 gText_WhichMoveToForget4[] = _("{PALETTE 5}{COLOR_HIGHLIGHT_SHADOW DYNAMIC_COLOR4 DYNAMIC_COLOR5 DYNAMIC_COLOR6}要忘记\n哪个招式？");
const u8 gText_BattleYesNoChoice[] = _("是\n否");
const u8 gText_BattleSwitchWhich[] = _("{PALETTE 5}{COLOR_HIGHLIGHT_SHADOW DYNAMIC_COLOR4 DYNAMIC_COLOR5 DYNAMIC_COLOR6}要替换\n哪一只？");
const u8 gText_BattleSwitchWhich2[] = _("{PALETTE 5}{COLOR_HIGHLIGHT_SHADOW DYNAMIC_COLOR4 DYNAMIC_COLOR5 DYNAMIC_COLOR6}");
const u8 gText_BattleSwitchWhich3[] = _("{UP_ARROW}");
const u8 gText_BattleSwitchWhich4[] = _("{ESCAPE 4}");
const u8 gText_BattleSwitchWhich5[] = _("-");
const u8 gText_SafariBalls[] = _("{HIGHLIGHT DARK_GRAY}狩猎球");
const u8 gText_SafariBallLeft[] = _("{HIGHLIGHT DARK_GRAY}剩余：{HIGHLIGHT DARK_GRAY}");
const u8 gText_Sleep[] = _("睡眠");
const u8 gText_Poison[] = _("毒");
const u8 gText_Burn[] = _("灼伤");
const u8 gText_Paralysis[] = _("麻痹");
const u8 gText_Ice[] = _("冰");
const u8 gText_Confusion[] = _("念力");
const u8 gText_Love[] = _("着迷");
const u8 gText_Bleed[] = _("流血");
const u8 gText_SpaceAndSpace[] = _("和");
const u8 gText_CommaSpace[] = _("、");
const u8 gText_Space2[] = _(" ");
const u8 gText_LineBreak[] = _("\l");
const u8 gText_NewLine[] = _("\n");
const u8 gText_Are[] = _("是");
const u8 gText_Are2[] = _("是");
const u8 gText_BadEgg[] = _("坏蛋");
const u8 gText_BattleWallyName[] = _("小胜");
const u8 gText_Win[] = _("{HIGHLIGHT TRANSPARENT}胜");
const u8 gText_Loss[] = _("{HIGHLIGHT TRANSPARENT}负");
const u8 gText_Draw[] = _("{HIGHLIGHT TRANSPARENT}平");
static const u8 sText_SpaceIs[] = _("is");
static const u8 sText_ApostropheS[] = _("的");
const u8 gText_BattleTourney[] = _("对战锦标赛");
static const u8 sText_Round1[] = _("第1轮");
static const u8 sText_Round2[] = _("第2轮");
static const u8 sText_Semifinal[] = _("半决赛");
static const u8 sText_Final[] = _("决赛");
const u8 gText_TheGreatNewHope[] = _("全新的希望！\p");
const u8 gText_WillChampionshipDreamComeTrue[] = _("冠军之梦会实现吗？！\p");
const u8 gText_AFormerChampion[] = _("前冠军！\p");
const u8 gText_ThePreviousChampion[] = _("上一任冠军！\p");
const u8 gText_TheUnbeatenChampion[] = _("不败的冠军！\p");
const u8 gText_PlayerMon1Name[] = _("{B_PLAYER_MON1_NAME}");
const u8 gText_Vs[] = _("VS");
const u8 gText_OpponentMon1Name[] = _("{B_OPPONENT_MON1_NAME}");
const u8 gText_Mind[] = _("心智");
const u8 gText_Skill[] = _("技巧");
const u8 gText_Body[] = _("身体");
const u8 gText_Judgement[] = _("{B_BUFF1}{CLEAR 13}判定{CLEAR 13}{B_BUFF2}");
static const u8 sText_TwoTrainersSentPkmn[] = _("{B_TRAINER1_CLASS} {B_TRAINER1_NAME}派出了\n{B_OPPONENT_MON1_NAME}！\p{B_TRAINER2_CLASS} {B_TRAINER2_NAME}派出了\n{B_OPPONENT_MON2_NAME}！");
static const u8 sText_Trainer2SentOutPkmn[] = _("{B_TRAINER2_CLASS} {B_TRAINER2_NAME}派出了\n{B_BUFF1}！");
static const u8 sText_TwoTrainersWantToBattle[] = _("{B_TRAINER1_CLASS} {B_TRAINER1_NAME}和\n{B_TRAINER2_CLASS} {B_TRAINER2_NAME}\l想要对战！\p");
static const u8 sText_InGamePartnerSentOutZGoN[] = _("{B_PARTNER_CLASS} {B_PARTNER_NAME}派出了\n{B_PLAYER_MON2_NAME}！\l上吧，{B_PLAYER_MON1_NAME}！");
static const u8 sText_RefIfNothingIsDecided[] = _("裁判：若3回合内未分胜负，\n将进入判定！");
static const u8 sText_RefThatsIt[] = _("裁判：时间到！现在进入判定\n来决定胜者！");
static const u8 sText_RefJudgeMind[] = _("裁判：判定类别1，心智！\n展现出最勇敢表现的宝可梦！\p");
static const u8 sText_RefJudgeSkill[] = _("裁判：判定类别2，技巧！\n招式运用最出色的宝可梦！\p");
static const u8 sText_RefJudgeBody[] = _("裁判：判定类别3，体魄！\n最有活力的宝可梦！\p");
static const u8 sText_RefPlayerWon[] = _("裁判：判定：{B_BUFF1}对{B_BUFF2}！\n胜者是{B_PLAYER_NAME}的{B_PLAYER_MON1_NAME}！\p");
static const u8 sText_RefOpponentWon[] = _("裁判：判定：{B_BUFF1}对{B_BUFF2}！\n胜者是{B_TRAINER1_NAME}的{B_OPPONENT_MON1_NAME}！\p");
static const u8 sText_RefDraw[] = _("裁判：判定：3对3！\n平局！\p");
static const u8 sText_RefCommenceBattle[] = _("裁判：{B_PLAYER_MON1_NAME} VS {B_OPPONENT_MON1_NAME}！\n开始对战！");
static const u8 sText_Trainer1Fled[] = _("{PLAY_SE SE_FLEE}{B_TRAINER1_CLASS} {B_TRAINER1_NAME}逃走了！");
static const u8 sText_PlayerLostAgainstTrainer1[] = _("玩家输给了\n{B_TRAINER1_CLASS} {B_TRAINER1_NAME}！");
static const u8 sText_PlayerBattledToDrawTrainer1[] = _("玩家与\n{B_TRAINER1_CLASS} {B_TRAINER1_NAME}打成平局！");
const u8 gText_RecordBattleToPass[] = _("要将这场对战记录到\n开拓区通行证上吗？");
const u8 gText_BattleRecordedOnPass[] = _("{B_PLAYER_NAME}的战斗结果\n已记录在开拓通行证上。");
static const u8 sText_LinkTrainerWantsToBattlePause[] = _("{B_LINK_OPPONENT1_NAME}\n想要对战！{PAUSE 49}");
static const u8 sText_TwoLinkTrainersWantToBattlePause[] = _("{B_LINK_OPPONENT1_NAME}和{B_LINK_OPPONENT2_NAME}\n想要对战！{PAUSE 49}");
static const u8 sText_Your1[] = _("你的");
static const u8 sText_Opposing1[] = _("对手的");
static const u8 sText_Your2[] = _("你的");
static const u8 sText_Opposing2[] = _("对手的");
const u8 gText_NoMoreRoomForPokemon[] = _("没有更多宝可梦的位置了！\p宝可梦盒子已满，\n无法再接收！");
const u8 gText_NicknameThisPokemon[] = _("要给这只{STR_VAR_1}\n起昵称吗？");
static const u8 sText_ExtraSkillPermaStickyWeb[] = _("{B_PLAYER_NAME}队伍周围被施放了\n永久黏黏网。");
static const u8 sText_LightWieldSetUpLightScreen[] = _("{B_ACTIVE_NAME_WITH_PREFIX}的持光\n展开了光墙！");
static const u8 sText_PkmnGotOverItsFear[] = _("{B_SCR_ACTIVE_NAME_WITH_PREFIX}\n克服了恐惧！");

const u8 *const gBattleStringsTable[BATTLESTRINGS_COUNT] =
{
    [STRINGID_TRAINER1LOSETEXT - 12] = sText_Trainer1LoseText,
    [STRINGID_PKMNGAINEDEXP - 12] = sText_PkmnGainedEXP,
    [STRINGID_PKMNGREWTOLV - 12] = sText_PkmnGrewToLv,
    [STRINGID_PKMNLEARNEDMOVE - 12] = sText_PkmnLearnedMove,
    [STRINGID_TRYTOLEARNMOVE1 - 12] = sText_TryToLearnMove1,
    [STRINGID_TRYTOLEARNMOVE2 - 12] = sText_TryToLearnMove2,
    [STRINGID_TRYTOLEARNMOVE3 - 12] = sText_TryToLearnMove3,
    [STRINGID_PKMNFORGOTMOVE - 12] = sText_PkmnForgotMove,
    [STRINGID_STOPLEARNINGMOVE - 12] = sText_StopLearningMove,
    [STRINGID_DIDNOTLEARNMOVE - 12] = sText_DidNotLearnMove,
    [STRINGID_PKMNLEARNEDMOVE2 - 12] = sText_PkmnLearnedMove2,
    [STRINGID_ATTACKMISSED - 12] = sText_AttackMissed,
    [STRINGID_PKMNPROTECTEDITSELF - 12] = sText_PkmnProtectedItself,
    [STRINGID_STATSWONTINCREASE2 - 12] = sText_StatsWontIncrease2,
    [STRINGID_AVOIDEDDAMAGE - 12] = sText_AvoidedDamage,
    [STRINGID_ITDOESNTAFFECT - 12] = sText_ItDoesntAffect,
    [STRINGID_ATTACKERFAINTED - 12] = sText_AttackerFainted,
    [STRINGID_TARGETFAINTED - 12] = sText_TargetFainted,
    [STRINGID_PLAYERGOTMONEY - 12] = sText_PlayerGotMoney,
    [STRINGID_PLAYERWHITEOUT - 12] = sText_PlayerWhiteout,
    [STRINGID_PLAYERWHITEOUT2 - 12] = sText_PlayerWhiteout2,
    [STRINGID_PREVENTSESCAPE - 12] = sText_PreventsEscape,
    [STRINGID_HITXTIMES - 12] = sText_HitXTimes,
    [STRINGID_PKMNFELLASLEEP - 12] = sText_PkmnFellAsleep,
    [STRINGID_PKMNMADESLEEP - 12] = sText_PkmnMadeSleep,
    [STRINGID_PKMNALREADYASLEEP - 12] = sText_PkmnAlreadyAsleep,
    [STRINGID_PKMNALREADYASLEEP2 - 12] = sText_PkmnAlreadyAsleep2,
    [STRINGID_PKMNWASNTAFFECTED - 12] = sText_PkmnWasntAffected,
    [STRINGID_PKMNWASPOISONED - 12] = sText_PkmnWasPoisoned,
    [STRINGID_PKMNPOISONEDBY - 12] = sText_PkmnPoisonedBy,
    [STRINGID_PKMNHURTBYPOISON - 12] = sText_PkmnHurtByPoison,
    [STRINGID_PKMNALREADYPOISONED - 12] = sText_PkmnAlreadyPoisoned,
    [STRINGID_PKMNBADLYPOISONED - 12] = sText_PkmnBadlyPoisoned,
    [STRINGID_PKMNENERGYDRAINED - 12] = sText_PkmnEnergyDrained,
    [STRINGID_PKMNWASBURNED - 12] = sText_PkmnWasBurned,
    [STRINGID_PKMNBURNEDBY - 12] = sText_PkmnBurnedBy,
    [STRINGID_PKMNHURTBYBURN - 12] = sText_PkmnHurtByBurn,
    [STRINGID_PKMNWASFROZEN - 12] = sText_PkmnWasFrozen,
    [STRINGID_PKMNFROZENBY - 12] = sText_PkmnFrozenBy,
    [STRINGID_PKMNISFROZEN - 12] = sText_PkmnIsFrozen,
    [STRINGID_PKMNWASDEFROSTED - 12] = sText_PkmnWasDefrosted,
    [STRINGID_PKMNWASDEFROSTED2 - 12] = sText_PkmnWasDefrosted2,
    [STRINGID_PKMNWASDEFROSTEDBY - 12] = sText_PkmnWasDefrostedBy,
    [STRINGID_PKMNWASPARALYZED - 12] = sText_PkmnWasParalyzed,
    [STRINGID_PKMNWASPARALYZEDBY - 12] = sText_PkmnWasParalyzedBy,
    [STRINGID_PKMNISPARALYZED - 12] = sText_PkmnIsParalyzed,
    [STRINGID_PKMNISALREADYPARALYZED - 12] = sText_PkmnIsAlreadyParalyzed,
    [STRINGID_PKMNHEALEDPARALYSIS - 12] = sText_PkmnHealedParalysis,
    [STRINGID_PKMNDREAMEATEN - 12] = sText_PkmnDreamEaten,
    [STRINGID_STATSWONTINCREASE - 12] = sText_StatsWontIncrease,
    [STRINGID_STATSWONTDECREASE - 12] = sText_StatsWontDecrease,
    [STRINGID_TEAMSTOPPEDWORKING - 12] = sText_TeamStoppedWorking,
    [STRINGID_FOESTOPPEDWORKING - 12] = sText_FoeStoppedWorking,
    [STRINGID_PKMNISCONFUSED - 12] = sText_PkmnIsConfused,
    [STRINGID_PKMNHEALEDCONFUSION - 12] = sText_PkmnHealedConfusion,
    [STRINGID_PKMNWASCONFUSED - 12] = sText_PkmnWasConfused,
    [STRINGID_PKMNALREADYCONFUSED - 12] = sText_PkmnAlreadyConfused,
    [STRINGID_PKMNFELLINLOVE - 12] = sText_PkmnFellInLove,
    [STRINGID_PKMNINLOVE - 12] = sText_PkmnInLove,
    [STRINGID_PKMNIMMOBILIZEDBYLOVE - 12] = sText_PkmnImmobilizedByLove,
    [STRINGID_PKMNBLOWNAWAY - 12] = sText_PkmnBlownAway,
    [STRINGID_PKMNCHANGEDTYPE - 12] = sText_PkmnChangedType,
    [STRINGID_PKMNFLINCHED - 12] = sText_PkmnFlinched,
    [STRINGID_PKMNREGAINEDHEALTH - 12] = sText_PkmnRegainedHealth,
    [STRINGID_PKMNHPFULL - 12] = sText_PkmnHPFull,
    [STRINGID_PKMNRAISEDSPDEF - 12] = sText_PkmnRaisedSpDef,
    [STRINGID_PKMNRAISEDDEF - 12] = sText_PkmnRaisedDef,
    [STRINGID_PKMNCOVEREDBYVEIL - 12] = sText_PkmnCoveredByVeil,
    [STRINGID_PKMNUSEDSAFEGUARD - 12] = sText_PkmnUsedSafeguard,
    [STRINGID_PKMNSAFEGUARDEXPIRED - 12] = sText_PkmnSafeguardExpired,
    [STRINGID_PKMNWENTTOSLEEP - 12] = sText_PkmnWentToSleep,
    [STRINGID_PKMNSLEPTHEALTHY - 12] = sText_PkmnSleptHealthy,
    [STRINGID_PKMNWHIPPEDWHIRLWIND - 12] = sText_PkmnWhippedWhirlwind,
    [STRINGID_PKMNTOOKSUNLIGHT - 12] = sText_PkmnTookSunlight,
    [STRINGID_PKMNLOWEREDHEAD - 12] = sText_PkmnLoweredHead,
    [STRINGID_PKMNISGLOWING - 12] = sText_PkmnIsGlowing,
    [STRINGID_PKMNFLEWHIGH - 12] = sText_PkmnFlewHigh,
    [STRINGID_PKMNDUGHOLE - 12] = sText_PkmnDugHole,
    [STRINGID_PKMNSQUEEZEDBYBIND - 12] = sText_PkmnSqueezedByBind,
    [STRINGID_PKMNTRAPPEDINVORTEX - 12] = sText_PkmnTrappedInVortex,
    [STRINGID_PKMNWRAPPEDBY - 12] = sText_PkmnWrappedBy,
    [STRINGID_PKMNCLAMPED - 12] = sText_PkmnClamped,
    [STRINGID_PKMNHURTBY - 12] = sText_PkmnHurtBy,
    [STRINGID_PKMNFREEDFROM - 12] = sText_PkmnFreedFrom,
    [STRINGID_PKMNCRASHED - 12] = sText_PkmnCrashed,
    [STRINGID_PKMNSHROUDEDINMIST - 12] = gText_PkmnShroudedInMist,
    [STRINGID_PKMNPROTECTEDBYMIST - 12] = sText_PkmnProtectedByMist,
    [STRINGID_PKMNGETTINGPUMPED - 12] = gText_PkmnGettingPumped,
    [STRINGID_PKMNHITWITHRECOIL - 12] = sText_PkmnHitWithRecoil,
    [STRINGID_PKMNPROTECTEDITSELF2 - 12] = sText_PkmnProtectedItself2,
    [STRINGID_PKMNBUFFETEDBYSANDSTORM - 12] = sText_PkmnBuffetedBySandstorm,
    [STRINGID_PKMNPELTEDBYHAIL - 12] = sText_PkmnPeltedByHail,
    [STRINGID_PKMNSEEDED - 12] = sText_PkmnSeeded,
    [STRINGID_PKMNEVADEDATTACK - 12] = sText_PkmnEvadedAttack,
    [STRINGID_PKMNSAPPEDBYLEECHSEED - 12] = sText_PkmnSappedByLeechSeed,
    [STRINGID_PKMNFASTASLEEP - 12] = sText_PkmnFastAsleep,
    [STRINGID_PKMNWOKEUP - 12] = sText_PkmnWokeUp,
    [STRINGID_PKMNUPROARKEPTAWAKE - 12] = sText_PkmnUproarKeptAwake,
    [STRINGID_PKMNWOKEUPINUPROAR - 12] = sText_PkmnWokeUpInUproar,
    [STRINGID_PKMNCAUSEDUPROAR - 12] = sText_PkmnCausedUproar,
    [STRINGID_PKMNMAKINGUPROAR - 12] = sText_PkmnMakingUproar,
    [STRINGID_PKMNCALMEDDOWN - 12] = sText_PkmnCalmedDown,
    [STRINGID_PKMNCANTSLEEPINUPROAR - 12] = sText_PkmnCantSleepInUproar,
    [STRINGID_PKMNSTOCKPILED - 12] = sText_PkmnStockpiled,
    [STRINGID_PKMNCANTSTOCKPILE - 12] = sText_PkmnCantStockpile,
    [STRINGID_PKMNCANTSLEEPINUPROAR2 - 12] = sText_PkmnCantSleepInUproar2,
    [STRINGID_UPROARKEPTPKMNAWAKE - 12] = sText_UproarKeptPkmnAwake,
    [STRINGID_PKMNSTAYEDAWAKEUSING - 12] = sText_PkmnStayedAwakeUsing,
    [STRINGID_PKMNSTORINGENERGY - 12] = sText_PkmnStoringEnergy,
    [STRINGID_PKMNUNLEASHEDENERGY - 12] = sText_PkmnUnleashedEnergy,
    [STRINGID_PKMNFATIGUECONFUSION - 12] = sText_PkmnFatigueConfusion,
    [STRINGID_PLAYERPICKEDUPMONEY - 12] = sText_PlayerPickedUpMoney,
    [STRINGID_PKMNUNAFFECTED - 12] = sText_PkmnUnaffected,
    [STRINGID_PKMNTRANSFORMEDINTO - 12] = sText_PkmnTransformedInto,
    [STRINGID_PKMNMADESUBSTITUTE - 12] = sText_PkmnMadeSubstitute,
    [STRINGID_PKMNHASSUBSTITUTE - 12] = sText_PkmnHasSubstitute,
    [STRINGID_SUBSTITUTEDAMAGED - 12] = sText_SubstituteDamaged,
    [STRINGID_PKMNSUBSTITUTEFADED - 12] = sText_PkmnSubstituteFaded,
    [STRINGID_PKMNMUSTRECHARGE - 12] = sText_PkmnMustRecharge,
    [STRINGID_PKMNRAGEBUILDING - 12] = sText_PkmnRageBuilding,
    [STRINGID_PKMNMOVEWASDISABLED - 12] = sText_PkmnMoveWasDisabled,
    [STRINGID_PKMNMOVEISDISABLED - 12] = sText_PkmnMoveIsDisabled,
    [STRINGID_PKMNMOVEDISABLEDNOMORE - 12] = sText_PkmnMoveDisabledNoMore,
    [STRINGID_PKMNGOTENCORE - 12] = sText_PkmnGotEncore,
    [STRINGID_PKMNENCOREENDED - 12] = sText_PkmnEncoreEnded,
    [STRINGID_PKMNTOOKAIM - 12] = sText_PkmnTookAim,
    [STRINGID_PKMNSKETCHEDMOVE - 12] = sText_PkmnSketchedMove,
    [STRINGID_PKMNTRYINGTOTAKEFOE - 12] = sText_PkmnTryingToTakeFoe,
    [STRINGID_PKMNTOOKFOE - 12] = sText_PkmnTookFoe,
    [STRINGID_PKMNREDUCEDPP - 12] = sText_PkmnReducedPP,
    [STRINGID_PKMNSTOLEITEM - 12] = sText_PkmnStoleItem,
    [STRINGID_TARGETCANTESCAPENOW - 12] = sText_TargetCantEscapeNow,
    [STRINGID_PKMNFELLINTONIGHTMARE - 12] = sText_PkmnFellIntoNightmare,
    [STRINGID_PKMNLOCKEDINNIGHTMARE - 12] = sText_PkmnLockedInNightmare,
    [STRINGID_PKMNLAIDCURSE - 12] = sText_PkmnLaidCurse,
    [STRINGID_PKMNAFFLICTEDBYCURSE - 12] = sText_PkmnAfflictedByCurse,
    [STRINGID_SPIKESSCATTERED - 12] = sText_SpikesScattered,
    [STRINGID_PKMNHURTBYSPIKES - 12] = sText_PkmnHurtBySpikes,
    [STRINGID_PKMNIDENTIFIED - 12] = sText_PkmnIdentified,
    [STRINGID_PKMNPERISHCOUNTFELL - 12] = sText_PkmnPerishCountFell,
    [STRINGID_PKMNBRACEDITSELF - 12] = sText_PkmnBracedItself,
    [STRINGID_PKMNENDUREDHIT - 12] = sText_PkmnEnduredHit,
    [STRINGID_MAGNITUDESTRENGTH - 12] = sText_MagnitudeStrength,
    [STRINGID_PKMNCUTHPMAXEDATTACK - 12] = sText_PkmnCutHPMaxedAttack,
    [STRINGID_PKMNCOPIEDSTATCHANGES - 12] = sText_PkmnCopiedStatChanges,
    [STRINGID_PKMNGOTFREE - 12] = sText_PkmnGotFree,
    [STRINGID_PKMNSHEDLEECHSEED - 12] = sText_PkmnShedLeechSeed,
    [STRINGID_PKMNBLEWAWAYSPIKES - 12] = sText_PkmnBlewAwaySpikes,
    [STRINGID_PKMNFLEDFROMBATTLE - 12] = sText_PkmnFledFromBattle,
    [STRINGID_PKMNFORESAWATTACK - 12] = sText_PkmnForesawAttack,
    [STRINGID_PKMNTOOKATTACK - 12] = sText_PkmnTookAttack,
    [STRINGID_PKMNATTACK - 12] = sText_PkmnAttack,
    [STRINGID_PKMNCENTERATTENTION - 12] = sText_PkmnCenterAttention,
    [STRINGID_PKMNCHARGINGPOWER - 12] = sText_PkmnChargingPower,
    [STRINGID_NATUREPOWERTURNEDINTO - 12] = sText_NaturePowerTurnedInto,
    [STRINGID_PKMNSTATUSNORMAL - 12] = sText_PkmnStatusNormal,
    [STRINGID_PKMNHASNOMOVESLEFT - 12] = sText_PkmnHasNoMovesLeft,
    [STRINGID_PKMNSUBJECTEDTOTORMENT - 12] = sText_PkmnSubjectedToTorment,
    [STRINGID_PKMNCANTUSEMOVETORMENT - 12] = sText_PkmnCantUseMoveTorment,
    [STRINGID_PKMNTIGHTENINGFOCUS - 12] = sText_PkmnTighteningFocus,
    [STRINGID_PKMNFELLFORTAUNT - 12] = sText_PkmnFellForTaunt,
    [STRINGID_PKMNCANTUSEMOVETAUNT - 12] = sText_PkmnCantUseMoveTaunt,
    [STRINGID_PKMNREADYTOHELP - 12] = sText_PkmnReadyToHelp,
    [STRINGID_PKMNSWITCHEDITEMS - 12] = sText_PkmnSwitchedItems,
    [STRINGID_PKMNCOPIEDFOE - 12] = sText_PkmnCopiedFoe,
    [STRINGID_PKMNMADEWISH - 12] = sText_PkmnMadeWish,
    [STRINGID_PKMNWISHCAMETRUE - 12] = sText_PkmnWishCameTrue,
    [STRINGID_PKMNPLANTEDROOTS - 12] = sText_PkmnPlantedRoots,
    [STRINGID_PKMNABSORBEDNUTRIENTS - 12] = sText_PkmnAbsorbedNutrients,
    [STRINGID_PKMNANCHOREDITSELF - 12] = sText_PkmnAnchoredItself,
    [STRINGID_PKMNWASMADEDROWSY - 12] = sText_PkmnWasMadeDrowsy,
    [STRINGID_PKMNKNOCKEDOFF - 12] = sText_PkmnKnockedOff,
    [STRINGID_PKMNSWAPPEDABILITIES - 12] = sText_PkmnSwappedAbilities,
    [STRINGID_PKMNSEALEDOPPONENTMOVE - 12] = sText_PkmnSealedOpponentMove,
    [STRINGID_PKMNCANTUSEMOVESEALED - 12] = sText_PkmnCantUseMoveSealed,
    [STRINGID_PKMNWANTSGRUDGE - 12] = sText_PkmnWantsGrudge,
    [STRINGID_PKMNLOSTPPGRUDGE - 12] = sText_PkmnLostPPGrudge,
    [STRINGID_PKMNSHROUDEDITSELF - 12] = sText_PkmnShroudedItself,
    [STRINGID_PKMNMOVEBOUNCED - 12] = sText_PkmnMoveBounced,
    [STRINGID_PKMNWAITSFORTARGET - 12] = sText_PkmnWaitsForTarget,
    [STRINGID_PKMNSNATCHEDMOVE - 12] = sText_PkmnSnatchedMove,
    [STRINGID_PKMNMADEITRAIN - 12] = sText_PkmnMadeItRain,
    [STRINGID_PKMNRAISEDSPEED - 12] = sText_PkmnRaisedSpeed,
    [STRINGID_PKMNPROTECTEDBY - 12] = sText_PkmnProtectedBy,
    [STRINGID_PKMNPREVENTSUSAGE - 12] = sText_PkmnPreventsUsage,
    [STRINGID_PKMNRESTOREDHPUSING - 12] = sText_PkmnRestoredHPUsing,
    [STRINGID_PKMNCHANGEDTYPEWITH - 12] = sText_PkmnChangedTypeWith,
    [STRINGID_PKMNPREVENTSPARALYSISWITH - 12] = sText_PkmnPreventsParalysisWith,
    [STRINGID_PKMNPREVENTSROMANCEWITH - 12] = sText_PkmnPreventsRomanceWith,
    [STRINGID_PKMNPREVENTSPOISONINGWITH - 12] = sText_PkmnPreventsPoisoningWith,
    [STRINGID_PKMNPREVENTSCONFUSIONWITH - 12] = sText_PkmnPreventsConfusionWith,
    [STRINGID_PKMNRAISEDFIREPOWERWITH - 12] = sText_PkmnRaisedFirePowerWith,
    [STRINGID_PKMNANCHORSITSELFWITH - 12] = sText_PkmnAnchorsItselfWith,
    [STRINGID_PKMNCUTSATTACKWITH - 12] = sText_PkmnCutsAttackWith,
    [STRINGID_PKMNPREVENTSSTATLOSSWITH - 12] = sText_PkmnPreventsStatLossWith,
    [STRINGID_PKMNHURTSWITH - 12] = sText_PkmnHurtsWith,
    [STRINGID_PKMNTRACED - 12] = sText_PkmnTraced,
    [STRINGID_STATSHARPLY - 12] = gText_StatSharply,
    [STRINGID_STATROSE - 12] = gText_StatRose,
    [STRINGID_STATHARSHLY - 12] = sText_StatHarshly,
    [STRINGID_STATFELL - 12] = sText_StatFell,
    [STRINGID_ATTACKERSSTATROSE - 12] = sText_AttackersStatRose,
    [STRINGID_DEFENDERSSTATROSE - 12] = gText_DefendersStatRose,
    [STRINGID_ATTACKERSSTATFELL - 12] = sText_AttackersStatFell,
    [STRINGID_DEFENDERSSTATFELL - 12] = sText_DefendersStatFell,
    [STRINGID_CRITICALHIT - 12] = sText_CriticalHit,
    [STRINGID_ONEHITKO - 12] = sText_OneHitKO,
    [STRINGID_123POOF - 12] = sText_123Poof,
    [STRINGID_ANDELLIPSIS - 12] = sText_AndEllipsis,
    [STRINGID_NOTVERYEFFECTIVE - 12] = sText_NotVeryEffective,
    [STRINGID_SUPEREFFECTIVE - 12] = sText_SuperEffective,
    [STRINGID_GOTAWAYSAFELY - 12] = sText_GotAwaySafely,
    [STRINGID_WILDPKMNFLED - 12] = sText_WildPkmnFled,
    [STRINGID_NORUNNINGFROMTRAINERS - 12] = sText_NoRunningFromTrainers,
    [STRINGID_CANTESCAPE - 12] = sText_CantEscape,
    [STRINGID_DONTLEAVEBIRCH - 12] = sText_DontLeaveBirch,
    [STRINGID_BUTNOTHINGHAPPENED - 12] = sText_ButNothingHappened,
    [STRINGID_BUTITFAILED - 12] = sText_ButItFailed,
    [STRINGID_ITHURTCONFUSION - 12] = sText_ItHurtConfusion,
    [STRINGID_MIRRORMOVEFAILED - 12] = sText_MirrorMoveFailed,
    [STRINGID_STARTEDTORAIN - 12] = sText_StartedToRain,
    [STRINGID_DOWNPOURSTARTED - 12] = sText_DownpourStarted,
    [STRINGID_RAINCONTINUES - 12] = sText_RainContinues,
    [STRINGID_DOWNPOURCONTINUES - 12] = sText_DownpourContinues,
    [STRINGID_RAINSTOPPED - 12] = sText_RainStopped,
    [STRINGID_SANDSTORMBREWED - 12] = sText_SandstormBrewed,
    [STRINGID_SANDSTORMRAGES - 12] = sText_SandstormRages,
    [STRINGID_SANDSTORMSUBSIDED - 12] = sText_SandstormSubsided,
    [STRINGID_SUNLIGHTGOTBRIGHT - 12] = sText_SunlightGotBright,
    [STRINGID_SUNLIGHTSTRONG - 12] = sText_SunlightStrong,
    [STRINGID_SUNLIGHTFADED - 12] = sText_SunlightFaded,
    [STRINGID_STARTEDHAIL - 12] = sText_StartedHail,
    [STRINGID_HAILCONTINUES - 12] = sText_HailContinues,
    [STRINGID_HAILSTOPPED - 12] = sText_HailStopped,
    [STRINGID_FAILEDTOSPITUP - 12] = sText_FailedToSpitUp,
    [STRINGID_FAILEDTOSWALLOW - 12] = sText_FailedToSwallow,
    [STRINGID_WINDBECAMEHEATWAVE - 12] = sText_WindBecameHeatWave,
    [STRINGID_STATCHANGESGONE - 12] = sText_StatChangesGone,
    [STRINGID_COINSSCATTERED - 12] = sText_CoinsScattered,
    [STRINGID_TOOWEAKFORSUBSTITUTE - 12] = sText_TooWeakForSubstitute,
    [STRINGID_SHAREDPAIN - 12] = sText_SharedPain,
    [STRINGID_BELLCHIMED - 12] = sText_BellChimed,
    [STRINGID_FAINTINTHREE - 12] = sText_FaintInThree,
    [STRINGID_NOPPLEFT - 12] = sText_NoPPLeft,
    [STRINGID_BUTNOPPLEFT - 12] = sText_ButNoPPLeft,
    [STRINGID_PLAYERUSEDITEM - 12] = sText_PlayerUsedItem,
    [STRINGID_WALLYUSEDITEM - 12] = sText_WallyUsedItem,
    [STRINGID_TRAINERBLOCKEDBALL - 12] = sText_TrainerBlockedBall,
    [STRINGID_DONTBEATHIEF - 12] = sText_DontBeAThief,
    [STRINGID_ITDODGEDBALL - 12] = sText_ItDodgedBall,
    [STRINGID_YOUMISSEDPKMN - 12] = sText_YouMissedPkmn,
    [STRINGID_PKMNBROKEFREE - 12] = sText_PkmnBrokeFree,
    [STRINGID_ITAPPEAREDCAUGHT - 12] = sText_ItAppearedCaught,
    [STRINGID_AARGHALMOSTHADIT - 12] = sText_AarghAlmostHadIt,
    [STRINGID_SHOOTSOCLOSE - 12] = sText_ShootSoClose,
    [STRINGID_GOTCHAPKMNCAUGHT - 12] = sText_GotchaPkmnCaught,
    [STRINGID_GOTCHAPKMNCAUGHT2 - 12] = sText_GotchaPkmnCaught2,
    [STRINGID_GIVENICKNAMECAPTURED - 12] = sText_GiveNicknameCaptured,
    [STRINGID_PKMNSENTTOPC - 12] = sText_PkmnSentToPC,
    [STRINGID_PKMNDATAADDEDTODEX - 12] = sText_PkmnDataAddedToDex,
    [STRINGID_ITISRAINING - 12] = sText_ItIsRaining,
    [STRINGID_SANDSTORMISRAGING - 12] = sText_SandstormIsRaging,
    [STRINGID_CANTESCAPE2 - 12] = sText_CantEscape2,
    [STRINGID_PKMNIGNORESASLEEP - 12] = sText_PkmnIgnoresAsleep,
    [STRINGID_PKMNIGNOREDORDERS - 12] = sText_PkmnIgnoredOrders,
    [STRINGID_PKMNBEGANTONAP - 12] = sText_PkmnBeganToNap,
    [STRINGID_PKMNLOAFING - 12] = sText_PkmnLoafing,
    [STRINGID_PKMNWONTOBEY - 12] = sText_PkmnWontObey,
    [STRINGID_PKMNTURNEDAWAY - 12] = sText_PkmnTurnedAway,
    [STRINGID_PKMNPRETENDNOTNOTICE - 12] = sText_PkmnPretendNotNotice,
    [STRINGID_ENEMYABOUTTOSWITCHPKMN - 12] = sText_EnemyAboutToSwitchPkmn,
    [STRINGID_CREPTCLOSER - 12] = sText_CreptCloser,
    [STRINGID_CANTGETCLOSER - 12] = sText_CantGetCloser,
    [STRINGID_PKMNWATCHINGCAREFULLY - 12] = sText_PkmnWatchingCarefully,
    [STRINGID_PKMNCURIOUSABOUTX - 12] = sText_PkmnCuriousAboutX,
    [STRINGID_PKMNENTHRALLEDBYX - 12] = sText_PkmnEnthralledByX,
    [STRINGID_PKMNIGNOREDX - 12] = sText_PkmnIgnoredX,
    [STRINGID_THREWPOKEBLOCKATPKMN - 12] = sText_ThrewPokeblockAtPkmn,
    [STRINGID_OUTOFSAFARIBALLS - 12] = sText_OutOfSafariBalls,
    [STRINGID_PKMNSITEMCUREDPARALYSIS - 12] = sText_PkmnsItemCuredParalysis,
    [STRINGID_PKMNSITEMCUREDPOISON - 12] = sText_PkmnsItemCuredPoison,
    [STRINGID_PKMNSITEMHEALEDBURN - 12] = sText_PkmnsItemHealedBurn,
    [STRINGID_PKMNSITEMDEFROSTEDIT - 12] = sText_PkmnsItemDefrostedIt,
    [STRINGID_PKMNSITEMWOKEIT - 12] = sText_PkmnsItemWokeIt,
    [STRINGID_PKMNSITEMSNAPPEDOUT - 12] = sText_PkmnsItemSnappedOut,
    [STRINGID_PKMNSITEMCUREDPROBLEM - 12] = sText_PkmnsItemCuredProblem,
    [STRINGID_PKMNSITEMRESTOREDHEALTH - 12] = sText_PkmnsItemRestoredHealth,
    [STRINGID_PKMNSITEMRESTOREDPP - 12] = sText_PkmnsItemRestoredPP,
    [STRINGID_PKMNSITEMRESTOREDSTATUS - 12] = sText_PkmnsItemRestoredStatus,
    [STRINGID_PKMNSITEMRESTOREDHPALITTLE - 12] = sText_PkmnsItemRestoredHPALittle,
    [STRINGID_ITEMALLOWSONLYYMOVE - 12] = sText_ItemAllowsOnlyYMove,
    [STRINGID_PKMNHUNGONWITHX - 12] = sText_PkmnHungOnWithX,
    [STRINGID_EMPTYSTRING3 - 12] = gText_EmptyString3,
    [STRINGID_PKMNSXPREVENTSBURNS - 12] = sText_PkmnsXPreventsBurns,
    [STRINGID_PKMNSXBLOCKSY - 12] = sText_PkmnsXBlocksY,
    [STRINGID_PKMNSXRESTOREDHPALITTLE2 - 12] = sText_PkmnsXRestoredHPALittle2,
    [STRINGID_PKMNSXWHIPPEDUPSANDSTORM - 12] = sText_PkmnsXWhippedUpSandstorm,
    [STRINGID_PKMNSXPREVENTSYLOSS - 12] = sText_PkmnsXPreventsYLoss,
    [STRINGID_PKMNSXINFATUATEDY - 12] = sText_PkmnsXInfatuatedY,
    [STRINGID_PKMNSXMADEYINEFFECTIVE - 12] = sText_PkmnsXMadeYIneffective,
    [STRINGID_PKMNSXCUREDYPROBLEM - 12] = sText_PkmnsXCuredYProblem,
    [STRINGID_ITSUCKEDLIQUIDOOZE - 12] = sText_ItSuckedLiquidOoze,
    [STRINGID_PKMNTRANSFORMED - 12] = sText_PkmnTransformed,
    [STRINGID_ELECTRICITYWEAKENED - 12] = sText_ElectricityWeakened,
    [STRINGID_FIREWEAKENED - 12] = sText_FireWeakened,
    [STRINGID_PKMNHIDUNDERWATER - 12] = sText_PkmnHidUnderwater,
    [STRINGID_PKMNSPRANGUP - 12] = sText_PkmnSprangUp,
    [STRINGID_HMMOVESCANTBEFORGOTTEN - 12] = sText_HMMovesCantBeForgotten,
    [STRINGID_XFOUNDONEY - 12] = sText_XFoundOneY,
    [STRINGID_PLAYERDEFEATEDTRAINER1 - 12] = sText_PlayerDefeatedLinkTrainerTrainer1,
    [STRINGID_SOOTHINGAROMA - 12] = sText_SoothingAroma,
    [STRINGID_ITEMSCANTBEUSEDNOW - 12] = sText_ItemsCantBeUsedNow,
    [STRINGID_FORXCOMMAYZ - 12] = sText_ForXCommaYZ,
    [STRINGID_USINGITEMSTATOFPKMNROSE - 12] = sText_UsingItemTheStatOfPkmnRose,
    [STRINGID_PKMNUSEDXTOGETPUMPED - 12] = sText_PkmnUsedXToGetPumped,
    [STRINGID_PKMNSXMADEYUSELESS - 12] = sText_PkmnsXMadeYUseless,
    [STRINGID_PKMNTRAPPEDBYSANDTOMB - 12] = sText_PkmnTrappedBySandTomb,
    [STRINGID_EMPTYSTRING4 - 12] = sText_EmptyString4,
    [STRINGID_ABOOSTED - 12] = sText_ABoosted,
    [STRINGID_PKMNSXINTENSIFIEDSUN - 12] = sText_PkmnsXIntensifiedSun,
    [STRINGID_PKMNMAKESGROUNDMISS - 12] = sText_PkmnMakesGroundMiss,
    [STRINGID_YOUTHROWABALLNOWRIGHT - 12] = sText_YouThrowABallNowRight,
    [STRINGID_PKMNSXTOOKATTACK - 12] = sText_PkmnsXTookAttack,
    [STRINGID_PKMNCHOSEXASDESTINY - 12] = sText_PkmnChoseXAsDestiny,
    [STRINGID_PKMNLOSTFOCUS - 12] = sText_PkmnLostFocus,
    [STRINGID_USENEXTPKMN - 12] = sText_UseNextPkmn,
    [STRINGID_PKMNFLEDUSINGITS - 12] = sText_PkmnFledUsingIts,
    [STRINGID_PKMNFLEDUSING - 12] = sText_PkmnFledUsing,
    [STRINGID_PKMNWASDRAGGEDOUT - 12] = sText_PkmnWasDraggedOut,
    [STRINGID_PREVENTEDFROMWORKING - 12] = sText_PreventedFromWorking,
    [STRINGID_PKMNSITEMNORMALIZEDSTATUS - 12] = sText_PkmnsItemNormalizedStatus,
    [STRINGID_TRAINER1USEDITEM - 12] = sText_Trainer1UsedItem,
    [STRINGID_BOXISFULL - 12] = sText_BoxIsFull,
    [STRINGID_PKMNAVOIDEDATTACK - 12] = sText_PkmnAvoidedAttack,
    [STRINGID_PKMNSXMADEITINEFFECTIVE - 12] = sText_PkmnsXMadeItIneffective,
    [STRINGID_PKMNSXPREVENTSFLINCHING - 12] = sText_PkmnsXPreventsFlinching,
    [STRINGID_PKMNALREADYHASBURN - 12] = sText_PkmnAlreadyHasBurn,
    [STRINGID_STATSWONTDECREASE2 - 12] = sText_StatsWontDecrease2,
    [STRINGID_PKMNSXBLOCKSY2 - 12] = sText_PkmnsXBlocksY2,
    [STRINGID_PKMNSXWOREOFF - 12] = sText_PkmnsXWoreOff,
    [STRINGID_PKMNRAISEDDEFALITTLE - 12] = sText_PkmnRaisedDefALittle,
    [STRINGID_PKMNRAISEDSPDEFALITTLE - 12] = sText_PkmnRaisedSpDefALittle,
    [STRINGID_THEWALLSHATTERED - 12] = sText_TheWallShattered,
    [STRINGID_PKMNSXPREVENTSYSZ - 12] = sText_PkmnsXPreventsYsZ,
    [STRINGID_PKMNSXCUREDITSYPROBLEM - 12] = sText_PkmnsXCuredItsYProblem,
    [STRINGID_ATTACKERCANTESCAPE - 12] = sText_AttackerCantEscape,
    [STRINGID_PKMNOBTAINEDX - 12] = sText_PkmnObtainedX,
    [STRINGID_PKMNOBTAINEDX2 - 12] = sText_PkmnObtainedX2,
    [STRINGID_PKMNOBTAINEDXYOBTAINEDZ - 12] = sText_PkmnObtainedXYObtainedZ,
    [STRINGID_BUTNOEFFECT - 12] = sText_ButNoEffect,
    [STRINGID_PKMNSXHADNOEFFECTONY - 12] = sText_PkmnsXHadNoEffectOnY,
    [STRINGID_TWOENEMIESDEFEATED - 12] = sText_TwoInGameTrainersDefeated,
    [STRINGID_TRAINER2LOSETEXT - 12] = sText_Trainer2LoseText,
    [STRINGID_PKMNINCAPABLEOFPOWER - 12] = sText_PkmnIncapableOfPower,
    [STRINGID_GLINTAPPEARSINEYE - 12] = sText_GlintAppearsInEye,
    [STRINGID_PKMNGETTINGINTOPOSITION - 12] = sText_PkmnGettingIntoPosition,
    [STRINGID_PKMNBEGANGROWLINGDEEPLY - 12] = sText_PkmnBeganGrowlingDeeply,
    [STRINGID_PKMNEAGERFORMORE - 12] = sText_PkmnEagerForMore,
    [STRINGID_DEFEATEDOPPONENTBYREFEREE - 12] = sText_DefeatedOpponentByReferee,
    [STRINGID_LOSTTOOPPONENTBYREFEREE - 12] = sText_LostToOpponentByReferee,
    [STRINGID_TIEDOPPONENTBYREFEREE - 12] = sText_TiedOpponentByReferee,
    [STRINGID_QUESTIONFORFEITMATCH - 12] = sText_QuestionForfeitMatch,
    [STRINGID_FORFEITEDMATCH - 12] = sText_ForfeitedMatch,
    [STRINGID_PKMNTRANSFERREDSOMEONESPC - 12] = gText_PkmnTransferredSomeonesPC,
    [STRINGID_PKMNTRANSFERREDLANETTESPC - 12] = gText_PkmnTransferredLanettesPC,
    [STRINGID_PKMNBOXSOMEONESPCFULL - 12] = gText_PkmnTransferredSomeonesPCBoxFull,
    [STRINGID_PKMNBOXLANETTESPCFULL - 12] = gText_PkmnTransferredLanettesPCBoxFull,
    [STRINGID_TRAINER1WINTEXT - 12] = sText_Trainer1WinText,
    [STRINGID_TRAINER2WINTEXT - 12] = sText_Trainer2WinText,
    [STRINGID_ENDUREDSTURDY - 12] = sText_EnduredViaSturdy,
    [STRINGID_POWERHERB - 12] = sText_PowerHerbActivation,
    [STRINGID_HURTBYITEM - 12] = sText_HurtByItem,
    [STRINGID_PSNBYITEM - 12] = sText_BadlyPoisonedByItem,
    [STRINGID_BRNBYITEM - 12] = sText_BurnedByItem,
    [STRINGID_DEFABILITYIN - 12] = sText_TargetAbilityActivates,
    [STRINGID_GRAVITYINTENSIFIED - 12] = sText_GravityIntensified,
    [STRINGID_TARGETIDENTIFIED - 12] = sText_TargetIdentified,
    [STRINGID_TARGETWOKEUP - 12] = sText_TargetWokeUp,
    [STRINGID_PKMNSTOLEANDATEITEM - 12] = sText_PkmnStoleAndAteItem,
    [STRINGID_TAILWINDBLEW - 12] = sText_TailWindBlew,
    [STRINGID_PKMNWENTBACK - 12] = sText_PkmnWentBack,
    [STRINGID_PKMNCANTUSEITEMSANYMORE - 12] = sText_PkmnCantUseItemsAnymore,
    [STRINGID_PKMNFLUNG - 12] = sText_PkmnFlung,
    [STRINGID_PKMNPREVENTEDFROMHEALING - 12] = sText_PkmnPreventedFromHealing,
    [STRINGID_PKMNSWITCHEDATKANDDEF - 12] = sText_PkmnSwitchedAtkAndDef,
    [STRINGID_PKMNSABILITYSUPPRESSED - 12] = sText_PkmnsAbilitySuppressed,
    [STRINGID_SHIELDEDFROMCRITICALHITS - 12] = sText_ShieldedFromCriticalHits,
    [STRINGID_SWITCHEDATKANDSPATK - 12] = sText_SwitchedAtkAndSpAtk,
    [STRINGID_SWITCHEDDEFANDSPDEF - 12] = sText_SwitchedDefAndSpDef,
    [STRINGID_PKMNACQUIREDABILITY - 12] = sText_PkmnAcquiredAbility,
    [STRINGID_POISONSPIKESSCATTERED - 12] = sText_PoisonSpikesScattered,
    [STRINGID_PKMNSWITCHEDSTATCHANGES - 12] = sText_PkmnSwitchedStatChanges,
    [STRINGID_PKMNSURROUNDEDWITHVEILOFWATER - 12] = sText_PkmnSurroundedWithVeilOfWater,
    [STRINGID_PKMNLEVITATEDONELECTROMAGNETISM - 12] = sText_PkmnLevitatedOnElectromagnetism,
    [STRINGID_PKMNTWISTEDDIMENSIONS - 12] = sText_PkmnTwistedDimensions,
    [STRINGID_POINTEDSTONESFLOAT - 12] = sText_PointedStonesFloat,
    [STRINGID_CLOAKEDINMYSTICALMOONLIGHT - 12] = sText_CloakedInMysticalMoonlight,
    [STRINGID_TRAPPEDBYSWIRLINGMAGMA - 12] = sText_TrappedBySwirlingMagma,
    [STRINGID_VANISHEDINSTANTLY - 12] = sText_VanishedInstantly,
    [STRINGID_PROTECTEDTEAM - 12] = sText_ProtectedTeam,
    [STRINGID_SHAREDITSGUARD - 12] = sText_SharedItsGuard,
    [STRINGID_SHAREDITSPOWER - 12] = sText_SharedItsPower,
    [STRINGID_SWAPSDEFANDSPDEFOFALLPOKEMON - 12] = sText_SwapsDefAndSpDefOfAllPkmn,
    [STRINGID_BECAMENIMBLE - 12] = sText_BecameNimble,
    [STRINGID_HURLEDINTOTHEAIR - 12] = sText_HurledIntoTheAir,
    [STRINGID_HELDITEMSLOSEEFFECTS - 12] = sText_HeldItemsLoseEffects,
    [STRINGID_FELLSTRAIGHTDOWN - 12] = sText_FellStraightDown,
    [STRINGID_TARGETCHANGEDTYPE - 12] = sText_TargetChangedType,
    [STRINGID_PKMNACQUIREDSIMPLE - 12] = sText_PkmnAcquiredSimple,
    [STRINGID_KINDOFFER - 12] = sText_KindOffer,
    [STRINGID_RESETSTARGETSSTATLEVELS - 12] = sText_ResetsTargetsStatLevels,
    [STRINGID_ALLYSWITCHPOSITION - 12] = sText_AllySwitchPosition,
    [STRINGID_RESTORETARGETSHEALTH - 12] = sText_RestoreTargetsHealth,
    [STRINGID_TOOKPJMNINTOTHESKY - 12] = sText_TookPkmnIntoTheSky,
    [STRINGID_FREEDFROMSKYDROP - 12] = sText_FreedFromSkyDrop,
    [STRINGID_POSTPONETARGETMOVE - 12] = sText_PostponeTargetMove,
    [STRINGID_REFLECTTARGETSTYPE - 12] = sText_ReflectTargetsType,
    [STRINGID_TRANSFERHELDITEM - 12] = sText_TransferHeldItem,
    [STRINGID_EMBARGOENDS - 12] = sText_EmbargoEnds,
    [STRINGID_ELECTROMAGNETISM - 12] = sText_Electromagnetism,
    [STRINGID_BUFFERENDS - 12] = sText_BufferEnds,
    [STRINGID_TELEKINESISENDS - 12] = sText_TelekinesisEnds,
    [STRINGID_TAILWINDENDS - 12] = sText_TailwindEnds,
    [STRINGID_LUCKYCHANTENDS - 12] = sText_LuckyChantEnds,
    [STRINGID_TRICKROOMENDS - 12] = sText_TrickRoomEnds,
    [STRINGID_WONDERROOMENDS - 12] = sText_WonderRoomEnds,
    [STRINGID_MAGICROOMENDS - 12] = sText_MagicRoomEnds,
    [STRINGID_MUDSPORTENDS - 12] = sText_MudSportEnds,
    [STRINGID_WATERSPORTENDS - 12] = sText_WaterSportEnds,
    [STRINGID_GRAVITYENDS - 12] = sText_GravityEnds,
    [STRINGID_AQUARINGHEAL - 12] = sText_AquaRingHeal,
    [STRINGID_AURORAVEILENDS - 12] = sText_AuroraVeilEnds,
    [STRINGID_ELECTRICTERRAINENDS - 12] = sText_ElectricTerrainEnds,
    [STRINGID_MISTYTERRAINENDS - 12] = sText_MistyTerrainEnds,
    [STRINGID_PSYCHICTERRAINENDS - 12] = sText_PsychicTerrainEnds,
    [STRINGID_GRASSYTERRAINENDS - 12] = sText_GrassyTerrainEnds,
    [STRINGID_TARGETABILITYSTATRAISE - 12] = sText_TargetAbilityRaisedStat,
    [STRINGID_TARGETSSTATWASMAXEDOUT - 12] = sText_TargetsStatWasMaxedOut,
    [STRINGID_ATTACKERABILITYSTATRAISE - 12] = sText_AttackerAbilityRaisedStat,
    [STRINGID_POISONHEALHPUP - 12] = sText_PoisonHealHpUp,
    [STRINGID_BADDREAMSDMG - 12] = sText_BadDreamsDmg,
    [STRINGID_MOLDBREAKERENTERS - 12] = sText_MoldBreakerEnters,
    [STRINGID_TERAVOLTENTERS - 12] = sText_TeravoltEnters,
    [STRINGID_TURBOBLAZEENTERS - 12] = sText_TurboblazeEnters,
    [STRINGID_SLOWSTARTENTERS - 12] = sText_SlowStartEnters,
    [STRINGID_SLOWSTARTEND - 12] = sText_SlowStartEnd,
    [STRINGID_SOLARPOWERHPDROP - 12] = sText_SolarPowerHpDrop,
    [STRINGID_AFTERMATHDMG - 12] = sText_AftermathDmg,
    [STRINGID_ANTICIPATIONACTIVATES - 12] = sText_AnticipationActivates,
    [STRINGID_FOREWARNACTIVATES - 12] = sText_ForewarnActivates,
    [STRINGID_ICEBODYHPGAIN - 12] = sText_IceBodyHpGain,
    [STRINGID_SNOWWARNINGHAIL - 12] = sText_SnowWarningHail,
    [STRINGID_FRISKACTIVATES - 12] = sText_FriskActivates,
    [STRINGID_UNNERVEENTERS - 12] = sText_UnnerveEnters,
    [STRINGID_HARVESTBERRY - 12] = sText_HarvestBerry,
    [STRINGID_LASTABILITYRAISEDSTAT - 12] = sText_LastAbilityRaisedBuff1,
    [STRINGID_MAGICBOUNCEACTIVATES - 12] = sText_MagicBounceActivates,
    [STRINGID_PROTEANTYPECHANGE - 12] = sText_ProteanTypeChange,
    [STRINGID_SYMBIOSISITEMPASS - 12] = sText_SymbiosisItemPass,
    [STRINGID_STEALTHROCKDMG - 12] = sText_StealthRockDmg,
    [STRINGID_TOXICSPIKESABSORBED - 12] = sText_ToxicSpikesAbsorbed,
    [STRINGID_TOXICSPIKESPOISONED - 12] = sText_ToxicSpikesPoisoned,
    [STRINGID_STICKYWEBSWITCHIN - 12] = sText_StickyWebSwitchIn,
    [STRINGID_HEALINGWISHCAMETRUE - 12] = sText_HealingWishCameTrue,
    [STRINGID_HEALINGWISHHEALED - 12] = sText_HealingWishHealed,
    [STRINGID_LUNARDANCECAMETRUE - 12] = sText_LunarDanceCameTrue,
    [STRINGID_CUSEDBODYDISABLED - 12] = sText_CursedBodyDisabled,
    [STRINGID_ATTACKERACQUIREDABILITY - 12] = sText_AttackerAquiredAbility,
    [STRINGID_TARGETABILITYSTATLOWER - 12] = sText_TargetAbilityLoweredStat,
    [STRINGID_TARGETSTATWONTGOHIGHER - 12] = sText_TargetStatWontGoHigher,
    [STRINGID_PKMNMOVEBOUNCEDABILITY - 12] = sText_PkmnMoveBouncedViaAbility,
    [STRINGID_IMPOSTERTRANSFORM - 12] = sText_ImposterTransform,
    [STRINGID_ASSAULTVESTDOESNTALLOW - 12] = sText_AssaultVestDoesntAllow,
    [STRINGID_GRAVITYPREVENTSUSAGE - 12] = sText_GravityPreventsUsage,
    [STRINGID_HEALBLOCKPREVENTSUSAGE - 12] = sText_HealBlockPreventsUsage,
    [STRINGID_NOTDONEYET - 12] = sText_NotDoneYet,
    [STRINGID_STICKYWEBUSED - 12] = sText_StickyWebUsed,
    [STRINGID_QUASHSUCCESS - 12] = sText_QuashSuccess,
    [STRINGID_PKMNBLEWAWAYTOXICSPIKES - 12] = sText_PkmnBlewAwayToxicSpikes,
    [STRINGID_PKMNBLEWAWAYSTICKYWEB - 12] = sText_PkmnBlewAwayStickyWeb,
    [STRINGID_PKMNBLEWAWAYSTEALTHROCK - 12] = sText_PkmnBlewAwayStealthRock,
    [STRINGID_IONDELUGEON - 12] = sText_IonDelugeOn,
    [STRINGID_TOPSYTURVYSWITCHEDSTATS - 12] = sText_TopsyTurvySwitchedStats,
    [STRINGID_TERRAINBECOMESMISTY - 12] = sText_TerrainBecomesMisty,
    [STRINGID_TERRAINBECOMESGRASSY - 12] = sText_TerrainBecomesGrassy,
    [STRINGID_TERRAINBECOMESELECTRIC - 12] = sText_TerrainBecomesElectric,
    [STRINGID_TERRAINBECOMESPSYCHIC - 12] = sText_TerrainBecomesPsychic,
    [STRINGID_TARGETELECTRIFIED - 12] = sText_TargetElectrified,
    [STRINGID_MEGAEVOREACTING - 12] = sText_MegaEvoReacting,
    [STRINGID_MEGAEVOEVOLVED - 12] = sText_MegaEvoEvolved,
    [STRINGID_DRASTICALLY - 12] = sText_drastically,
    [STRINGID_SEVERELY - 12] = sText_severely,
    [STRINGID_INFESTATION - 12] = sText_Infestation,
    [STRINGID_NOEFFECTONTARGET - 12] = sText_NoEffectOnTarget,
    [STRINGID_BURSTINGFLAMESHIT - 12] = sText_BurstingFlames,
    [STRINGID_BESTOWITEMGIVING - 12] = sText_BestowItemGiving,
    [STRINGID_THIRDTYPEADDED - 12] = sText_ThirdTypeAdded,
    [STRINGID_FELLFORFEINT - 12] = sText_FellForFeint,
    [STRINGID_POKEMONCANNOTUSEMOVE - 12] = sText_PokemonCannotUseMove,
    [STRINGID_COVEREDINPOWDER - 12] = sText_CoveredInPowder,
    [STRINGID_POWDEREXPLODES - 12] = sText_PowderExplodes,
    [STRINGID_BELCHCANTSELECT - 12] = sText_BelchCantUse,
    [STRINGID_SPECTRALTHIEFSTEAL - 12] = sText_SpectralThiefSteal,
    [STRINGID_GRAVITYGROUNDING - 12] = sText_GravityGrounding,
    [STRINGID_MISTYTERRAINPREVENTS - 12] = sText_MistyTerrainPreventsStatus,
    [STRINGID_GRASSYTERRAINHEALS - 12] = sText_GrassyTerrainHeals,
    [STRINGID_ELECTRICTERRAINPREVENTS - 12] = sText_ElectricTerrainPreventsSleep,
    [STRINGID_PSYCHICTERRAINPREVENTS - 12] = sText_PsychicTerrainPreventsPriority,
    [STRINGID_SAFETYGOGGLESPROTECTED - 12] = sText_SafetyGogglesProtected,
    [STRINGID_FLOWERVEILPROTECTED - 12] = sText_FlowerVeilProtected,
    [STRINGID_SWEETVEILPROTECTED - 12] = sText_SweetVeilProtected,
    [STRINGID_AROMAVEILPROTECTED - 12] = sText_AromaVeilProtected,
    [STRINGID_CELEBRATEMESSAGE - 12] = sText_CelebrateMessage,
    [STRINGID_USEDINSTRUCTEDMOVE - 12] = sText_UsedInstructedMove,
    [STRINGID_THROATCHOPENDS - 12] = sText_ThroatChopEnds,
    [STRINGID_PKMNCANTUSEMOVETHROATCHOP - 12] = sText_PkmnCantUseMoveThroatChop,
    [STRINGID_LASERFOCUS - 12] = sText_LaserFocusMessage,
    [STRINGID_GEMACTIVATES - 12] = sText_GemActivates,
    [STRINGID_BERRYDMGREDUCES - 12] = sText_BerryDmgReducing,
    [STRINGID_TARGETATEITEM - 12] = sText_TargetAteItem,
    [STRINGID_AIRBALLOONFLOAT - 12] = sText_AirBalloonFloat,
    [STRINGID_AIRBALLOONPOP - 12] = sText_AirBalloonPop,
    [STRINGID_INCINERATEBURN - 12] = sText_IncinerateBurn,
    [STRINGID_BUGBITE - 12] = sText_BugBite,
    [STRINGID_ILLUSIONWOREOFF - 12] = sText_IllusionWoreOff,
    [STRINGID_ATTACKERCUREDTARGETSTATUS - 12] = sText_AttackerCuredTargetStatus,
    [STRINGID_ATTACKERLOSTFIRETYPE - 12] = sText_AttackerLostFireType,
    [STRINGID_HEALERCURE - 12] = sText_HealerCure,
    [STRINGID_SCRIPTINGABILITYSTATRAISE - 12] = sText_ScriptingAbilityRaisedStat,
    [STRINGID_RECEIVERABILITYTAKEOVER - 12] = sText_ReceiverAbilityTakeOver,
    [STRINGID_PKNMABSORBINGPOWER - 12] = sText_PkmnAbsorbingPower,
    [STRINGID_NOONEWILLBEABLETORUNAWAY - 12] = sText_NoOneWillBeAbleToRun,
    [STRINGID_DESTINYKNOTACTIVATES - 12] = sText_DestinyKnotActivates,
    [STRINGID_CLOAKEDINAFREEZINGLIGHT - 12] = sText_CloakedInAFreezingLight,
    [STRINGID_STATWASNOTLOWERED - 12] = sText_StatWasNotLowered,
    [STRINGID_FERVENTWISHREACHED - 12] = sText_FerventWishReached,
    [STRINGID_AIRLOCKACTIVATES - 12] = sText_AirLockActivates,
    [STRINGID_PRESSUREENTERS - 12] = sText_PressureActivates,
    [STRINGID_DARKAURAENTERS - 12] = sText_DarkAuraActivates,
    [STRINGID_FAIRYAURAENTERS - 12] = sText_FairyAuraActivates,
    [STRINGID_AURABREAKENTERS - 12] = sText_AuraBreakActivates,
    [STRINGID_COMATOSEENTERS - 12] = sText_ComatoseActivates,
    [STRINGID_SCREENCLEANERENTERS - 12] = sText_ScreenCleanerActivates,
    [STRINGID_FETCHEDPOKEBALL - 12] = sText_FetchedPokeBall,
    [STRINGID_BATTLERABILITYRAISEDSTAT - 12] = sText_BattlerAbilityRaisedStat,
    [STRINGID_ASANDSTORMKICKEDUP - 12] = sText_ASandstormKickedUp,
    [STRINGID_PKMNSWILLPERISHIN3TURNS - 12] = sText_PkmnsWillPerishIn3Turns,
    [STRINGID_ABILITYRAISEDSTAT - 12] = sText_AbilityRaisedStat,
    [STRINGID_AURAFLAREDTOLIFE - 12] = sText_AuraFlaredToLife,
    [STRINGID_ASONEENTERS - 12] = sText_AsOneEnters,
    [STRINGID_CURIOUSMEDICINEENTERS - 12] = sText_CuriousMedicineEnters,
    [STRINGID_CANACTFASTERTHANKSTO - 12] = sText_CanActFaster,
    [STRINGID_MICLEBERRYACTIVATES - 12] = sText_MicleBerryActivates,
    [STRINGID_PKMNSHOOKOFFTHETAUNT - 12] = sText_PkmnShookOffTheTaunt,
    [STRINGID_PKMNGOTOVERITSINFATUATION - 12] = sText_PkmnGotOverItsInfatuation,
    [STRINGID_ITEMCANNOTBEREMOVED - 12] = sText_ItemCannotBeRemoved,
    [STRINGID_STICKYBARBTRANSFER - 12] = sText_StickyBarbTransfer,
    [STRINGID_PKMNBURNHEALED - 12] = sText_PkmnBurnHealed,
    [STRINGID_REDCARDACTIVATE - 12] = sText_RedCardActivate,
    [STRINGID_EJECTBUTTONACTIVATE - 12] = sText_EjectButtonActivate,
    [STRINGID_ATKGOTOVERINFATUATION - 12] = sText_AttackerGotOverInfatuation,
    [STRINGID_TORMENTEDNOMORE - 12] = sText_TormentedNoMore,
    [STRINGID_HEALBLOCKEDNOMORE - 12] = sText_HealBlockedNoMore,
    [STRINGID_ATTACKERBECAMEFULLYCHARGED - 12] = sText_AttackerBecameFullyCharged,
    [STRINGID_ATTACKERBECAMEASHSPECIES - 12] = sText_AttackerBecameAshSpecies,
    [STRINGID_EXTREMELYHARSHSUNLIGHT - 12] = sText_ExtremelyHarshSunlight,
    [STRINGID_EXTREMESUNLIGHTFADED - 12] = sText_ExtremeSunlightFaded,
    [STRINGID_MOVEEVAPORATEDINTHEHARSHSUNLIGHT - 12] = sText_MoveEvaporatedInTheHarshSunlight,
    [STRINGID_EXTREMELYHARSHSUNLIGHTWASNOTLESSENED - 12] = sText_ExtremelyHarshSunlightWasNotLessened,
    [STRINGID_HEAVYRAIN - 12] = sText_HeavyRain,
    [STRINGID_HEAVYRAINLIFTED - 12] = sText_HeavyRainLifted,
    [STRINGID_MOVEFIZZLEDOUTINTHEHEAVYRAIN - 12] = sText_MoveFizzledOutInTheHeavyRain,
    [STRINGID_NORELIEFROMHEAVYRAIN - 12] = sText_NoReliefFromHeavyRain,
    [STRINGID_MYSTERIOUSAIRCURRENT - 12] = sText_MysteriousAirCurrent,
    [STRINGID_STRONGWINDSDISSIPATED - 12] = sText_StrongWindsDissipated,
    [STRINGID_MYSTERIOUSAIRCURRENTBLOWSON - 12] = sText_MysteriousAirCurrentBlowsOn,
    [STRINGID_ATTACKWEAKENEDBSTRONGWINDS - 12] = sText_AttackWeakenedByStrongWinds,
    [STRINGID_STUFFCHEEKSCANTSELECT - 12] = sText_StuffCheeksCantSelect,
    [STRINGID_PKMNREVERTEDTOPRIMAL - 12] = sText_PkmnRevertedToPrimal,
    [STRINGID_BUTPOKEMONCANTUSETHEMOVE - 12] = sText_ButPokemonCantUseTheMove,
    [STRINGID_BUTHOOPACANTUSEIT - 12] = sText_ButHoopaCantUseIt,
    [STRINGID_BROKETHROUGHPROTECTION - 12] = sText_BrokeThroughProtection,
    [STRINGID_ABILITYALLOWSONLYMOVE - 12] = sText_AbilityAllowsOnlyMove,
    [STRINGID_SWAPPEDABILITIES - 12] = sText_SwappedAbilities,
    [STRINGID_PASTELVEILPROTECTED - 12] = sText_PastelVeilProtected,
    [STRINGID_PASTELVEILENTERS - 12] = sText_PastelVeilEnters,
    [STRINGID_BATTLERTYPECHANGEDTO - 12] = sText_BattlerTypeChangedTo,
    [STRINGID_BOTHCANNOLONGERESCAPE - 12] = sText_BothCanNoLongerEscape,
    [STRINGID_CANTESCAPEDUETOUSEDMOVE - 12] = sText_CantEscapeDueToUsedMove,
    [STRINGID_PKMNBECAMEWEAKERTOFIRE - 12] = sText_PkmnBecameWeakerToFire,
    [STRINGID_ABOUTTOUSEPOLTERGEIST - 12] = sText_PkmnAboutToBeAttackedByItsItem,
    [STRINGID_CANTESCAPEBECAUSEOFCURRENTMOVE - 12] = sText_CantEscapeBecauseOfCurrentMove,
    [STRINGID_NEUTRALIZINGGASENTERS - 12] = sText_NeutralizingGasEnters,
    [STRINGID_NEUTRALIZINGGASOVER - 12] = sText_NeutralizingGasOver,
    [STRINGID_PLAYERLOSTAGAINSTENEMYTRAINER - 12] = sText_PlayerLostAgainstEnemyTrainer,
    [STRINGID_PLAYERPAIDPRIZEMONEY - 12] = sText_PlayerPaidPrizeMoney,
    [STRINGID_PKMNRAISEDATTACK - 12] = sText_PkmnRaisedAttack,
    [STRINGID_INFLATABLEPKMNRAISEDDEFENSE - 12] = sText_InflatablePkmnRaisedDefenses,
    [STRINGID_ANGERPOINTSPKMNRAISEDATTACK - 12] = sText_AngerPointsPkmnRaisedAttack,
    [STRINGID_BATTLERINNATERAISEDSTAT - 12] = sText_BattlerInnateRaisedStat,
    [STRINGID_BATTLERADDEDTHETYPE - 12] = sText_BattlerAddedTheType,
    [STRINGID_BATTLERCOILEDUP - 12] = sText_BattlerCoiledUp,
    [STRINGID_ATTACKERTYPECHANGEDTO - 12] = sText_AttackerTypeChangedTo,
    [STRINGID_PKMNSABILITYRESTOREDHPALITTLE - 12] = sText_AbilityRestoredHPALittle,
    [STRINGID_BATTLERENVELOPEDITSELFINAVEIL - 12] = sText_BattlerEnvelopedItselfInAVeil,
    [STRINGID_AIRBLOWERACTIVATED - 12] = sText_BattlerAbilitySetUpTailwind,
    [STRINGID_TARGETPKMNHURTSWITH - 12] = sText_TargetPkmnHurtsWith,
    [STRINGID_PKMNCUTSSPATTACKWITH - 12] = sText_PkmnCutsSpecialAttackWith,
    [STRINGID_ATTACKERREGAINEDHEALTH - 12] = sText_AttackerRegainedHealth,
    [STRINGID_SWEETDREAMSHPUP - 12] = sText_SweetDreamsHealHpUp,
    [STRINGID_PKMNBECAMECURSED - 12] = sText_PkmnBecameCursed,
    [STRINGID_SPIDERLAIRACTIVATED - 12] = sText_BattlerAbilitySetUpStickyWeb,
    [STRINGID_TWISTEDDIMENSIONACTIVATED - 12] = sText_BattlerAbilitySetUpTrickRoom,
    [STRINGID_NORTHWINDACTIVATED - 12] = sText_BattlerAbilitySetUpAuroraVeil,
    [STRINGID_GRIPPINCERACTIVATED - 12] = sText_BattlerGripPincerActivated,
    [STRINGID_POKEMONDIDAMMOUNTDAMAGE - 12] = sText_PkmnDidAmmountDamage,
    [STRINGID_HEATUPBEAK - 12] = sText_HeatingUpBeak,
    [STRINGID_BATTLERABILITYLOWEREDTARGETSTAT - 12] = sText_BattlerAbilityLoweredEnemyStat,
    [STRINGID_CANACTFASTERTHANKSTOITEM - 12] = sText_CanActFasterItem,
    [STRINGID_PKMNHURTBYFROSTBITE - 12] = sText_PkmnHurtByFrostbite,
    [STRINGID_PKMNGOTFROSTBITE - 12] = sText_PkmnGotFrostbite,
    [STRINGID_PKMNSITEMHEALEDFROSTBITE - 12] = sText_PkmnsItemHealedFrostbite,
    [STRINGID_ATTACKERHEALEDITSFROSTBITE - 12] = sText_AttackerHealedItsFrostbite,
    [STRINGID_PKMNFROSTBITEHEALED - 12] = sText_PkmnFrostbiteHealed,
    [STRINGID_PKMNFROSTBITEHEALED2 - 12] = sText_PkmnFrostbiteHealed2,
    [STRINGID_PKMNFROSTBITEHEALEDBY - 12] = sText_PkmnFrostbiteHealedBy,
    [STRINGID_REGENERATOR_EXITS - 12] = sText_RegeneratorExits,
    [STRINGID_NATURAL_CURE_EXITS - 12] = sText_NaturalCureExits,
    [STRINGID_ABILITYLETITUSEMOVE - 12] = sText_AbilityLetItUseMove,
    [STRINGID_LETHARGYTENTERS - 12] = sText_LethargyEnters,
    [STRINGID_LETHARGYENDS - 12] = sText_LethargyEnd,
    [STRINGID_PICKUPACTIVATED - 12] = sText_PickupActivated,
    [STRINGID_PKMNRAISEDDEFENSE - 12] = sText_PkmnRaisedDefense,
    [STRINGID_PKMNRAISEDSPECIALATTACK - 12] = sText_PkmnRaisedSpecialAttack,
    [STRINGID_PKMNRAISEDSPECIALDEFENSE - 12] = sText_PkmnRaisedSpecialDefense,
    [STRINGID_TARGETGOTCURSED - 12] = sText_TargetyBecameCursed,
    [STRINGID_BATTLERHASNODAMAGEHITS - 12] = sText_BattlerHasNoDamageHits,
    [STRINGID_BATTLERCANNOLONGERENDUREHITS - 12] = sText_BattlerCanNoLongerEndureHits,
    [STRINGID_BATTLERCANSTILLENDUREHITS - 12] = sText_BattlerCanStillEndureHits,
    [STRINGID_BATTLERHASSINGLENODAMAGEHITS - 12] = sText_BattlerHasASingleNoDamageHit,
    [STRINGID_BATTLERCANSTILLENDUREASINGLEHIT - 12] = sText_BattlerCanStillEndureASingleHit,
    [STRINGID_BATTLERHASSPILLEDTOXICWASTE - 12] = sText_BattlerHasSpilledToxicWaste,
    [STRINGID_THETOXICWASTEHASDISSAPPEARED - 12] = sText_TheToxicWasteHasDissappeared,
    [STRINGID_PKMNHURTBYTOXICWASTE - 12] = sText_PkmnHurtByToxicWaste,
    [STRINGID_THEPOKEMONBLOCKEDTHEBALL - 12] = sText_ThePokemonBlockedTheBall,
    [STRINGID_ATTACKERISREADYTOTESTYOU - 12] = sText_AttackerIsReadyToTestYou,
    [STRINGID_PKMNSSTATSWEREBOOSTED - 12] = sText_AttackerStatsWereBoosted,
    [STRINGID_WILDPOKEMONMEGAEVOLVED - 12] = sText_WildMonMegaEvolved,
    [STRINGID_THEOPPOSINGATTACKERROARSWILDLY - 12] = sText_TheOpposingAttackerRoarsWildly,
    [STRINGID_ATTACKER_ATTACK_ROSE - 12] = sText_AttackerAttackRose,
    [STRINGID_ATTACKER_DEFENSE_ROSE - 12] = sText_AttackerDefenseRose,
    [STRINGID_ATTACKER_SPECIAL_ATTACK_ROSE - 12] = sText_AttackerSpecialAttackRose,
    [STRINGID_ATTACKER_SPECIAL_DEFENSE_ROSE - 12] = sText_AttackerSpecialDefenseRose,
    [STRINGID_ATTACKER_SPEED_ROSE - 12] = sText_AttackerSpeedRose,
    [STRINGID_ATTACKER_ACCURACY_ROSE - 12] = sText_AttackerAccuracyRose,
    [STRINGID_ATTACKER_EVASIVENESS_ROSE - 12] = sText_AttackerEvasivenessRose,
    [STRINGID_ATTACKER_ATTACK_SHARPLYROSE - 12] = sText_AttackerAttackSharplyRose,
    [STRINGID_ATTACKER_DEFENSE_SHARPLYROSE - 12] = sText_AttackerDefenseSharplyRose,
    [STRINGID_ATTACKER_SPECIAL_ATTACK_SHARPLYROSE - 12] = sText_AttackerSpecialAttackSharplyRose,
    [STRINGID_ATTACKER_SPECIAL_DEFENSE_SHARPLYROSE - 12] = sText_AttackerSpecialDefenseSharplyRose,
    [STRINGID_ATTACKER_SPEED_SHARPLYROSE - 12] = sText_AttackerSpeedSharplyRose,
    [STRINGID_ATTACKER_ACCURACY_SHARPLYROSE - 12] = sText_AttackerAccuracySharplyRose,
    [STRINGID_ATTACKER_EVASIVENESS_SHARPLYROSE - 12] = sText_AttackerEvasivenessSharplyRose,
    [STRINGID_DESERTCLOAKPROTECTED - 12] = sText_DesertCloakVeilProtected,
    [STRINGID_ATTACKER_STATS_ROSE - 12] = sText_AttackerStatsRose,
    [STRINGID_ANGELS_WRATH_TACKLE_EFFECT - 12] = sText_AngelsWrath_TackleEffect,
    [STRINGID_ANGELS_WRATH_STRING_SHOT_EFFECT - 12] = sText_AngelsWrath_StringShotEffect,
    [STRINGID_ANGELS_WRATH_HARDEN_EFFECT - 12] = sText_AngelsWrath_HardenEffect,
    [STRINGID_ANGELS_WRATH_IRON_DEFENSE_EFFECT - 12] = sText_AngelsWrath_IronDefenseEffect,
    [STRINGID_ANGELS_WRATH_ELECTROWEB_EFFECT - 12] = sText_AngelsWrath_ElectrowebEffect,
    [STRINGID_ANGELS_WRATH_BUG_BITE_EFFECT - 12] = sText_AngelsWrath_BugBiteEffect,
    [STRINGID_ANGELS_WRATH_PROTECT_EFFECT - 12] = sText_AngelsWrath_ProtectEffect,
    [STRINGID_DISCIPLINE_LOCK_ENDS - 12] = sText_PkmnsDisciplineLockEnds,
    [STRINGID_SLEEPCLAUSEDISABLESMOVE - 12] = sText_SleepClauseDisablesMove,
    [STRINGID_ELECTROMORPHOSIS_ACTIVATES - 12] = sText_ElectromorphosisActivates,
    [STRINGID_INVERSEROOMENDS - 12] = sText_InverseRoomEnds,
    [STRINGID_INVERSEROOMACTIVATED - 12] = sText_BattlerAbilitySetUpInverseRoom,
    [STRINGID_ATTACKERGOTTHETYPE - 12] = sText_AttackerGotTheType,
    [STRINGID_DEFENDERTYPECHANGEDTO - 12] = sText_DefenderTypeChangedTo,
    [STRINGID_ATTACKERBECAMEINFECTED - 12] = sText_AttackerBecameInfected,
    [STRINGID_PASTELVEILACTIVATED - 12] = sText_BattlerAbilitySetUpSafeGuard,
    [STRINGID_PKMNSTARTBLEED - 12] = sText_PkmnStartBleed,
    [STRINGID_PKMNHURTBYBLEED - 12] = sText_PkmnHurtByBleed,
    [STRINGID_PKMNBLEEDHEALED - 12] = sText_PkmnBleedHealed,
    [STRINGID_PKMNBLEEDHEALED2 - 12] = sText_PkmnBleedHealed2,
    [STRINGID_PKMNBLEEDHEALEDBY - 12] = sText_PkmnBleedHealedBy,
    [STRINGID_PKMNSITEMHEALEDBLEED - 12] = sText_PkmnsItemHealedBleed,
    [STRINGID_LASTABILITYLOWEREDSTAT - 12] = sText_LastAbilityLoweredBuff1,
    [STRINGID_TIPPINGPOINTSPKMNRAISEDSPATTACK - 12] = sText_TippingPointsPkmnRaisedSpecialAttack,
    [STRINGID_SETUPINVERSEROOM - 12] = sText_PkmnInverseDimensions,
    [STRINGID_CROWNEDKINGENTERS - 12] = sText_CrownedKingEnters,
    [STRINGID_PERMANENCE - 12] = sText_Permanence,
    [STRINGID_BERSERKDNA - 12] = sText_BerserkDNA,
    [STRINGID_HURTBYABILITY - 12] = sText_HurtByAbility,
    [STRINGID_RETRIEVEREXITS - 12] = sText_RetrieverExits,
    [STRINGID_SALT_CIRCLE - 12] = sText_BlockOnEntry,
    [STRINGID_SUPREME_OVERLORD - 12] = sText_SupremeOverlord,
    [STRINGID_PARADOX_BOOST - 12] = sText_ParadoxBoost,
    [STRINGID_PARADOX_BOOST_END - 12] = sText_ParadoxBoostEnd,
    [STRINGID_PARADOX_BOOST_WEATHER - 12] = sText_ParadoxBoostSun,
    [STRINGID_PARADOX_BOOST_ITEM - 12] = sText_ParadoxBoostItem,
    [STRINGID_PARADOX_BOOST_TERRAIN - 12] = sText_ParadoxBoostElectricTerrain,
    [STRINGID_COSTAR - 12] = sText_Costar,
    [STRINGID_OVERFLOWINGWITHSPACEPOWER - 12] = sText_OverflowingWithSpacePower,
    [STRINGID_PLAYERGOTBP - 12] = sText_PlayerGotBP,
    [STRINGID_CANT_USE_TWICE_IN_A_ROW - 12] = sText_CantUseTwiceInARow,
    [STRINGID_ATTACKERLOSTELECTRICTYPE - 12] = sText_AttackerLostElectricType,
    [STRINGID_HEAVYSPIKESSCATTERED - 12] = sText_HeavySpikesScattered,
    [STRINGID_PKMNALREADYTYPE - 12] = sText_PkmnAlreadyType,
    [STRINGID_PKMNCUTHPRAISEDSTATS - 12] = sText_PkmnCutHPRaisedStats,
    [STRINGID_PKMNAFFLICTEDBYSALTCURE - 12] = sText_PkmnAfflictedBySaltCure,
    [STRINGID_PKMN_SWITCHED_FIELD_EFFECTS - 12] = sText_PkmnSwitchedFieldEffects,
    [STRINGID_PKMNTELLCHILLINGRECEPTIONJOKE - 12] = sText_PkmnTellChillingReceptionJoke,
    [STRINGID_SHEDITSTAIL - 12] = sText_ShedItsTail,
    [STRINGID_GHASTLY_ECHO - 12] = sText_GhastlyEcho,
    [STRINGID_PKMNREVIVEDREADYTOFIGHT - 12] = sText_PkmnRevivedReadyToFight,
    [STRINGID_THESWAMPDISAPPEARED - 12] = sText_TheSwampDisappeared,
    [STRINGID_SWAMPENVELOPEDSIDE - 12] = sText_SwampEnvelopedSide,
    [STRINGID_THESEAOFFIREDISAPPEARED - 12] = sText_TheSeaOfFireDisappeared,
    [STRINGID_HURTBYTHESEAOFFIRE - 12] = sText_HurtByTheSeaOfFire,
    [STRINGID_SEAOFFIREENVELOPEDSIDE - 12] = sText_SeaOfFireEnvelopedSide,
    [STRINGID_WAITINGFORPARTNERSMOVE - 12] = sText_WaitingForPartnersMove,
    [STRINGID_THERAINBOWDISAPPEARED - 12] = sText_TheRainbowDisappeared,
    [STRINGID_ARAINBOWAPPEAREDONSIDE - 12] = sText_ARainbowAppearedOnSide,
    [STRINGID_STATBUFFSGONE - 12] = sText_StatBuffsGone,
    [STRINGID_BLEWAWAYCREEPINGTHORNS - 12] = sText_BlewAwayCreepingThorns,
    [STRINGID_HURTBYCREEPINGTHORNS - 12] = sText_CreepingThornsDmg,
    [STRINGID_VICIOUSTHORNSUSED - 12] = sText_ViciousThornsUsed,
    [STRINGID_ILL_WILL_LOST_PP - 12] = sText_IllWillLostPp,
    [STRINGID_CHARGEABILITYBATTLER - 12] = sText_ChargeAbilityBattler,
    [STRINGID_ATTACKERATEITEM - 12] = sText_AttackerAteItem,
    [STRINGID_AFFLICTED_WITH_SALT_CURE - 12] = sText_AfflictedWithSaltCure,
    [STRINGID_AFFLICTED_WITH_SYRUP - 12] = sText_AfflictedWithSyrup,
    [STRINGID_SCRIPTING_COPIED_ABILITY - 12] = sText_ScriptingAcquiredAbility,
    [STRINGID_TEAM_ACQUIRED_ABILITY - 12] = sText_TeamAcquiredAbility,
    [STRINGID_COMMANDER_ACTIVATES - 12] = sText_CommanderActivates,
    [STRINGID_COMMANDER_ENDS - 12] = sText_CommanderEnds,
    [STRINGID_COMMANDER_CANT_SWITCH - 12] = sText_CommanderCantSwitch,
    [STRINGID_VIOLENT_RUSH - 12] = sText_ViolentRush,
    [STRINGID_RAPID_RESPONSE - 12] = sText_RapidResponse,
    [STRINGID_READIED_ACTION - 12] = sText_ReadiedAction,
    [STRINGID_SHOWDOWN_MODE - 12] = sText_ShowdownMode,
    [STRINGID_TRICKROOMSTARTS - 12] = sText_TrickRoomStarts,
    [STRINGID_INVERSEROOMSTARTS - 12] = sText_InverseRoomStarts,
    [STRINGID_WONDERROOMSTARTS - 12] = sText_WonderRoomStarts,
    [STRINGID_MAGICROOMSTARTS - 12] = sText_MagicRoomStarts,
    [STRINGID_ORIGIN_REVERSION - 12] = sText_OriginReversion,
    [STRINGID_CROWNED_REVERSION - 12] = sText_CrownedReversion,
    [STRINGID_FOGISDEEP - 12] = sText_FogIsDeep,
    [STRINGID_FOGBLOWNAWAY - 12] = sText_FogWasBlownAway,
    [STRINGID_PKMNFROSTBITTENBY - 12] = sText_PkmnFrostbittenBy,
    [STRINGID_PKMNBLEDBY - 12] = sText_PkmnBledBy,
    [STRINGID_THUNDER_CAGE - 12] = sText_ThunderCage,
    [STRINGID_SNAP_TRAP - 12] = sText_SnapTrap,
    [STRINGID_ELECTRO_SHOT - 12] = sText_ElectroShot,
    [STRINGID_ANNOUNCE_MONOTYPEBOOST_NORMAL - 12] = sText_AnnounceMonotypeBoostNormal,
    [STRINGID_ANNOUNCE_MONOTYPEBOOST_FIGHTING - 12] = sText_AnnounceMonotypeBoostFighting,
    [STRINGID_ANNOUNCE_MONOTYPEBOOST_FLYING - 12] = sText_AnnounceMonotypeBoostFlying,
    [STRINGID_ANNOUNCE_MONOTYPEBOOST_POISON - 12] = sText_AnnounceMonotypeBoostPoison,
    [STRINGID_ANNOUNCE_MONOTYPEBOOST_GROUND - 12] = sText_AnnounceMonotypeBoostGround,
    [STRINGID_ANNOUNCE_MONOTYPEBOOST_ROCK - 12] = sText_AnnounceMonotypeBoostRock,
    [STRINGID_ANNOUNCE_MONOTYPEBOOST_BUG - 12] = sText_AnnounceMonotypeBoostBug,
    [STRINGID_ANNOUNCE_MONOTYPEBOOST_GHOST - 12] = sText_AnnounceMonotypeBoostGhost,
    [STRINGID_ANNOUNCE_MONOTYPEBOOST_STEEL - 12] = sText_AnnounceMonotypeBoostSteel,
    [STRINGID_ANNOUNCE_MONOTYPEBOOST_FIRE - 12] = sText_AnnounceMonotypeBoostFire,
    [STRINGID_ANNOUNCE_MONOTYPEBOOST_WATER - 12] = sText_AnnounceMonotypeBoostWater,
    [STRINGID_ANNOUNCE_MONOTYPEBOOST_GRASS - 12] = sText_AnnounceMonotypeBoostGrass,
    [STRINGID_ANNOUNCE_MONOTYPEBOOST_ELECTRIC - 12] = sText_AnnounceMonotypeBoostElectric,
    [STRINGID_ANNOUNCE_MONOTYPEBOOST_PSYCHIC - 12] = sText_AnnounceMonotypeBoostPsychic,
    [STRINGID_ANNOUNCE_MONOTYPEBOOST_ICE - 12] = sText_AnnounceMonotypeBoostIce,
    [STRINGID_ANNOUNCE_MONOTYPEBOOST_DRAGON - 12] = sText_AnnounceMonotypeBoostDragon,
    [STRINGID_ANNOUNCE_MONOTYPEBOOST_DARK - 12] = sText_AnnounceMonotypeBoostDark,
    [STRINGID_ANNOUNCE_MONOTYPEBOOST_FAIRY - 12] = sText_AnnounceMonotypeBoostFairy,
    [STRINGID_PARASITIC_SPORES - 12] = sText_ParasiticSpores,
    [STRINGID_PARASITIC_SPORES_SPREAD - 12] = sText_ParasiticSporesSpread,
    [STRINGID_HURT_BY_PARASITIC_SPORES - 12] = sText_HurtByParasiticSpores,
    [STRINGID_CRIT_CANT_INCREASE - 12] = sText_CritCantIncrease,
    [STRINGID_CRIT_PLUS_1 - 12] = sText_CritPlus1,
    [STRINGID_CRIT_PLUS_2 - 12] = sText_CritPlus2,
    [STRINGID_CRIT_PLUS_3 - 12] = sText_CritPlus3,
    [STRINGID_FILLED_WITH_FEAR - 12] = sText_FilledWithFear,
    [STRINGID_POWER_OF_ALCHEMY - 12] = sText_PowerOfAlchemy,
    [STRINGID_POWER_OF_ALCHEMY_SLUDGE - 12] = sText_PowerOfAlchemySludge,
    [STRINGID_POWER_OF_ALCHEMY_GOLD - 12] = sText_PowerOfAlchemyGold,
    [STRINGID_QUASH_ENDS - 12] = sText_QuashEnds,
    [STRINGID_FOGENDS - 12] = sText_FogEnds,
    [STRINGID_SMOKESCREEN - 12] = sText_Smokescreen,
    [STRINGID_EXTRASKILL_TERRAIN - 12] = sText_ExtraSkillTerrain,
    [STRINGID_EXTRASKILL_STEADYSTATSBOOST - 12] = sText_ExtraSkillSteadyStatsBoost,
    [STRINGID_EXTRASKILL_POSTURE - 12] = sText_ExtraSkillPosture,
    [STRINGID_EXTRASKILL_STATUSONTEAM - 12] = sText_ExtraSkillStatusOnTeam,
    [STRINGID_EXTRASKILL_LASTSTAND - 12] = sText_ExtraSkillLastStand,
    [STRINGID_EXTRASKILL_MATBLOCK - 12] = sText_ExtraSkillMatBlock,
    [STRINGID_EXTRASKILL_WOEUPONYE - 12] = sText_ExtraSkillWoeUponYe,
    [STRINGID_EXTRASKILL_FORESIGHT - 12] = sText_ExtraSkillForesight,
    [STRINGID_EXTRASKILL_EMBARGO - 12] = sText_ExtraSkillEmbargo,
    [STRINGID_EXTRASKILL_NOPROTECT - 12] = sText_ExtraSkillNoProtect,
    [STRINGID_EXTRASKILL_SMACKDOWN - 12] = sText_ExtraSkillWindSmackDown,
    [STRINGID_EXTRASKILL_REFLECT - 12] = sText_ExtraSkillReflect,
    [STRINGID_EXTRASKILL_LIGHTSCREEN - 12] = sText_ExtraSkillLightscreen,
    [STRINGID_EXTRASKILL_LUCKY_CHANT - 12] = sText_ExtraSkillLuckyChant,
    [STRINGID_EXTRASKILL_PERMA_WIDEGUARD - 12] = sText_ExtraSkillPermaWideGuard,
    [STRINGID_FOG_STAT_DROPS - 12] = sText_FogStatDrops,
    [STRINGID_ON_THE_PROWL - 12] = sText_OnTheProwl,
    [STRINGID_FOG_STARTS - 12] = sText_FogStarts,
    [STRINGID_FLARE_BOOST_IGNITES - 12] = sText_FlareBoostIgnites,
    [STRINGID_FOG_RETURNS - 12] = sText_FogReturns,
    [STRINGID_RECURRING_NIGHTMARE - 12] = sText_RecurringNightmare,
    [STRINGID_PKMNHURTBYFUNERALPYRE - 12] = sText_PkmnHurtByFuneralPyre,
    [STRINGID_FUNERAL_PYRE_ENTRY - 12] = sText_FuneralPyreEntry,
    [STRINGID_NO_TURNING_BACK - 12] = sText_NoTurningBack,
    [STRINGID_BATTLERACQUIREDABILITY - 12] = sText_BattlerAcquiredAbility,
    [STRINGID_SKY_DROP_ENDS_EARLY - 12] = sText_SkyDropEndsEarly,
    [STRINGID_SKY_DROP_CHARGE - 12] = sText_SkyDropCharge,
    [STRINGID_SKY_DROP_STUCK - 12] = sText_SkyDropStuck,
    [STRINGID_OPPOSING_STAT_BUFFS_GONE - 12] = sText_OpposingStatBuffsGone,
    [STRINGID_HOSPITALITY - 12] = sText_Hospitality,
    [STRINGID_TARGET_CANT_HEAL - 12] = sText_TargetCantHeal,
    [STRINGID_RESTRAINING_ORDER - 12] = sText_RestrainingOrder,
    [STRINGID_ATTACKERBREAKSPROTECTION - 12] = sText_AttackerBreaksProtection,
    [STRINGID_ATTACKERSHATTERSSCREENS - 12] = sText_AttackerShattersScreens,
    [STRINGID_ATTACKERDESTROYSSUBSTITUTE - 12] = sText_AttackerDestroysSubstitute,
    [STRINGID_WHITE_SMOKE - 12] = sText_WhiteSmoke,
    [STRINGID_HOT_COALS_SET - 12] = sText_HotCoalsSet,
    [STRINGID_HOT_COALS_BURN - 12] = sText_HotCoalsBurn,
    [STRINGID_HOT_COALS_EXTINGUISH - 12] = sText_HotCoalsExtinguish,
    [STRINGID_DISABLE_ATTACKER_ITEM - 12] = sText_DisableAttackerItem,
    [STRINGID_SUPER_STRAIN_RECOIL - 12] = sText_SuperStrainRecoil,
    [STRINGID_ATTACKERLOSTDARKTYPE - 12] = sText_AttackerLostDarkType,
    [STRINGID_CLEARSKIES - 12] = sText_ClearSkies,
    [STRINGID_WEATHER_NORMAL - 12] = sText_WeatherNormal,
    [STRINGID_KNOW_YOUR_PLACE - 12] = sText_KnowYourPlace,
    [STRINGID_ROOMS_CLEARED - 12] = sText_RoomsCleared,
    [STRINGID_TREPIDATION - 12] = sText_Trepidation,
    [STRINGID_KINESIS - 12] = sText_Kinesis,
    [STRINGID_CALTROPS_SET - 12] = sText_CaltropsSet,
    [STRINGID_CALTROPS_DAMAGE - 12] = sText_CaltropsDamage,
    [STRINGID_CALTROPS_FADE - 12] = sText_CaltropsFade,
    [STRINGID_HOT_COALS_FREE - 12] = sText_HotCoalsFree,
    [STRINGID_CALTROPS_FREE - 12] = sText_CaltropsFree,
    [STRINGID_ITEM_DESTROYED - 12] = sText_ItemDestroyed,
    [STRINGID_SWAPWITH - 12] = sText_SwapWith,
    [STRINGID_QUICKGUARD - 12] = sText_QuickGuard,
    [STRINGID_BLOOD_STAIN_ANNOUNCE - 12] = sText_BloodStainAnnounce,
    [STRINGID_TRAPPED - 12] = sText_Trapped,
    [STRINGID_HONEYGATHER - 12] = sText_HoneyGather,
    [STRINGID_LIFE_STEAL - 12] = sText_LifeSteal,
    [STRINGID_RUDE_AWAKENING - 12] = sText_RudeAwakening,
    [STRINGID_CUTTHROAT - 12] = sText_Cutthroat,
    [STRINGID_ITEM_STAT_PROTECTED - 12] = sText_ItemStatProtected,
    [STRINGID_STACKTYPECHANGEDTO - 12] = sText_StackChangedTypeTo,
    [STRINGID_STACKREGAINEDHEALTH - 12] = sText_StackRegainedHealth,
    [STRINGID_FIRECOATDAMAGE - 12] = sText_FireCoatDamage,
    [STRINGID_FIRECOATSTART - 12] = sText_FireCoatStart,
    [STRINGID_SOOTHSAYER - 12] = sText_Soothsayer,
    [STRINGID_ROSEGARDEN - 12] = sText_RoseGarden,
    [STRINGID_MIMICRYENDS - 12] = sText_MimicryEnds,
    [STRINGID_TERRAINBECOMESTOXIC - 12] = sText_TerrainBecomesToxic,
    [STRINGID_TOXICTERRAINENDS - 12] = sText_ToxicTerrainEnds,
    [STRINGID_TOXICTERRAINDAMAGES - 12] = sText_ToxicTerrainDamages,
    [STRINGID_ABSORB_STEALTH_ROCKS - 12] = sText_AbsorbStealthRocks,
    [STRINGID_EVASIONCLAUSEDISABLESMOVE - 12] = sText_EvasionClauseDisablesMove,
    [STRINGID_DISABLESMOVE - 12] = sText_CurrentFightDisablesMove,
    [STRINGID_WINTER_THRONE_ENTRY - 12] = sText_WinterThroneEntry,
    [STRINGID_PKMNHURTBYWINTERTHRONE - 12] = sText_PkmnHurtByWinterThrone,
    [STRINGID_BACKUP_POWER - 12] = sText_BackupPower,
    [STRINGID_EXTRA_ABILITIES_1 - 12] = sText_ExtraSkillAbility1,
    [STRINGID_EXTRA_ABILITIES_2 - 12] = sText_ExtraSkillAbility2,
    [STRINGID_EXTRA_ABILITIES_3 - 12] = sText_ExtraSkillAbility3,
    [STRINGID_CHRISTMAS_NIGHTMARE - 12] = sText_ChristmasNightmare,
    [STRINGID_DRAKELP_HEAD - 12] = sText_DrakelpHead,
    [STRINGID_DRAKELP_HEAD_RESET - 12] = sText_DrakelpHeadReset,
    [STRINGID_MENTAL_POLLUTION - 12] = sText_MentalPollution,
    [STRINGID_ICE_STATUE - 12] = sText_IceStatue,
    [STRINGID_TOXIC_BOOST_POISONS - 12] = sText_ToxicBoostPoisons,
    [STRINGID_PETAL_SHIELD - 12] = sText_PetalShield,
    [STRINGID_POWER_OUTAGE_ENTRY - 12] = sText_PowerOutageEntry,
    [STRINGID_STACKLOSTTYPE - 12] = sText_StackLostType,
    [STRINGID_ROCKCLIMB - 12] = sText_RockClimb,
    [STRINGID_IS_ENRAGED - 12] = sText_IsEnraged,
    [STRINGID_MADNESS_ENHANCEMENT - 12] = sText_MadnessEnhancement,
    [STRINGID_BECOMES_ENRAGED - 12] = sText_BecomesEnraged,
    [STRINGID_ENRAGE_HURT - 12] = sText_EnrageHurt,
    [STRINGID_MADNESS_ENRAGE - 12] = sText_MadnessEnrage,
    [STRINGID_BECOMES_DRENCHED - 12] = sText_BecomesDrenched,
    [STRINGID_CHUCKSTER - 12] = sText_Chuckster,
    [STRINGID_WATERLOG - 12] = sText_Waterlog,
    [STRINGID_FETCH - 12] = sText_Fetch,
    [STRINGID_FETCH_RETRIEVE - 12] = sText_FetchRetrieve,
    [STRINGID_FETCH_NOTHING - 12] = sText_FetchNothing,
    [STRINGID_FETCH_SEARCH - 12] = sText_FetchSearch,
    [STRINGID_FETCH_SWITCH_NO_ITEM - 12] = sText_FetchSwitchNoItem,
    [STRINGID_TEATIME - 12] = sText_Teatime,
    [STRINGID_EXTRASKILL_PERMA_STICKY_WEB - 12] = sText_ExtraSkillPermaStickyWeb,
    [STRINGID_LIGHT_WIELD_ACTIVATED - 12] = sText_LightWieldSetUpLightScreen,
    [STRINGID_PKMNGOTOVERITSFEAR - 12] = sText_PkmnGotOverItsFear,
};

const u16 gMentalHerbCureStringIds[] = 
{
    [B_MSG_MENTALHERBCURE_INFATUATION] = STRINGID_ATKGOTOVERINFATUATION,
    [B_MSG_MENTALHERBCURE_TAUNT] = STRINGID_BUFFERENDS,
    [B_MSG_MENTALHERBCURE_ENCORE] = STRINGID_PKMNENCOREENDED,
    [B_MSG_MENTALHERBCURE_TORMENT] = STRINGID_TORMENTEDNOMORE,
    [B_MSG_MENTALHERBCURE_HEALBLOCK] = STRINGID_HEALBLOCKEDNOMORE,
    [B_MSG_MENTALHERBCURE_DISABLE] = STRINGID_PKMNMOVEDISABLEDNOMORE,
};

const u16 gTerrainStringIds[] = 
{
    [B_MSG_TERRAINBECOMESMISTY] = STRINGID_TERRAINBECOMESMISTY,
    [B_MSG_TERRAINBECOMESGRASSY] = STRINGID_TERRAINBECOMESGRASSY,
    [B_MSG_TERRAINBECOMESELECTRIC] = STRINGID_TERRAINBECOMESELECTRIC,
    [B_MSG_TERRAINBECOMESPSYCHIC] = STRINGID_TERRAINBECOMESPSYCHIC,
    [B_MSG_TERRAINBECOMESTOXIC] = STRINGID_TERRAINBECOMESTOXIC,
};

const u16 gTerrainEndingStringIds[] = 
{
    [B_MSG_MISTYTERRAINENDS] = STRINGID_MISTYTERRAINENDS,
    [B_MSG_GRASSYTERRAINENDS] = STRINGID_GRASSYTERRAINENDS,
    [B_MSG_ELECTRICTERRAINENDS] = STRINGID_ELECTRICTERRAINENDS,
    [B_MSG_PSYCHICTERRAINENDS] = STRINGID_PSYCHICTERRAINENDS,
    [B_MSG_TOXICTERRAINENDS] = STRINGID_TOXICTERRAINENDS,
};

const u16 gTerrainPreventsStringIds[] = 
{
    [B_MSG_TERRAINPREVENTS_MISTY] = STRINGID_MISTYTERRAINPREVENTS,
    [B_MSG_TERRAINPREVENTS_ELECTRIC] = STRINGID_ELECTRICTERRAINPREVENTS,
    [B_MSG_TERRAINPREVENTS_PSYCHIC] = STRINGID_PSYCHICTERRAINPREVENTS,
};

const u16 gMagicCoatBounceStringIds[] = 
{
    [B_MSG_PKMNMOVEBOUNCED] = STRINGID_PKMNMOVEBOUNCED,
    [B_MSG_PKMNMOVEBOUNCEDABILITY] = STRINGID_PKMNMOVEBOUNCEDABILITY,
};

const u16 gHealingWishStringIds[] = 
{
    [B_MSG_HEALINGWISHCAMETRUE] = STRINGID_HEALINGWISHCAMETRUE,
    [B_MSG_LUNARDANCECAMETRUE] = STRINGID_LUNARDANCECAMETRUE,
};

const u16 gDmgHazardsStringIds[] = 
{
    [B_MSG_PKMNHURTBYSPIKES] = STRINGID_PKMNHURTBYSPIKES,
    [B_MSG_STEALTHROCKDMG] = STRINGID_STEALTHROCKDMG,
    [B_MSG_CREEPINGTHORNSDMG] = STRINGID_HURTBYCREEPINGTHORNS,
};

const u16 gSwitchInAbilityStringIds[] = 
{
    [B_MSG_SWITCHIN_MOLDBREAKER] = STRINGID_MOLDBREAKERENTERS,
    [B_MSG_SWITCHIN_TERAVOLT] = STRINGID_TERAVOLTENTERS,
    [B_MSG_SWITCHIN_TURBOBLAZE] = STRINGID_TURBOBLAZEENTERS,
    [B_MSG_SWITCHIN_SLOWSTART] = STRINGID_SLOWSTARTENTERS,
    [B_MSG_SWITCHIN_UNNERVE] = STRINGID_UNNERVEENTERS,
    [B_MSG_SWITCHIN_ANTICIPATION] = STRINGID_ANTICIPATIONACTIVATES,
    [B_MSG_SWITCHIN_FOREWARN] = STRINGID_FOREWARNACTIVATES,
    [B_MSG_SWITCHIN_PRESSURE] = STRINGID_PRESSUREENTERS,
    [B_MSG_SWITCHIN_DARKAURA] = STRINGID_DARKAURAENTERS,
    [B_MSG_SWITCHIN_FAIRYAURA] = STRINGID_FAIRYAURAENTERS,
    [B_MSG_SWITCHIN_AURABREAK] = STRINGID_AURABREAKENTERS,
    [B_MSG_SWITCHIN_COMATOSE] = STRINGID_COMATOSEENTERS,
    [B_MSG_SWITCHIN_SCREENCLEANER] = STRINGID_SCREENCLEANERENTERS,
    [B_MSG_SWITCHIN_ASONE] = STRINGID_ASONEENTERS,
    [B_MSG_SWITCHIN_CURIOUS_MEDICINE] = STRINGID_CURIOUSMEDICINEENTERS,
    [B_MSG_SWITCHIN_PASTEL_VEIL] = STRINGID_PASTELVEILENTERS,
    [B_MSG_SWITCHIN_NEUTRALIZING_GAS] = STRINGID_NEUTRALIZINGGASENTERS,
    [B_MSG_SWITCHIN_LETHARGY] = STRINGID_LETHARGYTENTERS,
    [B_MSG_SWITCHIN_CROWNEDKING] = STRINGID_CROWNEDKINGENTERS,
    [B_MSG_SWITCHIN_PERMANENCE] = STRINGID_PERMANENCE,
    [B_MSG_SWITCHIN_SALT_CIRCLE] = STRINGID_SALT_CIRCLE,
    [B_MSG_SWITCHIN_SUPREME_OVERLORD] = STRINGID_SUPREME_OVERLORD,
    [B_MSG_SWITCHIN_COSTAR] = STRINGID_COSTAR,
    [B_MSG_SWITCHIN_READIED_ACTION] = STRINGID_READIED_ACTION,
    [B_MSG_SWITCHIN_VIOLENT_RUSH] = STRINGID_VIOLENT_RUSH,
    [B_MSG_SWITCHIN_RAPID_RESPONSE] = STRINGID_RAPID_RESPONSE,
    [B_MSG_SWITCHIN_SHOWDOWN_MODE] = STRINGID_SHOWDOWN_MODE,
    [B_MSG_SWITCHIN_PARASITIC_SPORES] = STRINGID_PARASITIC_SPORES,
    [B_MSG_SWITCHIN_REJECTION] = STRINGID_QUASHSUCCESS,
    [B_MSG_SWITCHIN_ON_THE_PROWL] = STRINGID_ON_THE_PROWL,
    [B_MSG_SWITCHIN_FUNERAL_PYRE] = STRINGID_FUNERAL_PYRE_ENTRY,
    [B_MSG_SWITCHIN_WHITE_SMOKE] = STRINGID_WHITE_SMOKE,
    [B_MSG_SWITCHIN_HOT_COALS] = STRINGID_HOT_COALS_SET,
    [B_MSG_SWITCHIN_BLOOD_STAIN] = STRINGID_BLOOD_STAIN_ANNOUNCE,
    [B_MSG_SWITCHIN_CUTTHROAT] = STRINGID_CUTTHROAT,
    [B_MSG_SWITCHIN_FIRE_COAT] = STRINGID_FIRECOATSTART,
    [B_MSG_SWITCHIN_SOOTHSAYER] = STRINGID_SOOTHSAYER,
    [B_MSG_SWITCHIN_MOLTEN_CORE] = STRINGID_ABSORB_STEALTH_ROCKS,
    [B_MSG_SWITCHIN_WINTER_THRONE] = STRINGID_WINTER_THRONE_ENTRY,
    [B_MSG_SWITCHIN_CHRISTMAS_NIGHTMARE] = STRINGID_CHRISTMAS_NIGHTMARE,
    [B_MSG_SWITCHIN_PETAL_SHIELD] = STRINGID_PETAL_SHIELD,
    [B_MSG_SWITCHIN_POWER_OUTAGE] = STRINGID_POWER_OUTAGE_ENTRY,
};

const u16 gParadoxBoostSourceIds[] = 
{
    [B_MSG_PARADOX_BOOST_ITEM] = STRINGID_PARADOX_BOOST_ITEM,
    [B_MSG_PARADOX_BOOST_WEATHER] = STRINGID_PARADOX_BOOST_WEATHER,
    [B_MSG_PARADOX_BOOST_TERRAIN] = STRINGID_PARADOX_BOOST_TERRAIN,
};

const u16 gMissStringIds[] = 
{
    [B_MSG_MISSED] = STRINGID_ATTACKMISSED,
    [B_MSG_PROTECTED] = STRINGID_PKMNPROTECTEDITSELF,
    [B_MSG_AVOIDED_ATK] = STRINGID_PKMNAVOIDEDATTACK,
    [B_MSG_AVOIDED_DMG] = STRINGID_AVOIDEDDAMAGE,
    [B_MSG_GROUND_MISS] = STRINGID_PKMNMAKESGROUNDMISS,
};

const u16 gNoEscapeStringIds[] = 
{
    [B_MSG_CANT_ESCAPE] = STRINGID_CANTESCAPE,
    [B_MSG_DONT_LEAVE_BIRCH] = STRINGID_DONTLEAVEBIRCH,
    [B_MSG_PREVENTS_ESCAPE] = STRINGID_PREVENTSESCAPE,
    [B_MSG_CANT_ESCAPE_2] = STRINGID_CANTESCAPE2,
    [B_MSG_ATTACKER_CANT_ESCAPE] = STRINGID_ATTACKERCANTESCAPE,
};

const u16 gMoveWeatherChangeStringIds[] = 
{
    [B_MSG_STARTED_RAIN] = STRINGID_STARTEDTORAIN,
    [B_MSG_STARTED_DOWNPOUR] = STRINGID_DOWNPOURSTARTED,
    [B_MSG_WEATHER_FAILED] = STRINGID_BUTITFAILED,
    [B_MSG_STARTED_SANDSTORM] = STRINGID_SANDSTORMBREWED,
    [B_MSG_STARTED_SUNLIGHT] = STRINGID_SUNLIGHTGOTBRIGHT,
    [B_MSG_STARTED_HAIL] = STRINGID_STARTEDHAIL,
    [B_MSG_STARTED_FOG] = STRINGID_FOG_STARTS,
    [B_MSG_WEATHER_BECAME_NORMAL] = STRINGID_WEATHER_NORMAL,
};

const u16 gSandStormHailContinuesStringIds[] = 
{
    [B_MSG_SANDSTORM] = STRINGID_SANDSTORMRAGES,
    [B_MSG_HAIL] = STRINGID_HAILCONTINUES,
};

const u16 gSandStormHailDmgStringIds[] = 
{
    [B_MSG_SANDSTORM] = STRINGID_PKMNBUFFETEDBYSANDSTORM,
    [B_MSG_HAIL] = STRINGID_PKMNPELTEDBYHAIL,
};

const u16 gSandStormHailEndStringIds[] = 
{
    [B_MSG_SANDSTORM] = STRINGID_SANDSTORMSUBSIDED,
    [B_MSG_HAIL] = STRINGID_HAILSTOPPED,
};

const u16 gRainContinuesStringIds[] = 
{
    [B_MSG_RAIN_CONTINUES] = STRINGID_RAINCONTINUES,
    [B_MSG_DOWNPOUR_CONTINUES] = STRINGID_DOWNPOURCONTINUES,
    [B_MSG_RAIN_STOPPED] = STRINGID_RAINSTOPPED,
};

const u16 gProtectLikeUsedStringIds[] = 
{
    [B_MSG_PROTECTED_ITSELF] = STRINGID_PKMNPROTECTEDITSELF2,
    [B_MSG_BRACED_ITSELF] = STRINGID_PKMNBRACEDITSELF,
    [B_MSG_PROTECT_FAILED] = STRINGID_BUTITFAILED,
    [B_MSG_PROTECTED_TEAM] = STRINGID_PROTECTEDTEAM,
};

const u16 gReflectLightScreenSafeguardStringIds[] = 
{
    [B_MSG_SIDE_STATUS_FAILED] = STRINGID_BUTITFAILED,
    [B_MSG_SET_REFLECT_SINGLE] = STRINGID_PKMNRAISEDDEF,
    [B_MSG_SET_REFLECT_DOUBLE] = STRINGID_PKMNRAISEDDEFALITTLE,
    [B_MSG_SET_LIGHTSCREEN_SINGLE] = STRINGID_PKMNRAISEDSPDEF,
    [B_MSG_SET_LIGHTSCREEN_DOUBLE] = STRINGID_PKMNRAISEDSPDEFALITTLE,
    [B_MSG_SET_SAFEGUARD] = STRINGID_PKMNCOVEREDBYVEIL,
};

const u16 gLeechSeedStringIds[] = 
{
    [B_MSG_LEECH_SEED_SET] = STRINGID_PKMNSEEDED,
    [B_MSG_LEECH_SEED_MISS] = STRINGID_PKMNEVADEDATTACK,
    [B_MSG_LEECH_SEED_FAIL] = STRINGID_ITDOESNTAFFECT,
    [B_MSG_LEECH_SEED_DRAIN] = STRINGID_PKMNSAPPEDBYLEECHSEED,
    [B_MSG_LEECH_SEED_OOZE] = STRINGID_ITSUCKEDLIQUIDOOZE,
};

const u16 gRestUsedStringIds[] = 
{
    [B_MSG_REST] = STRINGID_PKMNWENTTOSLEEP,
    [B_MSG_REST_STATUSED] = STRINGID_PKMNSLEPTHEALTHY,
};

const u16 gUproarOverTurnStringIds[] = 
{
    [B_MSG_UPROAR_CONTINUES] = STRINGID_PKMNMAKINGUPROAR,
    [B_MSG_UPROAR_ENDS] = STRINGID_PKMNCALMEDDOWN,
};

const u16 gStockpileUsedStringIds[] = 
{
    [B_MSG_STOCKPILED] = STRINGID_PKMNSTOCKPILED,
    [B_MSG_CANT_STOCKPILE] = STRINGID_PKMNCANTSTOCKPILE,
};

const u16 gWokeUpStringIds[] = 
{
    [B_MSG_WOKE_UP] = STRINGID_PKMNWOKEUP,
    [B_MSG_WOKE_UP_UPROAR] = STRINGID_PKMNWOKEUPINUPROAR,
};

const u16 gSwallowFailStringIds[] = 
{
    [B_MSG_SWALLOW_FAILED] = STRINGID_FAILEDTOSWALLOW,
    [B_MSG_SWALLOW_FULL_HP] = STRINGID_PKMNHPFULL,
};

const u16 gUproarAwakeStringIds[] = 
{
    [B_MSG_CANT_SLEEP_UPROAR] = STRINGID_PKMNCANTSLEEPINUPROAR2,
    [B_MSG_UPROAR_KEPT_AWAKE] = STRINGID_UPROARKEPTPKMNAWAKE,
    [B_MSG_STAYED_AWAKE_USING] = STRINGID_PKMNSTAYEDAWAKEUSING,
};

const u16 gBurnUpStringIds[] = 
{
    [B_MSG_BURNUP_FIRE] = STRINGID_ATTACKERLOSTFIRETYPE,
    [B_MSG_BURNUP_ELECTRIC] = STRINGID_ATTACKERLOSTELECTRICTYPE,
    [B_MSG_BURNUP_DARK] = STRINGID_ATTACKERLOSTDARKTYPE,
};

const u16 gStatUpStringIds[] = 
{
    [B_MSG_ATTACKER_STAT_ROSE] = STRINGID_ATTACKERSSTATROSE,
    [B_MSG_DEFENDER_STAT_ROSE] = STRINGID_DEFENDERSSTATROSE,
    [B_MSG_STAT_WONT_INCREASE] = STRINGID_STATSWONTINCREASE,
    [B_MSG_STAT_ROSE_EMPTY] = STRINGID_EMPTYSTRING3,
    [B_MSG_STAT_ROSE_ITEM] = STRINGID_USINGITEMSTATOFPKMNROSE,
    [B_MSG_USED_DIRE_HIT] = STRINGID_PKMNUSEDXTOGETPUMPED,
};

const u16 gStatDownStringIds[] = 
{
    [B_MSG_ATTACKER_STAT_FELL] = STRINGID_ATTACKERSSTATFELL,
    [B_MSG_DEFENDER_STAT_FELL] = STRINGID_DEFENDERSSTATFELL,
    [B_MSG_STAT_WONT_DECREASE] = STRINGID_STATSWONTDECREASE,
    [B_MSG_STAT_FELL_EMPTY] = STRINGID_EMPTYSTRING3,
};

const u16 gFirstTurnOfTwoStringIds[] = 
{
    [B_MSG_TURN1_RAZOR_WIND] = STRINGID_PKMNWHIPPEDWHIRLWIND,
    [B_MSG_TURN1_SOLAR_BEAM] = STRINGID_PKMNTOOKSUNLIGHT,
    [B_MSG_TURN1_SKULL_BASH] = STRINGID_PKMNLOWEREDHEAD,
    [B_MSG_TURN1_SKY_ATTACK] = STRINGID_PKMNISGLOWING,
    [B_MSG_TURN1_FLY] = STRINGID_PKMNFLEWHIGH,
    [B_MSG_TURN1_DIG] = STRINGID_PKMNDUGHOLE,
    [B_MSG_TURN1_DIVE] = STRINGID_PKMNHIDUNDERWATER,
    [B_MSG_TURN1_BOUNCE] = STRINGID_PKMNSPRANGUP,
    [B_MSG_TURN1_PHANTOM_FORCE] = STRINGID_VANISHEDINSTANTLY,
    [B_MSG_TURN1_GEOMANCY] = STRINGID_PKNMABSORBINGPOWER,
    [B_MSG_TURN1_FREEZE_SHOCK] = STRINGID_CLOAKEDINAFREEZINGLIGHT,
    [B_MSG_TURN1_METEOR_BEAM] = STRINGID_OVERFLOWINGWITHSPACEPOWER,
    [B_MSG_TURN1_ELECTRO_SHOT] = STRINGID_ELECTRO_SHOT,
    [B_MSG_TURN1_ROCK_CLIMB] = STRINGID_ROCKCLIMB,
};

const u16 gWrappedStringIds[] = 
{
    [B_MSG_PKMNSQUEEZEDBYBIND] = STRINGID_PKMNSQUEEZEDBYBIND,
    [B_MSG_PKMNWRAPPEDBY] = STRINGID_PKMNWRAPPEDBY,
    [B_MSG_FIRESPINTRAP] = STRINGID_PKMNTRAPPEDINVORTEX,
    [B_MSG_PKMNCLAMPED] = STRINGID_PKMNCLAMPED,
    [B_MSG_PKMNTRAPPEDINVORTEX] = STRINGID_PKMNTRAPPEDINVORTEX,
    [B_MSG_PKMNTRAPPEDBYSANDTOMB] = STRINGID_PKMNTRAPPEDBYSANDTOMB,
    [B_MSG_TRAPPEDBYSWIRLINGMAGMA] = STRINGID_TRAPPEDBYSWIRLINGMAGMA,
    [B_MSG_INFESTATION] = STRINGID_INFESTATION,
    [B_MSG_THUNDER_CAGE] = STRINGID_THUNDER_CAGE,
    [B_MSG_SNAP_TRAP] = STRINGID_SNAP_TRAP,
};

const u16 gMistUsedStringIds[] = 
{
    [B_MSG_SET_MIST] = STRINGID_PKMNSHROUDEDINMIST,
    [B_MSG_MIST_FAILED] = STRINGID_BUTITFAILED,
};

const u16 gFocusEnergyUsedStringIds[] = 
{
    [B_MSG_GETTING_PUMPED] = STRINGID_PKMNGETTINGPUMPED,
    [B_MSG_FOCUS_ENERGY_FAILED] = STRINGID_BUTITFAILED,
};

const u16 gTransformUsedStringIds[] = 
{
    [B_MSG_TRANSFORMED] = STRINGID_PKMNTRANSFORMEDINTO,
    [B_MSG_TRANSFORM_FAILED] = STRINGID_BUTITFAILED,
};

const u16 gSubstituteUsedStringIds[] = 
{
    [B_MSG_SET_SUBSTITUTE] = STRINGID_PKMNMADESUBSTITUTE,
    [B_MSG_SUBSTITUTE_FAILED] = STRINGID_TOOWEAKFORSUBSTITUTE,
};

const u16 gGotPoisonedStringIds[] = 
{
    [B_MSG_STATUSED] = STRINGID_PKMNWASPOISONED,
    [B_MSG_STATUSED_BY_ABILITY] = STRINGID_PKMNPOISONEDBY,
};

const u16 gGotParalyzedStringIds[] = 
{
    [B_MSG_STATUSED] = STRINGID_PKMNWASPARALYZED,
    [B_MSG_STATUSED_BY_ABILITY] = STRINGID_PKMNWASPARALYZEDBY,
};

const u16 gFellAsleepStringIds[] = 
{
    [B_MSG_STATUSED] = STRINGID_PKMNFELLASLEEP,
    [B_MSG_STATUSED_BY_ABILITY] = STRINGID_PKMNMADESLEEP,
};

const u16 gGotBurnedStringIds[] = 
{
    [B_MSG_STATUSED] = STRINGID_PKMNWASBURNED,
    [B_MSG_STATUSED_BY_ABILITY] = STRINGID_PKMNBURNEDBY,
};

const u16 gGotFrostbiteStringIds[] = 
{
    [B_MSG_STATUSED] = STRINGID_PKMNGOTFROSTBITE,
    [B_MSG_STATUSED_BY_ABILITY] = STRINGID_PKMNFROSTBITTENBY,
};

const u16 gFrostbiteHealedStringIds[] = 
{
    [B_MSG_FROSTBITE_HEALED] = STRINGID_PKMNFROSTBITEHEALED2,
    [B_MSG_FROSTBITE_HEALED_BY_MOVE] = STRINGID_PKMNFROSTBITEHEALEDBY,
};

const u16 gBleedStartedStringIds[] = 
{
    [B_MSG_STATUSED] = STRINGID_PKMNSTARTBLEED,
    [B_MSG_STATUSED_BY_ABILITY] = STRINGID_PKMNBLEDBY,
};

const u16 gBleedHealedStringIds[] = 
{
    [B_MSG_BLEED_HEALED] = STRINGID_PKMNBLEEDHEALED2,
    [B_MSG_BLEED_HEALED_BY_MOVE] = STRINGID_PKMNBLEEDHEALEDBY,
};

const u16 gGotFrozenStringIds[] = 
{
    [B_MSG_STATUSED] = STRINGID_PKMNWASFROZEN,
    [B_MSG_STATUSED_BY_ABILITY] = STRINGID_PKMNFROZENBY,
};

const u16 gGotDefrostedStringIds[] = 
{
    [B_MSG_DEFROSTED] = STRINGID_PKMNWASDEFROSTED2,
    [B_MSG_DEFROSTED_BY_MOVE] = STRINGID_PKMNWASDEFROSTEDBY,
};

const u16 gKOFailedStringIds[] = 
{
    [B_MSG_KO_MISS] = STRINGID_ATTACKMISSED,
    [B_MSG_KO_UNAFFECTED] = STRINGID_PKMNUNAFFECTED,
};

const u16 gAttractUsedStringIds[] = 
{
    [B_MSG_STATUSED] = STRINGID_PKMNFELLINLOVE,
    [B_MSG_STATUSED_BY_ABILITY] = STRINGID_PKMNSXINFATUATEDY,
};

const u16 gAbsorbDrainStringIds[] = 
{
    [B_MSG_ABSORB] = STRINGID_PKMNENERGYDRAINED,
    [B_MSG_ABSORB_OOZE] = STRINGID_ITSUCKEDLIQUIDOOZE,
};

const u16 gSportsUsedStringIds[] = 
{
    [B_MSG_WEAKEN_ELECTRIC] = STRINGID_ELECTRICITYWEAKENED,
    [B_MSG_WEAKEN_FIRE] = STRINGID_FIREWEAKENED,
};

const u16 gPartyStatusHealStringIds[] = 
{
    [B_MSG_BELL] = STRINGID_BELLCHIMED,
    [B_MSG_BELL_SOUNDPROOF_ATTACKER] = STRINGID_BELLCHIMED,
    [B_MSG_BELL_SOUNDPROOF_PARTNER] = STRINGID_BELLCHIMED,
    [B_MSG_BELL_BOTH_SOUNDPROOF] = STRINGID_BELLCHIMED,
    [B_MSG_SOOTHING_AROMA] = STRINGID_SOOTHINGAROMA,
};

const u16 gFutureMoveUsedStringIds[] = 
{
    [B_MSG_FUTURE_SIGHT] = STRINGID_PKMNFORESAWATTACK,
    [B_MSG_DOOM_DESIRE] = STRINGID_PKMNCHOSEXASDESTINY,
};

const u16 gInobedientStringIds[] = 
{
    [B_MSG_LOAFING] = STRINGID_PKMNLOAFING,
    [B_MSG_WONT_OBEY] = STRINGID_PKMNWONTOBEY,
    [B_MSG_TURNED_AWAY] = STRINGID_PKMNTURNEDAWAY,
    [B_MSG_PRETEND_NOT_NOTICE] = STRINGID_PKMNPRETENDNOTNOTICE,
    [B_MSG_INCAPABLE_OF_POWER] = STRINGID_PKMNINCAPABLEOFPOWER,
};

const u16 gSafariGetNearStringIds[] = 
{
    [B_MSG_CREPT_CLOSER] = STRINGID_CREPTCLOSER,
    [B_MSG_CANT_GET_CLOSER] = STRINGID_CANTGETCLOSER,
};

const u16 gSafariPokeblockResultStringIds[] = 
{
    [B_MSG_MON_CURIOUS] = STRINGID_PKMNCURIOUSABOUTX,
    [B_MSG_MON_ENTHRALLED] = STRINGID_PKMNENTHRALLEDBYX,
    [B_MSG_MON_IGNORED] = STRINGID_PKMNIGNOREDX,
};

const u16 gBerryEffectStringIds[] = 
{
    [B_MSG_CURED_PROBLEM] = STRINGID_PKMNSITEMCUREDPROBLEM,
    [B_MSG_NORMALIZED_STATUS] = STRINGID_PKMNSITEMNORMALIZEDSTATUS,
};

const u16 gBRNPreventionStringIds[] = 
{
    [B_MSG_ABILITY_PREVENTS_MOVE_STATUS] = STRINGID_PKMNSXPREVENTSBURNS,
    [B_MSG_ABILITY_PREVENTS_ABILITY_STATUS] = STRINGID_PKMNSXPREVENTSYSZ,
    [B_MSG_STATUS_HAD_NO_EFFECT] = STRINGID_PKMNSXHADNOEFFECTONY,
};

const u16 gPRLZPreventionStringIds[] = 
{
    [B_MSG_ABILITY_PREVENTS_MOVE_STATUS] = STRINGID_PKMNPREVENTSPARALYSISWITH,
    [B_MSG_ABILITY_PREVENTS_ABILITY_STATUS] = STRINGID_PKMNSXPREVENTSYSZ,
    [B_MSG_STATUS_HAD_NO_EFFECT] = STRINGID_PKMNSXHADNOEFFECTONY,
};

const u16 gPSNPreventionStringIds[] = 
{
    [B_MSG_ABILITY_PREVENTS_MOVE_STATUS] = STRINGID_PKMNPREVENTSPOISONINGWITH,
    [B_MSG_ABILITY_PREVENTS_ABILITY_STATUS] = STRINGID_PKMNSXPREVENTSYSZ,
    [B_MSG_STATUS_HAD_NO_EFFECT] = STRINGID_PKMNSXHADNOEFFECTONY,
};

const u16 gItemSwapStringIds[] = 
{
    [B_MSG_ITEM_SWAP_TAKEN] = STRINGID_PKMNOBTAINEDX,
    [B_MSG_ITEM_SWAP_GIVEN] = STRINGID_PKMNOBTAINEDX2,
    [B_MSG_ITEM_SWAP_BOTH] = STRINGID_PKMNOBTAINEDXYOBTAINEDZ,
};

const u16 gFlashFireStringIds[] = 
{
    [B_MSG_FLASH_FIRE_BOOST] = STRINGID_PKMNRAISEDFIREPOWERWITH,
    [B_MSG_FLASH_FIRE_NO_BOOST] = STRINGID_PKMNSXMADEYINEFFECTIVE,
};

const u16 gCaughtMonStringIds[] = 
{
    [B_MSG_SENT_SOMEONES_PC] = STRINGID_PKMNTRANSFERREDSOMEONESPC,
    [B_MSG_SENT_LANETTES_PC] = STRINGID_PKMNTRANSFERREDLANETTESPC,
    [B_MSG_SOMEONES_BOX_FULL] = STRINGID_PKMNBOXSOMEONESPCFULL,
    [B_MSG_LANETTES_BOX_FULL] = STRINGID_PKMNBOXLANETTESPCFULL,
};

const u16 gRoomsStringIds[] = 
{
    [B_MSG_TRICKROOMSTARTS] = STRINGID_PKMNTWISTEDDIMENSIONS,
    [B_MSG_TRICKROOMENDS] = STRINGID_TRICKROOMENDS,
    [B_MSG_WONDERROOMSTARTS] = STRINGID_SWAPSDEFANDSPDEFOFALLPOKEMON,
    [B_MSG_WONDERROOMENDS] = STRINGID_WONDERROOMENDS,
    [B_MSG_MAGICROOMSTARTS] = STRINGID_HELDITEMSLOSEEFFECTS,
    [B_MSG_MAGICROOMENDS] = STRINGID_MAGICROOMENDS,
    [B_MSG_INVERSEROOMSTARTS] = STRINGID_INVERSEROOMACTIVATED,
    [B_MSG_INVERSEROOMENDS] = STRINGID_INVERSEROOMENDS,
    [B_MSG_ROOMEMPTYSTRING] = STRINGID_EMPTYSTRING3,
};

const u16 gStatusConditionsStringIds[] = 
{
    [B_MSG_PKMNWASPOISONED] = STRINGID_PKMNWASPOISONED,
    [B_MSG_PKMNBADLYPOISONED] = STRINGID_PKMNBADLYPOISONED,
    [B_MSG_PKMNWASBURNED] = STRINGID_PKMNWASBURNED,
    [B_MSG_PKMNWASPARALYZED] = STRINGID_PKMNWASPARALYZED,
    [B_MSG_PKMNFELLASLEEP] = STRINGID_PKMNFELLASLEEP,
    [B_MSG_PKMNGOTFROSTBITE] = STRINGID_PKMNGOTFROSTBITE,
    [B_MSG_PKMNSTARTBLEED] = STRINGID_PKMNSTARTBLEED,
};

const u16 gCantSelectMove[] = 
{
    [B_MSG_CANTSELECTTWICE] = STRINGID_CANT_USE_TWICE_IN_A_ROW,
};

const u16 gBattlePalaceFlavorTextTable[] = 
{
    [B_MSG_GLINT_IN_EYE] = STRINGID_GLINTAPPEARSINEYE,
    [B_MSG_GETTING_IN_POS] = STRINGID_PKMNGETTINGINTOPOSITION,
    [B_MSG_GROWL_DEEPLY] = STRINGID_PKMNBEGANGROWLINGDEEPLY,
    [B_MSG_EAGER_FOR_MORE] = STRINGID_PKMNEAGERFORMORE,
};

const u8* const gRefereeStringsTable[] = 
{
    [B_MSG_REF_NOTHING_IS_DECIDED] = sText_RefIfNothingIsDecided,
    [B_MSG_REF_THATS_IT] = sText_RefThatsIt,
    [B_MSG_REF_JUDGE_MIND] = sText_RefJudgeMind,
    [B_MSG_REF_JUDGE_SKILL] = sText_RefJudgeSkill,
    [B_MSG_REF_JUDGE_BODY] = sText_RefJudgeBody,
    [B_MSG_REF_PLAYER_WON] = sText_RefPlayerWon,
    [B_MSG_REF_OPPONENT_WON] = sText_RefOpponentWon,
    [B_MSG_REF_DRAW] = sText_RefDraw,
    [B_MSG_REF_COMMENCE_BATTLE] = sText_RefCommenceBattle,
};

const u16 gStealthRocksSet[] = 
{
    [B_MSG_STEALTH_ROCK_SET] = STRINGID_POINTEDSTONESFLOAT,
    [B_MSG_CREEPING_THORNS_SET] = STRINGID_VICIOUSTHORNSUSED,
};

const u16 gStealthRockFree[] = 
{
    [B_MSG_STEALTH_ROCK_FREE] = STRINGID_PKMNBLEWAWAYSTEALTHROCK,
    [B_MSG_CREEPING_THORNS_FREE] = STRINGID_BLEWAWAYCREEPINGTHORNS,
};

const u16 gWeatherCleared[] = 
{
    [B_MSG_HARSH_SUNLIGHT_CANT_END] = STRINGID_EXTREMELYHARSHSUNLIGHTWASNOTLESSENED,
    [B_MSG_HEAVY_RAIN_WONT_END] = STRINGID_NORELIEFROMHEAVYRAIN,
    [B_MSG_WIND_WONT_END] = STRINGID_MYSTERIOUSAIRCURRENTBLOWSON,
    [B_MSG_SUN_ENDS] = STRINGID_SUNLIGHTFADED,
    [B_MSG_RAIN_ENDS] = STRINGID_RAINSTOPPED,
    [B_MSG_SAND_ENDS] = STRINGID_SANDSTORMSUBSIDED,
    [B_MSG_HAIL_ENDS] = STRINGID_HAILSTOPPED,
    [B_MSG_REMOVE_WEATHER_FAILED] = STRINGID_BUTITFAILED,
};

const u16 gStatusAnnounce[] = 
{
    [B_MSG_SALT_CURE] = STRINGID_AFFLICTED_WITH_SALT_CURE,
    [B_MSG_SYRUP] = STRINGID_AFFLICTED_WITH_SYRUP,
    [B_MSG_SMOKESCREEN] = STRINGID_SMOKESCREEN,
};

const u16 gPrimalEvolutionAnnouncement[] = 
{
    [B_MSG_PRIMAL_REVERSION] = STRINGID_PKMNREVERTEDTOPRIMAL,
    [B_MSG_ORIGIN_REVERSION] = STRINGID_ORIGIN_REVERSION,
    [B_MSG_CROWNED_REVERSION] = STRINGID_CROWNED_REVERSION,
};

const u16 gCritRaisedStrings[] = 
{
    [B_MSG_CRIT_NO_INCREASE] = STRINGID_CRIT_CANT_INCREASE,
    [B_MSG_CRIT_INCREASE_1] = STRINGID_CRIT_PLUS_1,
    [B_MSG_CRIT_INCREASE_2] = STRINGID_CRIT_PLUS_2,
    [B_MSG_CRIT_INCREASE_3] = STRINGID_CRIT_PLUS_3,
};

const u16 gFaintMonMessage[] = 
{
    [B_MSG_MON_FAINTED] = STRINGID_ATTACKERFAINTED,
    [B_MSG_FADE_OUT] = STRINGID_RECURRING_NIGHTMARE,
    [B_MSG_BACKUP_POWER] = STRINGID_BACKUP_POWER,
};

const u16 gRecoilMessage[] = 
{
    [B_MSG_RECOIL_NORMAL] = STRINGID_PKMNHITWITHRECOIL,
    [B_MSG_RECOIL_ENRAGED] = STRINGID_ENRAGE_HURT,
    [B_MSG_RECOIL_STRAIN] = STRINGID_SUPER_STRAIN_RECOIL,
};

// END OF GENERATED

const u16 gBallEscapeStringIds[] = {[BALL_NO_SHAKES] = STRINGID_PKMNBROKEFREE,
                                    [BALL_1_SHAKE] = STRINGID_ITAPPEAREDCAUGHT,
                                    [BALL_2_SHAKES] = STRINGID_AARGHALMOSTHADIT,
                                    [BALL_3_SHAKES_FAIL] = STRINGID_SHOOTSOCLOSE};

const u8 *const gStatNamesTable[NUM_BATTLE_STATS] = {
    gText_HP3, gText_Attack, gText_Defense, gText_Speed, gText_SpAtk, gText_SpDef, sText_Accuracy, sText_Evasiveness};

const u8 *const gPokeblockWasTooXStringTable[FLAVOR_COUNT] = {[FLAVOR_SPICY] = sText_PokeblockWasTooSpicy,
                                                              [FLAVOR_DRY] = sText_PokeblockWasTooDry,
                                                              [FLAVOR_SWEET] = sText_PokeblockWasTooSweet,
                                                              [FLAVOR_BITTER] = sText_PokeblockWasTooBitter,
                                                              [FLAVOR_SOUR] = sText_PokeblockWasTooSour};

// Overworld weathers that don't have an associated battle weather default to "It is raining."
const u16 gWeatherStartsStringIds[] = {[WEATHER_NONE] = STRINGID_ITISRAINING,
                                       [WEATHER_SUNNY_CLOUDS] = STRINGID_ITISRAINING,
                                       [WEATHER_SUNNY] = STRINGID_ITISRAINING,
                                       [WEATHER_RAIN] = STRINGID_ITISRAINING,
                                       [WEATHER_SNOW] = STRINGID_ITISRAINING,
                                       [WEATHER_RAIN_THUNDERSTORM] = STRINGID_ITISRAINING,
                                       [WEATHER_FOG_HORIZONTAL] = STRINGID_FOGISDEEP,
                                       [WEATHER_VOLCANIC_ASH] = STRINGID_ITISRAINING,
                                       [WEATHER_SANDSTORM] = STRINGID_SANDSTORMISRAGING,
                                       [WEATHER_FOG_DIAGONAL] = STRINGID_FOGISDEEP,
                                       [WEATHER_UNDERWATER] = STRINGID_ITISRAINING,
                                       [WEATHER_SHADE] = STRINGID_ITISRAINING,
                                       [WEATHER_DROUGHT] = STRINGID_SUNLIGHTSTRONG,
                                       [WEATHER_DOWNPOUR] = STRINGID_ITISRAINING,
                                       [WEATHER_UNDERWATER_BUBBLES] = STRINGID_ITISRAINING,
                                       [WEATHER_ABNORMAL] = STRINGID_ITISRAINING};

const u16 gTrainerItemCuredStatusStringIds[] = {[AI_HEAL_CONFUSION] = STRINGID_PKMNSITEMSNAPPEDOUT,
                                                [AI_HEAL_PARALYSIS] = STRINGID_PKMNSITEMCUREDPARALYSIS,
                                                [AI_HEAL_FREEZE] = STRINGID_PKMNSITEMDEFROSTEDIT,
                                                [AI_HEAL_BURN] = STRINGID_PKMNSITEMHEALEDBURN,
                                                [AI_HEAL_POISON] = STRINGID_PKMNSITEMCUREDPOISON,
                                                [AI_HEAL_SLEEP] = STRINGID_PKMNSITEMWOKEIT};

const u8 *const gRoundsStringTable[DOME_ROUNDS_COUNT] = {
    [DOME_ROUND1] = sText_Round1, [DOME_ROUND2] = sText_Round2, [DOME_SEMIFINAL] = sText_Semifinal, [DOME_FINAL] = sText_Final};

// Unused
static const u8 *const sStatNamesTable2[] = {gText_HP3, gText_SpAtk, gText_Attack, gText_SpDef, gText_Defense, gText_Speed};

// For displaying names of invalid moves
static const u8 sATypeMove_Table[][NUMBER_OF_MON_TYPES - 1] = {
    [TYPE_NORMAL] _("一般属性招式"), [TYPE_FIGHTING] _("格斗属性招式"),  [TYPE_FLYING] _("飞行属性招式"),   [TYPE_POISON] _("毒属性招式"),
    [TYPE_GROUND] _("地面属性招式"), [TYPE_ROCK] _("岩石属性招式"),          [TYPE_BUG] _("虫属性招式"),         [TYPE_GHOST] _("幽灵属性招式"),
    [TYPE_STEEL] _("钢属性招式"),   [TYPE_MYSTERY] _("？？？属性招式"),        [TYPE_FIRE] _("火属性招式"),       [TYPE_WATER] _("水属性招式"),
    [TYPE_GRASS] _("草属性招式"),   [TYPE_ELECTRIC] _("电属性招式"), [TYPE_PSYCHIC] _("超能属性招式"), [TYPE_ICE] _("冰属性招式"),
    [TYPE_DRAGON] _("龙属性招式"), [TYPE_DARK] _("恶属性招式"),          [TYPE_FAIRY] _("妖精属性招式"),     [TYPE_STELLAR] _("恒星属性招式"),
};

// This is four lists of moves which use a different attack string in Japanese
// to the default. See the documentation for ChooseTypeOfMoveUsedString for more detail.
static const u16 sGrammarMoveUsedTable[] = {MOVE_SWORDS_DANCE,
                                            MOVE_STRENGTH,
                                            MOVE_GROWTH,
                                            MOVE_HARDEN,
                                            MOVE_MINIMIZE,
                                            MOVE_SMOKESCREEN,
                                            MOVE_WITHDRAW,
                                            MOVE_DEFENSE_CURL,
                                            MOVE_EGG_BOMB,
                                            MOVE_SMOG,
                                            MOVE_BONE_CLUB,
                                            MOVE_FLASH,
                                            MOVE_SPLASH,
                                            MOVE_ACID_ARMOR,
                                            MOVE_BONEMERANG,
                                            MOVE_REST,
                                            MOVE_SHARPEN,
                                            MOVE_SUBSTITUTE,
                                            MOVE_MIND_READER,
                                            MOVE_SNORE,
                                            MOVE_PROTECT,
                                            MOVE_SPIKES,
                                            MOVE_ENDURE,
                                            MOVE_ROLLOUT,
                                            MOVE_SWAGGER,
                                            MOVE_SLEEP_TALK,
                                            MOVE_HIDDEN_POWER,
                                            MOVE_PSYCH_UP,
                                            MOVE_EXTREME_SPEED,
                                            MOVE_FOLLOW_ME,
                                            MOVE_TRICK,
                                            MOVE_ASSIST,
                                            MOVE_INGRAIN,
                                            MOVE_KNOCK_OFF,
                                            MOVE_CAMOUFLAGE,
                                            MOVE_ASTONISH,
                                            MOVE_ODOR_SLEUTH,
                                            MOVE_GRASS_WHISTLE,
                                            MOVE_SHEER_COLD,
                                            MOVE_MUDDY_WATER,
                                            MOVE_IRON_DEFENSE,
                                            MOVE_BOUNCE,
                                            0,

                                            MOVE_TELEPORT,
                                            MOVE_RECOVER,
                                            MOVE_BIDE,
                                            MOVE_AMNESIA,
                                            MOVE_FLAIL,
                                            MOVE_TAUNT,
                                            MOVE_BULK_UP,
                                            0,

                                            MOVE_MEDITATE,
                                            MOVE_AGILITY,
                                            MOVE_MIMIC,
                                            MOVE_DOUBLE_TEAM,
                                            MOVE_BARRAGE,
                                            MOVE_TRANSFORM,
                                            MOVE_STRUGGLE,
                                            MOVE_SCARY_FACE,
                                            MOVE_CHARGE,
                                            MOVE_WISH,
                                            MOVE_BRICK_BREAK,
                                            MOVE_YAWN,
                                            MOVE_FEATHER_DANCE,
                                            MOVE_TEETER_DANCE,
                                            MOVE_MUD_SPORT,
                                            MOVE_FAKE_TEARS,
                                            MOVE_WATER_SPORT,
                                            MOVE_CALM_MIND,
                                            0,

                                            MOVE_POUND,
                                            MOVE_SCRATCH,
                                            MOVE_VISE_GRIP,
                                            MOVE_WING_ATTACK,
                                            MOVE_FLY,
                                            MOVE_BIND,
                                            MOVE_SLAM,
                                            MOVE_HORN_ATTACK,
                                            MOVE_WRAP,
                                            MOVE_THRASH,
                                            MOVE_TAIL_WHIP,
                                            MOVE_LEER,
                                            MOVE_BITE,
                                            MOVE_GROWL,
                                            MOVE_ROAR,
                                            MOVE_SING,
                                            MOVE_PECK,
                                            MOVE_ABSORB,
                                            MOVE_STRING_SHOT,
                                            MOVE_EARTHQUAKE,
                                            MOVE_FISSURE,
                                            MOVE_DIG,
                                            MOVE_TOXIC,
                                            MOVE_SCREECH,
                                            MOVE_METRONOME,
                                            MOVE_LICK,
                                            MOVE_CLAMP,
                                            MOVE_CONSTRICT,
                                            MOVE_POISON_GAS,
                                            MOVE_BUBBLE,
                                            MOVE_SLASH,
                                            MOVE_SPIDER_WEB,
                                            MOVE_NIGHTMARE,
                                            MOVE_CURSE,
                                            MOVE_FORESIGHT,
                                            MOVE_CHARM,
                                            MOVE_ATTRACT,
                                            MOVE_ROCK_SMASH,
                                            MOVE_UPROAR,
                                            MOVE_SPIT_UP,
                                            MOVE_SWALLOW,
                                            MOVE_TORMENT,
                                            MOVE_FLATTER,
                                            MOVE_ROLE_PLAY,
                                            MOVE_ENDEAVOR,
                                            MOVE_TICKLE,
                                            MOVE_COVET,
                                            0};

static const u8 sDummyWeirdStatusString[] = {EOS, EOS, EOS, EOS, EOS, EOS, EOS, EOS, 0, 0};

// Testo
static const struct BattleWindowText sTextOnWindowsInfo_Normal[] = {
    // The corresponding WindowTemplate is gStandardBattleWindowTemplates[] within src/battle_bg.c
    [B_WIN_MSG] =
        {
            .fillValue = PIXEL_FILL(0x3),
            .fontId = FONT_NARROW,
            .x = 0,
            .y = 1,
            .speed = 0,
            .fgColor = 10,
            .bgColor = 3,
            .shadowColor = 0,
        },
    [B_WIN_ACTION_PROMPT] =
        {
            .fillValue = PIXEL_FILL(0x3),
            .fontId = FONT_NORMAL,
            .x = 1,
            .y = 1,
            .speed = 0,
            .fgColor = 10,
            .bgColor = 3,
            .shadowColor = 8,
        },
    [B_WIN_ACTION_MENU] =
        {
            .fillValue = PIXEL_FILL(0x3),
            .fontId = FONT_NORMAL,
            .x = 0,
            .y = 1,
            .speed = 0,
            .fgColor = 10,
            .bgColor = 3,
            .shadowColor = 8,
        },
    [B_WIN_SWITCH_PROMPT] =
        {
            .fillValue = PIXEL_FILL(0x3),
            .fontId = FONT_NARROW,
            .x = 0,
            .y = 1,
            .speed = 0,
            .fgColor = 10,
            .bgColor = 3,
            .shadowColor = 8,
        },
    [B_WIN_YESNO] =
        {
            .fillValue = PIXEL_FILL(0x3),
            .fontId = FONT_NARROW,
            .x = 0,
            .y = 1,
            .speed = 0,
            .fgColor = 10,
            .bgColor = 0,
            .shadowColor = 0,
            .letterSpacing = 4,
        },
    [B_WIN_YESNO_TWO] =
        {
            .fillValue = PIXEL_FILL(0x3),
            .fontId = FONT_NARROW,
            .x = 0,
            .y = 1,
            .speed = 0,
            .fgColor = 10,
            .bgColor = 0,
            .shadowColor = 0,
            .letterSpacing = 4,
        },
    [B_WIN_LEVEL_UP_BOX] =
        {
            .fillValue = PIXEL_FILL(0xE),
            .fontId = FONT_NORMAL,
            .x = 0,
            .y = 1,
            .speed = 0,
            .fgColor = 13,
            .bgColor = 14,
            .shadowColor = 15,
        },
    [B_WIN_LEVEL_UP_BANNER] =
        {
            .fillValue = PIXEL_FILL(0),
            .fontId = FONT_NORMAL,
            .x = 32,
            .y = 1,
            .speed = 0,
            .fgColor = 1,
            .shadowColor = 2,
        },
    [B_WIN_VS_PLAYER] =
        {
            .fillValue = PIXEL_FILL(0xE),
            .fontId = FONT_NORMAL,
            .x = -1,
            .y = 1,
            .speed = 0,
            .fgColor = 13,
            .bgColor = 14,
            .shadowColor = 15,
        },
    [B_WIN_VS_OPPONENT] =
        {
            .fillValue = PIXEL_FILL(0xE),
            .fontId = FONT_NORMAL,
            .x = -1,
            .y = 1,
            .speed = 0,
            .fgColor = 13,
            .bgColor = 14,
            .shadowColor = 15,
        },
    [B_WIN_VS_MULTI_PLAYER_1] =
        {
            .fillValue = PIXEL_FILL(0xE),
            .fontId = FONT_NORMAL,
            .x = -1,
            .y = 1,
            .speed = 0,
            .fgColor = 13,
            .bgColor = 14,
            .shadowColor = 15,
        },
    [B_WIN_VS_MULTI_PLAYER_2] =
        {
            .fillValue = PIXEL_FILL(0xE),
            .fontId = FONT_NORMAL,
            .x = -1,
            .y = 1,
            .speed = 0,
            .fgColor = 13,
            .bgColor = 14,
            .shadowColor = 15,
        },
    [B_WIN_VS_MULTI_PLAYER_3] =
        {
            .fillValue = PIXEL_FILL(0xE),
            .fontId = FONT_NORMAL,
            .x = -1,
            .y = 1,
            .speed = 0,
            .fgColor = 13,
            .bgColor = 14,
            .shadowColor = 15,
        },
    [B_WIN_VS_MULTI_PLAYER_4] =
        {
            .fillValue = PIXEL_FILL(0xE),
            .fontId = FONT_NORMAL,
            .x = -1,
            .y = 1,
            .speed = 0,
            .fgColor = 13,
            .bgColor = 14,
            .shadowColor = 15,
        },
    [B_WIN_VS_OUTCOME_DRAW] =
        {
            .fillValue = PIXEL_FILL(0),
            .fontId = FONT_NORMAL,
            .x = -1,
            .y = 1,
            .speed = 0,
            .fgColor = 1,
            .shadowColor = 6,
        },
    [B_WIN_VS_OUTCOME_LEFT] =
        {
            .fillValue = PIXEL_FILL(0),
            .fontId = FONT_NORMAL,
            .x = -1,
            .y = 1,
            .speed = 0,
            .fgColor = 1,
            .shadowColor = 6,
        },
    [B_WIN_VS_OUTCOME_RIGHT] =
        {
            .fillValue = PIXEL_FILL(0x0),
            .fontId = FONT_NORMAL,
            .x = -1,
            .y = 1,
            .speed = 0,
            .fgColor = 1,
            .shadowColor = 6,
        },
    [B_WIN_SUPER_EFFECTIVE] =
        {
            .fillValue = PIXEL_FILL(0xE),
            .fontId = 7,
            .x = 0,
            .y = 1,
            .fgColor = 6,
            .bgColor = 14,
            .shadowColor = 5,
        },
    [B_WIN_NOT_VERY_EFFECTIVE] =
        {
            .fillValue = PIXEL_FILL(0xE),
            .fontId = 7,
            .x = 0,
            .y = 1,
            .fgColor = 1,
            .bgColor = 14,
            .shadowColor = 3,
        },
    [B_WIN_NO_EFFECT] =
        {
            .fillValue = PIXEL_FILL(0xE),
            .fontId = 7,
            .x = 0,
            .y = 1,
            .fgColor = 11,
            .bgColor = 14,
            .shadowColor = 11,
        },
};

static const struct BattleWindowText sTextOnWindowsInfo_Arena[] = {
    [B_WIN_MSG] =
        {
            .fillValue = PIXEL_FILL(0x4),
            .fontId = FONT_NORMAL,
            .x = 0,
            .y = 1,
            .speed = 1,
            .fgColor = 1,
            .bgColor = 15,
            .shadowColor = 6,
        },
    [B_WIN_ACTION_PROMPT] =
        {
            .fillValue = PIXEL_FILL(0x4),
            .fontId = FONT_NORMAL,
            .x = 1,
            .y = 1,
            .speed = 0,
            .fgColor = 1,
            .bgColor = 15,
            .shadowColor = 6,
        },
    [B_WIN_ACTION_MENU] =
        {
            .fillValue = PIXEL_FILL(0xE),
            .fontId = FONT_NORMAL,
            .x = 0,
            .y = 1,
            .speed = 0,
            .fgColor = 13,
            .bgColor = 14,
            .shadowColor = 15,
        },
    [B_WIN_SWITCH_PROMPT] =
        {
            .fillValue = PIXEL_FILL(0xE),
            .fontId = FONT_NARROW,
            .x = 0,
            .y = 1,
            .speed = 0,
            .fgColor = 13,
            .bgColor = 14,
            .shadowColor = 15,
        },
    [B_WIN_YESNO] =
        {
            .fillValue = PIXEL_FILL(0x3),
            .fontId = FONT_NORMAL,
            .x = 0,
            .y = 1,
            .speed = 0,
            .fgColor = 10,
            .bgColor = 0,
            .shadowColor = 0,
        },
    [B_WIN_YESNO_TWO] =
        {
            .fillValue = PIXEL_FILL(0x3),
            .fontId = FONT_NORMAL,
            .x = 0,
            .y = 1,
            .speed = 0,
            .fgColor = 10,
            .bgColor = 0,
            .shadowColor = 0,
        },
    [B_WIN_LEVEL_UP_BOX] =
        {
            .fillValue = PIXEL_FILL(0xE),
            .fontId = FONT_NORMAL,
            .x = 0,
            .y = 1,
            .speed = 0,
            .fgColor = 13,
            .bgColor = 14,
            .shadowColor = 15,
        },
    [B_WIN_LEVEL_UP_BANNER] =
        {
            .fillValue = PIXEL_FILL(0),
            .fontId = FONT_NORMAL,
            .x = 32,
            .y = 1,
            .speed = 0,
            .fgColor = 1,
            .shadowColor = 2,
        },
    [ARENA_WIN_PLAYER_NAME] =
        {
            .fillValue = PIXEL_FILL(0xE),
            .fontId = FONT_NORMAL,
            .x = -1,
            .y = 1,
            .speed = 0,
            .fgColor = 1,
            .bgColor = 14,
            .shadowColor = 15,
        },
    [ARENA_WIN_VS] =
        {
            .fillValue = PIXEL_FILL(0xE),
            .fontId = FONT_NORMAL,
            .x = -1,
            .y = 1,
            .speed = 0,
            .fgColor = 13,
            .bgColor = 14,
            .shadowColor = 15,
        },
    [ARENA_WIN_OPPONENT_NAME] =
        {
            .fillValue = PIXEL_FILL(0xE),
            .fontId = FONT_NORMAL,
            .x = -1,
            .y = 1,
            .speed = 0,
            .fgColor = 13,
            .bgColor = 14,
            .shadowColor = 15,
        },
    [ARENA_WIN_MIND] =
        {
            .fillValue = PIXEL_FILL(0xE),
            .fontId = FONT_NORMAL,
            .x = -1,
            .y = 1,
            .speed = 0,
            .fgColor = 13,
            .bgColor = 14,
            .shadowColor = 15,
        },
    [ARENA_WIN_SKILL] =
        {
            .fillValue = PIXEL_FILL(0xE),
            .fontId = FONT_NORMAL,
            .x = -1,
            .y = 1,
            .speed = 0,
            .fgColor = 13,
            .bgColor = 14,
            .shadowColor = 15,
        },
    [ARENA_WIN_BODY] =
        {
            .fillValue = PIXEL_FILL(0xE),
            .fontId = FONT_NORMAL,
            .x = -1,
            .y = 1,
            .speed = 0,
            .fgColor = 13,
            .bgColor = 14,
            .shadowColor = 15,
        },
    [ARENA_WIN_JUDGMENT_TITLE] =
        {
            .fillValue = PIXEL_FILL(0xE),
            .fontId = FONT_NORMAL,
            .x = -1,
            .y = 1,
            .speed = 0,
            .fgColor = 13,
            .bgColor = 14,
            .shadowColor = 15,
        },
    [ARENA_WIN_JUDGMENT_TEXT] =
        {
            .fillValue = PIXEL_FILL(0x1),
            .fontId = FONT_NORMAL,
            .x = 0,
            .y = 1,
            .speed = 1,
            .fgColor = 2,
            .bgColor = 1,
            .shadowColor = 3,
        },
};

static const struct BattleWindowText *const sBattleTextOnWindowsInfo[] = {sTextOnWindowsInfo_Normal, sTextOnWindowsInfo_Arena};

static const u8 sRecordedBattleTextSpeeds[] = {8, 4, 1, 0};

// code
void BufferStringBattle(u16 stringID) {
    s32 i;
    const u8 *stringPtr = NULL;

    gBattleMsgDataPtr = (struct BattleMsgData *)(&gBattleResources->bufferA[gActiveBattler][4]);
    gLastUsedItem = gBattleMsgDataPtr->lastItem;
    gBattleScripting.abilityPopupOverwrite = gBattleMsgDataPtr->lastAbility;
    gBattleScripting.battler = gBattleMsgDataPtr->scrActive;
    gBattleStruct->field_52 = gBattleMsgDataPtr->unk1605E;
    gBattleStruct->hpScale = gBattleMsgDataPtr->hpScale;
    gPotentialItemEffectBattler = gBattleMsgDataPtr->itemEffectBattler;
    gBattleStruct->stringMoveType = gBattleMsgDataPtr->moveType;

    for (i = 0; i < MAX_BATTLERS_COUNT; i++) {
        sBattlerAbilities[i] = gBattleMsgDataPtr->abilities[i];
    }
    for (i = 0; i < TEXT_BUFF_ARRAY_COUNT; i++) {
        gBattleTextBuff1[i] = gBattleMsgDataPtr->textBuffs[0][i];
        gBattleTextBuff2[i] = gBattleMsgDataPtr->textBuffs[1][i];
        gBattleTextBuff3[i] = gBattleMsgDataPtr->textBuffs[2][i];
        gBattleTextBuff4[i] = gBattleMsgDataPtr->textBuffs[3][i];
    }

    switch (stringID) {
        case STRINGID_INTROMSG:  // first battle msg
            if (gBattleTypeFlags & BATTLE_TYPE_TRAINER) {
                if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED_LINK)) {
                    if (gBattleTypeFlags & BATTLE_TYPE_TOWER_LINK_MULTI) {
                        stringPtr = sText_TwoTrainersWantToBattle;
                    } else if (gBattleTypeFlags & BATTLE_TYPE_MULTI) {
                        if (gBattleTypeFlags & BATTLE_TYPE_RECORDED)
                            stringPtr = sText_TwoLinkTrainersWantToBattlePause;
                        else
                            stringPtr = sText_TwoLinkTrainersWantToBattle;
                    } else {
                        if (gTrainerBattleOpponent_A == TRAINER_UNION_ROOM)
                            stringPtr = sText_Trainer1WantsToBattle;
                        else if (gBattleTypeFlags & BATTLE_TYPE_RECORDED)
                            stringPtr = sText_LinkTrainerWantsToBattlePause;
                        else
                            stringPtr = sText_LinkTrainerWantsToBattle;
                    }
                } else {
                    if (BATTLE_TWO_VS_ONE_OPPONENT)
                        stringPtr = sText_Trainer1WantsToBattle;
                    else if (gBattleTypeFlags & (BATTLE_TYPE_MULTI | BATTLE_TYPE_INGAME_PARTNER))
                        stringPtr = sText_TwoTrainersWantToBattle;
                    else if (gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS)
                        stringPtr = sText_TwoTrainersWantToBattle;
                    else
                        stringPtr = sText_Trainer1WantsToBattle;
                }
            } else {
                if (gBattleTypeFlags & BATTLE_TYPE_LEGENDARY)
                    stringPtr = sText_LegendaryPkmnAppeared;
                else if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE &&
                         IsValidForBattle(&gEnemyParty[gBattlerPartyIndexes[gActiveBattler ^ BIT_FLANK]]))  // interesting, looks like they had something
                                                                                                            // planned for wild double battles
                    stringPtr = sText_TwoWildPkmnAppeared;
                else if (gBattleTypeFlags & BATTLE_TYPE_WALLY_TUTORIAL)
                    stringPtr = sText_WildPkmnAppearedPause;
                else
                    stringPtr = sText_WildPkmnAppeared;
            }
            break;
        case STRINGID_INTROSENDOUT:  // poke first send-out
            if (GetBattlerSide(gActiveBattler) == B_SIDE_PLAYER) {
                if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE && IsValidForBattle(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler ^ BIT_FLANK]])) {
                    if (gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER)
                        stringPtr = sText_InGamePartnerSentOutZGoN;
                    else if (gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS)
                        stringPtr = sText_GoTwoPkmn;
                    else if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
                        stringPtr = sText_LinkPartnerSentOutPkmnGoPkmn;
                    else
                        stringPtr = sText_GoTwoPkmn;
                } else {
                    stringPtr = sText_GoPkmn;
                }
            } else {
                if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE && IsValidForBattle(&gEnemyParty[gBattlerPartyIndexes[gActiveBattler ^ BIT_FLANK]])) {
                    if (BATTLE_TWO_VS_ONE_OPPONENT)
                        stringPtr = sText_Trainer1SentOutTwoPkmn;
                    else if (gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS)
                        stringPtr = sText_TwoTrainersSentPkmn;
                    else if (gBattleTypeFlags & BATTLE_TYPE_TOWER_LINK_MULTI)
                        stringPtr = sText_TwoTrainersSentPkmn;
                    else if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
                        stringPtr = sText_TwoLinkTrainersSentOutPkmn;
                    else if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED_LINK))
                        stringPtr = sText_LinkTrainerSentOutTwoPkmn;
                    else
                        stringPtr = sText_Trainer1SentOutTwoPkmn;
                } else {
                    if (!(gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED_LINK)))
                        stringPtr = sText_Trainer1SentOutPkmn;
                    else if (gTrainerBattleOpponent_A == TRAINER_UNION_ROOM)
                        stringPtr = sText_Trainer1SentOutPkmn;
                    else
                        stringPtr = sText_LinkTrainerSentOutPkmn;
                }
            }
            break;
        case STRINGID_RETURNMON:  // sending poke to ball msg
            if (GetBattlerSide(gActiveBattler) == B_SIDE_PLAYER) {
                if (*(&gBattleStruct->hpScale) == 0)
                    stringPtr = sText_PkmnThatsEnough;
                else if (*(&gBattleStruct->hpScale) == 1 || gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
                    stringPtr = sText_PkmnComeBack;
                else if (*(&gBattleStruct->hpScale) == 2)
                    stringPtr = sText_PkmnOkComeBack;
                else
                    stringPtr = sText_PkmnGoodComeBack;
            } else {
                if (gTrainerBattleOpponent_A == TRAINER_LINK_OPPONENT || gBattleTypeFlags & BATTLE_TYPE_RECORDED_LINK) {
                    if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
                        stringPtr = sText_LinkTrainer2WithdrewPkmn;
                    else
                        stringPtr = sText_LinkTrainer1WithdrewPkmn;
                } else {
                    stringPtr = sText_Trainer1WithdrewPkmn;
                }
            }
            break;
        case STRINGID_SWITCHINMON:  // switch-in msg
            if (GetBattlerSide(gBattleScripting.battler) == B_SIDE_PLAYER) {
                if (*(&gBattleStruct->hpScale) == 0 || gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
                    stringPtr = sText_GoPkmn2;
                else if (*(&gBattleStruct->hpScale) == 1)
                    stringPtr = sText_DoItPkmn;
                else if (*(&gBattleStruct->hpScale) == 2)
                    stringPtr = sText_GoForItPkmn;
                else
                    stringPtr = sText_YourFoesWeakGetEmPkmn;
            } else {
                if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED_LINK)) {
                    if (gBattleTypeFlags & BATTLE_TYPE_TOWER_LINK_MULTI) {
                        if (gBattleScripting.battler == 1)
                            stringPtr = sText_Trainer1SentOutPkmn2;
                        else
                            stringPtr = sText_Trainer2SentOutPkmn;
                    } else {
                        if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
                            stringPtr = sText_LinkTrainerMultiSentOutPkmn;
                        else if (gTrainerBattleOpponent_A == TRAINER_UNION_ROOM)
                            stringPtr = sText_Trainer1SentOutPkmn2;
                        else
                            stringPtr = sText_LinkTrainerSentOutPkmn2;
                    }
                } else {
                    if (gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS) {
                        if (gBattleScripting.battler == 1)
                            stringPtr = sText_Trainer1SentOutPkmn2;
                        else
                            stringPtr = sText_Trainer2SentOutPkmn;
                    } else {
                        stringPtr = sText_Trainer1SentOutPkmn2;
                    }
                }
            }
            break;
        case STRINGID_USEDMOVE:  // pokemon used a move msg
            if (gBattleMsgDataPtr->currentMove >= MOVES_COUNT)
                StringCopy(gBattleTextBuff3, sATypeMove_Table[*(&gBattleStruct->stringMoveType)]);
            else
                StringCopy(gBattleTextBuff3, gMoveNamesLong[gBattleMsgDataPtr->currentMove]);
            stringPtr = sText_AttackerUsedX;
            break;
        case STRINGID_POKEMONDIDAMMOUNTDAMAGE:  // pokemon did damage
            ConvertIntToDecimalStringN(gBattleTextBuff4, VarGet(VAR_DAMAGE_DONE), STR_CONV_MODE_LEFT_ALIGN, 4);
            stringPtr = sText_PkmnDidAmmountDamage;
            break;
        case STRINGID_BATTLEEND:  // battle end
            if (gBattleTextBuff1[0] & B_OUTCOME_LINK_BATTLE_RAN) {
                gBattleTextBuff1[0] &= ~(B_OUTCOME_LINK_BATTLE_RAN);
                if (GetBattlerSide(gActiveBattler) == B_SIDE_OPPONENT && gBattleTextBuff1[0] != B_OUTCOME_DREW)
                    gBattleTextBuff1[0] ^= (B_OUTCOME_LOST | B_OUTCOME_WON);

                if (gBattleTextBuff1[0] == B_OUTCOME_LOST || gBattleTextBuff1[0] == B_OUTCOME_DREW)
                    stringPtr = sText_GotAwaySafely;
                else if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
                    stringPtr = sText_TwoWildFled;
                else
                    stringPtr = sText_WildFled;
            } else {
                if (GetBattlerSide(gActiveBattler) == B_SIDE_OPPONENT && gBattleTextBuff1[0] != B_OUTCOME_DREW)
                    gBattleTextBuff1[0] ^= (B_OUTCOME_LOST | B_OUTCOME_WON);

                if (gBattleTypeFlags & BATTLE_TYPE_MULTI) {
                    switch (gBattleTextBuff1[0]) {
                        case B_OUTCOME_WON:
                            if (gBattleTypeFlags & BATTLE_TYPE_TOWER_LINK_MULTI)
                                stringPtr = sText_TwoInGameTrainersDefeated;
                            else
                                stringPtr = sText_TwoLinkTrainersDefeated;
                            break;
                        case B_OUTCOME_LOST:
                            stringPtr = sText_PlayerLostToTwo;
                            break;
                        case B_OUTCOME_DREW:
                            stringPtr = sText_PlayerBattledToDrawVsTwo;
                            break;
                    }
                } else if (gTrainerBattleOpponent_A == TRAINER_UNION_ROOM) {
                    switch (gBattleTextBuff1[0]) {
                        case B_OUTCOME_WON:
                            stringPtr = sText_PlayerDefeatedLinkTrainerTrainer1;
                            break;
                        case B_OUTCOME_LOST:
                            stringPtr = sText_PlayerLostAgainstTrainer1;
                            break;
                        case B_OUTCOME_DREW:
                            stringPtr = sText_PlayerBattledToDrawTrainer1;
                            break;
                    }
                } else {
                    switch (gBattleTextBuff1[0]) {
                        case B_OUTCOME_WON:
                            stringPtr = sText_PlayerDefeatedLinkTrainer;
                            break;
                        case B_OUTCOME_LOST:
                            stringPtr = sText_PlayerLostAgainstLinkTrainer;
                            break;
                        case B_OUTCOME_DREW:
                            stringPtr = sText_PlayerBattledToDrawLinkTrainer;
                            break;
                    }
                }
            }
            break;
        case STRINGID_TRAINERSLIDE:
            stringPtr = gBattleStruct->trainerSlideMsg;
            break;
        case STRINGID_TABLESPECIAL:
            switch (gBattleCommunication[MULTISTRING_CHOOSER])
            {
                case TABLE_SPECIAL_BATTLE_SKILL_ANNOUNCE:
                    stringPtr = gBattleSkills[gBattleScripting.abilityPopupOverwrite].announceString;
                    break;
            }
            break;
        default:  // load a string from the table
            if (stringID >= BATTLESTRINGS_COUNT + BATTLESTRINGS_ID_ADDER) {
                gDisplayedStringBattle[0] = EOS;
                return;
            } else {
                stringPtr = gBattleStringsTable[stringID - BATTLESTRINGS_ID_ADDER];
            }
            break;
    }

    BattleStringExpandPlaceholdersToDisplayedString(stringPtr);
}

u32 BattleStringExpandPlaceholdersToDisplayedString(const u8 *src) { return BattleStringExpandPlaceholders(src, gDisplayedStringBattle); }

static const u8 *TryGetStatusString(u8 *src) {
    u32 i;
    u8 status[8];
    u32 chars1, chars2;
    u8 *statusPtr;

    memcpy(status, sDummyWeirdStatusString, 8);

    statusPtr = status;
    for (i = 0; i < 8; i++) {
        if (*src == EOS) break;  // one line required to match -g
        *statusPtr = *src;
        src++;
        statusPtr++;
    }

    chars1 = *(u32 *)(&status[0]);
    chars2 = *(u32 *)(&status[4]);

    for (i = 0; i < ARRAY_COUNT(gStatusConditionStringsTable); i++) {
        if (chars1 == *(u32 *)(&gStatusConditionStringsTable[i][0][0]) && chars2 == *(u32 *)(&gStatusConditionStringsTable[i][0][4]))
            return gStatusConditionStringsTable[i][1];
    }
    return NULL;
}

static void GetBattlerNick(u32 battlerId, u8 *dst) {
    struct Pokemon *mon, *illusionMon;

    if (GET_BATTLER_SIDE(battlerId) == B_SIDE_PLAYER)
        mon = &gPlayerParty[gBattlerPartyIndexes[battlerId]];
    else
        mon = &gEnemyParty[gBattlerPartyIndexes[battlerId]];

    illusionMon = GetIllusionMonPtr(battlerId);
    if (illusionMon != NULL) mon = illusionMon;

    if (isMonNicknamed(mon)) {
        GetMonData(mon, MON_DATA_NICKNAME, dst);
        StringGetEnd10(dst);
    } else {
        SpeciesEnum species = GetMonData(mon, MON_DATA_SPECIES, NULL);
        StringCopy(dst, gSpeciesNames[species]);
        StringGetEnd12(dst);
    }
}

#define HANDLE_NICKNAME_STRING_CASE(battlerId)        \
    if (GetBattlerSide(battlerId) != B_SIDE_PLAYER) { \
        if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)   \
            toCpy = sText_FoePkmnPrefix;              \
        else                                          \
            toCpy = sText_WildPkmnPrefix;             \
        while (*toCpy != EOS) {                       \
            dst[dstID] = *toCpy;                      \
            dstID++;                                  \
            toCpy++;                                  \
        }                                             \
    }                                                 \
    GetBattlerNick(battlerId, text);                  \
    toCpy = text;

static const u8 *BattleStringGetOpponentNameByTrainerId(u16 trainerId, u8 *text, u8 multiplayerId, u8 battlerId) {
    const u8 *toCpy;

    if (gBattleTypeFlags & BATTLE_TYPE_SECRET_BASE) {
        u32 i;
        for (i = 0; i < ARRAY_COUNT(gBattleResources->secretBase->trainerName); i++) text[i] = gBattleResources->secretBase->trainerName[i];
        text[i] = EOS;
        ConvertInternationalString(text, gBattleResources->secretBase->language);
        toCpy = text;
    } else if (trainerId == TRAINER_UNION_ROOM) {
        toCpy = gLinkPlayers[multiplayerId ^ BIT_SIDE].name;
    } else if (trainerId == TRAINER_LINK_OPPONENT) {
        if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
            toCpy = gLinkPlayers[GetBattlerMultiplayerId(battlerId)].name;
        else
            toCpy = gLinkPlayers[GetBattlerMultiplayerId(battlerId) & BIT_SIDE].name;
    } else if (trainerId == TRAINER_FRONTIER_BRAIN) {
        CopyFrontierBrainTrainerName(text);
        toCpy = text;
    } else if (gBattleTypeFlags & BATTLE_TYPE_FRONTIER) {
        GetFrontierTrainerName(text, trainerId);
        toCpy = text;
    } else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER_HILL) {
        GetTrainerHillTrainerName(text, trainerId);
        toCpy = text;
    } else if (gBattleTypeFlags & BATTLE_TYPE_EREADER_TRAINER) {
        GetEreaderTrainerName(text);
        toCpy = text;
    } else {
        toCpy = gTrainers[trainerId].trainerName;
    }

    return toCpy;
}

static const u8 *BattleStringGetOpponentName(u8 *text, u8 multiplayerId, u8 battlerId) {
    const u8 *toCpy = NULL;

    switch (GetBattlerPosition(battlerId)) {
        case B_POSITION_OPPONENT_LEFT:
            toCpy = BattleStringGetOpponentNameByTrainerId(gTrainerBattleOpponent_A, text, multiplayerId, battlerId);
            break;
        case B_POSITION_OPPONENT_RIGHT:
            if (gBattleTypeFlags & (BATTLE_TYPE_TWO_OPPONENTS | BATTLE_TYPE_MULTI) && !BATTLE_TWO_VS_ONE_OPPONENT)
                toCpy = BattleStringGetOpponentNameByTrainerId(gTrainerBattleOpponent_B, text, multiplayerId, battlerId);
            else
                toCpy = BattleStringGetOpponentNameByTrainerId(gTrainerBattleOpponent_A, text, multiplayerId, battlerId);
            break;
    }

    return toCpy;
}

static const u8 *BattleStringGetPlayerName(u8 *text, u8 battlerId) {
    const u8 *toCpy = NULL;

    switch (GetBattlerPosition(battlerId)) {
        case B_POSITION_PLAYER_LEFT:
            if (gBattleTypeFlags & BATTLE_TYPE_RECORDED)
                toCpy = gLinkPlayers[0].name;
            else
                toCpy = gSaveBlock2Ptr->playerName;
            break;
        case B_POSITION_PLAYER_RIGHT:
            if (gBattleTypeFlags & BATTLE_TYPE_LINK && gBattleTypeFlags & (BATTLE_TYPE_RECORDED | BATTLE_TYPE_MULTI)) {
                toCpy = gLinkPlayers[2].name;
            } else if (gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER) {
                GetFrontierTrainerName(text, gPartnerTrainerId);
                toCpy = text;
            } else {
                toCpy = gSaveBlock2Ptr->playerName;
            }
            break;
    }

    return toCpy;
}

static const u8 *BattleStringGetTrainerName(u8 *text, u8 multiplayerId, u8 battlerId) {
    if (GetBattlerSide(battlerId) == B_SIDE_PLAYER)
        return BattleStringGetPlayerName(text, battlerId);
    else
        return BattleStringGetOpponentName(text, multiplayerId, battlerId);
}

static const u8 *BattleStringGetOpponentClassByTrainerId(u16 trainerId) {
    const u8 *toCpy;

    if (gBattleTypeFlags & BATTLE_TYPE_SECRET_BASE)
        toCpy = gTrainerClassNames[GetSecretBaseTrainerClass()];
    else if (trainerId == TRAINER_UNION_ROOM)
        toCpy = gTrainerClassNames[GetUnionRoomTrainerClass()];
    else if (trainerId == TRAINER_FRONTIER_BRAIN)
        toCpy = gTrainerClassNames[GetFrontierBrainTrainerClass()];
    else if (gBattleTypeFlags & BATTLE_TYPE_FRONTIER)
        toCpy = gTrainerClassNames[GetFrontierOpponentClass(trainerId)];
    else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER_HILL)
        toCpy = gTrainerClassNames[GetTrainerHillOpponentClass(trainerId)];
    else if (gBattleTypeFlags & BATTLE_TYPE_EREADER_TRAINER)
        toCpy = gTrainerClassNames[GetEreaderTrainerClassId()];
    else
        toCpy = gTrainerClassNames[gTrainers[trainerId].trainerClass];

    return toCpy;
}

u32 BattleStringExpandPlaceholders(const u8 *src, u8 *dst) {
    u32 dstID = 0;  // if they used dstID, why not use srcID as well?
    const u8 *toCpy = NULL;
    u8 text[30];
    u8 multiplayerId;

    if (gBattleTypeFlags & BATTLE_TYPE_RECORDED_LINK)
        multiplayerId = gRecordedBattleMultiplayerId;
    else
        multiplayerId = GetMultiplayerId();

    while (*src != EOS) {
        toCpy = NULL;
        if (*src == PLACEHOLDER_BEGIN) {
            src++;
            switch (*src) {
                case B_TXT_BUFF1:
                    if (gBattleTextBuff1[0] == B_BUFF_PLACEHOLDER_BEGIN) {
                        ExpandBattleTextBuffPlaceholders(gBattleTextBuff1, gStringVar1);
                        toCpy = gStringVar1;
                    } else {
                        toCpy = TryGetStatusString(gBattleTextBuff1);
                        if (toCpy == NULL) toCpy = gBattleTextBuff1;
                    }
                    break;
                case B_TXT_BUFF2:
                    if (gBattleTextBuff2[0] == B_BUFF_PLACEHOLDER_BEGIN) {
                        ExpandBattleTextBuffPlaceholders(gBattleTextBuff2, gStringVar2);
                        toCpy = gStringVar2;
                    } else
                        toCpy = gBattleTextBuff2;
                    break;
                case B_TXT_BUFF3:
                    if (gBattleTextBuff3[0] == B_BUFF_PLACEHOLDER_BEGIN) {
                        ExpandBattleTextBuffPlaceholders(gBattleTextBuff3, gStringVar3);
                        toCpy = gStringVar3;
                    } else
                        toCpy = gBattleTextBuff3;
                    break;
                case B_TXT_BUFF4:
                    if (gBattleTextBuff4[0] == B_BUFF_PLACEHOLDER_BEGIN) {
                        ExpandBattleTextBuffPlaceholders(gBattleTextBuff4, gStringVar3);
                        toCpy = gStringVar4;
                    } else
                        toCpy = gBattleTextBuff4;
                    break;
                case B_TXT_COPY_VAR_1:
                    toCpy = gStringVar1;
                    break;
                case B_TXT_COPY_VAR_2:
                    toCpy = gStringVar2;
                    break;
                case B_TXT_COPY_VAR_3:
                    toCpy = gStringVar3;
                    break;
                case B_TXT_PLAYER_MON1_NAME:  // first player poke name
                    GetBattlerNick(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT), text);
                    toCpy = text;
                    break;
                case B_TXT_OPPONENT_MON1_NAME:  // first enemy poke name
                    GetBattlerNick(GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT), text);
                    toCpy = text;
                    break;
                case B_TXT_PLAYER_MON2_NAME:  // second player poke name
                    GetBattlerNick(GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT), text);
                    toCpy = text;
                    break;
                case B_TXT_OPPONENT_MON2_NAME:  // second enemy poke name
                    GetBattlerNick(GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT), text);
                    toCpy = text;
                    break;
                case B_TXT_LINK_PLAYER_MON1_NAME:  // link first player poke name
                    GetBattlerNick(gLinkPlayers[multiplayerId].id, text);
                    toCpy = text;
                    break;
                case B_TXT_LINK_OPPONENT_MON1_NAME:  // link first opponent poke name
                    GetBattlerNick(gLinkPlayers[multiplayerId].id ^ 1, text);
                    toCpy = text;
                    break;
                case B_TXT_LINK_PLAYER_MON2_NAME:  // link second player poke name
                    GetBattlerNick(gLinkPlayers[multiplayerId].id ^ 2, text);
                    toCpy = text;
                    break;
                case B_TXT_LINK_OPPONENT_MON2_NAME:  // link second opponent poke name
                    GetBattlerNick(gLinkPlayers[multiplayerId].id ^ 3, text);
                    toCpy = text;
                    break;
                case B_TXT_ATK_NAME_WITH_PREFIX_MON1:  // Unused, to change into sth else.
                    break;
                case B_TXT_ATK_PARTNER_NAME:  // attacker partner name
                    HANDLE_NICKNAME_STRING_CASE(BATTLE_PARTNER(gBattlerAttacker));
                    break;
                case B_TXT_ATK_NAME_WITH_PREFIX:  // attacker name with prefix
                    HANDLE_NICKNAME_STRING_CASE(gBattlerAttacker)
                    break;
                case B_TXT_DEF_NAME_WITH_PREFIX:  // target name with prefix
                    HANDLE_NICKNAME_STRING_CASE(gBattlerTarget)
                    break;
                case B_TXT_DEF_NAME:  // target name
                    GetBattlerNick(gBattlerTarget, text);
                    toCpy = text;
                    break;
                case B_TXT_ACTIVE_NAME:  // active name
                    GetBattlerNick(gActiveBattler, text);
                    toCpy = text;
                    break;
                case B_TXT_ACTIVE_NAME2:  // active battlerId name with prefix, no illusion
                    if (GetBattlerSide(gActiveBattler) == B_SIDE_PLAYER) {
                        if (isMonNicknamed(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]])) {
                            GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_NICKNAME, text);
                            StringGetEnd10(text);
                        } else {
                            SpeciesEnum species = GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_SPECIES, NULL);
                            StringCopy(text, gSpeciesNames[species]);
                            StringGetEnd12(text);
                        }
                    } else {
                        if (isMonNicknamed(&gEnemyParty[gBattlerPartyIndexes[gActiveBattler]])) {
                            GetMonData(&gEnemyParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_NICKNAME, text);
                            StringGetEnd10(text);
                        } else {
                            SpeciesEnum species = GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_SPECIES, NULL);
                            StringCopy(text, gSpeciesNames[species]);
                            StringGetEnd12(text);
                        }
                    }
                    toCpy = text;
                    break;
                case B_TXT_EFF_NAME_WITH_PREFIX:  // effect battlerId name with prefix
                    HANDLE_NICKNAME_STRING_CASE(gEffectBattler)
                    break;
                case B_TXT_ACTIVE_NAME_WITH_PREFIX:  // active battlerId name with prefix
                    HANDLE_NICKNAME_STRING_CASE(gActiveBattler)
                    break;
                case B_TXT_SCR_ACTIVE_NAME_WITH_PREFIX:  // scripting active battlerId name with prefix
                    HANDLE_NICKNAME_STRING_CASE(gBattleScripting.battler)
                    break;
                case B_TXT_STACK_1_NAME_WITH_PREFIX:  // scripting active battlerId name with prefix
                    HANDLE_NICKNAME_STRING_CASE(gStackBattler1)
                    break;
                case B_TXT_STACK_2_NAME_WITH_PREFIX:  // scripting active battlerId name with prefix
                    HANDLE_NICKNAME_STRING_CASE(gStackBattler2)
                    break;
                case B_TXT_STACK_3_NAME_WITH_PREFIX:  // scripting active battlerId name with prefix
                    HANDLE_NICKNAME_STRING_CASE(gStackBattler3)
                    break;
                case B_TXT_STACK_4_NAME_WITH_PREFIX:  // scripting active battlerId name with prefix
                    HANDLE_NICKNAME_STRING_CASE(gStackBattler4)
                    break;
                case B_TXT_STAT_CHANGER:
                    toCpy = gStatNamesTable[gBattleScripting.statChanger.statId];
                    break;
                case B_TXT_CURRENT_MOVE:  // current move name
                    if (gBattleMsgDataPtr->currentMove >= MOVES_COUNT)
                        toCpy = sATypeMove_Table[gBattleStruct->stringMoveType];
                    else
                        toCpy = gMoveNamesLong[gBattleMsgDataPtr->currentMove];
                    break;
                case B_TXT_LAST_MOVE:  // originally used move name
                    if (gBattleMsgDataPtr->originallyUsedMove >= MOVES_COUNT)
                        toCpy = sATypeMove_Table[gBattleStruct->stringMoveType];
                    else
                        toCpy = gMoveNamesLong[gBattleMsgDataPtr->originallyUsedMove];
                    break;
                case B_TXT_LAST_ITEM:  // last used item
                    if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED_LINK)) {
                        if (gLastUsedItem == ITEM_ENIGMA_BERRY) {
                            if (!(gBattleTypeFlags & BATTLE_TYPE_MULTI)) {
                                if ((gBattleScripting.multiplayerId != 0 && (gPotentialItemEffectBattler & BIT_SIDE)) ||
                                    (gBattleScripting.multiplayerId == 0 && !(gPotentialItemEffectBattler & BIT_SIDE))) {
                                    StringCopy(text, gEnigmaBerries[gPotentialItemEffectBattler].name);
                                    StringAppend(text, sText_BerrySuffix);
                                    toCpy = text;
                                } else {
                                    toCpy = sText_EnigmaBerry;
                                }
                            } else {
                                if (gLinkPlayers[gBattleScripting.multiplayerId].id == gPotentialItemEffectBattler) {
                                    StringCopy(text, gEnigmaBerries[gPotentialItemEffectBattler].name);
                                    StringAppend(text, sText_BerrySuffix);
                                    toCpy = text;
                                } else
                                    toCpy = sText_EnigmaBerry;
                            }
                        } else {
                            CopyItemName(gLastUsedItem, text);
                            toCpy = text;
                        }
                    } else {
                        CopyItemName(gLastUsedItem, text);
                        toCpy = text;
                    }
                    break;
                case B_TXT_LAST_ABILITY:  // last used ability
                    toCpy = gAbilities[gBattleScripting.abilityPopupOverwrite].name;
                    break;
                case B_TXT_ATK_ABILITY:  // attacker ability
                    toCpy = gAbilities[sBattlerAbilities[gBattlerAttacker]].name;
                    break;
                case B_TXT_DEF_ABILITY:  // target ability
                    toCpy = gAbilities[sBattlerAbilities[gBattlerTarget]].name;
                    break;
                case B_TXT_SCR_ACTIVE_ABILITY:  // scripting active ability
                    toCpy = gAbilities[sBattlerAbilities[gBattleScripting.battler]].name;
                    break;
                case B_TXT_EFF_ABILITY:  // effect battlerId ability
                    toCpy = gAbilities[sBattlerAbilities[gEffectBattler]].name;
                    break;
                case B_TXT_TRAINER1_CLASS:  // trainer class name
                    toCpy = BattleStringGetOpponentClassByTrainerId(gTrainerBattleOpponent_A);
                    break;
                case B_TXT_TRAINER1_NAME:  // trainer1 name
                    toCpy =
                        BattleStringGetOpponentNameByTrainerId(gTrainerBattleOpponent_A, text, multiplayerId, GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT));
                    break;
                case B_TXT_LINK_PLAYER_NAME:  // link player name
                    toCpy = gLinkPlayers[multiplayerId].name;
                    break;
                case B_TXT_LINK_PARTNER_NAME:  // link partner name
                    toCpy = gLinkPlayers[GetBattlerMultiplayerId(BATTLE_PARTNER(gLinkPlayers[multiplayerId].id))].name;
                    break;
                case B_TXT_LINK_OPPONENT1_NAME:  // link opponent 1 name
                    toCpy = gLinkPlayers[GetBattlerMultiplayerId(BATTLE_OPPOSITE(gLinkPlayers[multiplayerId].id))].name;
                    break;
                case B_TXT_LINK_OPPONENT2_NAME:  // link opponent 2 name
                    toCpy = gLinkPlayers[GetBattlerMultiplayerId(BATTLE_PARTNER(BATTLE_OPPOSITE(gLinkPlayers[multiplayerId].id)))].name;
                    break;
                case B_TXT_LINK_SCR_TRAINER_NAME:  // link scripting active name
                    toCpy = gLinkPlayers[GetBattlerMultiplayerId(gBattleScripting.battler)].name;
                    break;
                case B_TXT_PLAYER_NAME:  // player name
                    toCpy = BattleStringGetPlayerName(text, GetBattlerAtPosition(B_POSITION_PLAYER_LEFT));
                    break;
                case B_TXT_TRAINER1_LOSE_TEXT:  // trainerA lose text
                    if (gBattleTypeFlags & BATTLE_TYPE_FRONTIER) {
                        CopyFrontierTrainerText(FRONTIER_PLAYER_WON_TEXT, gTrainerBattleOpponent_A);
                        toCpy = gStringVar4;
                    } else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER_HILL) {
                        CopyTrainerHillTrainerText(TRAINER_HILL_TEXT_PLAYER_WON, gTrainerBattleOpponent_A);
                        toCpy = gStringVar4;
                    } else {
                        toCpy = GetTrainerALoseText();
                    }
                    break;
                case B_TXT_TRAINER1_WIN_TEXT:  // trainerA win text
                    if (gBattleTypeFlags & BATTLE_TYPE_FRONTIER) {
                        CopyFrontierTrainerText(FRONTIER_PLAYER_LOST_TEXT, gTrainerBattleOpponent_A);
                        toCpy = gStringVar4;
                    } else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER_HILL) {
                        CopyTrainerHillTrainerText(TRAINER_HILL_TEXT_PLAYER_LOST, gTrainerBattleOpponent_A);
                        toCpy = gStringVar4;
                    }
                    break;
                case B_TXT_26:  // ?
                    if (GetBattlerSide(gBattleScripting.battler) != B_SIDE_PLAYER) {
                        if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
                            toCpy = sText_FoePkmnPrefix;
                        else
                            toCpy = sText_WildPkmnPrefix;
                        while (*toCpy != EOS) {
                            dst[dstID] = *toCpy;
                            dstID++;
                            toCpy++;
                        }
                        GetMonData(&gEnemyParty[gBattleStruct->field_52], MON_DATA_NICKNAME, text);
                    } else {
                        GetMonData(&gPlayerParty[gBattleStruct->field_52], MON_DATA_NICKNAME, text);
                    }
                    StringGetEnd10(text);
                    toCpy = text;
                    break;
                case B_TXT_PC_CREATOR_NAME:  // lanette pc
                    if (FlagGet(FLAG_SYS_PC_LANETTE))
                        toCpy = sText_Lanettes;
                    else
                        toCpy = sText_Someones;
                    break;
                case B_TXT_ATK_PREFIX2:
                    if (GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER)
                        toCpy = sText_AllyPkmnPrefix2;
                    else
                        toCpy = sText_FoePkmnPrefix3;
                    break;
                case B_TXT_DEF_PREFIX2:
                    if (GetBattlerSide(gBattlerTarget) == B_SIDE_PLAYER)
                        toCpy = sText_AllyPkmnPrefix2;
                    else
                        toCpy = sText_FoePkmnPrefix3;
                    break;
                case B_TXT_ATK_PREFIX1:
                    if (GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER)
                        toCpy = sText_AllyPkmnPrefix;
                    else
                        toCpy = sText_FoePkmnPrefix2;
                    break;
                case B_TXT_DEF_PREFIX1:
                    if (GetBattlerSide(gBattlerTarget) == B_SIDE_PLAYER)
                        toCpy = sText_AllyPkmnPrefix;
                    else
                        toCpy = sText_FoePkmnPrefix2;
                    break;
                case B_TXT_ATK_PREFIX3:
                    if (GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER)
                        toCpy = sText_AllyPkmnPrefix3;
                    else
                        toCpy = sText_FoePkmnPrefix4;
                    break;
                case B_TXT_DEF_PREFIX3:
                    if (GetBattlerSide(gBattlerTarget) == B_SIDE_PLAYER)
                        toCpy = sText_AllyPkmnPrefix3;
                    else
                        toCpy = sText_FoePkmnPrefix4;
                    break;
                case B_TXT_TRAINER2_CLASS:
                    toCpy = BattleStringGetOpponentClassByTrainerId(gTrainerBattleOpponent_B);
                    break;
                case B_TXT_TRAINER2_NAME:
                    toCpy =
                        BattleStringGetOpponentNameByTrainerId(gTrainerBattleOpponent_B, text, multiplayerId, GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT));
                    break;
                case B_TXT_TRAINER2_LOSE_TEXT:
                    if (gBattleTypeFlags & BATTLE_TYPE_FRONTIER) {
                        CopyFrontierTrainerText(FRONTIER_PLAYER_WON_TEXT, gTrainerBattleOpponent_B);
                        toCpy = gStringVar4;
                    } else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER_HILL) {
                        CopyTrainerHillTrainerText(TRAINER_HILL_TEXT_PLAYER_WON, gTrainerBattleOpponent_B);
                        toCpy = gStringVar4;
                    } else {
                        toCpy = GetTrainerBLoseText();
                    }
                    break;
                case B_TXT_TRAINER2_WIN_TEXT:
                    if (gBattleTypeFlags & BATTLE_TYPE_FRONTIER) {
                        CopyFrontierTrainerText(FRONTIER_PLAYER_LOST_TEXT, gTrainerBattleOpponent_B);
                        toCpy = gStringVar4;
                    } else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER_HILL) {
                        CopyTrainerHillTrainerText(TRAINER_HILL_TEXT_PLAYER_LOST, gTrainerBattleOpponent_B);
                        toCpy = gStringVar4;
                    }
                    break;
                case B_TXT_PARTNER_CLASS:
                    toCpy = gTrainerClassNames[GetFrontierOpponentClass(gPartnerTrainerId)];
                    break;
                case B_TXT_PARTNER_NAME:
                    toCpy = BattleStringGetPlayerName(text, GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT));
                    break;
                case B_TXT_ATK_TRAINER_NAME:
                    toCpy = BattleStringGetTrainerName(text, multiplayerId, gBattlerAttacker);
                    break;
                case B_TXT_ATK_TRAINER_CLASS:
                    switch (GetBattlerPosition(gBattlerAttacker)) {
                        case B_POSITION_PLAYER_RIGHT:
                            if (gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER) toCpy = gTrainerClassNames[GetFrontierOpponentClass(gPartnerTrainerId)];
                            break;
                        case B_POSITION_OPPONENT_LEFT:
                            toCpy = BattleStringGetOpponentClassByTrainerId(gTrainerBattleOpponent_A);
                            break;
                        case B_POSITION_OPPONENT_RIGHT:
                            if (gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS && !BATTLE_TWO_VS_ONE_OPPONENT)
                                toCpy = BattleStringGetOpponentClassByTrainerId(gTrainerBattleOpponent_B);
                            else
                                toCpy = BattleStringGetOpponentClassByTrainerId(gTrainerBattleOpponent_A);
                            break;
                    }
                    break;
                case B_TXT_ATK_TEAM1:
                    if (GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER)
                        toCpy = sText_Your1;
                    else
                        toCpy = sText_Opposing1;
                    break;
                case B_TXT_ATK_TEAM2:
                    if (GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER)
                        toCpy = sText_Your2;
                    else
                        toCpy = sText_Opposing2;
                    break;
                case B_TXT_DEF_TEAM1:
                    if (GetBattlerSide(gBattlerTarget) == B_SIDE_PLAYER)
                        toCpy = sText_Your1;
                    else
                        toCpy = sText_Opposing1;
                    break;
                case B_TXT_DEF_TEAM2:
                    if (GetBattlerSide(gBattlerTarget) == B_SIDE_PLAYER)
                        toCpy = sText_Your2;
                    else
                        toCpy = sText_Opposing2;
                    break;
            }

            if (toCpy != NULL) {
                while (*toCpy != EOS) {
                    dst[dstID] = *toCpy;
                    dstID++;
                    toCpy++;
                }
            }

            if (*src == B_TXT_TRAINER1_LOSE_TEXT || *src == B_TXT_TRAINER2_LOSE_TEXT || *src == B_TXT_TRAINER1_WIN_TEXT || *src == B_TXT_TRAINER2_WIN_TEXT) {
                dst[dstID] = EXT_CTRL_CODE_BEGIN;
                dstID++;
                dst[dstID] = EXT_CTRL_CODE_PAUSE_UNTIL_PRESS;
                dstID++;
            }
        } else {
            dst[dstID] = *src;
            dstID++;
        }
        src++;
    }

    dst[dstID] = *src;
    dstID++;

    return dstID;
}

static void IllusionNickHack(u32 battlerId, u32 partyId, u8 *dst) {
    s32 id, i;
    // we know it's gEnemyParty
    struct Pokemon *mon = &gEnemyParty[partyId], *partnerMon;

    if (GetMonAbility(mon) == ABILITY_ILLUSION) {
        if (IsBattlerAlive(BATTLE_PARTNER(battlerId)))
            partnerMon = &gEnemyParty[gBattlerPartyIndexes[BATTLE_PARTNER(battlerId)]];
        else
            partnerMon = mon;

        // Find last alive non-egg pokemon.
        for (i = PARTY_SIZE - 1; i >= 0; i--) {
            id = i;
            if (GetMonData(&gEnemyParty[id], MON_DATA_SANITY_HAS_SPECIES) && GetMonData(&gEnemyParty[id], MON_DATA_HP) && &gEnemyParty[id] != mon &&
                &gEnemyParty[id] != partnerMon) {
                GetMonData(&gEnemyParty[id], MON_DATA_NICKNAME, dst);
                return;
            }
        }
    }

    GetMonData(mon, MON_DATA_NICKNAME, dst);
}

static void ExpandBattleTextBuffPlaceholders(const u8 *src, u8 *dst) {
    u32 srcID = 1;
    u32 value = 0;
    u8 text[12];
    u16 hword;

    *dst = EOS;
    while (src[srcID] != B_BUFF_EOS) {
        switch (src[srcID]) {
            case B_BUFF_STRING:  // battle string
                hword = T1_READ_16(&src[srcID + 1]);
                StringAppend(dst, gBattleStringsTable[hword - BATTLESTRINGS_ID_ADDER]);
                srcID += 3;
                break;
            case B_BUFF_NUMBER:  // int to string
                switch (src[srcID + 1]) {
                    case 1:
                        value = src[srcID + 3];
                        break;
                    case 2:
                        value = T1_READ_16(&src[srcID + 3]);
                        break;
                    case 4:
                        value = T1_READ_32(&src[srcID + 3]);
                        break;
                }
                ConvertIntToDecimalStringN(dst, value, STR_CONV_MODE_LEFT_ALIGN, src[srcID + 2]);
                srcID += src[srcID + 1] + 3;
                break;
            case B_BUFF_MOVE:  // move name
                StringAppend(dst, gMoveNamesLong[T1_READ_16(&src[srcID + 1])]);
                srcID += 3;
                break;
            case B_BUFF_TYPE:  // type name
                StringAppend(dst, gTypeNames[src[srcID + 1]]);
                srcID += 2;
                break;
            case B_BUFF_MON_NICK_WITH_PREFIX:  // poke nick with prefix
                if (GetBattlerSide(src[srcID + 1]) == B_SIDE_PLAYER) {
                    GetMonData(&gPlayerParty[src[srcID + 2]], MON_DATA_NICKNAME, text);
                } else {
                    if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
                        StringAppend(dst, sText_FoePkmnPrefix);
                    else
                        StringAppend(dst, sText_WildPkmnPrefix);

                    GetMonData(&gEnemyParty[src[srcID + 2]], MON_DATA_NICKNAME, text);
                }
                StringGetEnd10(text);
                StringAppend(dst, text);
                srcID += 3;
                break;
            case B_BUFF_STAT:  // stats
                StringAppend(dst, gStatNamesTable[src[srcID + 1]]);
                srcID += 2;
                break;
            case B_BUFF_SPECIES:  // species name
                GetSpeciesName(dst, T1_READ_16(&src[srcID + 1]));
                srcID += 3;
                break;
            case B_BUFF_MON_NICK:  // poke nick without prefix
                if (src[srcID + 2] == gBattlerPartyIndexes[src[srcID + 1]]) {
                    GetBattlerNick(src[srcID + 1], dst);
                } else if (gBattleScripting.illusionNickHack)  // for STRINGID_ENEMYABOUTTOSWITCHPKMN
                {
                    gBattleScripting.illusionNickHack = 0;
                    IllusionNickHack(src[srcID + 1], src[srcID + 2], dst);
                    StringGetEnd10(dst);
                } else {
                    if (GetBattlerSide(src[srcID + 1]) == B_SIDE_PLAYER)
                        GetMonData(&gPlayerParty[src[srcID + 2]], MON_DATA_NICKNAME, dst);
                    else
                        GetMonData(&gEnemyParty[src[srcID + 2]], MON_DATA_NICKNAME, dst);
                    StringGetEnd10(dst);
                }
                srcID += 3;
                break;
            case B_BUFF_NEGATIVE_FLAVOR:  // flavor table
                StringAppend(dst, gPokeblockWasTooXStringTable[src[srcID + 1]]);
                srcID += 2;
                break;
            case B_BUFF_ABILITY:  // ability names
                StringAppend(dst, gAbilities[T1_READ_16(&src[srcID + 1])].name);
                srcID += 3;
                break;
            case B_BUFF_ITEM:  // item name
                hword = T1_READ_16(&src[srcID + 1]);
                if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED_LINK)) {
                    if (hword == ITEM_ENIGMA_BERRY) {
                        if (gLinkPlayers[gBattleScripting.multiplayerId].id == gPotentialItemEffectBattler) {
                            StringCopy(dst, gEnigmaBerries[gPotentialItemEffectBattler].name);
                            StringAppend(dst, sText_BerrySuffix);
                        } else {
                            StringAppend(dst, sText_EnigmaBerry);
                        }
                    } else {
                        CopyItemName(hword, dst);
                    }
                } else {
                    CopyItemName(hword, dst);
                }
                srcID += 3;
                break;
        }
    }
}

// Loads one of two text strings into the provided buffer. This is functionally
// unused, since the value loaded into the buffer is not read; it loaded one of
// two particles (either "?" or "?") which works in tandem with ChooseTypeOfMoveUsedString
// below to effect changes in the meaning of the line.
static void ChooseMoveUsedParticle(u8 *textBuff) {
    s32 counter = 0;
    u32 i = 0;

    while (counter != MAX_MON_MOVES) {
        if (sGrammarMoveUsedTable[i] == 0) counter++;
        if (sGrammarMoveUsedTable[i++] == gBattleMsgDataPtr->currentMove) break;
    }

    if (counter >= 0) {
        if (counter <= 2)
            StringCopy(textBuff, sText_SpaceIs);  // is
        else if (counter <= MAX_MON_MOVES)
            StringCopy(textBuff, sText_ApostropheS);  // 's
    }
}

// Appends "!" to the text buffer `dst`. In the original Japanese this looked
// into the table of moves at sGrammarMoveUsedTable and varied the line accordingly.
//
// sText_ExclamationMark was a plain "!", used for any attack not on the list.
// It resulted in the translation "<NAME>'s <ATTACK>!".
//
// sText_ExclamationMark2 was "? ????!". This resulted in the translation
// "<NAME> used <ATTACK>!", which was used for all attacks in English.
//
// sText_ExclamationMark3 was "??!". This was used for those moves whose
// names were verbs, such as Recover, and resulted in translations like "<NAME>
// recovered itself!".
//
// sText_ExclamationMark4 was "? ??!" This resulted in a translation of
// "<NAME> did an <ATTACK>!".
//
// sText_ExclamationMark5 was " ????!" This resulted in a translation of
// "<NAME>'s <ATTACK> attack!".
static void ChooseTypeOfMoveUsedString(u8 *dst) {
    s32 counter = 0;
    s32 i = 0;

    while (*dst != EOS) dst++;

    while (counter != MAX_MON_MOVES) {
        if (sGrammarMoveUsedTable[i] == MOVE_NONE) counter++;
        if (sGrammarMoveUsedTable[i++] == gBattleMsgDataPtr->currentMove) break;
    }

    switch (counter) {
        case 0:
            StringCopy(dst, sText_ExclamationMark);
            break;
        case 1:
            StringCopy(dst, sText_ExclamationMark2);
            break;
        case 2:
            StringCopy(dst, sText_ExclamationMark3);
            break;
        case 3:
            StringCopy(dst, sText_ExclamationMark4);
            break;
        case 4:
            StringCopy(dst, sText_ExclamationMark5);
            break;
    }
}

void BattlePutTextOnWindow(const u8 *text, u8 windowId) {
    const struct BattleWindowText *textInfo = sBattleTextOnWindowsInfo[gBattleScripting.windowsType];
    bool32 copyToVram;
    struct TextPrinterTemplate printerTemplate;
    u8 speed;

    if (windowId & 0x80) {
        windowId &= ~(0x80);
        copyToVram = FALSE;
    } else {
        // Overrides
        switch (getBattleInterfaceTheme()) {
            case THEME_DARK:
                FillWindowPixelBuffer(windowId, PIXEL_FILL(3));
                break;
            case THEME_LIGHT:
            case THEME_DPPT:
            case THEME_CLASSIC:
                FillWindowPixelBuffer(windowId, PIXEL_FILL(10));
                break;
            default:
                FillWindowPixelBuffer(windowId, textInfo[windowId].fillValue);
                break;
        }
        gBattle_BG1_Y = 160;
        copyToVram = TRUE;
    }

    printerTemplate.currentChar = text;
    printerTemplate.windowId = windowId;
    printerTemplate.fontId = textInfo[windowId].fontId;
    printerTemplate.x = textInfo[windowId].x;
    printerTemplate.y = textInfo[windowId].y;
    printerTemplate.currentX = printerTemplate.x;
    printerTemplate.currentY = printerTemplate.y;
    printerTemplate.letterSpacing = textInfo[windowId].letterSpacing;
    printerTemplate.lineSpacing = textInfo[windowId].lineSpacing;
    printerTemplate.unk = 0;
    // Overrides
    switch (getBattleInterfaceTheme()) {
        case THEME_DARK:
            printerTemplate.fgColor = 10;     // Font Color
            printerTemplate.bgColor = 3;      // Background Color
            printerTemplate.shadowColor = 0;  // Shadow Color
            break;
        case THEME_LIGHT:
        case THEME_CLASSIC:
            printerTemplate.fgColor = 3;      // Font Color
            printerTemplate.bgColor = 10;     // Background Color
            printerTemplate.shadowColor = 0;  // Shadow Color
            break;
        case THEME_DPPT:
            printerTemplate.fgColor = 2;      // Font Color
            printerTemplate.bgColor = 10;     // Background Color
            printerTemplate.shadowColor = 0;  // Shadow Color
            break;
        default:
            printerTemplate.fgColor = textInfo[windowId].fgColor;
            printerTemplate.bgColor = textInfo[windowId].bgColor;
            printerTemplate.shadowColor = textInfo[windowId].shadowColor;
            break;
    }

    if (printerTemplate.x == 0xFF) {
        u32 width = GetBattleWindowTemplatePixelWidth(gBattleScripting.windowsType, windowId);
        s32 alignX = GetStringCenterAlignXOffsetWithLetterSpacing(printerTemplate.fontId, printerTemplate.currentChar, width, printerTemplate.letterSpacing);
        printerTemplate.x = printerTemplate.currentX = alignX;
    }

    if (windowId == 0x16)
        gTextFlags.useAlternateDownArrow = 0;
    else
        gTextFlags.useAlternateDownArrow = 1;

    if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED))
        gTextFlags.autoScroll = 1;
    else
        gTextFlags.autoScroll = 0;

    if (windowId == 0 || windowId == 0x16) {
        if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED_LINK))
            speed = 1;
        else if (gBattleTypeFlags & BATTLE_TYPE_RECORDED)
            speed = sRecordedBattleTextSpeeds[GetTextSpeedInRecordedBattle()];
        else
            speed = GetPlayerTextSpeedDelay();

        gTextFlags.canABSpeedUpPrint = 1;
    } else {
        speed = textInfo[windowId].speed;
        gTextFlags.canABSpeedUpPrint = 0;
    }

    AddTextPrinter(&printerTemplate, speed, NULL);

    if (copyToVram) {
        PutWindowTilemap(windowId);
        CopyWindowToVram(windowId, 3);
    }
}

void SetPpNumbersPaletteInMoveSelection(void) {
    struct ChooseMoveStruct *chooseMoveStruct = (struct ChooseMoveStruct *)(&gBattleResources->bufferA[gActiveBattler][4]);
    const u16 *palPtr = gUnknown_08D85620;
    u8 var = GetCurrentPpToMaxPpState(chooseMoveStruct->currentPp[gMoveSelectionCursor[gActiveBattler]],
                                      chooseMoveStruct->maxPp[gMoveSelectionCursor[gActiveBattler]]);

    gPlttBufferUnfaded[92] = palPtr[(var * 2) + 0];
    gPlttBufferUnfaded[91] = palPtr[(var * 2) + 1];

    CpuCopy16(&gPlttBufferUnfaded[92], &gPlttBufferFaded[92], sizeof(u16));
    CpuCopy16(&gPlttBufferUnfaded[91], &gPlttBufferFaded[91], sizeof(u16));
}

PpCountQuality GetCurrentPpToMaxPpState(u8 currentPp, u8 maxPp) {
    if (maxPp == currentPp) {
        return PP_MANY;
    } else if (maxPp <= 2) {
        if (currentPp > 1)
            return PP_MANY;
        else
            return 2 - currentPp;
    } else if (maxPp <= 7) {
        if (currentPp > 2)
            return PP_MANY;
        else
            return 2 - currentPp;
    } else {
        if (currentPp == 0) return PP_NO_PP;
        if (currentPp <= maxPp / 4) return PP_SOME;
        if (currentPp > maxPp / 2) return PP_MANY;
    }

    return PP_FEW;
}

struct TrainerSlide {
    u16 trainerId;
    const u8 *msgLastSwitchIn;
    const u8 *msgLastLowHp;
    const u8 *msgFirstDown;
};

static const struct TrainerSlide sTrainerSlides[] = {
    {0, sText_AarghAlmostHadIt, sText_BoxIsFull, sText_123Poof},
};

static u32 GetEnemyMonCount(bool32 onlyAlive) {
    u32 i, count = 0;

    for (i = 0; i < PARTY_SIZE; i++) {
        u32 species = GetMonData(&gEnemyParty[i], MON_DATA_SPECIES2, NULL);
        if (species != SPECIES_NONE && species != SPECIES_EGG && (!onlyAlive || GetMonData(&gEnemyParty[i], MON_DATA_HP, NULL))) count++;
    }

    return count;
}

static bool32 IsBattlerHpLow(u32 battler) {
    if ((gBattleMons[battler].hp * 100) / gBattleMons[battler].maxHP < 25)
        return TRUE;
    else
        return FALSE;
}

bool32 ShouldDoTrainerSlide(u32 battlerId, u32 trainerId, u32 which) {
    s32 i;

    if (!(gBattleTypeFlags & BATTLE_TYPE_TRAINER) || GetBattlerSide(battlerId) != B_SIDE_OPPONENT) return FALSE;

    for (i = 0; i < ARRAY_COUNT(sTrainerSlides); i++) {
        if (trainerId == sTrainerSlides[i].trainerId) {
            gBattleScripting.battler = battlerId;
            switch (which) {
                case TRAINER_SLIDE_LAST_SWITCHIN:
                    if (sTrainerSlides[i].msgLastSwitchIn != NULL && GetEnemyMonCount(TRUE) == 1) {
                        gBattleStruct->trainerSlideMsg = sTrainerSlides[i].msgLastSwitchIn;
                        return TRUE;
                    }
                    break;
                case TRAINER_SLIDE_LAST_LOW_HP:
                    if (sTrainerSlides[i].msgLastLowHp != NULL && GetEnemyMonCount(TRUE) == 1 && IsBattlerHpLow(battlerId) &&
                        !gBattleStruct->trainerSlideLowHpMsgDone) {
                        gBattleStruct->trainerSlideLowHpMsgDone = TRUE;
                        gBattleStruct->trainerSlideMsg = sTrainerSlides[i].msgLastLowHp;
                        return TRUE;
                    }
                    break;
                case TRAINER_SLIDE_FIRST_DOWN:
                    if (sTrainerSlides[i].msgFirstDown != NULL && GetEnemyMonCount(TRUE) == GetEnemyMonCount(FALSE) - 1) {
                        gBattleStruct->trainerSlideMsg = sTrainerSlides[i].msgFirstDown;
                        return TRUE;
                    }
                    break;
            }
            break;
        }
    }

    return FALSE;
}
