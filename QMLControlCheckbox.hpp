#ifndef QMLCONTROLCHECKBOX_HPP
#define QMLCONTROLCHECKBOX_HPP
#include "QMLWidgetBase.hpp"

class QMLControlCheckbox  : public QMLWidgetBase {
    Q_OBJECT
public:
    QMLControlCheckbox(const QString& text, bool checked, QWidget* parent = nullptr);
    void setIconPath(const QString& text);
    void setChecked(bool chState);
    bool isChecked();
    QQuickItem* checkBox();
};

#endif // QMLCONTROLCHECKBOX_HPP
