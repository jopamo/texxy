#include "texxy.h"
#include "language_support.h"
#include "languages.cpp"
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
    tabWidget = new QTabWidget(this);
    setCentralWidget(tabWidget);

    QIcon appIcon("/usr/share/icons/hicolor/256x256/apps/texxy.png");
    setWindowIcon(appIcon);

    createNewTab();

    QAction* newAction = new QAction(tr("&New"), this);
    QAction* openAction = new QAction(tr("&Open..."), this);
    QAction* saveAction = new QAction(tr("&Save"), this);
    QAction* saveAsAction = new QAction(tr("Save &As..."), this);
    QAction* exitAction = new QAction(tr("E&xit"), this);
    QAction* findReplaceAction = new QAction(tr("Find/Replace..."), this);
    QAction* closeTabAction = new QAction(tr("Close Tab"), this);

    newAction->setShortcut(QKeySequence::New);
    openAction->setShortcut(QKeySequence::Open);
    saveAction->setShortcut(QKeySequence::Save);
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    closeTabAction->setShortcut(QKeySequence("Ctrl+W"));

    connect(newAction, &QAction::triggered, this, &Texxy::newFile);
    connect(openAction, &QAction::triggered, this, &Texxy::openFile);
    connect(saveAction, &QAction::triggered, this, &Texxy::saveFile);
    connect(saveAsAction, &QAction::triggered, this, &Texxy::saveFileAs);
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
    connect(findReplaceAction, &QAction::triggered, this, &Texxy::showFindReplace);
    connect(closeTabAction, &QAction::triggered, this, &Texxy::closeCurrentTab);

    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addAction(closeTabAction);

    recentFilesMenu = fileMenu->addMenu(tr("Open Recent"));
    updateRecentFilesMenu();
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    QMenu* editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(findReplaceAction);

    statusLabel = new QLabel(this);
    statusBar()->addPermanentWidget(statusLabel);

    findReplaceDialog = new FindReplaceDialog(this);
    findReplaceDialog->setTextEdit(currentTextEdit());

    connect(tabWidget, &QTabWidget::currentChanged, this, [this](int) {
        findReplaceDialog->setTextEdit(currentTextEdit());
        updateCursorPosition();
        updateWindowTitle();
    });

    setWindowTitle(tr("Untitled - texxy"));
    resize(900, 600);

    loadSettings();
    updateCursorPosition();
}

void Texxy::closeCurrentTab() {
    int currentIndex = tabWidget->currentIndex();
    if (currentIndex != -1) {
        QWidget* currentTab = tabWidget->widget(currentIndex);
        if (maybeSaveChanges()) {
            tabWidget->removeTab(currentIndex);
            delete currentTab;
        }
    }
}

void Texxy::closeEvent(QCloseEvent* event) {
    if (maybeSaveChanges()) {
        saveSettings();
        event->accept();
    }
    else {
        event->ignore();
    }
}

void Texxy::newFile() {
    if (!maybeSaveChanges())
        return;
    createNewTab();
}

void Texxy::openFile() {
    if (!maybeSaveChanges())
        return;

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"));
    if (!fileName.isEmpty()) {
        int newTabIndex = createNewTab(fileName);
        tabWidget->setCurrentIndex(newTabIndex);
        loadFile(fileName);
        addToRecentFiles(fileName);
    }
}

bool Texxy::saveFile() {
    QString path = currentFilePath();
    if (path.isEmpty()) {
        return saveFileAs();
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
    QPlainTextEdit* edit = currentTextEdit();
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

    QPlainTextEdit* edit = currentTextEdit();
    if (edit && edit->document()->isModified()) {
        title += "*";
    }
    setWindowTitle(title + tr(" - texxy"));
}

int Texxy::createNewTab(const QString& filePath, const QString& content) {
    EditorWidget* editorWidget = new EditorWidget(this);
    editorWidget->setFilePath(filePath);
    editorWidget->textEdit()->setStyleSheet("QPlainTextEdit { background-color: #000; color: #FFF; }");

    if (!content.isEmpty()) {
        editorWidget->textEdit()->setPlainText(content);
    }

    connect(editorWidget->textEdit()->document(), &QTextDocument::modificationChanged, this, &Texxy::updateWindowTitle);
    connect(editorWidget->textEdit(), &QPlainTextEdit::cursorPositionChanged, this, &Texxy::updateCursorPosition);

    QString tabLabel = filePath.isEmpty() ? tr("Untitled") : QFileInfo(filePath).fileName();

    int idx = tabWidget->addTab(editorWidget, tabLabel);
    tabWidget->setCurrentIndex(idx);

    return idx;
}

EditorWidget* Texxy::currentEditorWidget() const {
    return qobject_cast<EditorWidget*>(tabWidget->currentWidget());
}

QPlainTextEdit* Texxy::currentTextEdit() const {
    EditorWidget* ew = currentEditorWidget();
    return ew ? ew->textEdit() : nullptr;
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

    int idx = tabWidget->indexOf(ew);
    if (idx >= 0) {
        QString name = path.isEmpty() ? tr("Untitled") : QFileInfo(path).fileName();
        tabWidget->setTabText(idx, name);
    }
}

bool Texxy::maybeSaveChanges() {
    QPlainTextEdit* edit = currentTextEdit();
    if (!edit)
        return true;

    if (!edit->document()->isModified())
        return true;

    auto ret =
        QMessageBox::warning(this, tr("Unsaved Changes"), tr("The document has been modified.\nDo you want to save your changes?"), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (ret == QMessageBox::Save) {
        return saveFile();
    }
    else if (ret == QMessageBox::Cancel) {
        return false;
    }
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

    QPlainTextEdit* edit = currentTextEdit();
    if (!edit)
        return;

    edit->setPlainText(content);
    edit->document()->setModified(false);

    setCurrentFilePath(filePath);
    updateWindowTitle();

    if (highlighter) {
        delete highlighter;
        highlighter = nullptr;
    }

    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForFile(filePath, QMimeDatabase::MatchContent);

    const LanguageDefinition* lang = findMatchingLanguage(mime, filePath.toLower());
    if (lang) {
        highlighter = lang->highlighterFactory(edit->document());
    }
}

bool Texxy::saveToPath(const QString& filePath) {
    QPlainTextEdit* edit = currentTextEdit();
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

    edit->document()->setModified(false);
    setCurrentFilePath(filePath);
    updateWindowTitle();
    return true;
}

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

void Texxy::loadSettings() {
    QSettings settings("MyCompany", "Texxy");
    recentFiles = settings.value("recentFiles").toStringList();
    updateRecentFilesMenu();
}

void Texxy::saveSettings() {
    QSettings settings("MyCompany", "Texxy");
    settings.setValue("recentFiles", recentFiles);
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    Texxy editor;
    editor.show();
    return app.exec();
}
