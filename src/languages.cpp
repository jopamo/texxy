#include "language_support.h"
#include "syntax-c.h"
// #include "syntax-python.h" // etc. if you have more

static const LanguageDefinition SUPPORTED_LANGUAGES[] = {
    {"C/C++", {"text/x-csrc", "text/x-c++src", "text/x-chdr"}, {".c", ".cpp", ".cxx", ".h", ".hpp"}, createCxxHighlighter},
    // Example for Python
    // {
    //     "Python",
    //     { "text/x-python" },
    //     { ".py" },
    //     createPythonHighlighter
    // },
    // etc...
};

const LanguageDefinition* findMatchingLanguage(const QMimeType& mime, const QString& lowerFilePath) {
    for (const auto& lang : SUPPORTED_LANGUAGES) {
        for (const QString& mt : lang.mimeTypes) {
            if (mime.inherits(mt)) {
                return &lang;
            }
        }
    }

    for (const auto& lang : SUPPORTED_LANGUAGES) {
        for (const QString& ext : lang.extensions) {
            if (lowerFilePath.endsWith(ext)) {
                return &lang;
            }
        }
    }
    return nullptr;  // No match
}
