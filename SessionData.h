#ifndef SESSIONDATA_H
#define SESSIONDATA_H
#include <QDateTime>

struct SessionData{
    QDateTime dateAndTime;
    int initialBaseline;
    int finalBaseline;
};
#endif // SESSIONDATA_H
