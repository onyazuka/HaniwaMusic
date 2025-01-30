//#ifdef _MSC_VER

#include <QWidget>
#include "HaniwaMusic.h"

class QMediaKeysWinapiAdaptor : public QWidget {
    Q_OBJECT
public:
    QMediaKeysWinapiAdaptor(QWidget* parent, HaniwaMusic* receiver);
    ~QMediaKeysWinapiAdaptor();
public slots:
    void Next();
    void Previous();
    void PlayPause();
    void Play();
    void Pause();
    void Stop();
signals:
    void NextSignal();
    void PreviousSignal();
    void PlayPauseSignal();
    void PlaySignal();
    void PauseSignal();
    void StopSignal();
private:
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
};

//#endif
