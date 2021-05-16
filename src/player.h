#ifndef PLAYER_H
#define PLAYER_H

#include "map.h"

class Player : public QWidget
{
    Q_OBJECT
public:
    QTextStream *log;
    Map *now_on;
    int speedfactor;
    int pos_number;
    int pos_x, pos_y;
    bool waiting;
    multi_routes routes_with_strat[3];
    multi_routes now_routes;
    strat now_using;
    Player(QTextStream *textstream, QWidget *parent = nullptr);
    bool is_on_vertex();
    void navigation(QVector<QPair<Map *, int>> des);
    void move();
    void show_route(multi_routes *routes);
    void hide_route(multi_routes *routes);
    QVector<route_info> checkSurrounding();

protected:
    void paintEvent(QPaintEvent *);
signals:
    void moving();
    void playAnimation(transport by);
    void changeMapFrom(Map *from);
};

#endif // PLAYER_H
