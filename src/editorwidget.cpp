#include "editorwidget.h"
#include <QVBoxLayout>
#include <QPainter>
#include <QTextBlock>
#include <QScrollBar>
#include <QPaintEvent>
#include <QTextCursor>

EditorWidget::LineNumberArea::LineNumberArea(EditorWidget* editor) : QWidget(editor), m_editor(editor) {
    setAttribute(Qt::WA_OpaquePaintEvent);
}

void EditorWidget::LineNumberArea::paintEvent(QPaintEvent* event) {
    if (!m_editor || !m_editor->textEdit()) {
        qWarning() << "LineNumberArea: Editor or textEdit is null!";
        return;
    }

    QPainter painter(this);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = m_editor->textEdit()->firstVisibleBlock();
    int blockNumber = block.blockNumber();

    int top = static_cast<int>(m_editor->textEdit()->blockBoundingGeometry(block).translated(m_editor->textEdit()->contentOffset()).top());
    int bottom = top + static_cast<int>(m_editor->textEdit()->blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);

            painter.setPen(QColor("#00008B"));

            painter.drawText(0, top, m_editor->lineNumberAreaWidth() - 4, fontMetrics().height(), Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + static_cast<int>(m_editor->textEdit()->blockBoundingRect(block).height());
        ++blockNumber;
    }
}

EditorWidget::EditorWidget(QWidget* parent) : QWidget(parent) {
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_textEdit = new MyPlainTextEdit(this);
    m_lineNumberArea = new LineNumberArea(this);

    layout->addWidget(m_textEdit);
    setLayout(layout);

    connect(m_textEdit, &MyPlainTextEdit::blockCountChanged, this, &EditorWidget::updateLineNumberAreaWidth);
    connect(m_textEdit, &MyPlainTextEdit::updateRequest, this, &EditorWidget::updateLineNumberArea);

    updateLineNumberAreaWidth(0);
}

void EditorWidget::setFilePath(const QString& path) {
    m_filePath = path;
}

QString EditorWidget::filePath() const {
    return m_filePath;
}

int EditorWidget::lineNumberAreaWidth() const {
    if (!m_textEdit) {
        qWarning() << "EditorWidget: textEdit is null!";
        return 0;
    }

    int digits = 1;
    int maxLines = qMax(1, m_textEdit->blockCount());
    while (maxLines >= 10) {
        maxLines /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}

void EditorWidget::updateLineNumberArea(const QRect& rect, int dy) {
    if (!m_lineNumberArea || !m_textEdit) {
        qWarning() << "EditorWidget: lineNumberArea or textEdit is null!";
        return;
    }

    if (dy != 0) {
        m_lineNumberArea->scroll(0, dy);
    }
    else {
        m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());
    }

    if (rect.contains(m_textEdit->viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void EditorWidget::resizeEvent(QResizeEvent* event) {
    if (!m_textEdit || !m_lineNumberArea) {
        qWarning() << "EditorWidget: textEdit or lineNumberArea is null!";
        return;
    }

    QWidget::resizeEvent(event);

    QRect cr = m_textEdit->contentsRect();
    int newWidth = lineNumberAreaWidth();

    m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), newWidth, cr.height()));

    updateLineNumberAreaWidth(0);

    m_textEdit->viewport()->update();
    updateGeometry();
}

void EditorWidget::updateLineNumberAreaWidth(int /* newBlockCount */) {
    if (!m_textEdit) {
        qWarning() << "EditorWidget: textEdit is null!";
        return;
    }

    const int newMargin = lineNumberAreaWidth();
    m_textEdit->setViewportMargins(newMargin, 0, 0, 0);

    m_lineNumberArea->repaint();
}
