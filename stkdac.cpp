#include "stkdac.h"
#include <QDebug>

SineWave *sine_test;

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

    dac = NULL;

    try {
        // Define and open the default realtime output device for one-channel playback
        dac = new RtAudio();
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
    qDebug() << "Starting RtAudio Stream...\n";
    dac->startStream();

}

int StkDac::tick(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *dataPointer)
{
//    qDebug() <<"tick tock\n";

    register StkFloat *samples = (StkFloat*) outputBuffer;
    for (unsigned int i=0; i<nBufferFrames; i++)
    {
        StkFloat val = sine_test->tick();
        samples[2*i]=val;
        samples[2*i+1] = val;
        //*samples++ = sine_test->tick();
    }
    return 0;
}

void StkDac::runSynth()
{
    running = true;
    qDebug() << "dac thread started\n";
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