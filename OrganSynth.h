#ifndef STKDAC_H
#define STKDAC_H

#include <QObject>
#include <QDebug>
#include <QThread>
#include "SineWave.h"
#include "RtWvOut.h"
#include "Stk.h"
#include "OrganSynth.h"
#include "RtAudio.h"


using namespace stk;

class StkDac : public QObject
{
    Q_OBJECT
public:
    explicit StkDac(QObject *parent = 0);
    void run();
    void stopDac() { running = false; }

signals:

public slots:
    void runSynth();
    void stopSynth() {
        qDebug() << "stop called\n";
        running = false;
    }
    void toggleSine();

private:

    static int tick( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
              double streamTime, RtAudioStreamStatus status, void *dataPointer );

    RtAudio *dac;
    StkFrames *audio_data;
    bool running;
};

#endif // STKDAC_H
