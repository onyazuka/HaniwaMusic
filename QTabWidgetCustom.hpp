#ifndef QTABWIDGETCUSTOM_HPP
#define QTABWIDGETCUSTOM_HPP
#include <QTabBar>
#include <QTabWidget>

class QTabBarCustom : public QTabBar {
public:
    QTabBarCustom(QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *event) override;
};

class QTabWidgetCustom : public QTabWidget {
public:
    QTabWidgetCustom(QWidget *parent = nullptr);
};

#endif // QTABWIDGETCUSTOM_HPP
