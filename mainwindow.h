#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTimer>
#include <QDateTime>
#include <QtCharts>
#include <QVector>
#include "Electrode.h"
#include "SessionData.h"
#include "Session.h"
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
    void terminateSession(bool turnOfDevice);
    void interuptionProtocol();
    void updateDisplayWave();
    void on_StopButton_clicked();
    void on_SessionLogButtonPC_clicked();
    void on_GraphButtonPC_clicked();


    void on_BackButton_2_clicked();

    void on_BackButton_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *batteryTimer;
    QTimer *displayTimer;  // Timer for updating the waveform display
    bool isDeviceOn;
    bool isSessionRunning;
    bool killTimerSent = false;
    bool blinkingImmitation = false;
    int sessionDuration;
    int countdownTime;
    int contactLossTracker = 0; // Tracks the number of contact lost electrodes
    int completedSessionsCount =  0;
    bool batteryWarningSent = false;
    int totalBatteryCapacity = 320; // capacity is to be set at roughly 3*session length*3 + 10 this is because battery rate is x3 during session and we want 3 sessions
    int noBattery = false;
    int batteryPercentage = 100;
    int batteryTime = 1000;
    int batteryCapacityTracker = totalBatteryCapacity;
    QDateTime dateTimeHolder; //holds the date and time
    QChartView *chartView;  // chart view for the sin wave graph
    QVector<Electrode> electrodes;
    QVector<SessionData> sessionsData;
    Session* currentSession;
    void toggleAllElectrodes(bool activate);
    void connectAllElectrodes();
    void activateElectrode(QPushButton *button);
    void deactivateElectrode(QPushButton *button);
    void toggleRedLight(bool turnON);
    void toggleBlueLight(bool turnON);
    void toggleGreenLight(bool turnON);
    void GreenlightBlinking();
    void displayWaveform(int electrodeIndex);
    void appendToSessionLogConsole();
    void intitializeSession();
    void showLowBatteryScreen();
    void showMainScreen();


};
#endif // MAINWINDOW_H
