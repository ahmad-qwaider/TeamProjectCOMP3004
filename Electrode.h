#ifndef ELECTRODE_H
#define ELECTRODE_H
#include <string>
#include <iostream>
using namespace std;
/* Purpose of Class: To act as electrode object
 * Data Members:
 * - bool isActive: bool to let us know if this electrode is active or not
 * - int position: an int to tell us what position this electrode is in
 * - string waveType: what type of wave is this Theta, Gamma, Etc.
 * - int signalStrength: a integer representation of how strong treatment frequency will be in Hz
 *
 * - Class Functions:
 * - emitTherapy() emits therapy wave, returns frequecy of the wave
 * - addOffset() adds offset of 5Hz to the current signal strength
 * - resetOffset() resets signalStrength to intial value
 * - getters and setters
 *
 */

class Electrode 
{
public:
    Electrode(const string& waveType, int signalStrength);
    ~Electrode();
    static int nextPosition;
    int emitTherapy();
    void addOffset();
    void resetOffset();
    void setIsActive(bool status);
    void switchIsActive();
    bool getIsActive();
private:
    bool isActive = false;
    int position;
    string waveType;
    int signalStrength;
};

#endif // ELECTRODE_H
