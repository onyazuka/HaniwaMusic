#ifndef QTABWIDGETCUSTOM_HPP
#define QTABWIDGETCUSTOM_HPP
#include <QTabWidget>

class QTabWidgetCustom : public QTabWidget {
public:
    QTabWidgetCustom(QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // QTABWIDGETCUSTOM_HPP
