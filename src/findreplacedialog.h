#ifndef FINDREPLACEDIALOG_H
#define FINDREPLACEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QTextEdit>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QTextCursor>
#include <QTextDocument>

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
    QLineEdit* findLineEdit = nullptr;
    QLineEdit* replaceLineEdit = nullptr;
    QPushButton* findButton = nullptr;
    QPushButton* replaceButton = nullptr;
    QPushButton* replaceAllButton = nullptr;
    QPushButton* closeButton = nullptr;
    QCheckBox* matchCaseCheckBox = nullptr;
};

#endif  // FINDREPLACEDIALOG_H
