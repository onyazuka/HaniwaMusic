#include "QOptionsDlg.hpp"
#include <QMessageBox>
#include <QSettings>
#include <QApplication>

QOptionsDlg::QOptionsDlg(QString langCode, QWidget *parent, Qt::WindowFlags flags)
    : QDialog(parent, flags)
{
    lLanguage = new QLabel(tr("Language: "), this);
    cbLanguage = new QComboBox(this);
    cbLanguage->addItem(tr("English"), QString("eng"));
    cbLanguage->addItem(tr("Russian"), QString("rus"));
    lWarning = new QLabel(tr("Changes will be applied after app restart"));
    //connect(cbLanguage, &QComboBox::currentIndexChanged, this, &QOptionsDlg::onLanguageChanged);
    initLayout();
    setWindowTitle(tr("Settings"));
    setMinimumSize(200, 100);
    if (langCode.isEmpty()){
        langCode = defaultLanguage();
    }
    setLanguage(langCode);
}

void QOptionsDlg::initLayout() {
    layout = new QGridLayout(this);
    layout->addWidget(lLanguage, 0, 0, 1, 1);
    layout->addWidget(cbLanguage, 0, 1, 1, 1);
    layout->addWidget(lWarning, 1, 0, 1, 2);
    setLayout(layout);
    layout->setColumnMinimumWidth(0, 100);
    layout->setColumnMinimumWidth(1, 100);
    layout->setSizeConstraint(QLayout::SetFixedSize);
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
