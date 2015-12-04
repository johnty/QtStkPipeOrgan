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

    initDacThread();

    ui->checkBoxRank0->setEnabled(false);
    ui->checkBoxRank1->setEnabled(false);
    ui->checkBoxRank2->setEnabled(false);
    ui->checkBoxRank3->setEnabled(false);

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
    int prevIndex = ui->comboBoxMidiIn->currentIndex();
    qDebug()<<"prevIndex = " << prevIndex;
    ui->comboBoxMidiIn->clear();

    QStringList list = my_organsynth->getMidiPorts();
    if (list.isEmpty())
        ui->comboBoxMidiIn->setDisabled(true);
    else {
        for (QStringList::iterator it = list.begin(); it != list.end(); ++it)
        {
            ui->comboBoxMidiIn->addItem(*it);
        }

        if (prevIndex != -1) //restore previous selection, if it exists
            ui->comboBoxMidiIn->setCurrentIndex(prevIndex);
    }
}

void MainWindow::on_pushButtonRefreshMidi_clicked()
{
    refreshMidi();
}

void MainWindow::on_StopToggled(int stop, bool active)
{
    qDebug() <<"Stop # " <<stop <<" active = " <<active;
    switch (stop) {
    case 0:
        ui->checkBoxRank0->setChecked(active);
        break;
    case 1:
        ui->checkBoxRank1->setChecked(active);
        break;
    case 2:
        ui->checkBoxRank2->setChecked(active);
        break;
    case 3:
        ui->checkBoxRank3->setChecked(active);
        break;
    default:
        break;
    }
}

void MainWindow::initDacThread()
{
    if (my_organsynth) //stop if running
        stopDacProc();

    if (synth_thread)
        delete synth_thread;
    synth_thread = 0;
    synth_thread = new QThread;

    if (my_organsynth) //TODO get rid of this
    {
        delete my_organsynth;
        my_organsynth = 0;
    }

    int port_index = ui->comboBoxMidiIn->currentIndex();
    if (port_index < 0) //in case something weird happened...
        port_index = 0;
    my_organsynth = new OrganSynth(0);
    my_organsynth->moveToThread(synth_thread);


    //connect slots

    //connect(my_dac, SIGNAL(testFromDac()), this, SLOT(fromDac()));

    //this launches the runSynth routine when thread is started
    connect(synth_thread, SIGNAL(started()), my_organsynth, SLOT(runSynth()));

    //NOTE: the thread cannot finish until the worker is done,
    // so this call below is useless!!
    connect(synth_thread, SIGNAL(finished()), my_organsynth, SLOT(stopSynth()));
    //connect(this, SIGNAL(toDac(int)), my_organsynth, SLOT(stopSynth()));
    //connect(this, SIGNAL(toDac(int)), my_dac, SLOT(toggleSine()));

    //from synth->UI, for stop toggle display
    connect(my_organsynth, SIGNAL(sendStopSig(int,bool)), this, SLOT(on_StopToggled(int,bool)));

    //port changing signal/slot

    connect(this, SIGNAL(ChangePort(int)), my_organsynth, SLOT(selectMidiPort(int)));

    synth_thread->start();
}

void MainWindow::on_comboBoxMidiIn_currentIndexChanged(int index)
{
    qDebug()<<"changing port to " << index;
    emit ChangePort(index);
}
