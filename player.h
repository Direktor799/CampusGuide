#ifndef PLAYER_H
#define PLAYER_H

#include "map.h"

class Player : public QWidget
{
    Q_OBJECT
public:
    Map* now_on;
    int number;
    int pos_x, pos_y;

    Player(QWidget *parent = nullptr);
    bool is_on_vertex();
    void navigation(int des, int strategy);
    void move();
    void teleport(int des);

protected:
    void paintEvent(QPaintEvent *);

signals:

};

#endif // PLAYER_H
