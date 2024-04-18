#ifndef SESSION_H
#define SESSION_H
#include "SessionData.h"
#include <QObject>
#include <QTimer>
#include <QString>
#include <QDateTime>
#include <QDebug>
#include "Electrode.h"
/* Purpose of Class: To act as a running session object
 * QDateTime dateAndTime - date and time object to put time stamp on session
 * QVector<Electrode> electrodes - QVector of electrodes
 * Session state defining elements:
 * int currentElectrode - track of current electrode position 1-21
 * int round - track current round, 0 indicates initial baseline round, 4 indicates final baseline round
 * int pulseCount - track current pulse of 1/16th of second treatment waves 1-16
 * int baselinePhase - track current phase of the baseline treament, helps with simulation.
 * int analyzeTime - time to calc baseline/dominate frequency
 * int therapyTime - time for treatment phase
 * int sessionLength - total session length
 * int baselineFrequency;
 * SessionState state - session state
 * SessionState prePauseState - session state prior to a pause
 *
 * Logs:
 * QVector<int> initialBaselineLog;
 * QVector<int> finalBaselineLog;
 * QVector<int> currentRoundFdLog;
 * QVector<int> waveInfo;
 *
 * Timers :
 * QTimer* progressTimer - timer for over porgress, syncs up with baseline events
 * QTimer* electrodeTimer - timer for treatment delivery
 *
 * Class Functions:
 * void treatElectrode() - simulate treatment phase (slot)
 * void calcBaseline() - simulate baseline calc phase(observing and analyzing)
 * void initializeEventTimers() - initialzed/connect timer
 * void endSession() - end session, stop and disconnect timer
 * void pauseTimers() - pause timer if its on, switch state
 * void resumeTimers() - resume timer, switch state
 * bool isActive() - return true if state is TREATING or ANALYZING
 * SessionData generateSessionData() - generate a SessionData object with this sessions information and return it
 * Getters and Setters
 */
enum class SessionState{
    READY,
    TREATING,
    ANALYZING,
    PAUSED,
    COMPLETE
};

class Session : public QObject{
    Q_OBJECT

    public:
        Session(QDateTime currDateAndTime, int analyzingTime, int treatmentTime, QVector<Electrode> &electrodeList);
        ~Session();
        void initializeEventTimers();
        void endSession();
        void pauseTimers();
        void resumeTimers();
        int getSessionLength();
        SessionState getState();
        bool isActive();
        QTimer* getProgressTimer();
        QTimer* getElectrodeTimer();
        SessionData generateSessionData();

    private:
        QDateTime dateAndTime;
        QVector<Electrode> electrodes;
        // session state elements
        int currentElectrode = 1;
        int round = 0;
        int pulseCount = 1;
        int baselinePhase = 0;
        int analyzeTime;
        int therapyTime;
        int sessionLength;
        int baselineFrequency;
        SessionState state = SessionState::READY;
        SessionState prePauseState;

        // Logs
        QVector<int> initialBaselineLog;
        QVector<int> finalBaselineLog;
        QVector<int> currentRoundFdLog;
        QVector<int> waveInfo;

        // Timers
        QTimer* progressTimer;
        QTimer* electrodeTimer;

    private slots:
        void treatElectrode();
        void calcBaseline();

};

#endif // SESSION_H
