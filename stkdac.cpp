#include "stkdac.h"
#include <QDebug>
#include <math.h>

SineWave *sine_test;
SineWave *sine2_test;

#define NUM_SINES 512

SineWave *sines[NUM_SINES];
StkFloat norm = (float)0.5/NUM_SINES;

bool isSine;

StkDac::StkDac(QObject *parent) : QObject(parent)
{
    // Set the global sample rate before creating class instances.
    Stk::setSampleRate( 44100.0 );
    Stk::showWarnings( true );
    qDebug() << Stk::sampleRate();

    audio_data = new StkFrames(512,1);

    //sine test
    sine_test = new SineWave();
    sine_test->setFrequency(440.0);
    sine2_test = new SineWave();
    sine2_test->setFrequency(660.0);
    isSine = true;\

    qDebug() <<"norm = " << norm <<"\n";

    for (int i=0; i<NUM_SINES; i++)
    {
        sines[i] = new SineWave();

        float freq = 440.0*pow(2.0, (double)(i%12)/12.0);


        sines[i]->setFrequency(freq);
        //qDebug() << "freq " <<i<<" = " << freq<<"\n";
    }

    dac = NULL;

    try {
        // Define and open the default realtime output device for one-channel playback
#if (defined(__OS_WIN32__))
        dac = new RtAudio();
#endif
#if (defined(__OS_MACOSX__))
        dac = new RtAudio(RtAudio::MACOSX_CORE);
#endif

#if defined(__MACOSX_CORE___)
        adddd;
#endif

        qDebug() << "numDevs = " << dac->getDeviceCount();
    }


    catch ( RtAudioError & ) {
        exit( 1 );
    }
    qDebug() << "Successfully initialized RTaudio dac!\n";

    RtAudioFormat format = ( sizeof(StkFloat) == 8 ) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
    RtAudio::StreamParameters parameters;
    parameters.deviceId = dac->getDefaultOutputDevice();
    parameters.nChannels = 2;
    unsigned int bufferFrames = RT_BUFFER_SIZE;
    try {
        dac->openStream( &parameters, NULL, format, (unsigned int)Stk::sampleRate(), &bufferFrames, &tick );
    }
    catch ( RtAudioError& error ) {
        error.printMessage();
    }

}

int StkDac::tick(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *dataPointer)
{
//    qDebug() <<"tick tock\n";
    register StkFloat *samples = (StkFloat*) outputBuffer;
    for (unsigned int i=0; i<nBufferFrames; i++)
    {
        samples[2*i] = 0.0;
        samples[2*i+1] = 0.0;
        StkFloat val;

        for (int j=0; j<NUM_SINES; j++)
        {
            val = sines[j]->tick();
            samples[2*i]+=val*norm;
            samples[2*i+1] += val*norm;
        }

        /*
        StkFloat val1 = sine_test->tick();
        StkFloat val2 = sine2_test->tick();

        if (isSine)
        {
            samples[2*i]=val1;
            samples[2*i+1] = val2;
            //*samples++ = sine_test->tick();
        }
        else
        {
            samples[2*i]=0.0;
            samples[2*i+1]=0.0;
        }
        */

    }
    return 0;
}

void StkDac::runSynth()
{
    running = true;
    qDebug() << "dac thread started\n";
    qDebug() << "Starting RtAudio Stream...\n";
    dac->startStream();

    unsigned int i=0;
    while (running)
    {

        try {

        }
        catch ( StkError & ) {

        }
    }
    dac->stopStream();
    qDebug() << " run stopped\n";
    if (audio_data)
        delete audio_data;
    if (sine_test)
        delete sine_test;
}

void StkDac::toggleSine()
{
    isSine = !isSine;
}
