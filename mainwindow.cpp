#include "mainwindow.h"
#include "ui_mainwindow.h"


#include "SineWave.h"
#include "RtWvOut.h"
#include "OrganSynth.h"
#include <cstdlib>
#include <QDebug>


using namespace stk;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    my_organsynth = NULL;
    synth_thread = NULL;
    ui->setupUi(this);


    refreshMidi();
}

MainWindow::~MainWindow()
{
    stopDacProc();
    delete ui;
}

void MainWindow::on_pushButtonSine_clicked()
{
    my_organsynth->toggleSound();
    return;
}

void MainWindow::on_pushButtonStart_clicked()
{
    if (synth_thread)
        delete synth_thread;
    synth_thread = 0;
    synth_thread = new QThread;

    if (my_organsynth) //TODO get rid of this
        delete my_organsynth;
    my_organsynth = 0;

    my_organsynth = new OrganSynth();
    my_organsynth->moveToThread(synth_thread);

    //connect slots

    //connect(my_dac, SIGNAL(testFromDac()), this, SLOT(fromDac()));

    connect(synth_thread, SIGNAL(started()), my_organsynth, SLOT(runSynth()));

    //NOTE: the thread cannot finish until the worker is done,
    // so this call below is useless!!
    connect(synth_thread, SIGNAL(finished()), my_organsynth, SLOT(stopSynth()));
    connect(this, SIGNAL(toDac(int)), my_organsynth, SLOT(stopSynth()));
    //connect(this, SIGNAL(toDac(int)), my_dac, SLOT(toggleSine()));

    synth_thread->start();
}

void MainWindow::on_pushButtonStop_clicked()
{
    stopDacProc();

}

void MainWindow::stopDacProc()
{
    if (my_organsynth)
    {
        qDebug() <<"stopping dac worker\n";
        my_organsynth->stopDac();
        qDebug() <<"calling exit on thread...\n";
        synth_thread->exit();
        synth_thread->wait();
        qDebug() <<"threads exited cleanly.\n";
    }
}

void MainWindow::refreshMidi()
{
    ui->comboBoxMidiIn->clear();
    RtMidiIn midiin;
    for (int i=0; i<midiin.getPortCount(); i++)
    {
        ui->comboBoxMidiIn->addItem(midiin.getPortName(i).c_str());
    }
    if (midiin.getPortCount()==0)
        ui->comboBoxMidiIn->addItem("N/A");
}

void MainWindow::on_pushButtonRefreshMidi_clicked()
{

}
