#include "OrganSynth.h"
#include <QDebug>
#include <math.h>

SineWave *sine_test;
SineWave *sine2_test;

#define NUM_SINES 32
#define testSine FALSE

SineWave *sines[NUM_SINES];
StkFloat norm = (float)0.5/NUM_SINES;

static StkFrames audio_data0;
static StkFrames audio_data1;
static OrganRank rank0;
static OrganRank rank1;
static OrganRank rank2;
static OrganRank rank3;

float testFreq = 440.0;

bool isSine;

OrganSynth::OrganSynth(QObject *parent, int midi_in_idx) : QObject(parent)
{

    qDebug()<< "launching synth with midi input " << midi_in_idx;
    // Set the global sample rate before creating class instances.
    Stk::setSampleRate( 44100.0 );
    Stk::showWarnings( true );
    qDebug() << Stk::sampleRate();

    audio_data0.resize(512);
    audio_data1.resize(512);

    //sine test

    qDebug() <<"norm = " << norm <<"num sines = " << NUM_SINES << "\n";

    for (int i=0; i<NUM_SINES; i++)
    {
        sines[i] = new SineWave();

        float freq = testFreq*pow(2.0, (double)(i%12)/12.0);
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
#if (defined(__OS_LINUX__))
        dac = new RtAudio(RtAudio::LINUX_ALSA);
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

    //Make MIDI port
    midi_in = new RtMidiIn();
    // Check available ports.
    unsigned int nPorts = midi_in->getPortCount();
    if ( nPorts == 0 )
    {
        std::cout << "No ports available!\n";
        delete midi_in;
        midi_in = NULL;

    }
    else
    {
        for (int i=0; i<midi_in->getPortCount(); i++)
        {
            qDebug() << "port"<<i<<":"<< midi_in->getPortName().c_str();
        }

#if (defined(__OS_WIN32__))
        //expect a loopMidi/virtual port, index 0 (name optional)
        midi_in->openPort(midi_in_idx, "OrganSynthIn");
#else
        //linux/mac have option to open virtal port:
        //midi_in->openVirtualPort("OrganSynthIn");

        midi_in->openPort(midi_in_idx, "OrganSynthIn");
#endif
        // Don't ignore sysex, timing, or active sensing messages.
        midi_in->ignoreTypes( false, false, false );
    }

}

OrganSynth::~OrganSynth()
{
    if (midi_in)
    {
        midi_in->closePort();
        delete midi_in;
    }
}

int OrganSynth::tick(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *dataPointer)
{
    register StkFloat *samples = (StkFloat*) outputBuffer;
    rank0.tick(audio_data0);
    rank1.tick(audio_data1);
    for (unsigned int i=0; i<nBufferFrames; i++)
    {
        samples[2*i] = audio_data0[i] + audio_data1[i];
        samples[2*i+1] = audio_data0[i] + audio_data1[i];
        //        samples[2*i] = 0.0;
        //        samples[2*i+1] = 0.0;
        //        if (isSine)
        //        {

        //            StkFloat val;

        //            for (int j=0; j<NUM_SINES; j++)
        //            {
        //                //sines[j]->setFrequency(testFreq*pow(2.0, (double)(i%12)/12.0));
        //                sines[j]->setFrequency(testFreq);
        //                val = sines[j]->tick();
        //                samples[2*i]+=val*norm;
        //                samples[2*i+1] += val*norm;
        //            }
        //        }
    }
    return 0;
}

void OrganSynth::runSynth()
{
    running = true;
    qDebug() << "dac thread started\n";
    qDebug() << "Starting RtAudio Stream...\n";
    dac->startStream();


    //MIDI data
    std::vector<unsigned char> message;
    int nBytes, i;
    double stamp;

    int noteStack = 0;

    int poly = 4;
    OrganFlue* fluePipes[poly];
    BlowBotl* btlPipes[poly];
    for (int i=0; i<poly; i++)
    {
        fluePipes[i] = new OrganFlue();
        btlPipes[i] = new BlowBotl();
        rank0.addInstrument(fluePipes[i]);
        rank1.addInstrument(btlPipes[i]);
        rank_active[i] = false;
    }





    while (running)
    {
        QApplication::processEvents();

        try {

            //read MIDI in
            if (midi_in)
            {
                stamp = midi_in->getMessage( &message );
                nBytes = message.size();
                for ( i=0; i<nBytes; i++ )
                    qDebug() << "Byte " << i << " = " << (int)message[i] << ", ";
                if ( nBytes > 0 )
                    qDebug() << "stamp = " << stamp << "\n";
                if (nBytes == 3) //possibly a note on
                {
                    int val = message[0]>>4 ;
                    //qDebug()<< "m>>4 = " << val <<"\n";
                    //1000 nnnn = note off (nnnn = channel #, 0 indexed)
                    //1001 nnnn = note on
                    int note = message[1];
                    if ((message[0]>>4 == 9)) //note on
                    {
                        noteStack++;
                        testFreq = 440.0 * pow(2.0, (note-69)/12.0);

                        if (rank_active[0])
                            rank0.noteOn(note, 1.0, 0);
                        if (rank_active[1])
                            rank1.noteOn(note, 1.0, 0);
                    }
                    if ((message[0]>>4 == 8)) //note off
                    {
                        if (rank_active[0])
                            rank0.noteOff(note, 1.0, 0);
                        if (rank_active[1])
                            rank1.noteOff(note, 1.0, 0);
                    }

                }
                if (nBytes == 2)
                {
                    if ((message[0]>>4 == 12)) //program change
                    {
                        int prg =message[1];
                        toggleStop(prg);
                    }
                }


            }

        }
        catch ( StkError & ) {
        }
    }

    //deallocate voices

    for (int i=0; i<poly; i++)
    {
        rank0.removeInstrument(fluePipes[i]);
        delete fluePipes[i];
    }



    dac->stopStream();
    dac->closeStream();
    qDebug() << " run stopped\n";
    if (sine_test)
        delete sine_test;

    for (int i=0; i<NUM_SINES; i++)
    {
        delete sines[i];
        sines[i] = NULL;
    }

}

void OrganSynth::toggleSound()
{
    isSine = !isSine;
}

void OrganSynth::toggleStop(int rank_number)
{
    rank_active[rank_number] = !rank_active[rank_number];
    //qDebug() <<"orgSynth: rank " <<rank_number<<" active = " << rank_active[rank_number];
    emit sendStopSig(rank_number, rank_active[rank_number]);
}
