#include "PC.h"

PC::PC()
{

}

void PC::upload(const QVector<SessionData> &sessionsToUpload){
    for(const SessionData &session : sessionsToUpload){
        sessions.append(session);
    }
}

void PC::viewHistory(){
    for(SessionData session: sessions){
        qDebug() << session.finalBaseline;
    }
}
