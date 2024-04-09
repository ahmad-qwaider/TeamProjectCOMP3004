#include "Electrode.h"

int Electrode::nextPosition = 1;

Electrode::Electrode(const string& name, int freq)
{
    waveType = name;
    signalStrength = freq;
    position = nextPosition++;
}

Electrode::~Electrode() {}

int Electrode::emitTherapy(){
    cout << "electrode at position " << position << " emits " << waveType << " wave of fequency " << signalStrength << " Hz." << endl;
    return signalStrength;
}

void Electrode::addOffset(){
    signalStrength += 5;
}

void Electrode::resetOffset(){
    signalStrength -= 20;
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
