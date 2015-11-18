#ifndef STKDAC_H
#define STKDAC_H

#include <QObject>
#include "SineWave.h"
#include "RtWvOut.h"
#include "Stk.h"
#include "StkDac.h"
#include "RtAudio.h"


using namespace stk;

class StkDac : public QObject
{
    Q_OBJECT
public:
    explicit StkDac(QObject *parent = 0);

signals:

public slots:
    void runSynth();
    void stopSynth() { running = false; }

private:

    static int tick( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
              double streamTime, RtAudioStreamStatus status, void *dataPointer );

    RtAudio *dac;
    StkFrames *audio_data;
    bool running;
};

#endif // STKDAC_H