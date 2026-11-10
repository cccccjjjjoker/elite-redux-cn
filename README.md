# Pokémon Elite Redux 中文化

面向 Pokémon Elite Redux 的社区中文化项目。当前候选构建标记为 `2.65.5.2b`，玩家入口是中文 BPS 补丁，使用步骤见下文。

本项目追踪[上游 Elite Redux 源码](https://github.com/Elite-Redux/eliteredux-source)，当前基线为 [`bd7acbdd355f3ee6fa0ac862add1f51933305b52`](https://github.com/Elite-Redux/eliteredux-source/commit/bd7acbdd355f3ee6fa0ac862add1f51933305b52)。上游项目基于 [pret/pokeemerald](https://github.com/pret/pokeemerald)。

## 下载和应用汉化补丁

[下载 2.65.5.2b 中文 BPS](patches/ER-2.65.5.2b-CN_from-ER-515089fc.preview.bps)。这份补丁用已经打好补丁的**英文 Elite Redux 2.65.5.2b** 作输入。

1. **准备匹配的英文 ER。** 已经有英文 ER 的玩家可直接校验。如果手里只有美版原版《宝可梦 绿宝石》，先应用对应的英文 ER 补丁，得到下列哈希对应的英文 ER。当前尚未验证哪份官方英文补丁能生成这份输入。
2. **核对英文 ER 的 SHA-256。** 打开 PowerShell，运行 `Get-FileHash -Algorithm SHA256 -LiteralPath '你的英文 ER.gba'`，对照下方的值。十六进制字母大小写均可。
3. **打中文 BPS。** 在支持 BPS 的工具里，把校验一致的英文 ER 选作 ROM，把上面的 `.bps` 选作补丁，另存为中文 `.gba`。
4. **核对输出。** 对照 [SHA-256 校验记录](SHA256SUMS.txt) 的「汉化结果」。确认一致后用 GBA 模拟器打开。

英文 ER 输入 SHA-256

```text
515089fc2587f4de5d52e0ea1263b4c0bd61cd94d2bacf09477dc30745777d48
```

### 哈希不一致时怎么办

- **输入 ROM 不一致。** 即便文件名和版本号相同，文件内容也有差别。BPS 工具通常会检查输入大小和 CRC32，两项中有一项不符时会提示源文件不匹配。换成符合上方 SHA-256 的英文 ER 后再试。
- **强行应用错版。** 输出可能无法通过目标校验，也可能出现黑屏、闪退、乱码或后续流程出错。遇到源文件不匹配时，应更换输入文件。
- **补丁或输出不一致。** 补丁的 SHA-256 不一致时，重新下载补丁。输出的 SHA-256 不一致时，重新核对输入、补丁及所选的输出文件。相同输入应用同一份 BPS，应得到相同输出。

## 候选版本验证

本仓库的英文 ER 版 BPS 已用输入 SHA-256 以 `515089fc` 开头的文件验证。打完与交接的汉化 ROM 逐字节一致。当前验收为补丁往返校验，模拟器游玩尚待测试。

## 源码构建（可选）

源码编译是给开发者和维护者的可选路径。Docker、WSL 和编译工具链只用于这条路径。

源码发行正在整理。主源码基线是 `bd7acbdd355f3ee6fa0ac862add1f51933305b52`，配置子模块 `proto` 的上游基线是 `050dc341f10a281c48c225d69e5662d559f315e8`。整理时需要保留汉化后的配置文本、中文映射表、字库挂载文件及可随源码分发的字库。完成干净环境构建后，将补充可复现的命令和验收结果。

## 致谢

感谢 Elite Redux 和 pokeemerald 项目的贡献者。

感谢 Elite Redux 汉化组完成源码汉化工程，也感谢贴吧网友 eeee5555 提供源码汉化的思路。

字库素材署名待核实后补齐。
