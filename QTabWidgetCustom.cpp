#include "QTabWidgetCustom.hpp"
#include <QStyle>
#include <QStylePainter>
#include <QStyleOption>

QTabWidgetCustom::QTabWidgetCustom(QWidget *parent)
    : QTabWidget(parent)
{
    ;
}

void QTabWidgetCustom::paintEvent(QPaintEvent *event) {
    /*QStylePainter painter(this);
    QStyleOptionTab opt;

    for (int i = 0; i < count(); i++)
    {
        initStyleOption()
        opt.palette.setColor(QPalette::Window, Qt::red);
        opt.palette.setColor(QPalette::Button, Qt::red);
        painter.drawControl(QStyle::CE_TabBarTabShape, opt);
        painter.drawControl(QStyle::CE_TabBarTabLabel, opt);
    }
    qDebug()<<"window color role color:"<<opt.palette.color(QPalette::Window);
    qDebug()<<"button color role color:"<<opt.palette.color(QPalette::Button);*/
    QTabWidget::paintEvent(event);
}
