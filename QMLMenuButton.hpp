#ifndef QMLMENUBUTTON_HPP
#define QMLMENUBUTTON_HPP
#include "QMLWidgetBase.hpp"

class QMLMenuButton  : public QMLWidgetBase  {
    Q_OBJECT
public:
    QMLMenuButton(const QString& iconSource, QWidget* parent = nullptr);
    QQmlProperty icon_source;
};

#endif // QMLCONTROLBUTTON_HPP
