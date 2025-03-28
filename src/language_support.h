#ifndef LANGUAGE_SUPPORT_H
#define LANGUAGE_SUPPORT_H

#include <QMimeType>
#include <QString>
#include <QStringList>
#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <functional>

/**
 * LanguageDefinition
 * ------------------
 * Holds data for each language we want to support:
 *  - A language name
 *  - A list of MIME types
 *  - A list of extensions
 *  - A factory function to create its QSyntaxHighlighter
 */
struct LanguageDefinition {
    QString languageName;
    QStringList mimeTypes;
    QStringList extensions;

    std::function<QSyntaxHighlighter*(QTextDocument*)> highlighterFactory;
};

#endif  // LANGUAGE_SUPPORT_H
