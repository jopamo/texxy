#include "texxy.h"
#include "language_support.h"
#include "languages.cpp"  // or wherever your language definitions live
#include "findreplacedialog.h"

#include <QAction>
#include <QFileDialog>
#include <QMenuBar>
#include <QMenu>
#include <QMessageBox>
#include <QMimeDatabase>
#include <QStatusBar>
#include <QTextStream>
#include <QTextCursor>
#include <QTextDocument>
#include <QFileInfo>
#include <QApplication>

Texxy::Texxy(QWidget* parent) : QMainWindow(parent) {
    // Create the QTabWidget as the central widget
    tabWidget = new QTabWidget(this);
    setCentralWidget(tabWidget);

    // Create an initial empty tab
    createNewTab();

    // Create actions
    QAction* newAction = new QAction(tr("&New"), this);
    QAction* openAction = new QAction(tr("&Open..."), this);
    QAction* saveAction = new QAction(tr("&Save"), this);
    QAction* saveAsAction = new QAction(tr("Save &As..."), this);
    QAction* exitAction = new QAction(tr("E&xit"), this);
    QAction* findReplaceAction = new QAction(tr("Find/Replace..."), this);

    // Optional shortcuts
    newAction->setShortcut(QKeySequence::New);
    openAction->setShortcut(QKeySequence::Open);
    saveAction->setShortcut(QKeySequence::Save);
    saveAsAction->setShortcut(QKeySequence::SaveAs);

    // Connect actions
    connect(newAction, &QAction::triggered, this, &Texxy::newFile);
    connect(openAction, &QAction::triggered, this, &Texxy::openFile);
    connect(saveAction, &QAction::triggered, this, &Texxy::saveFile);
    connect(saveAsAction, &QAction::triggered, this, &Texxy::saveFileAs);
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);

    connect(findReplaceAction, &QAction::triggered, this, &Texxy::showFindReplace);

    // File menu
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);

    // Recent files submenu
    recentFilesMenu = fileMenu->addMenu(tr("Open Recent"));
    updateRecentFilesMenu();

    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    // Edit menu
    QMenu* editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(findReplaceAction);

    // Status bar
    statusLabel = new QLabel(this);
    statusBar()->addPermanentWidget(statusLabel);

    // Create Find/Replace dialog
    findReplaceDialog = new FindReplaceDialog(this);
    // Assign the current tab’s textEdit
    findReplaceDialog->setTextEdit(currentTextEdit());

    // When current tab changes, re-assign the find dialog’s textEdit
    connect(tabWidget, &QTabWidget::currentChanged, this, [this](int) {
        findReplaceDialog->setTextEdit(currentTextEdit());
        updateCursorPosition();
        updateWindowTitle();
    });

    // Set window title and size
    setWindowTitle(tr("Untitled - Texxy"));
    resize(900, 600);

    // Load settings (recent files, etc.)
    loadSettings();
    updateCursorPosition();
}

void Texxy::closeEvent(QCloseEvent* event) {
    // Check unsaved changes on the *current* tab (you could extend to all tabs)
    if (maybeSaveChanges()) {
        saveSettings();
        event->accept();
    }
    else {
        event->ignore();
    }
}

// Slots
void Texxy::newFile() {
    if (!maybeSaveChanges())
        return;
    createNewTab();  // new, untitled tab
}

void Texxy::openFile() {
    if (!maybeSaveChanges())
        return;

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"));
    if (!fileName.isEmpty()) {
        int newTabIndex = createNewTab(fileName);
        tabWidget->setCurrentIndex(newTabIndex);  // Switch to it
        loadFile(fileName);
        addToRecentFiles(fileName);
    }
}

bool Texxy::saveFile() {
    QString path = currentFilePath();
    if (path.isEmpty()) {
        return saveFileAs();  // user might pick a new path
    }
    return saveToPath(path);
}

bool Texxy::saveFileAs() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File As"));
    if (fileName.isEmpty())
        return false;
    return saveToPath(fileName);
}

void Texxy::openRecentFile() {
    QAction* action = qobject_cast<QAction*>(sender());
    if (!action)
        return;

    QString fileName = action->data().toString();
    if (!maybeSaveChanges())
        return;

    int newTabIndex = createNewTab(fileName);
    tabWidget->setCurrentIndex(newTabIndex);
    loadFile(fileName);
    addToRecentFiles(fileName);
}

void Texxy::showFindReplace() {
    findReplaceDialog->show();
    findReplaceDialog->raise();
    findReplaceDialog->activateWindow();
}

void Texxy::updateCursorPosition() {
    QTextEdit* edit = currentTextEdit();
    if (!edit) {
        statusLabel->setText(tr("Line: -, Col: -"));
        return;
    }
    QTextCursor cursor = edit->textCursor();
    int line = cursor.blockNumber() + 1;
    int col = cursor.columnNumber() + 1;
    statusLabel->setText(tr("Line: %1, Col: %2").arg(line).arg(col));
}

void Texxy::updateWindowTitle() {
    QString path = currentFilePath();
    QString title = path.isEmpty() ? tr("Untitled") : QFileInfo(path).fileName();

    QTextEdit* edit = currentTextEdit();
    if (edit && edit->document()->isModified()) {
        title += "*";
    }
    setWindowTitle(title + tr(" - Texxy"));
}

// Private Methods
int Texxy::createNewTab(const QString& filePath, const QString& content) {
    // Create our EditorWidget
    EditorWidget* editorWidget = new EditorWidget(this);
    editorWidget->setFilePath(filePath);

    // Optional styling
    editorWidget->textEdit->setStyleSheet("QTextEdit { background-color: #000; color: #FFF; }");

    if (!content.isEmpty()) {
        editorWidget->textEdit->setPlainText(content);
    }

    // Connect signals for modification & cursor movement
    connect(editorWidget->textEdit->document(), &QTextDocument::modificationChanged, this, &Texxy::updateWindowTitle);
    connect(editorWidget->textEdit, &QTextEdit::cursorPositionChanged, this, &Texxy::updateCursorPosition);

    // Tab label
    QString tabLabel = filePath.isEmpty() ? tr("Untitled") : QFileInfo(filePath).fileName();

    int idx = tabWidget->addTab(editorWidget, tabLabel);
    tabWidget->setCurrentIndex(idx);

    return idx;
}

// Helper to retrieve the current EditorWidget
EditorWidget* Texxy::currentEditorWidget() const {
    return qobject_cast<EditorWidget*>(tabWidget->currentWidget());
}

QTextEdit* Texxy::currentTextEdit() const {
    EditorWidget* ew = currentEditorWidget();
    return ew ? ew->textEdit : nullptr;
}

QString Texxy::currentFilePath() const {
    EditorWidget* ew = currentEditorWidget();
    return ew ? ew->filePath() : QString();
}

void Texxy::setCurrentFilePath(const QString& path) {
    EditorWidget* ew = currentEditorWidget();
    if (!ew)
        return;

    ew->setFilePath(path);

    // Update the tab text to match the file name
    int idx = tabWidget->indexOf(ew);
    if (idx >= 0) {
        QString name = path.isEmpty() ? tr("Untitled") : QFileInfo(path).fileName();
        tabWidget->setTabText(idx, name);
    }
}

bool Texxy::maybeSaveChanges() {
    QTextEdit* edit = currentTextEdit();
    if (!edit)
        return true;

    if (!edit->document()->isModified())
        return true;

    auto ret = QMessageBox::warning(this, tr("Unsaved Changes"),
                                    tr("The document has been modified.\nDo you want to save your changes?"),
                                    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (ret == QMessageBox::Save) {
        return saveFile();
    }
    else if (ret == QMessageBox::Cancel) {
        return false;
    }
    // Otherwise, Discard
    return true;
}

void Texxy::loadFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), tr("Cannot open file: %1").arg(filePath));
        return;
    }
    QTextStream in(&file);

    QString content = in.readAll();
    file.close();

    QTextEdit* edit = currentTextEdit();
    if (!edit)
        return;

    edit->setPlainText(content);
    edit->document()->setModified(false);

    // Update file path in the current EditorWidget
    setCurrentFilePath(filePath);
    updateWindowTitle();

    // Re-create the syntax highlighter for the new document
    if (highlighter) {
        delete highlighter;
        highlighter = nullptr;
    }

    // MIME detection (example)
    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForFile(filePath, QMimeDatabase::MatchContent);

    // Language detection from your custom "language_support.h" / "languages.cpp"
    const LanguageDefinition* lang = findMatchingLanguage(mime, filePath.toLower());
    if (lang) {
        highlighter = lang->highlighterFactory(edit->document());
    }
}

bool Texxy::saveToPath(const QString& filePath) {
    QTextEdit* edit = currentTextEdit();
    if (!edit)
        return false;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), tr("Cannot save file: %1").arg(filePath));
        return false;
    }
    QTextStream out(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    out.setCodec("UTF-8");
#endif
    out << edit->toPlainText();
    file.close();

    // Mark as not modified
    edit->document()->setModified(false);
    setCurrentFilePath(filePath);
    updateWindowTitle();
    return true;
}

// Recent Files

void Texxy::addToRecentFiles(const QString& filePath) {
    recentFiles.removeAll(filePath);
    recentFiles.prepend(filePath);
    while (recentFiles.size() > MaxRecentFiles) {
        recentFiles.removeLast();
    }
    updateRecentFilesMenu();
}

void Texxy::updateRecentFilesMenu() {
    recentFilesMenu->clear();
    for (const QString& f : recentFiles) {
        QAction* act = new QAction(QFileInfo(f).fileName(), this);
        act->setData(f);
        connect(act, &QAction::triggered, this, &Texxy::openRecentFile);
        recentFilesMenu->addAction(act);
    }
    recentFilesMenu->setEnabled(!recentFiles.isEmpty());
}

// Settings
void Texxy::loadSettings() {
    QSettings settings("MyCompany", "Texxy");
    recentFiles = settings.value("recentFiles").toStringList();
    updateRecentFilesMenu();
}

void Texxy::saveSettings() {
    QSettings settings("MyCompany", "Texxy");
    settings.setValue("recentFiles", recentFiles);
}

// main()
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    Texxy editor;
    editor.show();
    return app.exec();
}
