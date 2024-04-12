#ifndef SESSION_H
#define SESSION_H
#include "SessionData.h"
#include <QObject>
#include <QTimer>
#include <QString>
#include <QDateTime>
#include <QDebug>
#include "electrode.h"
enum class SessionState{
    READY,
    TREATING,
    ANALYZING,
    PAUSED,
};

class Session : public QObject
{
    Q_OBJECT

    public:
        Session(QDateTime currDateAndTime, int analyzingTime, int treatmentTime);
        ~Session();
        void visitNextSite(); // incremmnet electrode
        void visitInitialSite(); // set currElec to 0
        QTimer* getProgressTimer();
        void initializeEventTimers(QVector<Electrode> &electrodes);
        void endSession(); // stop and disconnect timers
        void pauseTimers(); // toggle between treatment and analyzing timers
        void resumeTimers();
        int getSessionLength();
        int getRound();
        bool isActive(); // true if state is TREATING or ANALYZING
        SessionData generateSessionData();



    private:
        QDateTime dateAndTime;
        // session state elements
        int currentElectrode = 0;
        int round = 0;
        int pulseCount = -1;
        int analyzeTime;
        int therapyTime;
        int sessionLength;
        int baselineFrequency;
        SessionState state = SessionState::READY;
        SessionState prePauseState;

        // Logs
        QVector<int> intialBaselineLog;
        QVector<int> finalBaselineLog;
        QVector<int> currentRoundFdLog;

        // Timers
        QTimer* progressTimer;
        QTimer* electrodeTimer;
        QTimer* baselineTimer;


    private slots:
        void treatElectrode(QVector<Electrode> &electrodes);
        void calcBaseline(QVector<Electrode> &electrodes);



};

#endif // SESSION_H
