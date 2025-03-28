#include "findreplacedialog.h"
#include <QVBoxLayout>
#include <QPushButton>

FindReplaceDialog::FindReplaceDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle(tr("Find and Replace"));

    QGridLayout* layout = new QGridLayout(this);

    QLabel* findLabel = new QLabel(tr("Find:"));
    layout->addWidget(findLabel, 0, 0);
    findLineEdit = new QLineEdit(this);
    layout->addWidget(findLineEdit, 0, 1);

    QLabel* replaceLabel = new QLabel(tr("Replace:"));
    layout->addWidget(replaceLabel, 1, 0);
    replaceLineEdit = new QLineEdit(this);
    layout->addWidget(replaceLineEdit, 1, 1);

    matchCaseCheckBox = new QCheckBox(tr("Match case"), this);
    layout->addWidget(matchCaseCheckBox, 2, 0, 1, 2);

    findButton = new QPushButton(tr("Find"), this);
    layout->addWidget(findButton, 3, 0);
    connect(findButton, &QPushButton::clicked, this, &FindReplaceDialog::onFindClicked);

    replaceButton = new QPushButton(tr("Replace"), this);
    layout->addWidget(replaceButton, 3, 1);
    connect(replaceButton, &QPushButton::clicked, this, &FindReplaceDialog::onReplaceClicked);

    replaceAllButton = new QPushButton(tr("Replace All"), this);
    layout->addWidget(replaceAllButton, 4, 0, 1, 2);
    connect(replaceAllButton, &QPushButton::clicked, this, &FindReplaceDialog::onReplaceAllClicked);

    closeButton = new QPushButton(tr("Close"), this);
    layout->addWidget(closeButton, 5, 0, 1, 2);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);

    setLayout(layout);
}

void FindReplaceDialog::setTextEdit(QPlainTextEdit* edit) {
    textEdit = edit;
}

void FindReplaceDialog::onFindClicked() {
    if (!textEdit)
        return;

    QString findText = findLineEdit->text();
    QTextDocument::FindFlags flags;
    if (matchCaseCheckBox->isChecked()) {
        flags |= QTextDocument::FindCaseSensitively;
    }

    QTextCursor cursor = textEdit->textCursor();
    bool found = textEdit->find(findText, flags);
    if (!found) {
        QMessageBox::information(this, tr("Find"), tr("Text not found"));
    }
}

void FindReplaceDialog::onReplaceClicked() {
    if (!textEdit)
        return;

    QString findText = findLineEdit->text();
    QString replaceText = replaceLineEdit->text();
    QTextDocument::FindFlags flags;
    if (matchCaseCheckBox->isChecked()) {
        flags |= QTextDocument::FindCaseSensitively;
    }

    QTextCursor cursor = textEdit->textCursor();
    bool found = textEdit->find(findText, flags);
    if (found) {
        cursor.insertText(replaceText);
    }
    else {
        QMessageBox::information(this, tr("Replace"), tr("Text not found"));
    }
}

void FindReplaceDialog::onReplaceAllClicked() {
    if (!textEdit)
        return;

    QString findText = findLineEdit->text();
    QString replaceText = replaceLineEdit->text();
    QTextDocument::FindFlags flags;
    if (matchCaseCheckBox->isChecked()) {
        flags |= QTextDocument::FindCaseSensitively;
    }

    QTextCursor cursor = textEdit->textCursor();
    while (textEdit->find(findText, flags)) {
        cursor = textEdit->textCursor();
        cursor.insertText(replaceText);
    }
}
