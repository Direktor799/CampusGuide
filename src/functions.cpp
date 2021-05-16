#include "functions.h"

bool string_less(const QString &s1, const QString &s2)
{
    return (s1.localeAwareCompare(s2) < 0);
}

void sleep(int msec, int &speedfactor, bool &cancel)
{
    while (msec > 0)
    {
        if(cancel)
            return;
        _tinysleep100ms();
        msec -= 100 * speedfactor;
    }
    return;
}

void _tinysleep100ms()
{
    QTime dieTime = QTime::currentTime().addMSecs(100);
    while (QTime::currentTime() < dieTime)
    //    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    return;
}

int get_length(int a_pos_x, int a_pos_y, int b_pos_x, int b_pos_y)
{
    return abs(a_pos_x - b_pos_x) + abs(a_pos_y - b_pos_y);
}

int get_distance(int a_pos_x, int a_pos_y, int b_pos_x, int b_pos_y)
{
    return sqrt((a_pos_x - b_pos_x) * (a_pos_x - b_pos_x) + (a_pos_y - b_pos_y) * (a_pos_y - b_pos_y));
}