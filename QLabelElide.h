#ifndef QLABELELIDE_H
#define QLABELELIDE_H
#include <QLineEdit>
#include <QTimer>

class QLabelElide : public QLineEdit {
public:
    QLabelElide(const QString& txt, QWidget* parent = nullptr);
    void elideText(const QString& txt);
    inline bool moving() const { return _moving; }
    inline void setMoving(bool val) { _moving = val; }
private slots:
    void move();
private:
    void resizeEvent(QResizeEvent *event) override;
    bool elideText();
    void startMove();
    void stopMove();
    void resetMove();

    QString originalText;

    bool _moving = false;
    size_t movePos = 0;
    QTimer moveTimer;
};

#endif // QLABELELIDE_H
