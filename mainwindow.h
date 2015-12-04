#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include "OrganSynth.h"


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
    void on_StopToggled(int stop, bool active);

private slots:
    void on_pushButtonSine_clicked();
    void on_pushButtonRefreshMidi_clicked();

    void on_comboBoxMidiIn_currentIndexChanged(int index);

signals:
    void stopSignal();
    void toDac(int val);
    void ChangePort(int port);

private:

    void initDacThread();
    void stopDacProc();

    void refreshMidi();

    Ui::MainWindow *ui;
    OrganSynth *my_organsynth;
    QThread* synth_thread;
};

#endif // MAINWINDOW_H
