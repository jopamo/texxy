#include "findreplacedialog.h"

FindReplaceDialog::FindReplaceDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle(tr("Find/Replace"));
    setModal(false);

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
