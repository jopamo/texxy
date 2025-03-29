#ifndef TEXXY_H
#define TEXXY_H

#include <QMainWindow>
#include <QProcess>
#include <QTabWidget>
#include <QPlainTextEdit>
#include <QSettings>

#include "editorwidget.h"
#include "findreplacedialog.h"
#include "language_support.h"

class EditorWidget;  // forward declaration
class QLabel;
class QMenu;
class FindReplaceDialog;
class QCloseEvent;
class QMimeType;

class Texxy : public QMainWindow {
    Q_OBJECT
   public:
    explicit Texxy(QWidget* parent = nullptr);  // Initializes the main window and UI components.
    ~Texxy() override = default;                // Default destructor.

   protected:
    void closeEvent(QCloseEvent* event) override;  // Handles window close event with unsaved changes check.

   private slots:
    void newFile();               // Creates a new file tab.
    void openFile();              // Opens a file dialog to open an existing file.
    bool saveFile();              // Saves the current file, or prompts to "Save As".
    bool saveFileAs();            // Prompts the user to select a file path to save the document.
    void openRecentFile();        // Opens a recent file from the recent files menu.
    void showFindReplace();       // Opens the Find/Replace dialog.
    void updateCursorPosition();  // Updates the cursor position in the status bar.
    void updateWindowTitle();     // Updates window title with the current file name.

   private:
    int createNewTab(const QString& filePath = QString(), const QString& content = QString());  // Creates and returns a new tab with content.

    // Returns the current editor widget (EditorWidget).
    EditorWidget* currentEditorWidget() const;

    // Returns the current text editor (QPlainTextEdit) widget.
    QPlainTextEdit* currentTextEdit() const;

    QString currentFilePath() const;               // Returns the file path of the current editor.
    void setCurrentFilePath(const QString& path);  // Sets the file path of the current editor and updates the tab.
    bool maybeSaveChanges();                       // Checks if changes were made and prompts to save if needed.

    void loadFile(const QString& filePath);    // Loads a file into the editor.
    bool saveToPath(const QString& filePath);  // Saves the document to the specified path.

    void addToRecentFiles(const QString& filePath);  // Adds the file to the recent files list.
    void updateRecentFilesMenu();                    // Updates the recent files menu with the latest list.

    void loadSettings();  // Loads editor settings (recent files, etc.).
    void saveSettings();  // Saves editor settings (recent files, etc.).

    void closeCurrentTab();

    void applyClangFormat();

   private:
    QTabWidget* tabWidget = nullptr;       // Tab widget to manage multiple editor tabs.
    QLabel* statusLabel = nullptr;         // Status label for displaying the cursor position.
    QMenu* recentFilesMenu = nullptr;      // Menu for managing recent files.
    QStringList recentFiles;               // List of recently opened files.
    static const int MaxRecentFiles = 10;  // Max number of recent files to track.

    FindReplaceDialog* findReplaceDialog = nullptr;  // Dialog for Find/Replace functionality.

    QSyntaxHighlighter* highlighter = nullptr;  // Syntax highlighter (if available).
};

#endif  // TEXXY_H
