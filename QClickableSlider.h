#ifndef QCLICKABLESLIDER_H
#define QCLICKABLESLIDER_H

#include <QSlider>

class QClickableSlider : public QSlider {
    Q_OBJECT
public:
    QClickableSlider(QWidget *parent = nullptr);
    QClickableSlider(Qt::Orientation orientation, QWidget *parent = nullptr);
private:
    void mousePressEvent(QMouseEvent *ev) override;
};

#endif // QCLICKABLESLIDER_H
