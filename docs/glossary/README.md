# ER 2.65beta 翻译术语表与错误报告

汉化工程使用的《Pokémon Elite Redux》2.65beta 版术语表，以及从图鉴表提取数据后生成的名称与效果核对清单。正文按「核心机制术语、属性与状态、招式分类与特性族、特性、招式、宝可梦名」分组，附错误报告两表。

## 目录内容

| 文件 | 说明 |
| --- | --- |
| `glossary.html` | 网页版，下载后用浏览器打开即可查看，表格带检索布局 |
| `glossary.xlsx` | 工作表版，含 `00-使用说明` 至 `08-错误报告(其他问题)` 共九个表 |
| `out/*.csv` | 从图鉴 xlsm 提取的原始表，是生成脚本的输入 |
| `build_glossary.py` | 读取 `out/*.csv`，生成 `glossary.xlsx` |
| `build_html.py` | 复用 `build_glossary.py` 的数据段，生成 `glossary.html` |

当前收录特性 1029 条、招式 1031 条、图鉴条目 1909 条，错误报告 77 条。

## 重新生成

需要 Python 3 与 openpyxl，在本目录下执行：

```sh
pip install openpyxl
python build_glossary.py
python build_html.py
```

脚本使用相对路径，输出写在当前目录。`build_html.py` 只取 `build_glossary.py` 中「构建工作簿」之前的数据段，不重复写 xlsx。

## 术语约定（节选）

| 英文 | 译名 | 备注 |
| --- | --- | --- |
| Ability | 特性 | 与 Innate 区分 |
| Innate | 天性 | 旧译「固有」，已统一为「天性」 |
| STAB | 本系加成 | |
| Burn | 灼伤 | 不写「烧伤」 |
| Frostbite | 冻伤 | ER 已移除冰冻状态 |
| Hail | 冰雹 | 不写「雪天」 |
| Harsh Sunlight | 大晴天 | |

## 错误报告的适用范围

`07-错误报告(名称≠效果)` 针对的是图鉴 xlsm 的表格错位：中文列整体旋转、同名段内部置换、中英排序不一致导致串行。它描述的是那份表格的问题，不是游戏本体的文本错误。

游戏内特性文本在 `game/include/generated/data/abilities/ability_text.hh`，以英文枚举为键，名称与效果对应正确（例如 `ABILITY_SPECTRALIZE` 为「幽灵魂化」、`ABILITY_DISCIPLINE` 为「自律」）。因此这 77 条不需要按表重映射到源码，仅作为图鉴校对记录保留。

`08-错误报告(其他问题)` 收录译名冲突与缺译条目，属于术语表维护范围。

## 来源

图鉴数据来自 `ER2.65beta版图鉴v0.3.xlsm`。该文件未随仓库分发，`out/*.csv` 已是提取后的结果，脚本不需要原表。