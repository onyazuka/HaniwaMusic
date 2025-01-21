#ifndef QMLICON_HPP
#define QMLICON_HPP
#include "QMLWidgetBase.hpp"

class QMLIcon  : public QMLWidgetBase  {
    Q_OBJECT
public:
    QMLIcon(const QString& iconSource, QWidget* parent = nullptr);
    QQmlProperty icon_source;
};

#endif // QMLICON_HPP
