#include "Session.h"

Session::Session(QDateTime currentDateAndTime, int analyzingTime, int treatmentTime, QVector<Electrode> &electrodeList)
{
    electrodes = electrodeList;
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

void Session::initializeEventTimers(){
    connect(electrodeTimer, &QTimer::timeout, this, &Session::treatElectrode);

    connect(baselineTimer, &QTimer::timeout, this, &Session::calcBaseline);
    state = SessionState::ANALYZING;
    baselineTimer->start((analyzeTime/21) - 1);
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
        baselineTimer->blockSignals(true);
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
        baselineTimer->blockSignals(false);
        baselineTimer->start();
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


void Session::treatElectrode(){
    electrodeTimer->stop();
    //electrodeTimer->setInterval(therapyTime/16);
    qDebug() << "the dominant frequency at electrode " << currentElectrode << " location was: " << currentRoundFdLog.at(currentElectrode-1) << ". total offset for round "<<round<< " is " << 5*round;
    qDebug() << "sending treatment every 1/16 of a second for 1 second to electrode " << currentElectrode<< " of stregnth: " << currentRoundFdLog.at(currentElectrode-1) + 5*round;
    electrodes[currentElectrode-1].treatmentAffect(); // make the treatment affect apparent so baseline changes
                currentElectrode++;
                if(currentElectrode<22){
                    electrodeTimer->start();
                }
                else if(currentElectrode == 22){ // reached end of pulsing phase since 22 technically out of index. start at 0.
                    electrodeTimer->stop();
                    currentElectrode = 1;
                    currentRoundFdLog.clear();
                    state = SessionState::ANALYZING;
                    //baselineTimer->setInterval(analyzeTime/21);
                    qDebug() << "beginning baseline/dominantFreq phase for electrode: " << currentElectrode << " current waveform. currently in round: " << round;
                    baselineTimer->start();
                }
    }

void Session::calcBaseline(){
    //baselineTimer->stop();
    //baselineTimer->setInterval(analyzeTime/21);
            qDebug() << "observing electrode: " << currentElectrode;
            QVector<int> waveInfo = electrodes.at(currentElectrode-1).emitSignal();
            qDebug() << "calculating baseline/dominantFreq for electrode: " << currentElectrode;
            int Fd = (waveInfo.at(0)*waveInfo.at(3)*waveInfo.at(3)+waveInfo.at(1)*waveInfo.at(4)*waveInfo.at(4)+waveInfo.at(2)*waveInfo.at(5)*waveInfo.at(5))/
                    (waveInfo.at(3)*waveInfo.at(3)+waveInfo.at(4)*waveInfo.at(4)+waveInfo.at(5)*waveInfo.at(5));
            qDebug() << "dominantFreq for electrode: " << currentElectrode << " is: " << Fd;
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
                    qDebug() << "Session Complete";
                    endSession();
                }else{
                    baselineTimer->stop();
                    state = SessionState::TREATING;
                    electrodeTimer->start((therapyTime));
                }
            } 
    //if(state == SessionState::ANALYZING){
        //baselineTimer->start();
    //}

}
