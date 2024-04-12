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

QVector<int> Electrode::emitSignal() const{
    cout << "electrode at position " << position << " emits "<< f1 << f2 << f3 << endl;
    QVector<int> waveData;
    waveData.append(f1);
    waveData.append(f2);
    waveData.append(f3);
    waveData.append(a1);
    waveData.append(a2);
    waveData.append(a3);
    return waveData;
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
