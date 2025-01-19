#ifndef QMLWIDGETBASE_HPP
#define QMLWIDGETBASE_HPP
#include <QtQuickWidgets/QQuickWidget>
#include <QQuickItem>
#include <QWidget>

class QMLWidgetBase  : public QQuickWidget {
    Q_OBJECT
public:
    QMLWidgetBase(const QString& resourcePath, QWidget* parent = nullptr);
    QQuickItem* findItem(const QString& objectName);
};


#endif // QMLWIDGETBASE_HPP
