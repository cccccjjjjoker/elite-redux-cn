# -*- coding: utf-8 -*-
"""由 ER2.65beta 版图鉴 v0.3 生成「翻译参考术语表 + 名称/效果不符错误报告」"""
import csv, re, collections
import openpyxl
from openpyxl.styles import Font, PatternFill, Alignment, Border, Side
from openpyxl.utils import get_column_letter

# 图鉴源（未随仓库分发，仅记录来源，脚本不读取该文件）
SRC = r'ER2.65beta版图鉴v0.3.xlsm'
# 生成物写在脚本所在目录，请在本目录下运行
OUT = 'glossary.xlsx'

# ---------------- 读取 ----------------
def rd(p):
    return list(csv.reader(open(p, encoding='utf-8-sig')))

feat = rd('out/features.csv')[2:]
moves = rd('out/moves.csv')[2:]
dex = rd('out/dex.csv')
notes = rd('out/notes.csv')

# ---------------- 已核实的错位区段 ----------------
# (起始行, 结束行, 说明)
SHIFT_ZONES = [
    (386, 393, '中文列整体旋转：本段 8 行的中文名/中文描述/游戏内说明互相错位一行，'
               'Spectral Shroud 的中文名「幽灵魂化」被挤到段末（第393行）。'),
    (868, 873, '中文名在这 6 行内部发生置换（非平移）：Fire Aspect / Ice Plumes / '
               'Blistering Sun / Molten Core / Aurora\'s Gale 五个译名互相串位。'),
    (907, 933, '英文源与中文源排序不一致导致串行：Turf War~Hammer Fist 区间内，'
               'Laser Drill / Lightsaber / Loose Thorns / Turf War / Greedy / Musical Notes 等顺序被打乱，'
               '第919-933行两两组互换。'),
    (945, 990, '中文源比英文源多出 2 条（Chainsaw「链锯」、Galeforce Wings「飓风之翼」）'
               '且缺少 Rain Shroud 的位置，造成第945行起累计 -2 行错位，'
               '第964行又多出 Lucha Libre「自由式摔跤」，错位量在 -3~-1 之间摆动。'),
]

# (行号, 英文术语, 表中错误中文, 正确中文, 表中所列效果(英文), 正确效果(英文))
FIX = [
 # ---- 区段1 386-393 ----
 (387,'Spectral Shroud','自律','幽灵魂化','Spectralize + 30% chance to badly poison the foe.',
      'Spectralize + 30% chance to badly poison the foe.（名称对，中文名与中文描述错位）'),
 (388,'Discipline','雷召','自律','Can switch while rampaging. Cannot be confused or intimidated.',
      '同上（英文名与英文描述是自洽的，错的是中文列）'),
 (389,'Thundercall','海洋霸主','雷召','Triggers Smite at 20% power when using an Electric move.','同上'),
 (390,'Marine Apex','强大之角','海洋霸主','50% more damage to Water-types + Infiltrator.','同上'),
 (391,'Mighty Horn','硬化鞘','强大之角','Boosts the power of horn and drill-based moves by 1.3x.','同上'),
 (392,'Hardened Sheath','极地毛发','硬化鞘','Ups Attack by +1 when using horn moves.','同上'),
 (393,'Arctic Fur','幽灵魂化','极地毛发','Weakens incoming physical and special moves by 35%.','同上'),
 # ---- 区段2 868-873 ----
 (869,'Fire Aspect','灼日','火焰形态','Absorbs fire moves and always burns with fire.',
      '表中第869行中文描述写的是「终结之地+吹气筒」= Blistering Sun 的效果'),
 (870,'Ice Plumes','熔核','冰羽','Halves damage taken by Special moves. Does NOT double SpDef.',
      '表中第870行中文描述写的是「熔炉+吸收岩石招式/隐形岩」= Molten Core 的效果'),
 (871,'Blistering Sun','火焰形态','灼日','Desolate Land + Air Blower.',
      '表中第871行中文描述写的是「吸收火属性招式且必定灼伤」= Fire Aspect 的效果'),
 (872,'Molten Core','极光之风','熔核','Furnace + Absorbs Rock-moves / Stealth Rocks.',
      '表中第872行中文描述写的是「华贵之鸟+北风」= Aurora\'s Gale 的效果'),
 (873,"Aurora's Gale",'冰羽','极光之风','Majestic Bird + Aurora Veil.',
      '表中第873行中文描述写的是「受到的特殊招式伤害减半」= Ice Plumes 的效果'),
 # ---- 区段3 907-933 ----
 (907,'Turf War','激光钻','地盘争夺','Destroys terrain and boosts highest stat on entry.','英文名与英文描述自洽，中文列取到了 Laser Drill 的译名与描述'),
 (908,'Greedy','光剑','贪婪','Uses Thief when it loses an item.','中文列取到了 Lightsaber 的译名与描述'),
 (909,'Lightsaber','松散荆棘','光剑','Adds Fire-type. Keen Edge moves have 25% burn or paralysis.','中文列取到了 Loose Thorns 的译名与描述'),
 (910,'Loose Thorns','地盘争夺','松散荆棘','Sets Creeping Thorns when hit by contact.','中文列取到了 Turf War 的译名与描述'),
 (911,'Musical Notes','贪婪','音符','Status moves become sound-based.','中文列取到了 Greedy 的译名与描述'),
 (912,'Laser Drill','音符','激光钻','Horn moves have a 50% burn chance.','中文列取到了 Musical Notes 的译名与描述'),
 (919,'Rocky Exterior','铅爪','岩石外表','Adds Rock type on entry.','与第920行互换'),
 (920,'Lead Claws','岩石外表','铅爪','Tough Claws + Mineralize.','与第919行互换'),
 (922,'Chainsaw','女皇','链锯','Keen Edge attacks lower defense by -1.','中文列取到了 Empress 的译名与描述'),
 (923,'Galeforce Wings','土豆泥','飓风之翼','Flying moves get +1 Priority.','中文列取到了 Mashed Potato 的译名与描述'),
 (924,'Empress','品味彩虹','女皇','Queenly Majesty + Rivalry.','中文列取到了 Taste the Rainbow 的译名与描述'),
 (925,'Mashed Potato','彩虹鳞','土豆泥','Syrup Bomb effect on the foe for 3 turns.','中文列取到了 Rainbow Scales 的译名与描述'),
 (926,'Rainbow Scales','催眠之触','彩虹鳞','Fire Scales + Taste the Rainbow.','中文列取到了 Hypnotic Touch 的译名与描述'),
 (927,'Taste the Rainbow','疫病之翼','品味彩虹','Summons the Rainbow Pledge effect on entry.','中文列取到了 Wings of Pestilence 的译名与描述'),
 (928,'Hypnotic Touch','九头蛇','催眠之触','20% chance to sleep on contact. Also works on offense.','中文列取到了 Hydra 的译名与描述'),
 (929,'Hydra','圣代','九头蛇','Multiheaded + Beast Boost.','中文列取到了 Sundae 的译名与描述'),
 (930,'Wings of Pestilence','冰镐','疫病之翼','Every attack has a 20% Bleed chance and 10% Curse chance.','中文列取到了 Ice Picks 的译名与描述'),
 (931,'Sundae','锤拳','圣代','Snow Warning + Ice Body.','中文列取到了 Hammer Fist 的译名与描述'),
 (932,'Ice Picks','龙首藻','冰镐','Tough Claws + Slush Rush.','中文列取到了更靠后的 Drakelp Head 的译名'),
 (933,'Hammer Fist','极性','锤拳','Boosts punch and hammer moves by 25%.','中文列取到了 Polarity 的译名与描述'),
 # ---- 区段4 945-990 ----
 (945,'Echolocation','链锯','回声定位','In fog, deal 20% more damage and never miss.','中文列整段下移，取到了 Chainsaw 的译名'),
 (946,'Massive Pelt','飓风之翼','厚重毛皮','Tangling Hair + Fluffy.','中文列取到了 Galeforce Wings 的译名'),
 (947,'I Am Steve','回声定位','我是史蒂夫','Uses No Retreat on entry.','中文列上移2行，取到 Echolocation 的译名与描述'),
 (948,'Tangled Tails','厚重毛皮','缠尾','Know Your Place + Grappler.','中文列上移2行'),
 (949,'Foamy Web','我是史蒂夫','泡沫网','Casts an unremovable Sticky Web on entry. Lasts 5 turns.','中文列上移2行'),
 (950,'Sharp Talons','缠尾','锐爪','Kicking moves have a 50% Bleed chance.','中文列上移2行'),
 (951,'Zen Garden','泡沫网','禅意花园','Sets up Grassy or Psychic Terrain at random.','中文列上移2行'),
 (952,'Tummyache','锐爪','胃痛','Thick Fat + Corrosion.','中文列上移2行'),
 (953,'Hypnotic Trance','禅意花园','催眠恍惚','Hypnosis never misses and also causes Confusion.','中文列上移2行'),
 (954,'Brain Overload','胃痛','大脑过载','When hit, sets up Psychic Terrain.','中文列上移2行'),
 (955,'Brain Mass','催眠恍惚','脑容量','Halves damage taken while at full HP.','中文列上移2行'),
 (956,'Chestnut Shield','大脑过载','栗子盾','Impenetrable + Bulletproof.','中文列上移2行'),
 (957,'Chestnut Axe','脑容量','栗子斧','Keen Edge + Grass moves become Keen Edge boosted.','中文列上移2行'),
 (958,'Giant Shuriken','栗子盾','巨型手里剑','Water Shuriken hits once with 100BP and +1 crit.','中文列上移2行'),
 (959,'Rain Shroud','栗子斧','雨幕','Ups evasion by 30% in rain.','中文列上移2行'),
 (960,'Witch Broom','巨型手里剑','女巫扫帚','Hyper Aggressive + Hover.','中文列上移2行'),
 (961,'Angelic Wings','女巫扫帚','天使之翼','Prism Scales + Huge Wings.','中文列上移2行'),
 (962,'Wrestle Showman','天使之翼','摔跤表演家','Flying Press gains +10BP and causes Taunt.','中文列上移2行'),
 (963,'Fire Ruler','摔跤表演家','火之统治者','King\'s Wrath + Flame Body.','中文列上移2行'),
 (964,'Chandelier','自由式摔跤','枝形吊灯','Illuminate + Pyromancy.','中文列多出 Lucha Libre 一条，错位 -3'),
 (965,'Foggy Eye','火之统治者','雾眼','While in Fog, boost Ghost moves by 50% and resist Ghost moves.','中文列上移2行'),
 (966,'Toxic Shell','枝形吊灯','毒壳','Shell Armor + Poison Point.','中文列上移2行'),
 (967,'Hand Barnacles','雾眼','手藤壶','Multi-Headed + Water STAB.','中文列上移2行'),
 (968,'Voltron','毒壳','战神金刚','Metallic + Battle Armor.','中文列上移2行'),
 (969,'Fire\'s Wrath','手藤壶','火之怒','Intimidate + Scare; 10% burn chance on non-contact moves.','中文列上移2行'),
 (970,'Emperor\'s Wrath','战神金刚','帝王之怒','King\'s Wrath + Queen\'s Mourning.','中文列上移2行'),
 (971,'Lepidopteran','火之怒','鳞翅目','Swarm + Unaware.','中文列上移2行'),
 (972,'Break it Down','帝王之怒','分解','After using a dance move, follow up with a 20BP Rapid Spin.','中文列上移2行'),
 (973,'Talon Trap','鳞翅目','爪钩陷阱','50% chance to trap on contact. 100% if entered this turn.','中文列上移2行'),
 (974,'Backstreet Boy','分解','后街男孩','Striker + Kicking moves are Dance moves and vice-versa.','中文列上移2行'),
 (975,'Backflip','爪钩陷阱','后空翻','After using a Dance move, follow up with a 50BP Chip Away.','中文列上移2行'),
 (976,'Crushing Jaw','后空翻','粉碎之颚','Strong Jaw + Biting moves have a 50% chance to lower defense.','中文列上移2行'),
 (977,'Eternal Flower','粉碎之颚','永恒之花','Reduces the stats of other Megas by 20%.','中文列上移2行'),
 (978,'Nihil Blaster','永恒之花','虚无一击','Aura Break + Mega Launcher.','中文列上移2行'),
 (979,'Hollow Ice Zone','虚无一击','冰窟地带','Ice-type moves apply Ice Statue and then make the user switch.','中文列上移2行'),
 (980,'Overcast','冰窟地带','阴天','Low Visibility + Sets Mist on entry.','中文列错位 -3'),
 (982,'Flower Necklace','低温休眠','花项链','SpDef gets a 1.5x boost in Grassy Terrain.','中文列上移1行'),
 (983,'Mega Drill','阴天','巨型钻头','Mighty Horn + all Drill moves are 30% stronger.','中文列错位 -3'),
 (984,'Mucus Membrane','花项链','黏液膜','Takes 30% less damage from attacks + Gooey.','中文列上移2行'),
 (985,'Lucha Libre','巨型钻头','自由式摔跤','Dazzling + Defiant.','中文列上移2行'),
 (986,'Curlipede','黏液膜','卷曲蜈蚣','Let\'s Roll + Coil Up.','中文列上移2行'),
 (987,'Storm Cloud','雨幕','积雨云','Summon rain on entry for 8 turns. Gain Electric-type STAB.','「雨幕」实为 Rain Shroud(第959行) 的译名'),
 (988,'Waterborne','卷曲蜈蚣','水传播','Aquatic + Adaptability.','中文列上移2行'),
 (989,'Drakelp Head','积雨云','龙首藻','Weakens first move taken and drops opponent\'s attack.','中文列上移2行'),
 (990,'Polarity','水传播','极性','Increases the party\'s highest stat by 30%.','中文列上移2行'),
]

FIX_ROWS = {f[0]: f for f in FIX}

# ---------------- 核心机制术语 ----------------
CORE = [
 ('Ability','特性','宝可梦的常规被动能力，一场战斗中可被复制/覆盖/无效化（如「复制Trace」「木乃伊Mummy」「化学变化气体」）','特性页 C/D 列'),
 ('Innate','天性','ER 新增的第二被动槽，独立于特性：破格(Mold Breaker)可无视特性，但只有特定效果才能影响天性','特性天性列 U/V/W'),
 ('Base stats','种族值','HP/攻击/防御/特攻/特防/速度 六项基础数值，决定能力值成长','简易图鉴 J~P 列'),
 ('HP / Attack / Defense / Sp. Atk / Sp. Def / Speed','HP／攻击／防御／特攻／特防／速度','能力值六项','简易图鉴表头'),
 ('STAB (Same Type Attack Bonus)','本系加成','招式属性与自身属性一致时的伤害加成；本图鉴中 ER 将部分特性（适应力 Adaptability）改为 2.0 倍','特性描述高频词'),
 ('Physical / Special / Status','物理／特殊／变化','招式三大基础分类（详见「04-招式分类」页）','招式页 G 列'),
 ('Priority','优先度','决定出手顺序；+1 为先手。恶作剧之心(Prankster)等特性会改变优先度','招式/特性描述'),
 ('Critical hit / Crit','击中要害／暴击','伤害 1.5 倍；狙击手(Sniper)为 2.25 倍','特性描述'),
 ('Recoil','反伤','使用招式后自身受到的伤害；坚硬脑袋(Rock Head)可免疫','特性描述'),
 ('Contact move','接触类招式','会触发粗糙皮肤、毒刺、火焰之躯等接触类效果的招式','特性描述高频词'),
 ('Switch out / Switch-in','替换下场／替换上场','换人动作，大量特性以此为触发时机（威吓Intimidate、降雨Drizzle等）','特性描述'),
 ('Hazard / Entry hazard','入场陷阱（场地陷阱）','撒菱、毒菱、隐形岩、黏黏网等；捡拾(Pickup)可清除','特性描述'),
 ('Terrain','场地','电气/精神/薄雾/青草/毒场五种；本作持续 8 回合（携带场地制造机 12 回合）','说明页'),
 ('Weather','天气','雨、大晴天、沙暴、冰雹、雾；本作一律 8 回合，带对应岩石 12 回合','说明页'),
 ('Room','房间（空间）','戏法空间(Trick Room)、逆向房间(Inverse Room)，持续 3 回合','特性 346/473'),
 ('Tailwind','顺风','3 回合内己方速度翻倍；吹气筒(Air Blower)登场即开','特性 320'),
 ('Screens','墙壁（光墙/反射壁/极光幕）','减伤类护盾；除障(Screen Cleaner)可清除','特性 251'),
 ('Mega Evolution / Mega Stone','超级进化／超级石','本作大量宝可梦有专属 Mega 形态与超级石获取任务','Mega石页'),
 ('Legendary / Ultra Beast','神兽／异兽','获取方式见「神兽异兽及特殊」页','神兽页'),
 ('Shiny','闪光','异色宝可梦；伪 nuzloc 模式下抓闪光不受限制','说明页 常见问题'),
 ('Gym Leader','馆主','2.5 版起可跳过小弟直接挑战馆主，馆主自带 buff','馆主buff页'),
 ('Hell difficulty','地狱难度','2.65 新增难度，附带额外限制与 buff','馆主buff页 D 列'),
 ('Pseudo-Nuzlocke','伪 nuzloc 模式','地狱难度限制之一：每路线只能捕一只，但抓闪光可无视','馆主buff页'),
 ('Egg move / Field move','蛋招式／秘传类场地招式','居合劈(Cut)、冲浪(Surf)、怪力(Strength)等；旷野探险家(Field Explorer)加成 50%','特性 360'),
 ('Starter','初始宝可梦','按地区可选，也可在卡列兹花店(草)/凯娜港口(水)/釜炎药店(火)/水镜市(全)购买','说明页'),
 ('Dex number (decimal)','十进制编号','进化链后的数字即宝可梦 10 进制编号，详细图鉴红框输入此号','说明页 第3条'),
 ('Cheat code (raw code)','金手指／原始码','按 2.1~2.65 各版本分别列出，详见「金手指」页','金手指页'),
]

STATUS = [
 ('Poison','中毒','每回合损失 1/8 最大 HP',''),
 ('Badly Poison / Toxic','剧毒','伤害逐回合递增','本作「剧毒地形」为永久存在（见特性1 恶臭 Stench 补充说明）'),
 ('Burn','灼伤','物理招式伤害减半 + 每回合损失 1/16 HP',''),
 ('Paralysis (PARA)','麻痹','速度减半，概率无法行动',''),
 ('Sleep (SLP)','睡眠','数回合无法行动；本作「睡觉」招式可被部分特性阻断',''),
 ('Frostbite','冻伤','本作删除「冰冻」，改为「冻伤」：特殊招式伤害减半，回合末损失 1/16 最大 HP','与灼伤对应，属 ER 原创状态'),
 ('Flinch','畏缩','当回合无法行动','恶臭 Stench 为 10% 畏缩'),
 ('Confusion','混乱','本作重做：不再随机攻击自己，而是正常使用招式但受到所造成伤害 1/3 的反伤（2.65 又改回原版）',''),
 ('Infatuation / Attract','着迷','本作重做：招式威力减半（原版为 50% 概率不能行动）',''),
 ('Fear','恐惧','2 回合无法换走，且受到伤害 +50%','ER 原创状态 enrage/drench 同批新增'),
 ('Bleed','流血','每回合掉 1/16 HP；造成伤害时不计算自身能力上升（类似对手纯朴）；剩饭/草场无法回复；岩石与幽灵属性免疫','只能用真正的回复招式解除，睡觉亦可；超级吸取/归天之翼类无效'),
 ('Drench','浸湿','ER 新增状态（2.65）',''),
 ('Enrage','愤怒','ER 新增状态（2.65）；愤怒产生的反伤可被坚硬脑袋类特性免疫',''),
 ('Disable / Torment / Encore','封印／定身法／再来一次','限制招式使用的状态类效果','压制 Suppress 登场施加定身法'),
 ('Trap / Bound','束缚','无法替换下场','吸盘 Suction Cups 可免疫强制换下'),
 ('Ice Statue','冰雕','冰属性招式附加（见特性 979 Hollow Ice Zone）','ER 原创'),
]

WEATHER_FIELD = [
 ('Rain','雨（下雨）','水属性招式 +50%，火属性 -50%；本作 8 回合','降雨 Drizzle / 雨盘 Rain Dish / 悠游自如 Swift Swim'),
 ('Harsh Sunlight','大晴天','火属性 +50%，水属性 -50%','干旱 Drought / 叶绿素 Chlorophyll'),
 ('Sandstorm','沙暴','每回合非岩/地/钢损失 1/16 HP；岩属性特防 +50%','扬沙 Sand Stream / 挖沙 Sand Rush'),
 ('Hail','冰雹','本作删除「雪天」改回冰雹：每回合砸 1/16 HP，对冰系物防加成为 9 代雪天水平','降雪 Snow Warning / 拨雪 Slush Rush'),
 ('Fog','雾天','除超能与幽灵外每回合失去 1 级强化（只降不升）；幽灵系受伤 -20%；天气类回复招式回复量减半；复生祈祷与收获失效；所有宝可梦用诅咒均为幽灵版效果；气象球变幽灵系且威力翻倍','造雾机 Fog Machine / 烟雾球改为雾天岩石（8→12 回合）'),
 ('Heavy Rain','始源之海（原始天气）','火属性招式无法使用','特性 189'),
 ('Intense Sun','终结之地（原始天气）','水属性招式无法使用','特性 190'),
 ('Strong Winds','德尔塔气流（原始天气）','天气类招式无法使用','特性 191'),
 ('Electric Terrain','电气场地','电属性招式增强，地面上的宝可梦不会睡眠','电气制造者 Electro Surge'),
 ('Psychic Terrain','精神场地','超能招式增强，免疫先制招式','精神制造者 Psychic Surge'),
 ('Misty Terrain','薄雾场地','龙系伤害减半，免疫异常状态','薄雾制造者 Misty Surge'),
 ('Grassy Terrain','青草场地','草系招式增强，每回合回血','青草制造者 Grassy Surge'),
 ('Poison Terrain / Toxic Terrain','毒场（剧毒地形）','ER 新增场地；本作永久存在','恶臭 Stench 补充说明'),
 ('Trick Room','戏法空间','3 回合内速度慢的先出手','特性 346 扭曲空间'),
 ('Inverse Room','逆向空间','3 回合内属性相性反转','特性 473 反转'),
 ('Gravity','重力','5~8 回合内命中与飘浮判定改变','特性 436 阿特拉斯 / 443 引力井'),
 ('Smokescreen','烟幕','3 回合，己方全队闪避 +25%','特性 73 白烟'),
 ('Aurora Veil','极光幕','5 回合（光之黏土 8 回合）己方物理与特殊伤害减半','特性 348 北风'),
 ('Rainbow Pledge','彩虹誓约','ER 新增场地效果','特性 927 品味彩虹'),
]

TYPES = [
 ('0','Normal','一般'),('1','Fighting','格斗'),('2','Fire','火'),('3','Ice','冰'),
 ('4','Electric','电'),('5','Bug','虫'),('6','Flying','飞行'),('7','Steel','钢'),
 ('8','Grass','草'),('9','Ground','地面'),('10','Poison','毒'),('11','Dark','恶'),
 ('12','Water','水'),('13','Psychic','超能'),('14','Rock','岩石'),('15','Dragon','龙'),
 ('16','Ghost','幽灵'),('17','Fairy','妖精'),('18','Typeless (???)','神秘（无属性）'),
]

CATS = [
 ('0','PHYSICAL','物理','以攻击/防御结算'),
 ('1','SPECIAL','特殊','以特攻/特防结算'),
 ('2','STATUS','变化','无威力，产生状态/能力变化'),
 ('3','USE_HIGHEST_OFFENSE','使用最高攻击','自动取自身攻击与特攻中较高的一项结算（如三重攻击、爆炸烈焰、加农水炮）'),
 ('4','HITS_DEF','对方物防结算','超能/草系特殊招式改为按对手物防结算（如魔法叶、精神冲击、精神击破、神秘之剑）'),
 ('5','USE_HIGHEST_DAMAGE','造成最高伤害','在两种结算方式中取伤害更高者（臂贝武器）'),
 ('6','HITS_SPDEF','对方特防结算','改为按对手特防结算（妖精法球）'),
]

FAMILY = [
 ('猛类 (Blaze family)','猛火/茂盛/激流/虫之预感等','本作统一：常态 1.2 倍，HP 1/3 以下 1.5 倍','Overgrow / Blaze / Torrent / Swarm'),
 ('猛猛类 (Blaze+ family)','地狱烈焰/激流(Riptide)/森林之怒/炼狱等','高级版：常态 1.3 倍，HP 1/3 以下 1.8 倍','Hellblaze / Riptide / Forest Rage / Purgatory'),
 ('皮肤类 (Skin family)','一般皮肤/冰冻皮肤/妖精皮肤/飞行皮肤/电气皮肤等','统一调整为 1.1 倍；高级皮肤（地之歌、猛鬼之声）1.2 倍','Normalize / Refrigerate / Pixilate / Aerilate / Galvanize'),
 ('天气类 (Weather family)','降雨/干旱/扬沙/降雪','持续回合统一改为 8 回合（带对应岩石 12 回合）','Drizzle / Drought / Sand Stream / Snow Warning'),
 ('天气加速类','悠游自如/叶绿素/挖沙/拨雪','由原版 2 倍改为 1.5 倍','Swift Swim / Chlorophyll / Sand Rush / Slush Rush'),
 ('多头类 (Multi Headed)','多头龙','两个头 1+0.25；三个头 1+0.2+0.15','Multi Headed'),
 ('拳类 (Punch)','铁拳 Iron Fist 系','拳类招式 +30%；「铁拳之力 Power Fists」改为按特防结算','Iron Fist / Power Fists'),
 ('踢类 (Kick)','前锋 Striker 系','踢类招式 +30%','Striker'),
 ('咬类 (Bite/Fang)','强壮之颚 Strong Jaw 系','啃咬类 +30%','Strong Jaw'),
 ('角/钻类 (Horn/Drill)','强大之角 Mighty Horn 系','角类与钻类 +30%','Mighty Horn'),
 ('刃类 (Slash)','锐利刀刃 Keen Edge 系','斩击类 +30%','Keen Edge'),
 ('箭类 (Arrow)','弓箭手 Archer 系','箭类 +30%','Archer'),
 ('炮类 (Launcher)','超级发射器 Mega Launcher 系','脉冲/波导/波动/光环类 +30%','Mega Launcher'),
 ('声音类 (Sound)','庞克摇滚/湿润之声等','声音类 +30%；「湿润之声」使一般系变水，「沙之歌」变地面','Punk Rock / Liquid Voice / Sand Song'),
 ('粉末类 (Powder)','鳞粉 Shield Dust 系','粉末类招式可被免疫','Shield Dust'),
 ('舞蹈类 (Dance)','舞者 Dancer 系','使用后可衔接追击（如分解 Break it Down）','Dancer'),
 ('投掷类 (Throwing)','岩石载荷 Rocky Payload 系','岩石类与投掷类 +50%','Rocky Payload'),
]

ITEM = [
 ('Light Ball','电气球','改为皮卡丘全家（含各形态雷丘）均可使用',''),
 ('Shell Bell','贝壳之铃','由原版吸血 1/8 改为 1/4',''),
 ('Smooth Rock','烟雾球','改为雾天岩石：雾天由 8 回合延长到 12 回合',''),
 ('Damp Rock / Heat Rock / Icy Rock','潮湿岩石／炎热岩石／冰冷岩石','对应天气由 8 回合延长到 12 回合',''),
 ('Terrain Extender','场地制造机','场地由 8 回合延长到 12 回合',''),
 ('Light Clay','光之黏土','极光幕等由 5 回合延长到 8 回合',''),
 ('Berries','树果','贪吃鬼改为 HP 1/2 触发（原版 1/4），且食用后额外回复 1/3 最大 HP',''),
 ('Focus Sash / Focus Band','气势披带／先制之爪','结实 Sturdy 效果等同于气势披带',''),
 ('Mega Stone','超级石','笨拙 Klutz 不会使超级石失效；察觉 Frisk 也不会阻止超级石生效',''),
 ('Red Card / Eject Button','红牌／逃脱按键','可脱离沙穴、磁力等束缚',''),
]

# ---------------- 构建工作簿 ----------------
wb = openpyxl.Workbook()
HDR_FILL = PatternFill('solid', fgColor='1F4E79')
HDR_FONT = Font(color='FFFFFF', bold=True, size=11)
TITLE_FONT = Font(bold=True, size=13, color='1F4E79')
WARN_FILL = PatternFill('solid', fgColor='FFF2CC')
BAD_FILL = PatternFill('solid', fgColor='FCE4E4')
OK_FILL = PatternFill('solid', fgColor='E9F2E9')
THIN = Side(style='thin', color='BFBFBF')
BORDER = Border(left=THIN, right=THIN, top=THIN, bottom=THIN)

def put_header(ws, headers, widths, row=1):
    for i, h in enumerate(headers, 1):
        c = ws.cell(row=row, column=i, value=h)
        c.fill = HDR_FILL; c.font = HDR_FONT
        c.alignment = Alignment(horizontal='center', vertical='center', wrap_text=True)
        c.border = BORDER
    for i, w in enumerate(widths, 1):
        ws.column_dimensions[get_column_letter(i)].width = w
    ws.freeze_panes = ws.cell(row=row + 1, column=1)
    ws.row_dimensions[row].height = 26

def fill_rows(ws, rows, start=2, wrap_cols=()):
    for ri, r in enumerate(rows, start):
        for ci, v in enumerate(r, 1):
            c = ws.cell(row=ri, column=ci, value=v)
            c.border = BORDER
            c.alignment = Alignment(vertical='top', wrap_text=(ci in wrap_cols))
    return start + len(rows)

# ===== 00 使用说明 =====
ws = wb.active; ws.title = '00-使用说明'
ws.column_dimensions['A'].width = 22
ws.column_dimensions['B'].width = 120
info = [
 ('文件主题', '《ER2.65beta 版图鉴 v0.3》(Elite Redux 改版) 翻译参考术语表 + 名称/效果不符错误报告'),
 ('数据来源', 'G:\\Download\\ER2.65beta版图鉴v0.2v0.3\\ER2.65beta版图鉴v0.3.xlsm'),
 ('', '英文名称与英文描述取自作者图鉴站 forwardfeed.github.io/ER-nextdex；中文名与中文描述取自「耿鬼汉化版」游戏内文本'),
 ('生成时间', '2026-11-03'),
 ('', ''),
 ('【页签导航】', ''),
 ('01-核心机制术语', '游戏系统层面的术语（特性/天性/种族值/场地/陷阱/难度等），含原文、译名、说明'),
 ('02-属性与状态', '18 属性对照 + 状态异常 + 天气/场地/空间 词条'),
 ('03-招式分类与特性族', 'ER 七种招式分类、特性族群（猛类/皮肤类/天气类…）、招式族群、道具改动'),
 ('04-特性术语表', '1035 条特性：英文 → 中文 → 效果（中英对照），带「是否存疑」标记'),
 ('05-招式术语表', '1031 条招式：英文 → 中文 → 属性/威力/分类 → 效果'),
 ('06-宝可梦名对照', '1909 条宝可梦：编号 / 英文 / 中文 / 属性 / 特性 / 天性'),
 ('07-错误报告(名称≠效果)', '★ 核心交付：逐条列出名称与实际效果对不上的条目、原始内容、问题说明、修正建议'),
 ('08-错误报告(其他问题)', '缺译、译名冲突、乱码、无效果特性等其他数据问题'),
 ('', ''),
 ('【重要提醒 1】', '本图鉴的特性表存在 4 个「英文列与中文列整段错位」区段（386-393、868-873、907-933、945-990），'),
 ('', '共核实出 77 条「名称与实际效果对不上」的错误项，另有 8 项独立错误，全部列在「07-错误报告」页。'),
 ('', '这些行的英文名称与英文效果是自洽的，错的是与之配对的中文名/中文描述/游戏内说明。'),
 ('', '→ 做翻译时请以「07-错误报告」给出的「建议译名」为准，不要直接抄表里的中文。'),
 ('【重要提醒 2】', '招式表第 1005~1031 行（共 27 条）完全没有中文译名，需补译。'),
 ('【重要提醒 3】', '招式 #966 Spectral Flame 的属性列标为「一般」，但原始码与中文名都指向「幽灵」，属源站数据错误。'),
 ('【重要提醒 4】', 'ER 对大量原版特性做了重做（例如「瑜伽之力 Pure Power」改为加倍特攻而非物攻），'),
 ('', '译名沿用官方中文但效果已变，属「翻译陷阱」，已在 04 页用「△ER改动」标注典型条目。'),
]
for i, (a, b) in enumerate(info, 1):
    ws.cell(row=i, column=1, value=a).font = Font(bold=True, color='1F4E79')
    c = ws.cell(row=i, column=2, value=b); c.alignment = Alignment(wrap_text=True, vertical='top')

# ===== 01 核心机制术语 =====
ws = wb.create_sheet('01-核心机制术语')
put_header(ws, ['原文术语', '对应译名', '简要说明', '出处/备注'], [30, 34, 82, 26])
fill_rows(ws, CORE, wrap_cols=(3, 4))

# ===== 02 属性与状态 =====
ws = wb.create_sheet('02-属性与状态')
put_header(ws, ['类别', '原文术语', '对应译名', '简要说明 / 本作改动', '备注'], [14, 26, 24, 78, 30])
rows = []
for code, en, cn in TYPES:
    note = ''
    if code == '18':
        note = '仅 2 个招式：#217 Present 礼物、#1022 Concoction；实际为「无属性/???」，译「神秘」易误解'
    rows.append(['属性', f'{en}  (raw={code})', cn, 'ER 内部属性代号见此列；金手指改属性时按代号填写', note])
for a, b, c_, d in STATUS:
    rows.append(['状态异常', a, b, c_, d])
for a, b, c_, d in WEATHER_FIELD:
    rows.append(['天气/场地/空间', a, b, c_, d])
fill_rows(ws, rows, wrap_cols=(4, 5))
for r in range(2, 2 + len(rows)):
    if ws.cell(row=r, column=1).value == '状态异常':
        ws.cell(row=r, column=1).fill = OK_FILL
    elif ws.cell(row=r, column=1).value == '天气/场地/空间':
        ws.cell(row=r, column=1).fill = WARN_FILL

# ===== 03 招式分类与特性族 =====
ws = wb.create_sheet('03-招式分类与特性族')
put_header(ws, ['类别', '原文术语', '对应译名', '简要说明', '代表条目'], [18, 30, 26, 80, 34])
rows = [['招式分类', f'{code} = {en}', cn, d, '' ] for code, en, cn, d in CATS]
rows += [['特性族群', a, b, c_, d] for a, b, c_, d in FAMILY]
rows += [['道具改动', a, b, c_, d] for a, b, c_, d in ITEM]
fill_rows(ws, rows, wrap_cols=(4, 5))

# ===== 04 特性术语表 =====
ws = wb.create_sheet('04-特性术语表')
put_header(ws, ['行号', '原文术语 (EN)', '译名 (CN)', '效果(英文原文)',
                '效果(中文)', '较之原版变化', '游戏内详细说明', '可信度标记'],
           [7, 30, 24, 62, 60, 34, 70, 22])
rows = []
for i, r in enumerate(feat, 1):
    if not r[2].strip():
        continue
    g = (r[6] if len(r) > 6 else '').strip()
    h = (r[7] if len(r) > 7 else '').strip()
    if i in FIX_ROWS:
        mark = '⚠ 名称/效果不符（见07页）'
    elif 'no effect' in r[4].lower() or 'not implemented' in r[4].lower():
        mark = '△ 无效果/未实装'
    else:
        mark = '✓ 可用'
    rows.append([i, r[2], r[3], r[4], r[5], g, h, mark])
fill_rows(ws, rows, wrap_cols=(4, 5, 6, 7))
for ri, r in enumerate(rows, 2):
    v = r[7]
    ws.cell(row=ri, column=8).fill = BAD_FILL if v.startswith('⚠') else (WARN_FILL if v.startswith('△') else OK_FILL)
ws.auto_filter.ref = f'A1:H{1+len(rows)}'

# ===== 05 招式术语表 =====
ws = wb.create_sheet('05-招式术语表')
put_header(ws, ['行号', '原文术语 (EN)', '译名 (CN)', '属性', '威力', '分类',
                '效果(英文原文)', '效果(中文)', '标记'], [7, 30, 24, 10, 8, 22, 62, 60, 20])
CATNAME = {c[0]: c[2] for c in CATS}
rows = []
for i, r in enumerate(moves, 1):
    if not r[2].strip() or r[2].strip() == '-':
        continue
    code = r[6].strip()
    cat = CATNAME.get(code, '')
    if code in ('3', '4', '5', '6'):
        cat += f'（{dict((c[0],c[1]) for c in CATS)[code]}）'
    mark = ''
    if not r[3].strip() or r[3].strip() == r[2].strip():
        mark = '△ 缺中文译名'
    elif r[7].strip() == '神秘':
        mark = '△ 无属性(???)'
    elif i == 966:
        mark = '⚠ 属性列有误'
    else:
        mark = '✓ 可用'
    rows.append([i, r[2], r[3], r[7], r[4], cat, r[9], r[10], mark])
fill_rows(ws, rows, wrap_cols=(7, 8))
for ri, r in enumerate(rows, 2):
    v = r[8]
    ws.cell(row=ri, column=9).fill = BAD_FILL if v.startswith('⚠') else (WARN_FILL if v.startswith('△') else OK_FILL)
ws.auto_filter.ref = f'A1:I{1+len(rows)}'

# ===== 06 宝可梦名对照 =====
ws = wb.create_sheet('06-宝可梦名对照')
put_header(ws, ['编号', '原文术语 (EN)', '译名 (CN)', '属性1', '属性2',
                '特性1', '特性2', '特性3', '天性1', '天性2', '天性3'],
           [8, 30, 22, 10, 10, 18, 18, 18, 18, 18, 18])
rows = []
for r in dex[3:]:
    if len(r) < 22 or not r[5].strip():
        continue
    rows.append([r[2], r[5], r[6], r[7], r[8], r[16], r[17], r[18], r[19], r[20], r[21]])
fill_rows(ws, rows)
ws.auto_filter.ref = f'A1:K{1+len(rows)}'

# ===== 07 错误报告（名称≠效果）=====
ws = wb.create_sheet('07-错误报告(名称≠效果)')
ws.column_dimensions['A'].width = 6
for col, w in zip('BCDEFG', (12, 26, 22, 22, 58, 62)):
    ws.column_dimensions[col].width = w

r = 1
c = ws.cell(row=r, column=1, value='错误报告：术语名称与其实际效果不符（源站数据与中文源整段错位）')
c.font = TITLE_FONT
r += 1
c = ws.cell(row=r, column=1, value='核对方法：英文名称↔英文效果描述自洽；中文名↔中文描述↔游戏内说明三者自洽；但英文组与中文组在下列区段互相错位，导致「名称对不上效果」。')
c.font = Font(italic=True, color='7F6000')
r += 2

# 区段总览
ws.cell(row=r, column=1, value='一、错位区段总览').font = TITLE_FONT
r += 1
hdr = ['区段', '行范围', '涉及行数', '错位性质', '问题说明']
widths = [14, 16, 12, 22, 96]
for i, h in enumerate(hdr, 1):
    cc = ws.cell(row=r, column=i, value=h)
    cc.fill = HDR_FILL; cc.font = HDR_FONT
    cc.alignment = Alignment(horizontal='center', wrap_text=True)
r += 1
zonerows = [
    ['区段 1', '386 - 393', '8', '块内旋转错位', SHIFT_ZONES[0][2]],
    ['区段 2', '868 - 873', '6', '块内置换', SHIFT_ZONES[1][2]],
    ['区段 3', '907 - 933', '20（含正确行）', '两组排序不一致', SHIFT_ZONES[2][2]],
    ['区段 4', '945 - 990', '46', '累计 -2 行错位', SHIFT_ZONES[3][2]],
]
for zr in zonerows:
    for i, v in enumerate(zr, 1):
        cc = ws.cell(row=r, column=i, value=v)
        cc.border = BORDER
        cc.alignment = Alignment(vertical='top', wrap_text=(i == 5))
        cc.fill = WARN_FILL
    ws.row_dimensions[r].height = 42
    r += 1
r += 1

# 逐条明细
ws.cell(row=r, column=1, value='二、逐条明细（错误项 / 原始内容 / 疑似问题 / 修正建议）').font = TITLE_FONT
r += 1
hdr2 = ['行号', '原文术语 (EN)', '表中译名(错)', '建议译名(对)', '表中所列效果（英文原文）', '问题说明与修正建议']
for i, h in enumerate(hdr2, 1):
    cc = ws.cell(row=r, column=i, value=h)
    cc.fill = HDR_FILL; cc.font = HDR_FONT
    cc.alignment = Alignment(horizontal='center', wrap_text=True)
ws.row_dimensions[r].height = 28
r += 1
for (ln, en, wrong, right, eff, why) in FIX:
    vals = [ln, en, wrong, right, eff, why]
    for i, v in enumerate(vals, 1):
        cc = ws.cell(row=r, column=i, value=v)
        cc.border = BORDER
        cc.alignment = Alignment(vertical='top', wrap_text=(i >= 5))
    ws.cell(row=r, column=3).fill = BAD_FILL
    ws.cell(row=r, column=4).fill = OK_FILL
    r += 1
r += 1

# 非错位类的名称/效果不符
ws.cell(row=r, column=1, value='三、其他「名称与效果对不上」的独立错误项').font = TITLE_FONT
r += 1
hdr3 = ['位置', '原文术语 (EN)', '表中译名', '原始内容', '疑似问题说明', '修正建议']
for i, h in enumerate(hdr3, 1):
    cc = ws.cell(row=r, column=i, value=h)
    cc.fill = HDR_FILL; cc.font = HDR_FONT
    cc.alignment = Alignment(horizontal='center', wrap_text=True)
r += 1
other = [
 ['招式表 第966行', 'Spectral Flame', '幽灵火',
  '属性列 = 一般；属性原始码 raw = 16；威力 0；分类 = 变化',
  'raw=16 在本表中对应「幽灵」（共 39 个幽灵属性招式全部为 raw=16），且中文名也叫「幽灵火」。'
  '但属性列被标成「一般」，与名称和原始数据都矛盾，属源站数据错误。',
  '属性列改为「幽灵」。注意其效果是「灼伤目标（含火属性）并在雾天压制特性」，属变化招式，威力 0 正常。'],
 ['招式表 第217行', 'Present', '礼物',
  '属性列 = 神秘（raw=18）；威力 80；分类为空',
  'raw=18 实为「无属性 / ???」，中文写作「神秘」容易被误认为第 19 种属性。'
  '该招式描述为「对对手造成无类型伤害，恢复盟友 50% HP」，确为无属性。',
  '译名建议改为「无属性（???）」或保留「神秘」但在术语表中注明其含义；分类列缺失，应补「变化/特殊」判定。'],
 ['招式表 第1022行', 'Concoction', '（未翻译）',
  '中文名与中文描述均为英文原文；属性 = 神秘（raw=18）；威力 30；分类为空',
  '该条完全未翻译，且同样是无属性招式。',
  '建议补译：Concoction → 调配／混合物；属性标注为无属性。'],
 ['特性表 第74行', 'Pure Power', '瑜伽之力',
  '效果：Doubles own Sp.Atk stat（自身特攻加倍）',
  '原版 Pure Power（瑜伽之力）是「物攻加倍」，与 Huge Power（大力士，第37行，物攻加倍）重合。'
  'ER 将其改为「特攻加倍」。译名沿用官方中文，但名称「Power」不再指向物理攻击，极易误译/误解。',
  '译名可保留「瑜伽之力」，但必须在术语表注明「本作效果＝特攻 ×2，与原版不同」；'
  '第301行 Cryptic Power 同样译为「神秘力量」且也是特攻加倍，注意区分。'],
 ['特性表 第287/301/494行', 'Mystic Power / Cryptic Power / Arcane Force', '神秘力量（三者同名）',
  '287：所有招式获得本系加成；301：特攻加倍；494：所有招式获得本系加成且效果绝佳 +10%',
  '三个不同的英文特性共用一个中文名「神秘力量」，且三者效果互不相同，'
  '按中文名无法区分，翻译与查表都会出错。',
  '建议区分译名：Mystic Power → 神秘之力；Cryptic Power → 隐秘之力；Arcane Force → 奥术之力。'],
 ['特性表 第219/232行', 'Dazzling / Prism Armor', '棱镜装甲（两者同名）',
  '219 Dazzling：免疫对手先制招式；232 Prism Armor：受到效果绝佳伤害 -35%',
  '两个不同特性共用一个中文名「棱镜装甲」，效果完全不同。',
  '「棱镜装甲」应专属 Prism Armor；Dazzling 建议改译「炫彩之躯」或「鲜艳之躯」以作区分。'],
 ['特性表 第67/418行', 'Torrent / Riptide', '激流（两者同名）',
  '67 Torrent：水属性 1.2 倍，HP<1/3 时 1.5 倍；418 Riptide：水属性 1.3 倍，HP<1/3 时 1.8 倍',
  'Torrent（原版猛类）与 Riptide（本作猛猛类）中文名同为「激流」，倍率不同。',
  '建议 Torrent → 激流；Riptide → 激潮／回流，加以区分。'],
 ['特性表 第1016行', '3 > 1', '3 ョ 1',
  '特性名含半角大于号，中文列显示为「3 ョ 1」',
  '字符编码错误：大于号「>」被转成了日文片假名「ョ」(U+30E7)。',
  '修正为「3 > 1」。批量导出/转码时应对「>」「<」「&」等字符做转义处理。'],
]
for o in other:
    for i, v in enumerate(o, 1):
        cc = ws.cell(row=r, column=i, value=v)
        cc.border = BORDER
        cc.alignment = Alignment(vertical='top', wrap_text=(i >= 4))
    ws.cell(row=r, column=1).fill = BAD_FILL
    ws.row_dimensions[r].height = 62
    r += 1

# ===== 08 其他数据问题 =====
ws = wb.create_sheet('08-错误报告(其他问题)')
put_header(ws, ['类别', '位置', '条目数', '说明', '建议'], [18, 26, 10, 78, 56])
o8 = [
 ['缺译', '招式表 第1005 - 1031 行', 27,
  'Incite、Jetstream Burst、Sky Quake、Toxic Terrain、Sunstrike (N)、Tempest Storm (N)、'
  'Prism Blast (P)、Seismic Slam、Chiller、Spread Bomb、Ball Toss、Party Favors (P)、Shot Put、'
  'Block Dropper、Saber Slashes、Dragon Dash、Pocket Sand、Concoction、Hacksaw、Godspeed、'
  'Psycho Wave、Atomic Fire、Rain Flush、Ice Wall、Obscured Shot、Blitz Arrow、Rumble Kick —— '
  '中文名与中文描述均缺失（#1022 连英文描述都未翻译）。',
  '优先补译这 27 条；带 (N)/(P) 后缀者为未实装或形态限定版本，译名中保留后缀标记。'],
 ['缺译/缺值', '招式表 分类列为空', 2,
  '#217 Present 礼物、#1022 Concoction 的招式分类（物理/特殊/变化）为空。',
  '补填分类；无属性伤害通常按「特殊」结算，需在游戏内实测确认。'],
 ['无效果特性', '特性表 第167/237/369/828 行', 4,
  'Cheek Pouch 颊囊「This ability has no effect.」；Ball Fetch 拾球「No effect in battle.」；'
  'Bad Company 坏伙伴「Not implemented right now.」；Overzealous (N) 过度狂热(N) 游戏内说明为「不会实装」。',
  '译名可保留，但需在术语表标注「本作无效果/未实装」，避免玩家误以为翻译遗漏。'],
 ['负面特性无持有者', '特性表 第394 行', 1,
  'Lethargy 昏昏欲睡：作者备注「没有精灵有这个特性，明显的负面特性，以前版本作者设计给 mega 请假王的」。',
  '保留条目并注明「当前版本无宝可梦持有」。'],
 ['译名冲突', '特性表 多处', 15,
  '同一中文名对应多个英文特性：恶臭(Stench/Malodor)、加速(Speed Boost/Accelerate)、'
  '毒刺(Poison Point/Poison Quills)、神奇鳞片(Marvel Scale/Wonder Scale)、治愈之心(Healer/Triage)、'
  '压制(Overwhelm/Subdue/Suppress)、烈焰之颚(Flaming Jaws/Flaming Maw)、电击之颚(Shocking Jaws/Shocking Maw)、'
  '致命精准(Fatal Precision/Deadly Precision)、神秘力量(×3)、梦境(Dream State/Dreamscape)等。',
  '翻译时以英文名为准反查，中文名不可作为唯一键；建议对冲突项加后缀区分（如「治愈之心(回复)」「治愈之心(优先度)」）。'],
 ['版本差异', '说明页 常见问题 第6条', 1,
  '「英文 debug 版破格特性失效，汉化版是以这个为蓝本所以继承了这个问题，英文普通版破格可以正常生效。」',
  '涉及破格(Mold Breaker)类特性的文案需注明版本差异，避免误判为翻译错误。'],
 ['机翻痕迹', '招式表 中文描述列', '全表',
  '作者自述招式中文描述为 GPT 机翻，部分表述生硬（如「基于空气(Air)」「基于场地(Field)」）。',
  '术语表 03 页已整理招式族群的标准说法，校对时可对照替换。'],
]
fill_rows(ws, o8, wrap_cols=(4, 5))
for ri in range(2, 2 + len(o8)):
    ws.cell(row=ri, column=1).fill = WARN_FILL
    ws.row_dimensions[ri].height = 60

wb.save(OUT)
print('saved:', OUT)
print('特性条目', sum(1 for r in feat if r[2].strip()))
print('招式条目', sum(1 for r in moves if r[2].strip() and r[2].strip() != '-'))
print('图鉴条目', sum(1 for r in dex[3:] if len(r) > 6 and r[5].strip()))
print('错误明细条数', len(FIX))
