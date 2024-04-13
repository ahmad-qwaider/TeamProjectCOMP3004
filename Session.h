#ifndef SESSION_H
#define SESSION_H
#include "SessionData.h"
#include <QObject>
#include <QTimer>
#include <QString>
#include <QDateTime>
#include <QDebug>
#include "Electrode.h"

enum class SessionState {
    READY,
    TREATING,
    ANALYZING,
    PAUSED,
};

class Session : public QObject {
    Q_OBJECT

public:
    Session(QDateTime currDateAndTime, int analyzingTime, int treatmentTime, QVector<Electrode> &electrodeList);
    ~Session();
    void visitNextSite(); // increment electrode
    void visitInitialSite(); // set currElec to 0 or 1 based on application logic
    QTimer* getProgressTimer();
    void initializeEventTimers();
    void endSession(); // stop and disconnect timers
    void pauseTimers(); // toggle between treatment and analyzing timers
    void resumeTimers();
    int getSessionLength();
    int getRound();
    bool isActive(); // true if state is TREATING or ANALYZING
    SessionData generateSessionData();

private:
    QDateTime dateAndTime;
    QVector<Electrode> electrodes; // Electrodes managed internally
    int currentElectrode = 0; // Set to 0 to start from the first electrode
    int round = 0;
    int pulseCount = 1; // Set to 1 assuming counting starts from 1
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

    // Timers
    QTimer* progressTimer;
    QTimer* electrodeTimer;
    QTimer* baselineTimer;

private slots:
    void treatElectrode();
    void calcBaseline();

};

#endif // SESSION_H
