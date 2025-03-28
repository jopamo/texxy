#ifndef TEXXY_H
#define TEXXY_H

#include <QAction>
#include <QCheckBox>
#include <QCloseEvent>
#include <QDialog>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QMimeDatabase>
#include <QPushButton>
#include <QRegularExpression>
#include <QSettings>
#include <QStatusBar>
#include <QSyntaxHighlighter>
#include <QTextEdit>
#include <QTextStream>
#include <QToolBar>
#include <QVector>

class FindReplaceDialog : public QDialog {
    Q_OBJECT

   public:
    explicit FindReplaceDialog(QWidget* parent = nullptr);

    void setTextEdit(QTextEdit* edit);

   private slots:
    void onFindClicked();
    void onReplaceClicked();
    void onReplaceAllClicked();

   private:
    QTextEdit* textEdit = nullptr;
    QLineEdit* findLineEdit;
    QLineEdit* replaceLineEdit;
    QPushButton* findButton;
    QPushButton* replaceButton;
    QPushButton* replaceAllButton;
    QPushButton* closeButton;
    QCheckBox* matchCaseCheckBox;
};

class Texxy : public QMainWindow {
    Q_OBJECT

   public:
    explicit Texxy(QWidget* parent = nullptr);

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
    bool maybeSaveChanges();
    void loadFile(const QString& filePath);
    bool saveToPath(const QString& filePath);
    void addToRecentFiles(const QString& filePath);
    void updateRecentFilesMenu();
    void loadSettings();
    void saveSettings();

    QTextEdit* textEdit = nullptr;
    QLabel* statusLabel = nullptr;
    QSyntaxHighlighter* highlighter = nullptr;
    FindReplaceDialog* findReplaceDialog = nullptr;

    QString currentFilePath;
    QMenu* recentFilesMenu = nullptr;
    QStringList recentFiles;
    static const int MaxRecentFiles = 5;
};

#endif  // TEXXY_H
