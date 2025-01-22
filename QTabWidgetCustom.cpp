#include "QTabWidgetCustom.hpp"
#include <QStyle>
#include <QStylePainter>
#include <QStyleOption>
#include <QApplication>
#include <QPainterPath>

QTabBarCustom::QTabBarCustom(QWidget *parent)
    : QTabBar(parent)
{

}

void QTabBarCustom::paintEvent(QPaintEvent *event) {
    QStylePainter tabPainter(this);
    tabPainter.setRenderHint(QPainter::Antialiasing);
    for (int i = 0; i < count(); ++i) {
        if (i == currentIndex()) {
            tabPainter.setPen(QColor(80,80,80));
            //tabPainter.drawRoundedRect(tabRect(i), 20, 20, Qt::SizeMode::RelativeSize);
            QPainterPath path;
            path.addRoundedRect(tabRect(i), 20, 20, Qt::SizeMode::RelativeSize);
            tabPainter.fillPath(path, QApplication::palette().highlight());
            tabPainter.drawPath(path);
            tabPainter.setPen(QApplication::palette().base().color());
            tabPainter.drawText(tabRect(i), Qt::AlignVCenter | Qt::AlignHCenter, tabText(i));
        }
        else {
            tabPainter.setPen(QColor(80,80,80));
            //tabPainter.drawRoundedRect(tabRect(i), 20, 20, Qt::SizeMode::RelativeSize);
            QPainterPath path;
            path.addRoundedRect(tabRect(i), 20, 20, Qt::SizeMode::RelativeSize);
            tabPainter.fillPath(path, QApplication::palette().base());
            tabPainter.drawPath(path);
            tabPainter.setPen(QApplication::palette().highlight().color());
            tabPainter.drawText(tabRect(i), Qt::AlignVCenter | Qt::AlignHCenter, tabText(i));
        }
    }
}

QTabWidgetCustom::QTabWidgetCustom(QWidget *parent)
    : QTabWidget(parent)
{
    setTabBar(new QTabBarCustom(this));
}
