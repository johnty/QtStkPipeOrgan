#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include "stkdac.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButtonSine_clicked();

    void on_pushButtonStart_clicked();

    void on_pushButtonStop_clicked();

signals:
    void stopSignal();

private:
    Ui::MainWindow *ui;
    StkDac *my_dac;
    QThread* synth_thread;
};

#endif // MAINWINDOW_H
