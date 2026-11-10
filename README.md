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

[下载 2.65.5.2b 中文 BPS 候选版](patches/ER-2.65.5.2b-CN_from-bd7acbdd.preview.bps)

**打补丁顺序**：美版原版绿宝石 → 英文 Elite Redux → 中文 Elite Redux。

1. **准备美版原版《宝可梦 绿宝石》ROM。** 在 [Elite Redux 官方补丁器](https://elite-redux.com/) 选中这份 `.gba`，应用与本汉化包对应的 **Elite Redux 英文补丁**，保存生成的英文 ER `.gba`。
2. **核对英文 ER 的 SHA-256。** 与下方的 `bd7acbdd` 英文基线哈希逐字比较。若不一致，先换成匹配的英文 ER 构建。
3. **应用中文 BPS。** 在支持 BPS 的补丁器里，把第 1 步生成、且校验一致的**英文 ER** 选作 ROM 输入，把 [中文 BPS](patches/ER-2.65.5.2b-CN_from-bd7acbdd.preview.bps) 选作补丁。点击应用，将结果另存为汉化版 `.gba`。
4. **检查汉化结果。** 对照 [校验记录](SHA256SUMS.txt) 中「汉化结果」的 SHA-256。核对后用 GBA 模拟器打开生成的汉化版。

英文 ER 输入 SHA-256

```text
5640ebc3691cb56a57dbbc3144b02d2eef8b4c92fa1da79cb5523101d08d2c57
```

补丁与汉化结果的 SHA-256 见 [校验记录](SHA256SUMS.txt)。应用后可以核对输出哈希。

## 工程维护

项目计划提供两种使用方式。玩家可下载 BPS 应用到匹配的英文 ER。需要修改或自行构建的维护者，可以使用应用过汉化改动的源码和构建说明生成中文 `.gba`。

源码发行正在整理。主源码基线是 `bd7acbdd355f3ee6fa0ac862add1f51933305b52`，配置子模块 `proto` 的上游基线是 `050dc341f10a281c48c225d69e5662d559f315e8`。整理时需要保留汉化后的配置文本、中文映射表、字库挂载文件及可随源码分发的字库。完成干净环境构建后，将补充可复现的命令和验收结果。

## 致谢

感谢 Elite Redux 和 pokeemerald 项目的贡献者。

感谢 Elite Redux 汉化组完成源码汉化工程，也感谢贴吧网友 eeee5555 提供源码汉化的思路。

字库素材署名待核实后补齐。
