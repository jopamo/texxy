#include "texxy.h"
#include "language_support.h"
#include "languages.cpp"

//----------------------------------------------------
// FindReplaceDialog
//----------------------------------------------------
FindReplaceDialog::FindReplaceDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle(tr("Find/Replace"));
    setModal(false);  // allow user to keep it open while editing

    QLabel* findLabel = new QLabel(tr("Find:"), this);
    findLineEdit = new QLineEdit(this);

    QLabel* replaceLabel = new QLabel(tr("Replace:"), this);
    replaceLineEdit = new QLineEdit(this);

    findButton = new QPushButton(tr("Find Next"), this);
    replaceButton = new QPushButton(tr("Replace"), this);
    replaceAllButton = new QPushButton(tr("Replace All"), this);
    closeButton = new QPushButton(tr("Close"), this);

    matchCaseCheckBox = new QCheckBox(tr("Match Case"), this);

    QGridLayout* layout = new QGridLayout;
    layout->addWidget(findLabel, 0, 0);
    layout->addWidget(findLineEdit, 0, 1, 1, 3);
    layout->addWidget(replaceLabel, 1, 0);
    layout->addWidget(replaceLineEdit, 1, 1, 1, 3);

    layout->addWidget(matchCaseCheckBox, 2, 0, 1, 4);

    layout->addWidget(findButton, 3, 0);
    layout->addWidget(replaceButton, 3, 1);
    layout->addWidget(replaceAllButton, 3, 2);
    layout->addWidget(closeButton, 3, 3);

    setLayout(layout);

    connect(findButton, &QPushButton::clicked, this, &FindReplaceDialog::onFindClicked);
    connect(replaceButton, &QPushButton::clicked, this, &FindReplaceDialog::onReplaceClicked);
    connect(replaceAllButton, &QPushButton::clicked, this, &FindReplaceDialog::onReplaceAllClicked);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::close);
}

void FindReplaceDialog::setTextEdit(QTextEdit* edit) {
    textEdit = edit;
}

void FindReplaceDialog::onFindClicked() {
    if (!textEdit)
        return;

    QString searchString = findLineEdit->text();
    if (searchString.isEmpty())
        return;

    QTextDocument::FindFlags flags;
    if (matchCaseCheckBox->isChecked()) {
        flags |= QTextDocument::FindCaseSensitively;
    }

    bool found = textEdit->find(searchString, flags);
    if (!found) {
        // Restart from beginning
        QTextCursor cursor(textEdit->document());
        textEdit->setTextCursor(cursor);
        found = textEdit->find(searchString, flags);
        if (!found) {
            QMessageBox::information(this, tr("Find"), tr("No further matches found."));
        }
    }
}

void FindReplaceDialog::onReplaceClicked() {
    if (!textEdit)
        return;

    QTextCursor cursor = textEdit->textCursor();
    QString selectedText = cursor.selectedText();
    QString searchString = findLineEdit->text();

    Qt::CaseSensitivity cs = (matchCaseCheckBox->isChecked()) ? Qt::CaseSensitive : Qt::CaseInsensitive;

    // If selection matches the find text, replace it
    if (selectedText.compare(searchString, cs) == 0) {
        cursor.insertText(replaceLineEdit->text());
    }
    // Then jump to next match
    onFindClicked();
}

void FindReplaceDialog::onReplaceAllClicked() {
    if (!textEdit)
        return;

    QString searchString = findLineEdit->text();
    QString replaceString = replaceLineEdit->text();
    if (searchString.isEmpty())
        return;

    Qt::CaseSensitivity cs = (matchCaseCheckBox->isChecked()) ? Qt::CaseSensitive : Qt::CaseInsensitive;

    QTextDocument* doc = textEdit->document();
    QTextCursor cursor(doc);
    int replacements = 0;

    cursor.beginEditBlock();
    while (!cursor.isNull() && !cursor.atEnd()) {
        cursor = doc->find(searchString, cursor,
                           (cs == Qt::CaseSensitive) ? QTextDocument::FindCaseSensitively : QTextDocument::FindFlags());
        if (!cursor.isNull()) {
            cursor.insertText(replaceString);
            replacements++;
        }
    }
    cursor.endEditBlock();

    QMessageBox::information(this, tr("Replace All"), tr("Replaced %1 occurrences.").arg(replacements));
}

//----------------------------------------------------
// Texxy
//----------------------------------------------------
Texxy::Texxy(QWidget* parent) : QMainWindow(parent) {
    textEdit = new QTextEdit(this);
    setCentralWidget(textEdit);

    textEdit->setStyleSheet(
        "QTextEdit { "
        "  background-color: #000000; "
        "  color: #FFFFFF; "
        "}");

    // Create actions
    QAction* newAction = new QAction(tr("&New"), this);
    QAction* openAction = new QAction(tr("&Open..."), this);
    QAction* saveAction = new QAction(tr("&Save"), this);
    QAction* saveAsAction = new QAction(tr("Save &As..."), this);
    QAction* exitAction = new QAction(tr("E&xit"), this);

    newAction->setShortcut(QKeySequence::New);
    openAction->setShortcut(QKeySequence::Open);
    saveAction->setShortcut(QKeySequence::Save);
    saveAsAction->setShortcut(QKeySequence::SaveAs);

    connect(newAction, &QAction::triggered, this, &Texxy::newFile);
    connect(openAction, &QAction::triggered, this, &Texxy::openFile);
    connect(saveAction, &QAction::triggered, this, &Texxy::saveFile);
    connect(saveAsAction, &QAction::triggered, this, &Texxy::saveFileAs);
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);

    // Find/Replace
    QAction* findReplaceAction = new QAction(tr("Find/Replace..."), this);
    connect(findReplaceAction, &QAction::triggered, this, &Texxy::showFindReplace);

    // File menu
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);

    // "Open Recent" sub-menu
    recentFilesMenu = fileMenu->addMenu(tr("Open Recent"));
    updateRecentFilesMenu();

    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    // Edit menu
    QMenu* editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(findReplaceAction);

    // Toolbar
    QToolBar* toolBar = addToolBar(tr("File"));
    toolBar->addAction(newAction);
    toolBar->addAction(openAction);
    toolBar->addAction(saveAction);
    toolBar->addAction(saveAsAction);

    // Status bar
    statusLabel = new QLabel(this);
    statusBar()->addPermanentWidget(statusLabel);

    connect(textEdit->document(), &QTextDocument::modificationChanged, this, &Texxy::updateWindowTitle);
    connect(textEdit, &QTextEdit::cursorPositionChanged, this, &Texxy::updateCursorPosition);

    // Create find/replace dialog
    findReplaceDialog = new FindReplaceDialog(this);
    findReplaceDialog->setTextEdit(textEdit);

    setWindowTitle(tr("Untitled - Texxy"));
    resize(900, 600);

    loadSettings();  // Load recent files
    updateCursorPosition();
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

// --------------------------------------------------
// Slots
// --------------------------------------------------
void Texxy::newFile() {
    if (!maybeSaveChanges())
        return;
    textEdit->clear();
    currentFilePath.clear();
    setWindowTitle(tr("Untitled - Texxy"));
}

void Texxy::openFile() {
    if (!maybeSaveChanges())
        return;

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"));
    if (!fileName.isEmpty()) {
        loadFile(fileName);
        addToRecentFiles(fileName);
    }
}

bool Texxy::saveFile() {
    if (currentFilePath.isEmpty())
        return saveFileAs();
    return saveToPath(currentFilePath);
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
    loadFile(fileName);
    addToRecentFiles(fileName);
}

void Texxy::showFindReplace() {
    findReplaceDialog->show();
    findReplaceDialog->raise();
    findReplaceDialog->activateWindow();
}

void Texxy::updateCursorPosition() {
    QTextCursor cursor = textEdit->textCursor();
    int line = cursor.blockNumber() + 1;
    int col = cursor.columnNumber() + 1;
    statusLabel->setText(tr("Line: %1, Col: %2").arg(line).arg(col));
}

void Texxy::updateWindowTitle() {
    QString title = currentFilePath.isEmpty() ? tr("Untitled") : QFileInfo(currentFilePath).fileName();
    if (textEdit->document()->isModified()) {
        title += "*";
    }
    setWindowTitle(title + tr(" - Texxy"));
}

// --------------------------------------------------
// Private Methods
// --------------------------------------------------
bool Texxy::maybeSaveChanges() {
    if (!textEdit->document()->isModified())
        return true;

    auto ret = QMessageBox::warning(this, tr("Unsaved Changes"),
                                    tr("The document has been modified.\nDo you want to save your changes?"),
                                    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    if (ret == QMessageBox::Save)
        return saveFile();
    else if (ret == QMessageBox::Cancel)
        return false;
    return true;  // Discard
}

void Texxy::loadFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), tr("Cannot open file: %1").arg(filePath));
        return;
    }
    QTextStream in(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    in.setCodec("UTF-8");  // For older Qt
#endif
    textEdit->setPlainText(in.readAll());
    file.close();

    currentFilePath = filePath;
    textEdit->document()->setModified(false);
    updateWindowTitle();

    // 1) Remove old highlighter
    if (highlighter) {
        delete highlighter;
        highlighter = nullptr;
    }

    // 2) MIME detection
    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForFile(filePath, QMimeDatabase::MatchContent);
    QString lowerFilePath = filePath.toLower();

    // 3) Find the best match
    const LanguageDefinition* lang = findMatchingLanguage(mime, lowerFilePath);
    if (lang) {
        // 4) Create the language’s syntax highlighter
        highlighter = lang->highlighterFactory(textEdit->document());
    }
    else {
        // No known language; do nothing or apply a default
        // highlighter = createPlainTextHighlighter(...) // if you had one, or just
        // do nothing
        highlighter = nullptr;
    }
}

bool Texxy::saveToPath(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), tr("Cannot save file: %1").arg(filePath));
        return false;
    }
    QTextStream out(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    out.setCodec("UTF-8");
#endif
    out << textEdit->toPlainText();
    file.close();

    currentFilePath = filePath;
    textEdit->document()->setModified(false);
    updateWindowTitle();
    return true;
}

// --------------------------------------------------
// Recent Files
// --------------------------------------------------
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
    for (const QString& f : qAsConst(recentFiles)) {
        QAction* act = new QAction(QFileInfo(f).fileName(), this);
        act->setData(f);
        connect(act, &QAction::triggered, this, &Texxy::openRecentFile);
        recentFilesMenu->addAction(act);
    }
    recentFilesMenu->setEnabled(!recentFiles.isEmpty());
}

// --------------------------------------------------
// Settings
// --------------------------------------------------
void Texxy::loadSettings() {
    QSettings settings("MyCompany", "Texxy");
    recentFiles = settings.value("recentFiles").toStringList();
    updateRecentFilesMenu();
}

void Texxy::saveSettings() {
    QSettings settings("MyCompany", "Texxy");
    settings.setValue("recentFiles", recentFiles);
}

// --------------------------------------------------
// main()
// --------------------------------------------------
#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    Texxy editor;
    editor.show();
    return app.exec();
}
