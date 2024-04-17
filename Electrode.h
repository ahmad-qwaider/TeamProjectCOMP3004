#ifndef ELECTRODE_H
#define ELECTRODE_H
#include <string>
#include <iostream>
#include <QRandomGenerator>
#include <QVector>
#include <QDebug>
using namespace std;
/* Purpose of Class: To act as electrode object
 * Data Members:
 * bool isActive - status of electrode, help confirm connection
 * int position - location
 * int f1,f1,f2,f3,a1,a2,a3 - sin wave elements of current electrodes "eeg waveform". f is frequncy and a is ampliture. 3 sets
 *
 * Class Functions:
 *  QVector<int> emitSignal() - simulate electrode giving off certain wave form via printing out its atributes, returns a integer QVector with wave elements
 *                              in the following order f1 f2 f3 a1 a2 a3.
 *  QVector<double> getWaveData(int durationInSeconds, int sampleRate) - gets wave data for graphing
 *  void treatmentAffect() - simulate the affect of treatment by incrementing frequency
 *  void treat(int Fd, int offSet) - simulate treatment by printing to console
 *  void switchIsActive() - swap is active to oposite
 *
 *  Getters and Setters
 */

class Electrode 
{
public:
    Electrode();
    ~Electrode();
    static int nextPosition;
    QVector<int> emitSignal() const;
    QVector<double> getWaveData(int durationInSeconds, int sampleRate) const;
    void treatmentAffect();
    void treat(int Fd, int offSet);
    void setIsActive(bool status);
    bool getIsActive();
    int getPosition();
    void switchIsActive();
private:
    bool isActive = false;
    int position;
    int f1,f2,f3,a1,a2,a3;
};

#endif // ELECTRODE_H
