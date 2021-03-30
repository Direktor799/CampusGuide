#ifndef PLAYER_H
#define PLAYER_H

#include "map.h"

class Player : public QWidget
{
    Q_OBJECT
public:
    Map* now_on;
    int pos_number;
    int pos_x, pos_y;
    route_info distance_first;
    route_info time_first;
    route_info bike_allowed;
    route_info now_route;

    Player(QWidget *parent = nullptr);
    bool is_on_vertex();
    void navigation(int des);
    void move();
    void show_route(route_info* route);
    void hide_route(route_info* route);
protected:
    void paintEvent(QPaintEvent *);
signals:

};

#endif // PLAYER_H
