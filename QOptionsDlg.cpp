#include "QOptionsDlg.hpp"
#include <QMessageBox>
#include <QSettings>
#include <QApplication>
#include <qpushbutton.h>

QOptionsDlg::QOptionsDlg(QColor defColor, QString langCode, QWidget *parent, Qt::WindowFlags flags)
    : QDialog(parent, flags), _color(defColor)
{
    lLanguage = new QLabel(tr("Language: "), this);
    cbLanguage = new QComboBox(this);
    cbLanguage->addItem(tr("English"), QString("eng"));
    cbLanguage->addItem(tr("Russian"), QString("rus"));
    lColor = new QLabel(tr("Main color: "), this);
    lColorSample = new QLabel(this);
    lColorSample->setAutoFillBackground(true);
    bColor = new QPushButton(tr("Select color"), this);
    bColorReset = new QPushButton(tr("Reset color"), this);
    lWarning = new QLabel(tr("Changes will be applied after app restart"));
    {
        QPalette pal = lWarning->palette();
        pal.setColor(lWarning->foregroundRole(), QColor("red"));
        lWarning->setPalette(pal);
    }
    //connect(cbLanguage, &QComboBox::currentIndexChanged, this, &QOptionsDlg::onLanguageChanged);
    initLayout();
    setWindowTitle(tr("Settings"));
    setMinimumSize(200, 100);
    if (langCode.isEmpty()){
        langCode = defaultLanguage();
    }
    setLanguage(langCode);
    setColorSampleBckgColor();
    connect(bColor, &QPushButton::released, this, &QOptionsDlg::onColorButtonPushed);
    connect(bColorReset, &QPushButton::released, this, &QOptionsDlg::onColorReset);
}

void QOptionsDlg::initLayout() {
    layout = new QGridLayout(this);
    int row = 0;
    layout->addWidget(lLanguage, row, 0, 1, 2);
    layout->addWidget(cbLanguage, row++, 2, 1, 2);
    layout->addWidget(lColor, row, 0, 1, 1);
    layout->addWidget(lColorSample, row, 1, 1, 1);
    layout->addWidget(bColor, row, 2, 1, 1);
    layout->addWidget(bColorReset, row++, 3, 1, 1);
    layout->addWidget(lWarning, row++, 0, 1, 4);
    setLayout(layout);
    layout->setColumnMinimumWidth(0, 100);
    layout->setColumnMinimumWidth(1, 100);
    layout->setSizeConstraint(QLayout::SetFixedSize);
}

void QOptionsDlg::onColorButtonPushed() {
    QColorDialog dlg;
    if (dlg.exec() == QDialog::Accepted) {
        _color = dlg.selectedColor();
    }
    setColorSampleBckgColor();
}

void QOptionsDlg::onColorReset() {
    _color = QColor();
    setColorSampleBckgColor();
}

void QOptionsDlg::setColorSampleBckgColor() {
    QPalette pal = lColorSample->palette();
    if (_color.isValid()) {
        lColorSample->setAutoFillBackground(true);
        pal.setColor(lColorSample->backgroundRole(), _color);
    }
    else {
        lColorSample->setAutoFillBackground(false);
    }
    lColorSample->setPalette(pal);
}

QString QOptionsDlg::language() const {
    return cbLanguage->itemData(cbLanguage->currentIndex(), Qt::UserRole).toString();
}

QString QOptionsDlg::defaultLanguage() {
    return "eng";
}

void QOptionsDlg::setLanguage(const QString& langCode) {
    if (int i = cbLanguage->findData(langCode); i != -1) {
        cbLanguage->setCurrentIndex(i);
    }
    else {
        QMessageBox::warning(this, tr("Error"), tr("Invalid language selected"));
    }
}

QColor QOptionsDlg::color() {
    return _color;
}

/*void QOptionsDlg::onLanguageChanged(int i) {
    QString code = cbLanguage->itemData(i).toString();
    if (code == "eng") {

    }
    else if (code == "rus") {

    }
    else {
        QMessageBox::warning(this, tr("Error"), tr("Invalid language selected"));
    }
}*/
