#include "electrode.h"

Electrode::Electrode(int pos, string& name, int freq)
{
    position = pos;
    waveType = name;
    signalStrength = freq;
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
