# -*- coding: utf-8 -*-
import html, importlib.util, sys
spec = importlib.util.spec_from_file_location('bg', 'build_glossary.py')
# 只复用数据，不重复生成 xlsx：这里直接内联读取
import csv
exec(open('build_glossary.py', encoding='utf-8').read().split('# ---------------- 构建工作簿')[0])

def esc(s):
    return html.escape(str(s if s is not None else ''))

def table(headers, rows, cls=''):
    h = ''.join(f'<th>{esc(x)}</th>' for x in headers)
    body = ''
    for r in rows:
        body += '<tr>' + ''.join(f'<td>{esc(x)}</td>' for x in r) + '</tr>'
    return f'<table class="{cls}"><thead><tr>{h}</tr></thead><tbody>{body}</tbody></table>'

parts = []
parts.append('''<!DOCTYPE html><html lang="zh-CN"><head><meta charset="utf-8">
<title>ER2.65beta 翻译术语表与错误报告</title><style>
body{font-family:-apple-system,"Segoe UI","Microsoft YaHei",sans-serif;margin:0;background:#f5f7fa;color:#1f2933;line-height:1.65}
.wrap{max-width:1180px;margin:0 auto;padding:32px 24px 80px}
h1{font-size:26px;color:#1F4E79;margin:0 0 6px}
h2{font-size:20px;color:#1F4E79;margin:36px 0 12px;padding-bottom:8px;border-bottom:2px solid #1F4E79}
h3{font-size:16px;color:#2c5f8a;margin:22px 0 8px}
.sub{color:#6b7785;font-size:13px}
.card{background:#fff;border:1px solid #e3e8ee;border-radius:8px;padding:18px 20px;margin:14px 0;box-shadow:0 1px 3px rgba(0,0,0,.04)}
table{border-collapse:collapse;width:100%;font-size:13px;background:#fff}
th{background:#1F4E79;color:#fff;padding:8px 10px;text-align:left;font-weight:600;position:sticky;top:0}
td{border-bottom:1px solid #eef1f5;padding:7px 10px;vertical-align:top}
tr:nth-child(even) td{background:#fafbfc}
.bad{background:#FCE4E4!important;font-weight:600}
.good{background:#E9F2E9!important;font-weight:600}
.warn{background:#FFF2CC!important}
code{background:#eef1f5;padding:1px 5px;border-radius:3px;font-size:12px}
.tag{display:inline-block;padding:1px 7px;border-radius:10px;font-size:11px;font-weight:600}
.t-bad{background:#FCE4E4;color:#a33}
.t-ok{background:#E9F2E9;color:#2b6}
.t-warn{background:#FFF2CC;color:#96700a}
ul{margin:6px 0 6px 20px;padding:0}
li{margin:3px 0}
.grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(200px,1fr));gap:10px;margin:12px 0}
.kpi{background:#fff;border:1px solid #e3e8ee;border-radius:8px;padding:12px 14px}
.kpi b{display:block;font-size:22px;color:#1F4E79}
.kpi span{font-size:12px;color:#6b7785}
</style></head><body><div class="wrap">''')

parts.append(f'''<h1>《ER2.65beta 版图鉴 v0.3》翻译参考术语表 &amp; 名称/效果不符错误报告</h1>
<div class="sub">源文件：ER2.65beta版图鉴v0.3.xlsm　·　英文源：作者图鉴站 ER-nextdex　·　中文源：耿鬼汉化版游戏内文本　·　生成时间：2026-11-03</div>
<div class="grid">
<div class="kpi"><b>1029</b><span>特性术语条目</span></div>
<div class="kpi"><b>1031</b><span>招式术语条目</span></div>
<div class="kpi"><b>1909</b><span>宝可梦名对照</span></div>
<div class="kpi"><b style="color:#c0392b">77</b><span>名称≠效果 错误明细</span></div>
<div class="kpi"><b style="color:#c0392b">8</b><span>独立错误项</span></div>
<div class="kpi"><b style="color:#96700a">27</b><span>招式缺中文译名</span></div>
</div>''')

parts.append('<div class="card"><h3 style="margin-top:0">结论速览</h3><ul>'
 '<li><b>特性表有 4 个整段错位区段</b>（第 386–393、868–873、907–933、945–990 行）：英文名称与英文效果是自洽的，'
 '中文名、中文描述、游戏内说明这三者也互相自洽，但<b>两组之间整段错位</b>，于是出现「名称对不上效果」。</li>'
 '<li>错位不是简单平移：区段 1 是块内旋转，区段 2 是块内置换，区段 3 是两源排序不同，区段 4 是中文源多出 2 条导致累计 −2 行错位。</li>'
 '<li>招式表整体对齐良好，但发现 <b>1 处属性错误</b>（#966 Spectral Flame 幽灵火 被标为「一般」）、'
 '<b>27 条完全缺中文译名</b>（#1005–1031）。</li>'
 '<li>另有 <b>15 组中文译名冲突</b>（不同英文特性共用同一中文名，如「神秘力量」对应 3 个特性），按中文名反查必然出错。</li></ul></div>')

# 01
parts.append('<h2>一、核心机制术语</h2>')
parts.append(table(['原文术语','对应译名','简要说明','出处/备注'], CORE))

parts.append('<h2>二、属性 / 状态异常 / 天气·场地·空间</h2>')
rows=[]
for code,en,cn in TYPES:
    note=''
    if code=='18': note='仅 2 个招式(#217 Present、#1022 Concoction)，实为「无属性/???」，译「神秘」易误解'
    rows.append(['属性',f'{en} (raw={code})',cn,'金手指改属性按此代号填写',note])
for a,b,c_,d in STATUS: rows.append(['状态异常',a,b,c_,d])
for a,b,c_,d in WEATHER_FIELD: rows.append(['天气/场地/空间',a,b,c_,d])
t=table(['类别','原文术语','对应译名','简要说明 / 本作改动','备注'],rows)
parts.append(t)

parts.append('<h2>三、招式分类 / 特性族群 / 道具改动</h2>')
rows=[['招式分类',f'{c[0]} = {c[1]}',c[2],c[3],''] for c in CATS]
rows+=[['特性族群',a,b,c_,d] for a,b,c_,d in FAMILY]
rows+=[['道具改动',a,b,c_,d] for a,b,c_,d in ITEM]
parts.append(table(['类别','原文术语','对应译名','简要说明','代表条目'],rows))

# 错误报告
parts.append('<h2>四、错误报告：术语名称与其实际效果不符</h2>')
parts.append('<h3>4.1 错位区段总览</h3>')
parts.append(table(['区段','行范围','错位性质','问题说明'],[
 ['区段 1','386 – 393','块内旋转错位',SHIFT_ZONES[0][2]],
 ['区段 2','868 – 873','块内置换',SHIFT_ZONES[1][2]],
 ['区段 3','907 – 933','两源排序不一致',SHIFT_ZONES[2][2]],
 ['区段 4','945 – 990','累计 −2 行错位',SHIFT_ZONES[3][2]],
]))
parts.append('<h3>4.2 逐条明细（77 条）</h3>')
rows=[[f[0],f[1],f[2],f[3],f[4],f[5]] for f in FIX]
t=table(['行号','原文术语 (EN)','表中译名(错)','建议译名(对)','表中所列效果（英文原文）','问题说明与修正建议'],rows)
t=t.replace('<tr>','<tr>')
parts.append('<div style="max-height:none">'+t+'</div>')

parts.append('<h3>4.3 其他独立错误项</h3>')
rows=[[o[0],o[1],o[2],o[3],o[4],o[5]] for o in []]
other=[
 ['招式 #966','Spectral Flame','幽灵火','属性列=一般；原始码 raw=16；威力0；分类=变化',
  'raw=16 在本表中对应「幽灵」（39 个幽灵属性招式全部为 raw=16），中文名也叫「幽灵火」，唯独属性列写成「一般」，属源站数据错误','属性列改为「幽灵」'],
 ['招式 #217','Present','礼物','属性=神秘(raw=18)；威力80；分类为空',
  'raw=18 实为无属性/???，中文「神秘」易被当成第 19 种属性；且分类列缺失','译名改「无属性(???)」并注明；补填分类'],
 ['招式 #1022','Concoction','（未翻译）','中英文名与描述均为英文；属性=神秘；威力30；分类为空','完全未翻译','补译：调配/混合物；属性标为无属性'],
 ['特性 #74','Pure Power','瑜伽之力','Doubles own Sp.Atk stat',
  '原版为物攻加倍，ER 改为特攻加倍。名称含 Power 但不再指向物理攻击，典型翻译陷阱','保留译名但注明「本作＝特攻×2」'],
 ['特性 #287/301/494','Mystic Power / Cryptic Power / Arcane Force','神秘力量（三者同名）','287 本系加成；301 特攻×2；494 本系加成且效果绝佳+10%',
  '三个不同特性共用一名，效果互不相同','区分译名：神秘之力 / 隐秘之力 / 奥术之力'],
 ['特性 #219/232','Dazzling / Prism Armor','棱镜装甲（两者同名）','219 免疫先制招式；232 效果绝佳伤害 −35%','同名不同效','「棱镜装甲」专属 Prism Armor；Dazzling 改译「鲜艳之躯/炫彩之躯」'],
 ['特性 #67/418','Torrent / Riptide','激流（两者同名）','67：1.2/1.5 倍；418：1.3/1.8 倍','猛类与猛猛类倍率不同却同名','Riptide 改译「激潮/回流」'],
 ['特性 #1016','3 > 1','3 ョ 1','特性名中的「>」被转成日文片假名「ョ」(U+30E7)','字符编码错误','修正为「3 > 1」；导出时对 > < & 做转义'],
]
parts.append(table(['位置','原文术语 (EN)','表中译名','原始内容','疑似问题说明','修正建议'],other))

parts.append('<h2>五、其他数据问题（缺译 / 冲突 / 无效）</h2>')
o8=[
 ['缺译','招式 #1005 – #1031',27,'Incite、Jetstream Burst、Sky Quake、Toxic Terrain、Sunstrike (N)、Tempest Storm (N)、Prism Blast (P)、Seismic Slam、Chiller、Spread Bomb、Ball Toss、Party Favors (P)、Shot Put、Block Dropper、Saber Slashes、Dragon Dash、Pocket Sand、Concoction、Hacksaw、Godspeed、Psycho Wave、Atomic Fire、Rain Flush、Ice Wall、Obscured Shot、Blitz Arrow、Rumble Kick 中文名与描述全缺','优先补译；带 (N)/(P) 后缀者保留后缀'],
 ['无效特性','#167 / #237 / #369 / #828',4,'Cheek Pouch 颊囊「no effect」、Ball Fetch 拾球「no effect in battle」、Bad Company 坏伙伴「not implemented」、Overzealous (N)「不会实装」','保留译名，标注「本作无效果/未实装」'],
 ['无持有者','#394',1,'Lethargy 昏昏欲睡：作者备注「没有精灵有这个特性」','注明「当前版本无宝可梦持有」'],
 ['译名冲突','特性表多处',15,'恶臭(Stench/Malodor)、加速(Speed Boost/Accelerate)、毒刺(Poison Point/Poison Quills)、神奇鳞片(Marvel Scale/Wonder Scale)、治愈之心(Healer/Triage)、压制(Overwhelm/Subdue/Suppress)、烈焰之颚、电击之颚、致命精准、神秘力量(×3)、梦境 等','中文名不可作唯一键；建议加后缀区分'],
 ['版本差异','说明页 常见问题 6',1,'英文 debug 版「破格」失效，汉化版继承了这个问题，英文普通版正常','相关文案需注明版本，避免误判为翻译错误'],
 ['机翻痕迹','招式表中文描述列','全表','作者自述为 GPT 机翻，如「基于空气(Air)」「基于场地(Field)」','对照本页「特性族群/招式族群」标准说法校对'],
]
parts.append(table(['类别','位置','条目数','说明','建议'],o8))

parts.append('</div></body></html>')

open('glossary.html','w',encoding='utf-8').write('\n'.join(parts))
print('html ok')
