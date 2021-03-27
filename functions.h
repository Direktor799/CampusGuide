#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <QString>
#include <QTime>
#include <QApplication>

bool string_less(const QString &s1, const QString &s2);
bool sleep(unsigned int msec);
int get_length(int a_pos_x, int a_pos_y, int b_pos_x, int b_pos_y);
#endif // FUNCTIONS_H
