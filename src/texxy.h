#ifndef TEXXY_H
#define TEXXY_H

#include <QMainWindow>
#include <QTabWidget>
#include <QLabel>
#include <QSettings>
#include <QCloseEvent>
#include <QSyntaxHighlighter>

#include "editorwidget.h"
#include "findreplacedialog.h"

class Texxy : public QMainWindow {
    Q_OBJECT
   public:
    explicit Texxy(QWidget* parent = nullptr);
    ~Texxy() override {}

   protected:
    void closeEvent(QCloseEvent* event) override;

   private slots:
    void newFile();
    void openFile();
    bool saveFile();
    bool saveFileAs();
    void openRecentFile();
    void showFindReplace();
    void updateCursorPosition();
    void updateWindowTitle();

   private:
    // Helper / utility methods
    int createNewTab(const QString& filePath = QString(), const QString& content = QString());
    EditorWidget* currentEditorWidget() const;
    QTextEdit* currentTextEdit() const;

    QString currentFilePath() const;
    void setCurrentFilePath(const QString& path);

    bool maybeSaveChanges();
    void loadFile(const QString& filePath);
    bool saveToPath(const QString& filePath);

    // Recent files
    void addToRecentFiles(const QString& filePath);
    void updateRecentFilesMenu();
    void loadSettings();
    void saveSettings();

   private:
    QTabWidget* tabWidget;
    FindReplaceDialog* findReplaceDialog;
    QLabel* statusLabel;

    // If you want one syntax highlighter per tab, you’d store them differently;
    // for simplicity, we reuse a single pointer each time we load a file:
    QSyntaxHighlighter* highlighter = nullptr;

    // “Open Recent” support
    QMenu* recentFilesMenu;
    QStringList recentFiles;
    static const int MaxRecentFiles = 5;
};

#endif  // TEXXY_H
