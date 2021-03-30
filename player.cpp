#include "player.h"

Player::Player(QWidget *parent) : QWidget(parent)
{
    lower();
    pos_number = 3;
    pos_x = 0;
    pos_y = 0;
    resize(1080, 672);
}

bool Player::is_on_vertex()
{
    for (auto i = now_on->vertices.begin(); i < now_on->vertices.end(); i++)
        if (i->pos_x == pos_x && i->pos_y == pos_y)
        {
            pos_number = i->number;
            return true;
        }
    return false;
}

void Player::navigation(int des)
{
    if (!is_on_vertex())
    {
        if (now_on->vertices_size < now_on->vertices.size())
            now_on->vertices.pop_back();
        now_on->vertices.push_back({int(now_on->vertices.size()), "Crossing", pos_x, pos_y});
        pos_number = now_on->vertices.size() - 1;
        edge tmp = *now_route.now;
        tmp.from = pos_number;
        tmp.length = map_ratio * get_length(pos_x, pos_y, now_on->vertices[tmp.to].pos_x, now_on->vertices[tmp.to].pos_y);
        now_on->vertices.back().adjlist.push_back(tmp);

        tmp.to = now_route.now->from;
        tmp.length = map_ratio * get_length(pos_x, pos_y, now_on->vertices[tmp.to].pos_x, now_on->vertices[tmp.to].pos_y);
        now_on->vertices.back().adjlist.push_back(tmp);
    }
    distance_first = now_on->distance_first_dijkstra(pos_number, des);
    time_first = now_on->time_first_dijkstra(pos_number, des);
    bike_allowed = now_on->bike_allowed_dijkstra(pos_number, des);
}

void Player::move()
{
    update();
    for (auto i = now_route.edges.begin(); i < now_route.edges.end(); i++)
    {
        now_route.now = *i;
        int distance = fabs(now_on->vertices[(*i)->to].pos_x - pos_x) + fabs(now_on->vertices[(*i)->to].pos_y - pos_y);
        int x_move = 0;
        int y_move = 0;
        if (fabs(now_on->vertices[(*i)->to].pos_x - pos_x) == 0)
        {
            if (now_on->vertices[(*i)->to].pos_y - pos_y > 0)
                y_move = 1;
            else
                y_move = -1;
        }
        else
        {
            if (now_on->vertices[(*i)->to].pos_x - pos_x > 0)
                x_move = 1;
            else
                x_move = -1;
        }
        for (auto i = 0; i < distance; i++)
        {
            if (now_route.canceled)
                break;
            if (now_route.is_riding && now_route.now->is_bike_allowed)
                sleep(map_ratio * 1.0 / (now_route.now->congestion * walk_speed * ride_multiplier) * 10000);
            else
                sleep(map_ratio * 1.0 / (now_route.now->congestion * walk_speed) * 10000);
            pos_x += x_move;
            pos_y += y_move;
            update();
        }
        if (now_route.canceled)
            break;
        pos_number = now_on->vertices[(*i)->to].number;
        pos_x = now_on->vertices[(*i)->to].pos_x;
        pos_y = now_on->vertices[(*i)->to].pos_y;
    }
    now_route.visable = false;
    now_route.canceled = false;
    update();
}

void Player::show_route(route_info *route)
{
    route->visable = true;
    update();
}

void Player::hide_route(route_info *route)
{
    route->visable = false;
    update();
}

void Player::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPen pen;
    pen.setWidth(5);
    pen.setColor(QColor(25, 25, 25));
    painter.setPen(pen);
    if (distance_first.visable)
        for (auto i = distance_first.edges.begin(); i < distance_first.edges.end(); i++)
            painter.drawLine(QPointF(now_on->vertices[(*i)->from].pos_x * my_ratio + my_drift,
                                     now_on->vertices[(*i)->from].pos_y * my_ratio + my_drift),
                             QPointF(now_on->vertices[(*i)->to].pos_x * my_ratio + my_drift,
                                     now_on->vertices[(*i)->to].pos_y * my_ratio + my_drift));
    if (time_first.visable)
        for (auto i = time_first.edges.begin(); i < time_first.edges.end(); i++)
            painter.drawLine(QPointF(now_on->vertices[(*i)->from].pos_x * my_ratio + my_drift,
                                     now_on->vertices[(*i)->from].pos_y * my_ratio + my_drift),
                             QPointF(now_on->vertices[(*i)->to].pos_x * my_ratio + my_drift,
                                     now_on->vertices[(*i)->to].pos_y * my_ratio + my_drift));
    if (bike_allowed.visable)
        for (auto i = bike_allowed.edges.begin(); i < bike_allowed.edges.end(); i++)
            painter.drawLine(QPointF(now_on->vertices[(*i)->from].pos_x * my_ratio + my_drift,
                                     now_on->vertices[(*i)->from].pos_y * my_ratio + my_drift),
                             QPointF(now_on->vertices[(*i)->to].pos_x * my_ratio + my_drift,
                                     now_on->vertices[(*i)->to].pos_y * my_ratio + my_drift));
    if (now_route.visable)
        for (auto i = now_route.edges.begin(); i < now_route.edges.end(); i++)
            painter.drawLine(QPointF(now_on->vertices[(*i)->from].pos_x * my_ratio + my_drift,
                                     now_on->vertices[(*i)->from].pos_y * my_ratio + my_drift),
                             QPointF(now_on->vertices[(*i)->to].pos_x * my_ratio + my_drift,
                                     now_on->vertices[(*i)->to].pos_y * my_ratio + my_drift));
    QPixmap player_img;
    player_img.load("me.png");
    painter.drawPixmap(pos_x * my_ratio + my_drift - 20, pos_y * my_ratio + my_drift - 30, 40, 40, player_img);
}