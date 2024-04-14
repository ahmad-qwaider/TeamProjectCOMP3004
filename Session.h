#ifndef SESSION_H
#define SESSION_H
#include "SessionData.h"
#include <QObject>
#include <QTimer>
#include <QString>
#include <QDateTime>
#include <QDebug>
#include "Electrode.h"

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
        void visitNextSite(); // incremmnet electrode
        void visitInitialSite(); // set currElec to 1
        QTimer* getProgressTimer();
        QTimer* getElectrodeTimer();
        void initializeEventTimers();
        void endSession(); // stop and disconnect timers
        void pauseTimers(); // toggle between treatment and analyzing timers
        void resumeTimers();
        int getSessionLength();
        int getRound();
        SessionState getState();
        bool isActive(); // true if state is TREATING or ANALYZING
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
