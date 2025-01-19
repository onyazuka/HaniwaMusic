#ifndef QMLCONTROLCHECKBOX_HPP
#define QMLCONTROLCHECKBOX_HPP
#include <QtQuickWidgets/QQuickWidget>
#include <QQuickItem>
#include <QWidget>

class QMLControlCheckbox  : public QQuickWidget {
    Q_OBJECT
public:
    /*enum CheckState {
        Unchecked,
        PartiallyChecked,
        Checked
    };*/
    QMLControlCheckbox(const QString& text, bool checked, QWidget* parent = nullptr);
    void setIconPath(const QString& text);
    void setChecked(bool chState);
    bool isChecked() const;
    QQuickItem* checkBox() const;
};

#endif // QMLCONTROLCHECKBOX_HPP
