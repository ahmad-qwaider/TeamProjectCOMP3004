#include "Session.h"

Session::Session(QDateTime currentDateAndTime, int analyzingTime, int treatmentTime)
{
    dateAndTime = currentDateAndTime;
    analyzeTime = analyzingTime;
    therapyTime = treatmentTime;

    sessionLength = (analyzeTime * 5) + ((therapyTime * 21) * 4);

    progressTimer = new QTimer(this);
    electrodeTimer = new QTimer(this);
    baselineTimer = new QTimer(this);
}
Session::~Session(){
    delete progressTimer;
    delete electrodeTimer;
    delete baselineTimer;
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

void Session::initializeEventTimers(QVector<Electrode> &electrodes){
    connect(electrodeTimer, &QTimer::timeout, [this, &electrodes]() {
        Session::treatElectrode(electrodes);
    });
    connect(baselineTimer, &QTimer::timeout, [this, &electrodes]() {
        Session::calcBaseline(electrodes);
    });
    baselineTimer->start(analyzeTime/21);
}

void Session::endSession(){
    electrodeTimer->stop();
    baselineTimer->stop();
    electrodeTimer->disconnect();
    baselineTimer->disconnect();
} // stop and disconnect timers

void Session::pauseTimers(){
    if(state == SessionState::ANALYZING){
        prePauseState = SessionState::ANALYZING;
        state = SessionState::PAUSED;
        baselineTimer->stop();
    }else{
        prePauseState = SessionState::TREATING;
        state = SessionState::PAUSED;
        electrodeTimer->stop();
    }
} //pauses timer stops the currently active timer dependent on the state and set state to paused

void Session::resumeTimers(){
    if(prePauseState == SessionState::ANALYZING){
        state = SessionState::ANALYZING;
        baselineTimer->start();
    }else{
        state = SessionState::TREATING;
        electrodeTimer->start();
    }
} //resumes the most recently active state

int Session::getSessionLength(){
    return sessionLength;
}

int Session::getRound(){
    return round;
}

bool Session::isActive(){
    if(state == SessionState::TREATING || state == SessionState::ANALYZING){
        return true;
    }
    return false;
} // true if state is TREATING or ANALYZING

SessionData Session::generateSessionData(){
    SessionData data;
    data.dateAndTime = dateAndTime;
    int initialBaseAve = 0;
    int finalBaseAve = 0;
    for(int i: intialBaselineLog){
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


void Session::treatElectrode(QVector<Electrode> &electrodes){
    qDebug() << "the dominant frequency at electrode " << currentElectrode << " location was: " << currentRoundFdLog.at(currentElectrode-1) << ". total offset this round is " << 5*round;
    qDebug() << "sending treatment pulse number: " << pulseCount << "to electrode " << currentElectrode<< " of stregnth: " << currentRoundFdLog.at(currentElectrode-1) + 5*round;
    pulseCount ++;
    if(pulseCount == 15){ // we have emited 16th pulse
        for(Electrode& electrode: electrodes){ // go to the proper electrode
            if(currentElectrode == electrode.getPosition()){
                electrode.treatmentAffect(); // make the treatment affect apparent so baseline changes
                electrode.setIsActive(false); // turn location off
                visitNextSite();
                if(currentElectrode == 22){ // reached end of pulsing phase since 22 technically out of index. start at 0.
                    visitInitialSite();
                    currentRoundFdLog.clear();
                    electrodeTimer->stop();
                    state = SessionState::ANALYZING;
                    baselineTimer->setInterval(therapyTime/21);
                    baselineTimer->start();
                }
            }
        }
    }
}

void Session::calcBaseline(QVector<Electrode> &electrodes){
    qDebug() << "calculating baseline/dominantFreq for electrode: " << currentElectrode << " current waveform in round: " << round;
    for(Electrode electrode: electrodes){
        if(currentElectrode == electrode.getPosition()){
            qDebug() << "observing electrode: " << currentElectrode << " current waveform in round: " << round;
            QVector<int> waveInfo = electrode.emitSignal();
            qDebug() << "calculating baseline/dominantFreq for electrode: " << currentElectrode << " current waveform in round: " << round;
            int Fd = (waveInfo.at(0)*waveInfo.at(3)*waveInfo.at(3)+waveInfo.at(1)*waveInfo.at(4)*waveInfo.at(4)+waveInfo.at(2)*waveInfo.at(5)*waveInfo.at(5))/
                    (waveInfo.at(3)*waveInfo.at(3)+waveInfo.at(4)*waveInfo.at(4)+waveInfo.at(5)*waveInfo.at(5));
            qDebug() << "dominantFreq for electrode: " << currentElectrode << " is: " << Fd << " Hz for waveform  observed in round: " << round;
            currentRoundFdLog.append(Fd);
            if(round == 0){
                intialBaselineLog.append(Fd);
            }else if (round == 4){
                finalBaselineLog.append(Fd);
            }
            visitNextSite();
            if(currentElectrode == 22){
                visitInitialSite();
                round ++;
                if(round == 5){
                    endSession();
                }else{
                    baselineTimer->stop();
                    state = SessionState::TREATING;
                    baselineTimer->setInterval(analyzeTime/21);
                    electrodeTimer->start(therapyTime/21);
                }
            }
        }
    }

}