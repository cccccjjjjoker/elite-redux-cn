	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata

@ 中文字库：16x16 / 2bpp / 每格 64 字节，格序号 = glyphId（0 起）
@ 生成流程见 work/cn/font_build.py，勿手改
	.align 2
gFontCNNormalGlyphs::
	.incbin "graphics/fonts/font_cn_normal.cnfont"

	.align 2
gFontCNSmallGlyphs::
	.incbin "graphics/fonts/font_cn_small.cnfont"
