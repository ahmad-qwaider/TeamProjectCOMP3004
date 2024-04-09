#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include "Electrode.h"
#include <iostream>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_powerButton_clicked();
    void onMultipleButtonsClicked();



private:
    Ui::MainWindow *ui;
    bool isDeviceOn;
    std::vector<Electrode> electrodes;
    void toggleAllElectrodes();
    void connectAllElectrodes();
    void activateElectrode(QPushButton *button);
    void deactivateElectrode(QPushButton *button);

};
#endif // MAINWINDOW_H
