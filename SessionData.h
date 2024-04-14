#ifndef SESSIONDATA_H
#define SESSIONDATA_H
#include <QDateTime>

struct SessionData{
    QDateTime dateAndTime;
    int initialBaseline;
    int finalBaseline;


    SessionData(QDateTime dt, int initBaseline, int finBaseline) : dateAndTime(dt), initialBaseline(initBaseline), finalBaseline(finBaseline) {}

    QString toString(int session) const {
        return QString("Session " + QString::number(session) + ": \nDate and Time: %1\nInitial Baseline: %2\nFinal Baseline: %3")
                    .arg(dateAndTime.toString("yyyy-MM-dd hh:mm:ss"))
                    .arg(initialBaseline)
                    .arg(finalBaseline);
    }
};


#endif // SESSIONDATA_H
