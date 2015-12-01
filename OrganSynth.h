#ifndef OrganSynth_H
#define OrganSynth_H

#include <QObject>
#include <QDebug>
#include <QThread>
#include "SineWave.h"
#include "RtWvOut.h"
#include "Stk.h"
#include "OrganSynth.h"
#include "RtAudio.h"
#include "RtMidi.h"

#include "Instrmnt.h"
#include "Voicer.h"

#include "OrganRank.h"


using namespace stk;

class OrganSynth : public QObject
{
    Q_OBJECT
public:
    explicit OrganSynth(QObject *parent = 0, int midi_in_idx = 0);
    ~OrganSynth();
    void run();
    void stopDac() { running = false; }

signals:

public slots:
    void runSynth();
    void stopSynth() {
        qDebug() << "stop called\n";
        running = false;
    }
    void toggleSound();

private:

    static int tick( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
              double streamTime, RtAudioStreamStatus status, void *dataPointer );

    RtAudio *dac;
    RtMidiIn *midi_in;
    bool running;

};

#endif // OrganSynth_H
