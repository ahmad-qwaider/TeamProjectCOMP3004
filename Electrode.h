#ifndef ELECTRODE_H
#define ELECTRODE_H
#include <string>
#include <iostream>
#include <QRandomGenerator>
#include <QVector>
using namespace std;
/* Purpose of Class: To act as electrode object
 *
 */

class Electrode 
{
public:
    Electrode();
    ~Electrode();
    static int nextPosition;
    QVector<int> emitSignal() const;
    void treatmentAffect();
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
