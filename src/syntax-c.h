#ifndef SYNTAX_C_H
#define SYNTAX_C_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QVector>
#include <QTextCharFormat>
#include <QTextDocument>

/**
 * @brief The CxxSyntaxHighlighter class
 *        A custom syntax highlighter for C and C++ code. It supports highlighting
 *        various code elements like keywords, types, operators, comments, etc.
 */
class CxxSyntaxHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

   public:
    /**
     * @brief Constructor that initializes the syntax highlighter with the document.
     * @param parent The parent document for this highlighter.
     */
    explicit CxxSyntaxHighlighter(QTextDocument* parent = nullptr);

   protected:
    /**
     * @brief Overrides the highlightBlock method to apply syntax highlighting to the given block of text.
     * @param text The text content of the current block to be highlighted.
     */
    void highlightBlock(const QString& text) override;

   private:
    // Struct representing a highlighting rule with a regex pattern and corresponding format.
    struct HighlightingRule {
        QRegularExpression pattern;  ///< Regex pattern for matching tokens.
        QTextCharFormat format;      ///< Formatting to apply when the pattern is matched.
    };

    // Private methods to initialize different categories of syntax elements.
    void initializeKeywords();                ///< Initializes keyword patterns and formatting.
    void initializeTypes();                   ///< Initializes type patterns and formatting.
    void initializePreprocessorDirectives();  ///< Initializes preprocessor directive patterns and formatting.
    void initializeComments();                ///< Initializes comment patterns (single-line, multi-line, TODO/FIXME).
    void initializeStrings();                 ///< Initializes string literal patterns and formatting.
    void initializeNumericLiterals();         ///< Initializes numeric literal patterns and formatting.
    void initializeOperators();               ///< Initializes operator patterns and formatting.
    void initializeTodoFixme();               ///< Initializes TODO/FIXME patterns and formatting for inline comments.

    QVector<HighlightingRule> highlightingRules;  ///< Collection of all highlighting rules.

    QTextCharFormat multiLineCommentFormat;  ///< Format for multi-line comments (/* ... */).
};

/**
 * @brief createCxxHighlighter
 *        Factory function for creating a CxxSyntaxHighlighter instance.
 * @param document The document to which the highlighter will be applied.
 * @return A new instance of CxxSyntaxHighlighter.
 */
QSyntaxHighlighter* createCxxHighlighter(QTextDocument* document);

#endif  // SYNTAX_C_H
