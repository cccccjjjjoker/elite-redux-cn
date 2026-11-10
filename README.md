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

输入和输出的 SHA-256 见 [校验记录](SHA256SUMS.txt)。使用候选 BPS 前，应先确认输入文件的 SHA-256 等于记录中的英文基线。应用后可核对输出哈希。候选补丁按英文 Elite Redux 构建制作。

## 工程维护

交接工程将译文库、注入脚本、源码工作树及字库构建分开管理。下一步会逐项核对可发布的译文、工具和源码改动，并记录原作者、贡献者与字体来源。源码复现以固定提交和配置子模块为基线。

## 致谢

感谢 Elite Redux 和 pokeemerald 项目的贡献者。

感谢 Elite Redux 汉化组完成源码汉化工程，也感谢贴吧网友 eeee5555 提供源码汉化的思路。

字库素材署名待核实后补齐。
