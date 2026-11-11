# Pokémon Elite Redux 中文汉化版

Pokémon Elite Redux 的社区汉化项目。当前版本为 `2.65.5.2b`，使用匹配的英文 ER ROM 和中文 BPS 补丁即可生成汉化版。

本项目追踪[上游 Elite Redux 源码](https://github.com/Elite-Redux/eliteredux-source)，当前基线为 [`bd7acbdd`](https://github.com/Elite-Redux/eliteredux-source/commit/bd7acbdd355f3ee6fa0ac862add1f51933305b52)。上游基于 [pret/pokeemerald](https://github.com/pret/pokeemerald) 开发。

## 下载与使用指南

**快速下载** [2.65.5.2b 中文 BPS 预览版](patches/ER-2.65.5.2b-CN_from-ER-515089fc.preview.bps)

> **打补丁顺序**　美版原版《宝可梦 绿宝石》 → 英文 Elite Redux → 中文 Elite Redux

1. **准备英文 ER ROM。** 已有英文 ER 2.65.5.2b 的玩家可以直接核对哈希。如果手上只有美版原版《宝可梦 绿宝石》，先用 [Elite Redux 官方补丁器](https://elite-redux.com/) 或相应的英文补丁生成英文 ER。目前尚未确认哪一份官方英文补丁能生成下方 SHA-256 对应的文件。
2. **核对英文 ROM。** 在 PowerShell 运行 `Get-FileHash -Algorithm SHA256 -LiteralPath '你的英文 ER.gba'`，将结果与下方的 SHA-256 对照。
3. **应用中文补丁。** 用 Floating IPS 等支持 BPS 的工具，把校验一致的英文 ER 选作输入 ROM，应用中文 BPS，另存为汉化版 `.gba`。
4. **核对输出。** 对照 [SHA256SUMS.txt](SHA256SUMS.txt) 中的「汉化结果」，确认一致后用 GBA 模拟器打开。

### 校验参考

英文 ER 输入 SHA-256

```text
515089fc2587f4de5d52e0ea1263b4c0bd61cd94d2bacf09477dc30745777d48
```

文件名和版本号相同，SHA-256 也可能不同。BPS 工具通常会检查输入大小和 CRC32，不符时会拒绝应用。强行应用错版可能导致黑屏、乱码或流程出错。遇到校验不一致，请换成符合上方 SHA-256 的英文 ER。

已实测用上述英文 ROM 打补丁，生成的中文版与从源码编译的结果逐字节一致。当前为预览版。已调整特性页色条、中文行距和文字留白，并补翻部分开场与界面文字；游戏内仍有漏翻，完整流程与本轮间距效果仍待测试。

## 源码构建（开发者可选）

源码编译适用于开发者和维护者。Windows 下的构建方案使用 Docker、WSL 和相关编译工具链。

已在 Windows 的 Docker Linux 容器中从干净源码包执行 `make modern`，生成的 32 MiB 中文 ROM 与 [校验记录](SHA256SUMS.txt) 中的「汉化结果」一致。

- **主源码基线** `bd7acbdd355f3ee6fa0ac862add1f51933305b52`
- **`proto` 子模块基线** `050dc341f10a281c48c225d69e5662d559f315e8`

主源码提交用于定位汉化工程的构建基线。玩家打补丁时请以本页列出的英文 ROM SHA-256 为准。

汉化源码位于 [game/](game/)，包含汉化配置、中文字库及生成输入。克隆仓库后，在根目录运行：

```powershell
.\build.ps1 -Jobs 8
```

需要 Git、Docker Desktop 和 WSL 2。修改源码后先提交到本地 Git，再执行构建；脚本会把当前提交的 game/ 归档到 Linux 容器，保留文件大小写和执行权限。构建所需的一项上游二进制资源会从固定提交下载，并校验 SHA-256。

首次构建需要联网下载工具链，结果和日志保存在 build-results/。
## 致谢

感谢 Elite Redux 和 pokeemerald 项目的贡献者。

感谢 Elite Redux 汉化组完成源码汉化工程，也感谢贴吧网友 eeee5555 提供源码汉化的思路。

字库素材署名待核实后补齐。
