#ifndef FINDREPLACEDIALOG_H
#define FINDREPLACEDIALOG_H

#include <QDialog>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QTextCursor>
#include <QTextDocument>
#include <QMessageBox>
#include <QGridLayout>

class FindReplaceDialog : public QDialog {
    Q_OBJECT

   public:
    explicit FindReplaceDialog(QWidget* parent = nullptr);  // Constructor sets up UI components and layout

    void setTextEdit(QPlainTextEdit* edit);  // Sets the text editor where Find/Replace will occur

   private slots:
    void onFindClicked();        // Handles the "Find" button click event
    void onReplaceClicked();     // Handles the "Replace" button click event
    void onReplaceAllClicked();  // Handles the "Replace All" button click event

   private:
    QLineEdit* findLineEdit;        // Input field for search term
    QLineEdit* replaceLineEdit;     // Input field for replacement term
    QCheckBox* matchCaseCheckBox;   // Checkbox to toggle case-sensitive search
    QPushButton* findButton;        // Button to trigger "Find"
    QPushButton* replaceButton;     // Button to trigger "Replace"
    QPushButton* replaceAllButton;  // Button to trigger "Replace All"
    QPushButton* closeButton;       // Button to close the dialog
    QPlainTextEdit* textEdit;       // The text editor to perform find/replace operations on
};

#endif  // FINDREPLACEDIALOG_H
