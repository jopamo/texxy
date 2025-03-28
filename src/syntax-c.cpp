#include "syntax-c.h"
#include <QColor>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QVector>
#include <QTextDocument>

CxxSyntaxHighlighter::CxxSyntaxHighlighter(QTextDocument* parent) : QSyntaxHighlighter(parent) {
    initializeKeywords();
    initializeTypes();
    initializePreprocessorDirectives();
    initializeComments();
    initializeStrings();
    initializeNumericLiterals();
    initializeOperators();
}

void CxxSyntaxHighlighter::initializeKeywords() {
    QStringList keywordPatterns = {
        "\\bbreak\\b",    "\\bcase\\b",      "\\bcontinue\\b",  "\\bdefault\\b",   "\\bdo\\b",          "\\belse\\b",
        "\\bfor\\b",      "\\bgoto\\b",      "\\bif\\b",        "\\breturn\\b",    "\\bswitch\\b",      "\\bwhile\\b",
        "\\basm\\b",      "\\bdecltype\\b",  "\\bdelete\\b",    "\\bfriend\\b",    "\\bnamespace\\b",   "\\bnew\\b",
        "\\boperator\\b", "\\bthis\\b",      "\\bthrow\\b",     "\\btry\\b",       "\\bcatch\\b",       "\\busing\\b",
        "\\bco_await\\b", "\\bco_return\\b", "\\bco_yield\\b",  "\\bconcept\\b",   "\\brequires\\b",    "\\bmodule\\b",
        "\\bimport\\b",   "\\bexport\\b",    "\\bconsteval\\b", "\\bconstinit\\b", "\\boperator\"\"\\b"};

    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(QColor("#C586C0"));
    keywordFormat.setFontWeight(QFont::Bold);

    for (const QString& pattern : keywordPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }
}

void CxxSyntaxHighlighter::initializeTypes() {
    QStringList typePatterns = {
        "\\bbool\\b",         "\\bchar\\b",      "\\bwchar_t\\b",   "\\bshort\\b",     "\\blong\\b",
        "\\bsigned\\b",       "\\bunsigned\\b",  "\\bfloat\\b",     "\\bdouble\\b",    "\\bvoid\\b",
        "\\bint8_t\\b",       "\\buint8_t\\b",   "\\bint16_t\\b",   "\\buint16_t\\b",  "\\bint32_t\\b",
        "\\buint32_t\\b",     "\\bint64_t\\b",   "\\buint64_t\\b",  "\\bsize_t\\b",    "\\bptrdiff_t\\b",
        "\\bintptr_t\\b",     "\\buintptr_t\\b", "\\bchar16_t\\b",  "\\bchar32_t\\b",  "\\bchar8_t\\b",
        "\\bstruct\\b",       "\\bunion\\b",     "\\benum\\b",      "\\bclass\\b",     "\\btypename\\b",
        "\\btemplate\\b",     "\\bauto\\b",      "\\bconstexpr\\b", "\\bconstinit\\b", "\\bconsteval\\b",
        "\\bthread_local\\b", "\\bmutable\\b",   "\\binline\\b",    "\\bvirtual\\b",   "\\bexplicit\\b",
        "\\bstatic\\b",       "\\bregister\\b",  "\\bextern\\b",    "\\bvolatile\\b",  "\\brestrict\\b",
        "\\btypename\\b",     "\\bNULL\\b",      "\\bnullptr\\b",   "\\bnullptr_t\\b", "\\bTRUE\\b",
        "\\bFALSE\\b",        "\\btrue\\b",      "\\bfalse\\b",     "\\bstd::\\w+\\b", "\\[\\[\\w+\\]\\]"};

    QTextCharFormat typeFormat;
    typeFormat.setForeground(QColor("#4FC1FF"));

    for (const QString& pattern : typePatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = typeFormat;
        highlightingRules.append(rule);
    }
}

void CxxSyntaxHighlighter::initializePreprocessorDirectives() {
    HighlightingRule rule;
    QTextCharFormat preprocessorFormat;
    preprocessorFormat.setForeground(QColor("#569CD6"));
    preprocessorFormat.setFontWeight(QFont::Bold);

    rule.pattern = QRegularExpression(
        "^[ \t]*(#|%:)\\s*(?:if\\b|ifdef\\b|ifndef\\b|endif\\b|include\\b|"
        "define\\b|undef\\b|elif\\b|else\\b|pragma\\b|error\\b|warning\\b)");
    rule.format = preprocessorFormat;
    highlightingRules.append(rule);
}

void CxxSyntaxHighlighter::initializeComments() {
    {
        HighlightingRule rule;
        QTextCharFormat singleLineCommentFormat;
        singleLineCommentFormat.setForeground(QColor("#6A9955"));
        rule.pattern = QRegularExpression("//[^\n]*");
        rule.format = singleLineCommentFormat;
        highlightingRules.append(rule);
    }

    {
        HighlightingRule rule;
        QTextCharFormat doxygenCommentFormat;
        doxygenCommentFormat.setForeground(QColor("#B5CEA8"));
        doxygenCommentFormat.setFontWeight(QFont::Bold);
        rule.pattern = QRegularExpression("///[^\n]*|/\\*\\*[^\n]*");
        rule.format = doxygenCommentFormat;
        highlightingRules.append(rule);
    }

    {
        HighlightingRule rule;
        QTextCharFormat todoFormat;
        todoFormat.setForeground(QColor("#FF9C00"));
        todoFormat.setFontWeight(QFont::Bold);
        rule.pattern = QRegularExpression("\\b(TODO|FIXME)\\b");
        rule.format = todoFormat;
        highlightingRules.append(rule);
    }

    multiLineCommentFormat.setForeground(QColor("#6A9955"));
}

void CxxSyntaxHighlighter::initializeStrings() {
    {
        HighlightingRule rule;
        QTextCharFormat stringFormat;
        stringFormat.setForeground(QColor("#CE9178"));
        rule.pattern = QRegularExpression("\"([^\"\\\\]|\\\\.)*\"");
        rule.format = stringFormat;
        highlightingRules.append(rule);
    }

    {
        HighlightingRule rule;
        QTextCharFormat charFormat;
        charFormat.setForeground(QColor("#CE9178"));
        rule.pattern = QRegularExpression("'([^'\\\\]|\\\\.)*'");
        rule.format = charFormat;
        highlightingRules.append(rule);
    }

    // Fixed raw string pattern
    {
        HighlightingRule rule;
        QTextCharFormat rawStringFormat;
        rawStringFormat.setForeground(QColor("#CE9178"));
        // This pattern is a simplified approach that recognizes C++ raw strings:
        //   R"(...)" or LR"(...)", u8R"(...)", etc. (Optional prefix + R"( ... )")
        // It's still not 100% perfect for all delimiter sequences but good enough for typical usage.
        rule.pattern = QRegularExpression(R"(\b(?:[uU]|u8|L)?R"([^()\\"]*)\([^)]*\)\1"\b)");
        rule.format = rawStringFormat;
        highlightingRules.append(rule);
    }
}

void CxxSyntaxHighlighter::initializeNumericLiterals() {
    HighlightingRule rule;
    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor("#4EC9B0"));

    QString numericPattern = R"REGEX(
        \b(
          0[xX][0-9A-Fa-f']+     |
          0[bB][01']+           |
          \d[\d']*(\.[\d']+)?([eE][+-]?\d+)?[fFlL]?
        )\b
    )REGEX";

    rule.pattern = QRegularExpression(numericPattern, QRegularExpression::ExtendedPatternSyntaxOption);
    rule.format = numberFormat;
    highlightingRules.append(rule);
}

void CxxSyntaxHighlighter::initializeOperators() {
    HighlightingRule rule;
    QTextCharFormat operatorFormat;
    operatorFormat.setForeground(QColor("#D7BA7D"));

    rule.pattern = QRegularExpression("[+\\-*/%&|^!=<>]=?|\\|\\||&&|<<=?|>>=?|\\+\\+|--|\\?\\:|~|->|=>");
    rule.format = operatorFormat;
    highlightingRules.append(rule);
}

void CxxSyntaxHighlighter::highlightBlock(const QString& text) {
    // First, we set the block state to 'not in a comment' by default.
    setCurrentBlockState(0);

    // 1) Handle multi-line comments (/* ... */) right away.
    int startIndex = (previousBlockState() == 1) ? 0 : text.indexOf("/*");
    while (startIndex >= 0) {
        int endIndex = text.indexOf("*/", startIndex);
        int commentLength = 0;

        if (endIndex == -1) {
            // No closing '*/' found — stay in comment state to next block
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        else {
            commentLength = endIndex - startIndex + 2;  // +2 for "*/"
        }

        setFormat(startIndex, commentLength, multiLineCommentFormat);

        if (endIndex == -1) {
            break;
        }
        else {
            startIndex = text.indexOf("/*", endIndex + 2);
        }
    }

    // 2) Now apply all single-pass regex rules (keywords, strings, single-line "//", etc.).
    for (const auto& rule : highlightingRules) {
        auto matchIt = rule.pattern.globalMatch(text);
        while (matchIt.hasNext()) {
            auto match = matchIt.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}

QSyntaxHighlighter* createCxxHighlighter(QTextDocument* document) {
    return new CxxSyntaxHighlighter(document);
}
