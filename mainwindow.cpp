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
    my_dac = NULL;
    synth_thread = NULL;
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    stopDacProc();
    delete ui;
}

void MainWindow::on_pushButtonSine_clicked()
{
    my_dac->toggleSound();
    return;
}

void MainWindow::on_pushButtonStart_clicked()
{
    if (synth_thread)
        delete synth_thread;
    synth_thread = 0;
    synth_thread = new QThread;

    if (my_dac)
        delete my_dac;
    my_dac = 0;

    my_dac = new StkDac();
    my_dac->moveToThread(synth_thread);

    //connect slots

    //connect(my_dac, SIGNAL(testFromDac()), this, SLOT(fromDac()));

    connect(synth_thread, SIGNAL(started()), my_dac, SLOT(runSynth()));

    //NOTE: the thread cannot finish until the worker is done,
    // so this call below is useless!!
    connect(synth_thread, SIGNAL(finished()), my_dac, SLOT(stopSynth()));
    connect(this, SIGNAL(toDac(int)), my_dac, SLOT(stopSynth()));
    //connect(this, SIGNAL(toDac(int)), my_dac, SLOT(toggleSine()));

    synth_thread->start();
}

void MainWindow::on_pushButtonStop_clicked()
{
    stopDacProc();

}

void MainWindow::stopDacProc()
{
    qDebug() <<"stopping dac worker\n";
    my_dac->stopDac();
    qDebug() <<"calling exit on thread...\n";
    synth_thread->exit();
    synth_thread->wait();
    qDebug() <<"threads exited cleanly.\n";
}
