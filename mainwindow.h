#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTimer>
#include <QDateTime>
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
    void on_newSessionButton_clicked();
    void updateCountdown();
    void on_TimeAndDateButton_clicked();
    void updateBatteryCapacity();
    void on_EnterTimeButton_clicked();
    void on_MenuButton_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *countdownTimer;
    QTimer *batteryTimer;
    bool isDeviceOn;
    int countdownTime = 180; // Time in seconds (3 minutes)
    int contactLossTracker = 0; // Tracks the number of contact lost electrodes
    int totalBatteryCapacity = 1500;
    int batteryPercentage = 100;
    int batteryCapacityTracker = totalBatteryCapacity;
    QDateTime dateTimeHolder; //holds the date and time entered by the use
    std::vector<Electrode> electrodes;
    void toggleAllElectrodes();
    void connectAllElectrodes();
    void activateElectrode(QPushButton *button);
    void deactivateElectrode(QPushButton *button);
    void toggleRedLight(bool turnON);
    void toggleBlueLight(bool turnON);
    void toggleGreenLight(bool turnON);
};
#endif // MAINWINDOW_H
