#ifndef QMLWIDGETBASE_HPP
#define QMLWIDGETBASE_HPP
#include <QtQuickWidgets/QQuickWidget>
#include <QQuickItem>
#include <QWidget>
#include <QQmlProperty>

class QMLWidgetBase  : public QQuickWidget {
    Q_OBJECT
public:
    QMLWidgetBase(const QString& resourcePath, QWidget* parent = nullptr);
    virtual ~QMLWidgetBase();
    QQuickItem* findItem(const QString& objectName);
    virtual QQuickItem* base();
};

#endif // QMLWIDGETBASE_HPP
