#ifndef EDITORWIDGET_H
#define EDITORWIDGET_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QTextBlock>
#include <QPainter>
#include <QPaintEvent>

// Subclass QPlainTextEdit to expose protected methods for editor functionality
class MyPlainTextEdit : public QPlainTextEdit {
   public:
    MyPlainTextEdit(QWidget* parent = nullptr) : QPlainTextEdit(parent) {}

    // Expose QPlainTextEdit's protected methods for use in the editor widget
    QTextBlock firstVisibleBlock() const { return QPlainTextEdit::firstVisibleBlock(); }
    QRectF blockBoundingGeometry(const QTextBlock& block) const { return QPlainTextEdit::blockBoundingGeometry(block); }
    QPointF contentOffset() const { return QPlainTextEdit::contentOffset(); }
    QRectF blockBoundingRect(const QTextBlock& block) const { return QPlainTextEdit::blockBoundingRect(block); }
    void setViewportMargins(int left, int top, int right, int bottom) { QPlainTextEdit::setViewportMargins(left, top, right, bottom); }
};

// Main EditorWidget class that integrates the text editor and line number area
class EditorWidget : public QWidget {
    Q_OBJECT

   public:
    explicit EditorWidget(QWidget* parent = nullptr);  // Constructor to initialize editor components

    // Getter for text editor (MyPlainTextEdit) instance
    MyPlainTextEdit* textEdit() const { return m_textEdit; }

    // Getter and setter for file path
    void setFilePath(const QString& path);
    QString filePath() const;

   protected:
    void resizeEvent(QResizeEvent* event) override;  // Handles resizing of the widget

   private:
    // Calculates the width required for the line number area
    int lineNumberAreaWidth() const;

    // Updates the width of the line number area based on the number of blocks
    void updateLineNumberAreaWidth(int newBlockCount);

    // Updates the line number area on scrolling or content update
    void updateLineNumberArea(const QRect& rect, int dy);

    // Handles the paint event for the line number area
    void lineNumberAreaPaintEvent(QPaintEvent* event);

   private:
    MyPlainTextEdit* m_textEdit = nullptr;       // Instance of MyPlainTextEdit for text editing
    class LineNumberArea;                        // Forward declaration of LineNumberArea
    LineNumberArea* m_lineNumberArea = nullptr;  // Line number area widget
    QString m_filePath;                          // Stores the current file path

    // Nested class for displaying line numbers beside the text editor
    class LineNumberArea : public QWidget {
       public:
        explicit LineNumberArea(EditorWidget* editor);  // Constructor with editor reference

       protected:
        void paintEvent(QPaintEvent* event) override;  // Paint the line number area

       private:
        EditorWidget* m_editor;  // Reference to the parent EditorWidget
    };
};

#endif  // EDITORWIDGET_H
