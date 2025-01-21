#ifndef QMLCONTROLCHECKBOX_HPP
#define QMLCONTROLCHECKBOX_HPP
#include "QMLWidgetBase.hpp"

class QMLControlCheckbox  : public QMLWidgetBase {
    Q_OBJECT
public:
    QMLControlCheckbox(const QString& text, bool checked, QWidget* parent = nullptr);
    QQuickItem* base() override;
    QQmlProperty checked;
    QQmlProperty iconPath;
};

#endif // QMLCONTROLCHECKBOX_HPP
