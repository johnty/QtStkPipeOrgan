#include "mainwindow.h"
#include "ui_mainwindow.h"


#include "SineWave.h"
#include "RtWvOut.h"
#include "StkDac.h"

#include <cstdlib>
#include <QDebug>


using namespace stk;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    my_dac = NULL;
    synth_thread = NULL;
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonSine_clicked()
{

    return;

    // Set the global sample rate before creating class instances.
    Stk::setSampleRate( 44100.0 );
    Stk::showWarnings( true );
    qDebug() << Stk::sampleRate();

    int nFrames = 100000;
    SineWave sine;
    RtWvOut *dac = 0;

    try {
        // Define and open the default realtime output device for one-channel playback
        dac = new RtWvOut( 1 );
    }
    catch ( StkError & ) {
        exit( 1 );
    }

    sine.setFrequency( 441.0 );

    // Option 1: Use StkFrames

    StkFrames frames( nFrames, 1 );
    try {
        dac->tick( sine.tick( frames ) );
    }
    catch ( StkError & ) {
        goto cleanup;
    }


    // Option 2: Single-sample computations
    //      for ( int i=0; i<nFrames; i++ ) {
    //        try {
    //          dac->tick( sine.tick() );
    //         }
    //        catch ( StkError & ) {
    //          goto cleanup;
    //        }
    //      }

cleanup:
    delete dac;

}

void MainWindow::on_pushButtonStart_clicked()
{
    synth_thread = new QThread;
    my_dac = new StkDac();
    my_dac->moveToThread(synth_thread);

    //connect slots
    connect(synth_thread, SIGNAL(started()), my_dac, SLOT(runSynth()));
    connect(synth_thread, SIGNAL(finished()), my_dac, SLOT(stopSynth()));

    synth_thread->start();
}

void MainWindow::on_pushButtonStop_clicked()
{
    qDebug() <<"calling exit on thread...\n";
    synth_thread->currentThread()->quit();
    while (!synth_thread->isFinished()) {}
    qDebug() <<"thread stopped\n";


}
