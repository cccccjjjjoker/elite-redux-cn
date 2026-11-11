package er.text

import java.io.File

object FontMapping {
    enum class Font(val id: Int, val widths: FontWidths = FontWidths(id)) {
        SMALL(0),
        NORMAL(1),
        SHORT(2),
        SHORT_COPY_1(3),
        SHORT_COPY_2(4),
        SHORT_COPY_3(5),
        BRAILLE(6),
        NARROW(7),
        SMALL_NARROW(8),
        BOLD(9),
    }

    class FontWidths(val id: Int) {
        val entries by lazy {
            File("../../graphics/fonts/font${id}_latin_widths.inc").readLines()
                .flatMap { """\d+""".toRegex().findAll(it) }.map {
                    it.value.toInt()
                }
        }
    }

    // ------------------------------------------------------------------
    // 中文字形宽度
    //
    // 中文字模来自 16x16 / 2bpp / 64 字节每格的扩展字库（graphics/fonts/*.cnfont），
    // 字身并非铺满整格：normal 字库字身 12x12、small 字库字身 10x10，
    // 因此渲染时进度量必须取 12 / 10，而不是 16。
    //
    // 这两个常量必须与 gflib/text.c 里 CJK 分支实际推进的像素保持一致，
    // 否则 breakString 算出的换行点与渲染结果错位，表现为溢出或空洞。
    //
    // small 族（SMALL / NARROW / SMALL_NARROW）用小字库，其余用 normal 字库。
    // ------------------------------------------------------------------
    const val CJK_WIDTH_NORMAL = 12
    const val CJK_WIDTH_SMALL = 10

    fun cjkWidth(font: Font): Int = when (font) {
        Font.SMALL, Font.NARROW, Font.SMALL_NARROW -> CJK_WIDTH_SMALL
        else -> CJK_WIDTH_NORMAL
    }

    /**
     * 该字体一行最多能放几个汉字。供内容侧预估译文长度上限：
     * 可用字数 ≈ [cjkCapacity] 的结果，超过就必须改写而不是硬塞。
     */
    fun cjkCapacity(font: Font, maxLength: Int, maxLines: Int): Int =
        (maxLength / cjkWidth(font)) * maxLines

    /** 是否按全角宽度渲染的字符（CJK 汉字、假名、全角标点等）。 */
    fun Char.isWideChar(): Boolean = when (code) {
        in 0x1100..0x115F -> true   // 谚文字母
        in 0x2E80..0x303E -> true   // 部首扩展、CJK 符号与标点（。、。〈〉《》「」【】）
        in 0x3041..0x33FF -> true   // 假名、注音、CJK 兼容
        in 0x3400..0x4DBF -> true   // 扩展 A
        in 0x4E00..0x9FFF -> true   // 基本区
        in 0xA000..0xA4CF -> true   // 彝文
        in 0xF900..0xFAFF -> true   // 兼容汉字
        in 0xFE30..0xFE4F -> true   // 兼容形式
        in 0xFF00..0xFF60 -> true   // 全角 ASCII（，！？：；（）等）
        in 0xFFE0..0xFFE6 -> true   // 全角符号
        else -> false
    }

    /** 不允许出现在行首的字符（避头点）。 */
    private const val NO_LINE_START = "、。，．·；：！？‥…）］｝〉》」』】〕”’%‰℃,.;:!?)]}"

    /** 不允许出现在行尾的字符（避尾点，开括号类）。 */
    private const val NO_LINE_END = "（［｛〈《「『【〔“‘([{"

    private fun Char.width(font: Font, acceptNewline: Boolean = false): Int {
        if (acceptNewline && this == '\n') return 0
        val index = checkNotNull(CHAR_INDICES[this]) { "No index present for character $this" }
        return font.widths.entries[index]
    }

    fun breakString(string: String, font: Font, maxLength: Int, maxLines: Int): String =
        if (string.any { it.isWideChar() }) breakWideString(string, font, maxLength, maxLines)
        else breakLatinString(string, font, maxLength, maxLines)

    /**
     * 原英文断行逻辑，逐字保留以维持既有英文产物的换行位置不变。
     */
    private fun breakLatinString(string: String, font: Font, maxLength: Int, maxLines: Int) = buildString {
        fun String.pixelLength() =
            replace("{PKMN}", "").sumOf { it.width(font, true) } + ("\\{PKMN}".toRegex().findAll(this)
                .count() * (font.widths.entries[0x53] + font.widths.entries[0x54]))

        var curLength = 0
        string.split("""((?= )|(?<=-)|(?=\n))""".toRegex()).forEach { piece ->
            val pieceLength = piece.pixelLength()
                if (piece.startsWith("\n") ||
                    curLength + pieceLength > maxLength
                ) {
                    append("\\n")
                    curLength = piece.trim().pixelLength()
                    append(piece.trim())
                } else {
                    append(piece)
                    curLength += pieceLength
                }
        }
    }.also { result ->
        check(result.split("\\n").size <= maxLines) {
            "String $result exceeds max line length of $maxLines."
        }
    }

    /**
     * 中日韩文本断行。英文按空格断词，中文没有空格，必须逐字累积到超宽再折行，
     * 同时处理避头点/避尾点，否则会出现标点起行或开括号收尾。
     */
    private fun breakWideString(string: String, font: Font, maxLength: Int, maxLines: Int): String {
        val units = tokenizeWide(string)
        val cjkW = cjkWidth(font)
        val lines = mutableListOf<String>()
        var cur = mutableListOf<String>()
        var curW = 0

        fun pushLine() {
            if (cur.isEmpty()) {
                if (lines.isEmpty()) lines.add("")
                return
            }
            lines.add(cur.joinToString(""))
            cur = mutableListOf()
            curW = 0
        }

        fun dropLast(): String {
            val u = cur.removeAt(cur.size - 1)
            curW -= unitWidth(u, font)
            return u
        }

        for (u in units) {
            if (u == "\n") {
                pushLine()
                continue
            }
            val w = unitWidth(u, font)
            if (cur.isEmpty() || curW + w <= maxLength) {
                cur.add(u)
                curW += w
                continue
            }

            // 当前行已放不下 u，需要折行。折行前先做两个禁则修正。
            val carry = mutableListOf<String>()

            // 避头点：标点不能起行，把它前面一个单元一起带到下一行，保证它们同处一行。
            if (u.length == 1 && u[0] in NO_LINE_START && cur.size >= 2) {
                carry.add(0, dropLast())
            }
            // 避尾点：开括号不能收尾，带到下一行。
            while (cur.size >= 2 && cur.last().length == 1 && cur.last()[0] in NO_LINE_END) {
                carry.add(0, dropLast())
            }

            pushLine()
            for (c in carry) {
                cur.add(c)
                curW += unitWidth(c, font)
            }
            cur.add(u)
            curW += w
        }
        pushLine()

        // 报错信息用 ASCII 前缀 + 中文说明：构建容器的 locale 往往不是 UTF-8，
        // 纯中文消息在控制台会显示成 ??????，前面这段英文保证任何环境下都能定位问题。
        check(lines.size <= maxLines) {
            "[CJK-OVERFLOW] exceeds $maxLines lines (limit ${maxLength}px/line, " +
                "${cjkW}px per CJK char, ~${maxLength / cjkW} chars/line, got ${lines.size} lines). " +
                "中文文本超出 ${maxLines} 行上限，请改写：${string.take(48)}" +
                if (string.length > 48) "…" else ""
        }
        // 单个不可分割单元（超长英文单词、URL 等）本身宽于整行时，上面的循环会直接放行，
        // 这里补一道兜底，避免生成超出文本框的行。
        lines.forEach { line ->
            val w = tokenizeWide(line).sumOf { unitWidth(it, font) }
            check(w <= maxLength) {
                "[CJK-OVERWIDTH] line ${w}px > ${maxLength}px. " +
                    "中文文本单行超宽（不可分割的长串）：$line"
            }
        }

        return lines.joinToString("\\n")
    }

    /**
     * 把字符串切成不可再分的换行单元：
     * - CJK 字符、全角标点：单字成组（中文可在任意字间断行）
     * - ASCII 连续串：整个单词成组（英文不能从词中断开）
     * - `{XXX}`：整体成组，宽度单独处理
     * - 空格、换行：各自成组
     */
    private fun tokenizeWide(s: String): List<String> {
        val out = ArrayList<String>(s.length)
        var i = 0
        while (i < s.length) {
            val c = s[i]
            when {
                c == '\n' -> {
                    out.add("\n"); i++
                }
                c == ' ' -> {
                    out.add(" "); i++
                }
                c == '{' -> {
                    val j = s.indexOf('}', i)
                    if (j > i) {
                        out.add(s.substring(i, j + 1)); i = j + 1
                    } else {
                        out.add("{"); i++
                    }
                }
                c.isWideChar() -> {
                    out.add(c.toString()); i++
                }
                else -> {
                    val st = i
                    while (i < s.length && s[i] != '\n' && s[i] != ' ' && s[i] != '{' && !s[i].isWideChar()) i++
                    out.add(s.substring(st, i))
                }
            }
        }
        return out
    }

    private fun unitWidth(unit: String, font: Font): Int {
        if (unit == "\n") return 0
        // {PKMN} 在渲染时展开成两个字符，宽度按 P/K/M/N 计（与英文路径同一口径）。
        if (unit == "{PKMN}") return font.widths.entries[0x53] + font.widths.entries[0x54]
        // 其它 {XXX} 是文本控制码，不占像素。
        if (unit.startsWith("{")) return 0
        return unit.sumOf { ch ->
            if (ch.isWideChar()) cjkWidth(font)
            else CHAR_INDICES[ch]?.let { font.widths.entries.getOrElse(it) { AVERAGE_LATIN_WIDTH } }
                ?: AVERAGE_LATIN_WIDTH
        }
    }

    /** 字宽表里查不到的 ASCII 字符按平均字宽估算，避免 codegen 因个别符号中断。 */
    private const val AVERAGE_LATIN_WIDTH = 6

    private val CHAR_INDICES = mapOf(
        ' ' to 0x00,
        'À' to 0x01,
        'Á' to 0x02,
        'Â' to 0x03,
        'Ç' to 0x04,
        'È' to 0x05,
        'É' to 0x06,
        'Ê' to 0x07,
        'Ë' to 0x08,
        'Ì' to 0x09,
        'Î' to 0x0B,
        'Ï' to 0x0C,
        'Ò' to 0x0D,
        'Ó' to 0x0E,
        'Ô' to 0x0F,
        'Œ' to 0x10,
        'Ù' to 0x11,
        'Ú' to 0x12,
        'Û' to 0x13,
        'Ñ' to 0x14,
        'ß' to 0x15,
        'à' to 0x16,
        'á' to 0x17,
        'ç' to 0x19,
        'è' to 0x1A,
        'é' to 0x1B,
        'ê' to 0x1C,
        'ë' to 0x1D,
        'ì' to 0x1E,
        'î' to 0x20,
        'ï' to 0x21,
        'ò' to 0x22,
        'ó' to 0x23,
        'ô' to 0x24,
        'œ' to 0x25,
        'ù' to 0x26,
        'ú' to 0x27,
        'û' to 0x28,
        'ñ' to 0x29,
        'º' to 0x2A,
        'ª' to 0x2B,
        '&' to 0x2D,
        '+' to 0x2E,
        '=' to 0x35,
        ';' to 0x36,
        '¿' to 0x51,
        '¡' to 0x52,
        'Í' to 0x5A,
        '%' to 0x5B,
        '(' to 0x5C,
        ')' to 0x5D,
        'â' to 0x68,
        'í' to 0x6F,
        '<' to 0x85,
        '>' to 0x86,
        '0' to 0xA1,
        '1' to 0xA2,
        '2' to 0xA3,
        '3' to 0xA4,
        '4' to 0xA5,
        '5' to 0xA6,
        '6' to 0xA7,
        '7' to 0xA8,
        '8' to 0xA9,
        '9' to 0xAA,
        '!' to 0xAB,
        '?' to 0xAC,
        '.' to 0xAD,
        '-' to 0xAE,
        '·' to 0xAF,
        '…' to 0xB0,
        '“' to 0xB1,
        '”' to 0xB2,
        '‘' to 0xB3,
        '\'' to 0xB4,
        '♂' to 0xB5,
        '♀' to 0xB6,
        '¥' to 0xB7,
        ',' to 0xB8,
        '×' to 0xB9,
        '/' to 0xBA,
        'A' to 0xBB,
        'B' to 0xBC,
        'C' to 0xBD,
        'D' to 0xBE,
        'E' to 0xBF,
        'F' to 0xC0,
        'G' to 0xC1,
        'H' to 0xC2,
        'I' to 0xC3,
        'J' to 0xC4,
        'K' to 0xC5,
        'L' to 0xC6,
        'M' to 0xC7,
        'N' to 0xC8,
        'O' to 0xC9,
        'P' to 0xCA,
        'Q' to 0xCB,
        'R' to 0xCC,
        'S' to 0xCD,
        'T' to 0xCE,
        'U' to 0xCF,
        'V' to 0xD0,
        'W' to 0xD1,
        'X' to 0xD2,
        'Y' to 0xD3,
        'Z' to 0xD4,
        'a' to 0xD5,
        'b' to 0xD6,
        'c' to 0xD7,
        'd' to 0xD8,
        'e' to 0xD9,
        'f' to 0xDA,
        'g' to 0xDB,
        'h' to 0xDC,
        'i' to 0xDD,
        'j' to 0xDE,
        'k' to 0xDF,
        'l' to 0xE0,
        'm' to 0xE1,
        'n' to 0xE2,
        'o' to 0xE3,
        'p' to 0xE4,
        'q' to 0xE5,
        'r' to 0xE6,
        's' to 0xE7,
        't' to 0xE8,
        'u' to 0xE9,
        'v' to 0xEA,
        'w' to 0xEB,
        'x' to 0xEC,
        'y' to 0xED,
        'z' to 0xEE,
        '▶' to 0xEF,
        ':' to 0xF0,
        'Ä' to 0xF1,
        'Ö' to 0xF2,
        'Ü' to 0xF3,
        'ä' to 0xF4,
        'ö' to 0xF5,
        'ü' to 0xF6,
    )
}
