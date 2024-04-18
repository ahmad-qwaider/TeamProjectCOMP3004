#include "Electrode.h"

int Electrode::nextPosition = 1;

Electrode::Electrode() {
    position = nextPosition++;
    int bandChoice = QRandomGenerator::global()->bounded(0, 5); // Choose a band from 0 to 4

    // Frequency bands
    switch(bandChoice) {
    case 0: // Delta (1-4 Hz)
        f1 = QRandomGenerator::global()->bounded(1, 5);
        break;
    case 1: // Theta (4-8 Hz)
        f1 = QRandomGenerator::global()->bounded(4, 9);
        break;
    case 2: // Alpha (8-12 Hz)
        f1 = QRandomGenerator::global()->bounded(8, 13);
        break;
    case 3: // Beta (12-30 Hz)
        f1 = QRandomGenerator::global()->bounded(12, 31);
        break;
    case 4: // Gamma (25-140 Hz)
        f1 = QRandomGenerator::global()->bounded(25, 141);
        break;
    }

    // Ensure variations are meaningful and do not overlap frequency bands
    f2 = f1 + QRandomGenerator::global()->bounded(-1, 2); // slight variation
    f3 = f1 + QRandomGenerator::global()->bounded(-1, 2); // slight variation
    a1 = QRandomGenerator::global()->bounded(1, 5);
    a2 = QRandomGenerator::global()->bounded(1, 5);
    a3 = QRandomGenerator::global()->bounded(1, 5);
}


Electrode::~Electrode() {}


QVector<double> Electrode::getWaveData(int durationInSeconds, int sampleRate) const {
//    cout << "electrode at position " << position << " emits "<< f1 << f2 << f3 << endl;
    QVector<double> waveData(durationInSeconds * sampleRate);
    double step = 1.0 / sampleRate;
    double x = 0.0;

    for (int i = 0; i < waveData.size(); ++i) {
        // Combining three sinusoidal waveforms
        waveData[i] = a1 * sin(2 * M_PI * f1 * x) +
                      a2 * sin(2 * M_PI * f2 * x) +
                      a3 * sin(2 * M_PI * f3 * x);
        x += step;
    }
    return waveData;
}

QVector<int> Electrode::emitSignal() const{
    qDebug() << "electrode at position " << position << " emits f1: "<< f1 << "  f2: " << f2 << " f3: "<< f3 << " a1: " << a1 << " a2: " << a2 << " a3: "<< a3;
    // indexes 0-2 are the frequecnies. indecies 3-5 are amplitudes
    QVector<int> waveData;
    waveData.append(f1);
    waveData.append(f2);
    waveData.append(f3);
    waveData.append(a1);
    waveData.append(a2);
    waveData.append(a3);
    return waveData;
}

void Electrode::treat(int Fd, int offSet){
    qDebug() << "Electrode: " << position << ", was treated with: " << Fd << " Hz + offset" << offSet << " Hz = " << offSet + Fd << " Hz.";
}

void Electrode::treatmentAffect(){
    f1 += 1;
    f2 += 1;
    f2 += 1;
}

void Electrode::setIsActive(bool status){
    isActive  = status;
}

void Electrode::switchIsActive(){
    isActive  = !isActive;
}

bool Electrode::getIsActive(){
    return isActive;
}

int Electrode::getPosition(){
    return position;
}
