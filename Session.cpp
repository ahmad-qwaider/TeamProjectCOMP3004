#include "Session.h"

Session::Session(QDateTime currentDateAndTime, int analyzingTime, int treatmentTime, QVector<Electrode> &electrodeList)
{
    electrodes = electrodeList;
    // time in ms
    dateAndTime = currentDateAndTime;
    analyzeTime = analyzingTime;
    therapyTime = treatmentTime;

    // 5 observation/analyzing rounds + 4 treatments
    sessionLength = (analyzeTime * 5) + (therapyTime * 4);

    // event timers
    progressTimer = new QTimer(this);
    electrodeTimer = new QTimer(this);

}
Session::~Session(){
    delete progressTimer;
    delete electrodeTimer;

}

QTimer* Session::getProgressTimer(){
    return progressTimer;
}

QTimer* Session::getElectrodeTimer(){
    return electrodeTimer;
}

void Session::initializeEventTimers(){
    connect(electrodeTimer, &QTimer::timeout, this, &Session::treatElectrode);

    connect(progressTimer, &QTimer::timeout, this, &Session::calcBaseline);

    // first state is the ANALYZING phase. this includes observation
    state = SessionState::ANALYZING;
    electrodeTimer->setInterval(therapyTime/16);
}

// stop and disconnect timer
void Session::endSession(){
    if(electrodeTimer-isActive()){
    electrodeTimer->stop();
    electrodeTimer->disconnect();
    }
}

// state to PAUSED. keep track of previous state in prePauseState so can resume accordingly
void Session::pauseTimers(){
    if(state == SessionState::ANALYZING){
        prePauseState = SessionState::ANALYZING;
        state = SessionState::PAUSED;
    }else{
        prePauseState = SessionState::TREATING;
        state = SessionState::PAUSED;
        electrodeTimer->stop();
        electrodeTimer->blockSignals(true);
    }
}

// resume timers according becased on which state was interupted by the pause
void Session::resumeTimers(){
    if(prePauseState == SessionState::ANALYZING){
        state = SessionState::ANALYZING;
    }else{
        state = SessionState::TREATING;
        electrodeTimer->blockSignals(false);
        electrodeTimer->start();
    }
}

int Session::getSessionLength(){
    return sessionLength;
}

SessionState Session::getState(){
    return state;
}

// true if state is TREATING or ANALYZING.
bool Session::isActive(){
    if(state == SessionState::TREATING || state == SessionState::ANALYZING){
        return true;
    }
    return false;
}

// put sessions data indo a SessionData struct and return
SessionData Session::generateSessionData(){
    SessionData data(dateAndTime, 0, 0);
    int initialBaseAve = 0;
    int finalBaseAve = 0;

    // take averages for the initial baselines and final baselines at each site.
    for(int i: initialBaselineLog){
        initialBaseAve += i;
    }
    for(int j: finalBaselineLog){
        finalBaseAve += j;
    }
    initialBaseAve /= 21;
    finalBaseAve /= 21;

    data.initialBaseline = initialBaseAve;
    data.finalBaseline = finalBaseAve;
    return data;
}

// treatment simulation slot (the timer for this slot goes of every 1/16th of a second
void Session::treatElectrode(){
    electrodeTimer->stop(); // pause timer
    qDebug() << "delivering pulse " << pulseCount << " of treatment";

    // iterate through all electrodes and deliver a single pulse
    for(int i = 0; i<21; i++){
        electrodes[i].treat(currentRoundFdLog.at(i), round*5);
        if(pulseCount == 16){
            electrodes[i].treatmentAffect(); // make effects of treatment show at the final pulse (16th pulse) of the treatment
        }
    }
    pulseCount++;

    // resume electrode timer loop at 16th of a second interval untill all 16 pulses delivered
    // the loop structure is a series of starts and stops until final pulse delivered
    if(pulseCount<17){
        electrodeTimer->start(therapyTime/16);
    }
    // stop loop structure
    if(pulseCount==17){
        pulseCount = 1; // reset pulse count for next treatment
        electrodeTimer->stop();
        currentRoundFdLog.clear(); //clear Fd log for next observation
        state = SessionState::ANALYZING; // state to ANALYZING so we can begin baseline observation and calculation
        }
    }

void Session::calcBaseline(){
    // the by second by second progress timer will constantly ping this, use session state if guard to block from unwanted event
    const int endPoint = (analyzeTime/1000)-1; // get the second at which we want to end.
    if(state == SessionState::ANALYZING){
        switch(baselinePhase){ // 5 arbritary baseline phase triggered.
            case 0:
                qDebug() << "beginning baseline calculations phase for all electrodes conurrently...";
                if(round!=0){
                    qDebug() << "ROUND: " << round;
                }
                break;
            case 1:
                qDebug() << "observing electrodes...";
                break;
            case 2:
                // most crucial phase at which actual observation and analyation is complete
                qDebug() << "calculating baseline for all electrodes concurrently...";
                for(int i = 0; i<21; i++){
                    QVector<int> waveInfo = electrodes.at(i).emitSignal();
                    int Fd = (waveInfo.at(0)*waveInfo.at(3)*waveInfo.at(3)+waveInfo.at(1)*waveInfo.at(4)*waveInfo.at(4)+waveInfo.at(2)*waveInfo.at(5)*waveInfo.at(5))/(waveInfo.at(3)*waveInfo.at(3)+waveInfo.at(4)*waveInfo.at(4)+
                                                                                                                                                     waveInfo.at(5)*waveInfo.at(5));
                    // append current site Fd readings so treatment can be delivered respectively
                    currentRoundFdLog.append(Fd);
                }
                break;
            case 3:
                qDebug() << "gathering results...";
                break;
            default:
                if(baselinePhase == endPoint){
                    for(int i = 0; i<21; i++){
                        qDebug() << "dominant frequency for electrod: " << i+1 << ", is " << currentRoundFdLog.at(i) << " Hz";
                    }
                }
                break;
        }
        // increment the phase
        baselinePhase ++;

        if(baselinePhase == endPoint + 1){ // final phase complete
            round ++; // increment round
            baselinePhase = 0; // reset phase tracker
            if(round == 5){ // if final round, copy site Fd's to the final round log
                finalBaselineLog = currentRoundFdLog;
                qDebug() << "therapy complete";
                state = SessionState::COMPLETE;
            }else{
                if(round == 1){
                initialBaselineLog = currentRoundFdLog;  // if first round of initial observation and analyzation , copy site Fd's to inital round log
                }
                // at end of observation/anaylyze round, swpa state and start treatment timer.
                state = SessionState::TREATING;
                qDebug() << "initiaing treatment every 16th of a second for 1 second";
                electrodeTimer->start(therapyTime/16);
            }
        }
    }

}
