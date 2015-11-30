#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include "OrganSynth.h"
#include "RtMidi.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:

private slots:
    void on_pushButtonSine_clicked();

    void on_pushButtonStart_clicked();

    void on_pushButtonStop_clicked();

    void on_pushButtonRefreshMidi_clicked();

signals:
    void stopSignal();
    void toDac(int val);

private:

    void stopDacProc();

    void refreshMidi();

    Ui::MainWindow *ui;
    OrganSynth *my_organsynth;
    QThread* synth_thread;
};

#endif // MAINWINDOW_H
