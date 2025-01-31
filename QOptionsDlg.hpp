#ifndef QOPTIONSDLG_HPP
#define QOPTIONSDLG_HPP

#include <QDialog>
#include <QLabel>
#include <QGridLayout>
#include <QComboBox>
#include <QColorDialog>
#include <QPushButton>

class QOptionsDlg : public QDialog {
    Q_OBJECT
public:
    QOptionsDlg(QColor defColor, QString langCode, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    QString language() const;
    static QString defaultLanguage();
    void setLanguage(const QString& langCode);
    QColor color();
private:
    void onLanguageChanged(int i);
    void initLayout();
    void onColorButtonPushed();
    void onColorReset();
    void setColorSampleBckgColor();
    QLabel* lLanguage;
    QComboBox* cbLanguage;
    QLabel* lColor;
    QLabel* lColorSample;
    QPushButton* bColor;
    QPushButton* bColorReset;
    QLabel* lWarning;
    QGridLayout* layout;
    QColor _color;
};


#endif // QOPTIONSDLG_HPP
