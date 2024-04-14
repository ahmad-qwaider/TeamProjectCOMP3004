#include "Session.h"

Session::Session(QDateTime currentDateAndTime, int analyzingTime, int treatmentTime, QVector<Electrode> &electrodeList)
{
    electrodes = electrodeList;
    dateAndTime = currentDateAndTime;
    analyzeTime = analyzingTime;
    therapyTime = treatmentTime;

    sessionLength = (analyzeTime * 5) + (therapyTime * 4);

    progressTimer = new QTimer(this);
    electrodeTimer = new QTimer(this);

}
Session::~Session(){
    delete progressTimer;
    delete electrodeTimer;

}

void Session::visitNextSite(){
    currentElectrode ++;
} // incremmnet electrode

void Session::visitInitialSite(){
    currentElectrode = 1;
} // set currElec to 0

QTimer* Session::getProgressTimer(){
    return progressTimer;
}

QTimer* Session::getElectrodeTimer(){
    return electrodeTimer;
}

void Session::initializeEventTimers(){
    connect(electrodeTimer, &QTimer::timeout, this, &Session::treatElectrode);

    connect(progressTimer, &QTimer::timeout, this, &Session::calcBaseline);

    state = SessionState::ANALYZING;
    electrodeTimer->setInterval(therapyTime/16);
}

void Session::endSession(){
    if(electrodeTimer-isActive()){
    electrodeTimer->stop();
    electrodeTimer->disconnect();
    }
} // stop and disconnect timers

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
} //pauses timer stops the currently active timer dependent on the state and set state to paused

void Session::resumeTimers(){
    if(prePauseState == SessionState::ANALYZING){
        state = SessionState::ANALYZING;
    }else{
        state = SessionState::TREATING;
        electrodeTimer->blockSignals(false);
        electrodeTimer->start();
    }
} //resumes the most recently active state

int Session::getSessionLength(){
    return sessionLength;
}

int Session::getRound(){
    return round;
}

SessionState Session::getState(){
    return state;
}

bool Session::isActive(){
    if(state == SessionState::TREATING || state == SessionState::ANALYZING){
        return true;
    }
    return false;
} // true if state is TREATING or ANALYZING

SessionData Session::generateSessionData(){
    SessionData data(dateAndTime, 0, 0);
    int initialBaseAve = 0;
    int finalBaseAve = 0;

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


void Session::treatElectrode(){
    electrodeTimer->stop();
    qDebug() << "delivering pulse " << pulseCount << " of treatment";
    for(int i = 0; i<21; i++){
        electrodes[i].treat(currentRoundFdLog.at(i), round*5);
        if(pulseCount == 16){
            electrodes[i].treatmentAffect(); // make effects show at the end of treatment phase
        }
    }
    pulseCount++;
    if(pulseCount<17){
        electrodeTimer->start(therapyTime/16);

    }

    if(pulseCount==17){
        pulseCount = 1;
        electrodeTimer->stop();
        currentRoundFdLog.clear();
        state = SessionState::ANALYZING;
        }
    }

void Session::calcBaseline(){
    if(state == SessionState::ANALYZING){
        switch(baselinePhase){
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
                qDebug() << "calculating baseline for all electrodes concurrently...";
                for(int i = 0; i<21; i++){
                    QVector<int> waveInfo = electrodes.at(i).emitSignal();
                    int Fd = (waveInfo.at(0)*waveInfo.at(3)*waveInfo.at(3)+waveInfo.at(1)*waveInfo.at(4)*waveInfo.at(4)+waveInfo.at(2)*waveInfo.at(5)*waveInfo.at(5))/(waveInfo.at(3)*waveInfo.at(3)+waveInfo.at(4)*waveInfo.at(4)+
                                                                                                                                                        waveInfo.at(5)*waveInfo.at(5));
                    currentRoundFdLog.append(Fd);
                }
                break;
            case 3:
                qDebug() << "gathering results...";
                break;
            case 4:
                for(int i = 0; i<21; i++){
                    qDebug() << "dominant frequency for electrod: " << i+1 << ", is " << currentRoundFdLog.at(i) << " Hz";
                }
                break;
        }
        baselinePhase ++;
        if(baselinePhase == 5){
            round ++;
            baselinePhase = 0;
            if(round == 5){
                finalBaselineLog = currentRoundFdLog;
                qDebug() << "therapy complete";
                state = SessionState::COMPLETE;
            }else{
                if(round == 0){
                initialBaselineLog = currentRoundFdLog;
                }
                state = SessionState::TREATING;
                qDebug() << "initiaing treatment every 16th of a second for 1 second";
                electrodeTimer->start(therapyTime/16);
            }
        }
    }

}
