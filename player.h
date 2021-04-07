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
    multi_routes distance_first;
    multi_routes time_first;
    multi_routes bike_allowed;
    multi_routes now_routes;

    Player(QWidget *parent = nullptr);
    bool is_on_vertex();
    void navigation(vector<int> des);
    void move();
    void show_route(multi_routes* routes);
    void hide_route(multi_routes* routes);
protected:
    void paintEvent(QPaintEvent *);
signals:

};

#endif // PLAYER_H
