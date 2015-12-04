#include "OrganSynth.h"
#include <QDebug>
#include <math.h>

static StkFrames audio_data0;
static StkFrames audio_data1;
static StkFrames audio_data2;
static StkFrames audio_data3;
static OrganRank rank0;
static OrganRank rank1;
static OrganRank rank2;
static OrganRank rank3;

float testFreq = 440.0;

OrganSynth::OrganSynth(QObject *parent) : QObject(parent)
{

    initMidi();

    // Set the global sample rate before creating class instances.
    Stk::setSampleRate( 44100.0 );
    Stk::showWarnings( true );
    qDebug() << Stk::sampleRate();

    audio_data0.resize(512);
    audio_data1.resize(512);
    audio_data2.resize(512);
    audio_data3.resize(512);

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

OrganSynth::~OrganSynth()
{
    if (midi_in)
    {
        midi_in->closePort();
        delete midi_in;
    }
}

void OrganSynth::initMidi()
{
    //Make MIDI port
    midi_in = new RtMidiIn();
    // Check available ports.
    unsigned int nPorts = midi_in->getPortCount();
    if ( nPorts == 0 )
    {
        std::cout << "No ports available!\n";
    }

    for (int i=0; i<midi_in->getPortCount(); i++)
    {
        qDebug() << "port"<<i<<":"<< midi_in->getPortName().c_str();
    }

    //MIDI INPUT Handling:
    // In Windows, we have no choice but to expect at least 1 port
    // can use loopMIDI to create a virtual port
    // called "OrganSynthIn" or something

#if (defined(__OS_WIN32__))
    midi_in->openPort(0, "OrganSynthIn");
#else
    //linux/mac have option to open virtal port with RtMidi:
    // lets do that if we don't have any existing ones
    if (midi_in->getPortCount() == 0)
    {
        qDebug() <<"creating virtual midi port\n";
        midi_in->openVirtualPort("OrganSynthIn2");
        //midi_in->openPort(midi_in_idx, "OrganSynthIn");
    }
#endif
    // Don't ignore sysex, timing, or active sensing messages.
    midi_in->ignoreTypes( false, false, false );

}

int OrganSynth::selectMidiPort(int port)
{
    qDebug() <<"Synth received changeport = " <<port;
    if (midi_in)
    {
        if (midi_in->getPortCount() >= port) //valid selection
        {
            if (midi_in->isPortOpen()) //close port first
                midi_in->closePort();
            midi_in->openPort(port);
            return 0;
        }
        else
            return -1;
    }
    else
        return -1;
}

int OrganSynth::tick(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *dataPointer)
{
    register StkFloat *samples = (StkFloat*) outputBuffer;
    rank0.tick(audio_data0);
    rank1.tick(audio_data1);
    rank2.tick(audio_data2);
    rank3.tick(audio_data3);
    for (unsigned int i=0; i<nBufferFrames; i++)
    {
        samples[2*i] = audio_data0[i] + audio_data1[i] + audio_data2[i] + audio_data3[i];
        samples[2*i+1] = audio_data0[i] + audio_data1[i] + audio_data2[i] + audio_data3[i];
    }
    return 0;
}

void OrganSynth::runSynth()
{
    running = true;
    qDebug() << "dac runSynth:";
    qDebug() << "Starting RtAudio Stream...\n";
    dac->startStream();


    //MIDI data
    std::vector<unsigned char> message;
    int nBytes, i;
    double stamp;

    const int poly = 16;
    OrganFlue* fluePipes[poly];
    BlowBotl* btlPipes[poly];
    Clarinet* clarPipes[poly];
    Saxofony* saxPipes[poly];

    for (int i=0; i<poly; i++)
    {
        fluePipes[i] = new OrganFlue();
        btlPipes[i] = new BlowBotl();
        clarPipes[i] = new Clarinet();
        saxPipes[i] = new Saxofony(16.0);

        rank0.addInstrument(fluePipes[i]);
        rank1.addInstrument(btlPipes[i]);
        rank2.addInstrument(clarPipes[i]);
        rank3.addInstrument(saxPipes[i]);
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
                        testFreq = 440.0 * pow(2.0, (note-69)/12.0);

                        if (rank_active[0])
                            rank0.noteOn(note, 64.0, 0);
                        if (rank_active[1])
                            rank1.noteOn(note-12, 64.0, 0);
                        if (rank_active[2])
                            rank2.noteOn(note, 64.0, 0);
                        if (rank_active[3])
                            rank3.noteOn(note+12, 64.0, 0);

                        //extra case: MAX/MSP's "makenote" doesn't send note-off, but a note-on with vel=0
                        if (message[2] == 0)
                        {
                            if (rank_active[0])
                                rank0.noteOff(note, 64.0, 0);
                            if (rank_active[1])
                                rank1.noteOff(note-12, 64.0, 0);
                            if (rank_active[2])
                                rank2.noteOff(note, 64.0, 0);
                            if (rank_active[3])
                                rank3.noteOff(note+12, 64.0, 0);
                        }

                    }
                    if ((message[0]>>4 == 8)) //note off
                    {
                        if (rank_active[0])
                            rank0.noteOff(note, 64.0, 0);
                        if (rank_active[1])
                            rank1.noteOff(note-12, 64.0, 0);
                        if (rank_active[2])
                            rank2.noteOff(note, 64.0, 0);
                        if (rank_active[3])
                            rank3.noteOff(note+12, 64.0, 0);
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

        rank1.removeInstrument(btlPipes[i]);
        delete btlPipes[i];

        rank2.removeInstrument(clarPipes[i]);
        delete clarPipes[i];

        rank3.removeInstrument(saxPipes[i]);
        delete saxPipes[i];
    }

    dac->stopStream();
    dac->closeStream();
    qDebug() << "   runSynth dac routine stopped\n";
}

void OrganSynth::toggleSound()
{

}

void OrganSynth::toggleStop(int rank_number)
{
    rank_active[rank_number] = !rank_active[rank_number];
    //qDebug() <<"orgSynth: rank " <<rank_number<<" active = " << rank_active[rank_number];
    emit sendStopSig(rank_number, rank_active[rank_number]);
}

QStringList OrganSynth::getMidiPorts()
{
    QStringList list;
    for (int i=0; i<midi_in->getPortCount(); i++)
    {
        list.append(midi_in->getPortName(i).c_str());
    }
    return list;
}
