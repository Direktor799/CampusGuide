#include "functions.h"

bool string_less(const QString &s1, const QString &s2)
{
    return (s1.localeAwareCompare(s2) < 0);
};

bool sleep(unsigned int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    return true;
};
