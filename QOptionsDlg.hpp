#ifndef QOPTIONSDLG_HPP
#define QOPTIONSDLG_HPP

#include <QDialog>
#include <QLabel>
#include <QGridLayout>
#include <QComboBox>

class QOptionsDlg : public QDialog {
    Q_OBJECT
public:
    QOptionsDlg(QString langCode, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    QString language() const;
    static QString defaultLanguage();
    void setLanguage(const QString& langCode);
private:
    void onLanguageChanged(int i);
    void initLayout();
    QLabel* lLanguage;
    QComboBox* cbLanguage;
    QLabel* lWarning;
    QGridLayout* layout;
};


#endif // QOPTIONSDLG_HPP
