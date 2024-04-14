#include "Electrode.h"

int Electrode::nextPosition = 1;

Electrode::Electrode()
{
    position = nextPosition++;
    QRandomGenerator generator;
    f1 = generator.bounded(1, 10);
    f2 = generator.bounded(1, 10);
    f3 = generator.bounded(1, 10);
    a1 = generator.bounded(1, 5);
    a2 = generator.bounded(1, 5);
    a3 = generator.bounded(1, 5);
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
    qDebug() << "electrode at position " << position << " emits "<< f1 << " " << f2 << " "<< f3;
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
