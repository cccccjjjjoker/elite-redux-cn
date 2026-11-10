# Pokémon Elite Redux 中文化

面向 Pokémon Elite Redux 的社区中文化项目。当前候选构建标记为 `2.65.5.2b`，中文显示、文本注入和字库由源码工程构建。

本项目追踪[上游 Elite Redux 源码](https://github.com/Elite-Redux/eliteredux-source)，当前基线为 [`bd7acbdd355f3ee6fa0ac862add1f51933305b52`](https://github.com/Elite-Redux/eliteredux-source/commit/bd7acbdd355f3ee6fa0ac862add1f51933305b52)。上游项目基于 [pret/pokeemerald](https://github.com/pret/pokeemerald)。

## 候选版本验证

| 项目 | 当前记录 |
| --- | --- |
| 候选版本 | `2.65.5.2b`，取自交接产物的文件名 |
| BPS 输入 | 本次交接工程中的英文基线 `ER_upstream_bd7acbdd.gba` |
| BPS 验证 | 对上列基线应用补丁，产物与交接的汉化 ROM 逐字节一致 |
| 公开版基线 | 待核对取得方式和 SHA-256 |
| 模拟器验收 | 本次整理待验证 |

## 下载和应用汉化补丁

- [下载 2.65.5.2b 中文 BPS 候选版](patches/ER-2.65.5.2b-CN_from-bd7acbdd.preview.bps)
- 玩家自备**未汉化的英文 Elite Redux ROM**，输入文件应与 `bd7acbdd` 英文基线的 SHA-256 一致。
- 如果手上是原版英文《宝可梦 绿宝石》，先用对应的 Elite Redux 英文补丁得到匹配的英文 ER，再应用这份中文 BPS。

英文 ER 输入 SHA-256

```text
5640ebc3691cb56a57dbbc3144b02d2eef8b4c92fa1da79cb5523101d08d2c57
```

补丁与汉化结果的 SHA-256 见 [校验记录](SHA256SUMS.txt)。应用后可以核对输出哈希。

## 工程维护

交接工程将译文库、注入脚本、源码工作树及字库构建分开管理。下一步会逐项核对可发布的译文、工具和源码改动，并记录原作者、贡献者与字体来源。源码复现以固定提交和配置子模块为基线。

## 致谢

感谢 Elite Redux 和 pokeemerald 项目的贡献者。

感谢 Elite Redux 汉化组完成源码汉化工程，也感谢贴吧网友 eeee5555 提供源码汉化的思路。

字库素材署名待核实后补齐。
