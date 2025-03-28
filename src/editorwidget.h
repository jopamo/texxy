#ifndef EDITORWIDGET_H
#define EDITORWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QVBoxLayout>

class EditorWidget : public QWidget {
    Q_OBJECT
   public:
    explicit EditorWidget(QWidget* parent = nullptr) : QWidget(parent) {
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        textEdit = new QTextEdit(this);
        layout->addWidget(textEdit);

        setLayout(layout);
    }

    void setFilePath(const QString& path) { m_filePath = path; }
    QString filePath() const { return m_filePath; }

    QTextEdit* textEdit;

   private:
    QString m_filePath;
};

#endif  // EDITORWIDGET_H
