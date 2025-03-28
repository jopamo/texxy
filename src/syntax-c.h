#ifndef SYNTAX_C_H
#define SYNTAX_C_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QVector>
#include <QTextCharFormat>
#include <QTextDocument>

/**
 * @brief The CxxSyntaxHighlighter class
 *        Extended highlighter for C and C++ syntax.
 */
class CxxSyntaxHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

   public:
    explicit CxxSyntaxHighlighter(QTextDocument* parent = nullptr);

   protected:
    void highlightBlock(const QString& text) override;

   private:
    // Each pattern + format rule
    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    // We'll break out the setup into separate methods
    void initializeKeywords();
    void initializeTypes();
    void initializePreprocessorDirectives();
    void initializeComments();
    void initializeStrings();
    void initializeNumericLiterals();
    void initializeOperators();
    void initializeTodoFixme();

    QVector<HighlightingRule> highlightingRules;

    // For multi-line comment highlighting
    QTextCharFormat multiLineCommentFormat;

    // We might have a separate "doxygen comment" format, etc., if desired
    QTextCharFormat doxygenCommentFormat;
};

/**
 * @brief createCxxHighlighter
 *        Factory function returning a new instance of CxxSyntaxHighlighter.
 */
QSyntaxHighlighter* createCxxHighlighter(QTextDocument* document);

#endif  // SYNTAX_C_H
