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
    void terminateSession();
    void interuptionProtocol();

private:
    Ui::MainWindow *ui;
    QTimer *batteryTimer;
    QTimer *displayTimer;  // Timer for updating the waveform display
    bool isDeviceOn;
    bool isSessionRunning;
    bool killTimerSent = false;
    int sessionDuration;
    int countdownTime;
    int contactLossTracker = 0; // Tracks the number of contact lost electrodes
    int totalBatteryCapacity = 1500;
    int batteryPercentage = 100;
    int batteryTime = 1000;
    int batteryCapacityTracker = totalBatteryCapacity;
    QDateTime dateTimeHolder; //holds the date and time
    QChartView *chartView;  // chart view for the sin wave graph
    QVector<Electrode> electrodes;
    QVector<SessionData> sessionsData;
    Session* currentSession;
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
    void intitializeSession();


};
#endif // MAINWINDOW_H
