#include "QLabelElide.h"
#include <QApplication>

QLabelElide::QLabelElide(const QString& txt, QWidget* parent)
    : QLineEdit(txt, parent), originalText(txt)
{
    setReadOnly(true);
    //setStyleSheet("border: none");
    QPalette pal;
    pal.setBrush(backgroundRole(), QApplication::palette().window());
    setFrame(false);
    setPalette(pal);
    connect(&moveTimer, &QTimer::timeout, this, &QLabelElide::move);
}

void QLabelElide::resizeEvent(QResizeEvent*) {
    resetMove();
    elideText();
}

void QLabelElide::elideText(const QString& txt) {
    resetMove();
    originalText = txt;
    elideText();
}

bool QLabelElide::elideText() {
    if (originalText.isEmpty()) {
        return true;
    }
    QFontMetrics metrics(font());
    QString txt = movePos ? QString(originalText.data() + movePos, originalText.size() - movePos) : originalText;
    QString elidedText = metrics.elidedText(txt, Qt::ElideRight, width());
    setText(elidedText);
    setCursorPosition(0);
    if (moving() && !moveTimer.isActive() && (elidedText != originalText)) {
        startMove();
    }
    else if (moving() && moveTimer.isActive() && (elidedText == originalText)) {
        stopMove();
    }
    else if (moving() && moveTimer.isActive() && originalText.contains(elidedText)) {
        movePos = 0;
        moveTimer.stop();
        moveTimer.setInterval(2000);
        moveTimer.start();
        return false;
    }
    return true;
}

void QLabelElide::startMove() {
    //moveTimer.setInterval(2000);
    moveTimer.start(0);
}

void QLabelElide::stopMove() {
    moveTimer.stop();
}

void QLabelElide::move() {
    if (!movePos) {
        moveTimer.stop();
        moveTimer.setInterval(2000);
        moveTimer.start();
    }
    else if (movePos == 1) {
        moveTimer.stop();
        moveTimer.setInterval(250);
        moveTimer.start();
    }
    if (elideText()) {
        ++movePos;
    }
}

void QLabelElide::resetMove() {
    moveTimer.stop();
    movePos = 0;
}
