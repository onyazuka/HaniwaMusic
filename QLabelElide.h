#ifndef QLABELELIDE_H
#define QLABELELIDE_H
#include <QLineEdit>

class QLabelElide : public QLineEdit {
public:
    QLabelElide(const QString& txt, QWidget* parent = nullptr);
    void elideText(const QString& txt);
private:
    void resizeEvent(QResizeEvent *event) override;
    void elideText();

    QString originalText;
};

#endif // QLABELELIDE_H
