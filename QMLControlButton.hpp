#ifndef QMLCONTROLBUTTON_HPP
#define QMLCONTROLBUTTON_HPP
#include <QtQuickWidgets/QQuickWidget>
#include <QQuickItem>
#include <QWidget>

class QMLControlButton  : public QQuickWidget {
    Q_OBJECT
public:
    QMLControlButton(const QString& text, QWidget* parent = nullptr);
    void setText(const QString& text);

};


#endif // QMLCONTROLBUTTON_HPP
