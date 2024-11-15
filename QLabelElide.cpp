#include "QLabelElide.h"

QLabelElide::QLabelElide(const QString& txt, QWidget* parent)
    : QLineEdit(txt, parent), originalText(txt)
{
    setReadOnly(true);
    setStyleSheet("border: none");
}

void QLabelElide::resizeEvent(QResizeEvent*) {
    elideText();
}

void QLabelElide::elideText(const QString& txt) {
    originalText = txt;
    elideText();
}

void QLabelElide::elideText() {
    QFontMetrics metrics(font());
    QString elidedText = metrics.elidedText(originalText, Qt::ElideRight, width());
    setText(elidedText);
    setCursorPosition(0);
}
