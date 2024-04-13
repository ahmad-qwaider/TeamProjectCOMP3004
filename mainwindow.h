#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTimer>
#include <QDateTime>
#include <QtCharts>
#include "Electrode.h"
#include "SessionData.h"
#include <iostream>

using namespace QtCharts;

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
    void on_PlayButton_clicked();
    void on_PauseButton_clicked();

    void on_sessionLogButton_clicked();

    void on_UploadToPCButton_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *countdownTimer;
    QTimer *batteryTimer;
    QTimer *displayTimer;  // Timer for updating the waveform display
    bool isDeviceOn;
    bool isSessionRunning;
    int sessionDuration = 2;
    int countdownTime = sessionDuration; // Time in seconds (3 minutes)
    int contactLossTracker = 0; // Tracks the number of contact lost electrodes
    int totalBatteryCapacity = 1500;
    int batteryPercentage = 100;
    int batteryTime = 1000;
    int batteryCapacityTracker = totalBatteryCapacity;
    QDateTime dateTimeHolder; //holds the date and time entered by the use
    QChartView *chartView;  // chart view for the sin wave graph
    std::vector<Electrode> electrodes;
    std::vector<SessionData> sessionsData;
    void toggleAllElectrodes();
    void connectAllElectrodes();
    void activateElectrode(QPushButton *button);
    void deactivateElectrode(QPushButton *button);
    void toggleRedLight(bool turnON);
    void toggleBlueLight(bool turnON);
    void toggleGreenLight(bool turnON);
    void displayWaveform(int electrodeIndex);
    void startWaveformDisplay();
    void appendToSessionLogConsole();

};
#endif // MAINWINDOW_H
