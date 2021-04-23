#include "player.h"

Player::Player(QTextStream *textstream, QWidget *parent) : QWidget(parent)
{
    lower();
    pos_number = 3;
    pos_x = 0;
    pos_y = 0;
    speedfactor = 6;
    log = textstream;
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

void Player::navigation(QVector<QPair<Map *, int>> des)
{
    if (!is_on_vertex())
    {
        if (now_on->vertices_size < now_on->vertices.size())
            now_on->vertices.pop_back();
        now_on->vertices.push_back({int(now_on->vertices.size()), "Crossing", pos_x, pos_y});
        pos_number = now_on->vertices.size() - 1;

        edge tmp = *now_routes.now->now;
        tmp.from = pos_number;
        tmp.length = map_ratio * get_length(pos_x, pos_y, now_on->vertices[tmp.to].pos_x, now_on->vertices[tmp.to].pos_y);
        now_on->vertices.back().adjlist.push_back(tmp);

        tmp.to = now_routes.now->now->from;
        tmp.length = map_ratio * get_length(pos_x, pos_y, now_on->vertices[tmp.to].pos_x, now_on->vertices[tmp.to].pos_y);
        now_on->vertices.back().adjlist.push_back(tmp);
    }
    des.insert(des.begin(), qMakePair(now_on, pos_number));

    {
        multi_routes tmp;
        for (auto i = des.begin(); i < des.end() - 1; i++)
        {
            if (i->first == (i + 1)->first)
                tmp.routes.push_back(i->first->distance_first_dijkstra(i->second, (i + 1)->second));
            else
            {
                route_info transport[2][2];
                if (i->first->filename == "main_campus")
                {
                    transport[0][0] = i->first->distance_first_dijkstra(i->second, 0);
                    transport[0][1] = (i + 1)->first->distance_first_dijkstra(83, (i + 1)->second);
                    transport[1][0] = i->first->distance_first_dijkstra(i->second, 116);
                    transport[1][1] = (i + 1)->first->distance_first_dijkstra(88, (i + 1)->second);
                }
                else
                {
                    transport[0][0] = i->first->distance_first_dijkstra(i->second, 83);
                    transport[0][1] = (i + 1)->first->distance_first_dijkstra(0, (i + 1)->second);
                    transport[1][0] = i->first->distance_first_dijkstra(i->second, 88);
                    transport[1][1] = (i + 1)->first->distance_first_dijkstra(116, (i + 1)->second);
                }

                if (transport[0][0].distance + transport[0][1].distance < transport[1][0].distance + transport[1][1].distance)
                {
                    tmp.routes.push_back(transport[0][0]);
                    tmp.routes.push_back(transport[0][1]);
                }
                else
                {
                    tmp.routes.push_back(transport[1][0]);
                    tmp.routes.push_back(transport[1][1]);
                }
            }
        }
        for (auto i = tmp.routes.begin(); i < tmp.routes.end(); i++)
        {
            tmp.distance += i->distance;
            tmp.time += i->time;
        }
        distance_first = tmp;
    }

    {
        multi_routes tmp;
        for (auto i = des.begin(); i < des.end() - 1; i++)
        {
            if (i->first == (i + 1)->first)
                tmp.routes.push_back(i->first->time_first_dijkstra(i->second, (i + 1)->second));
            else
            {
                route_info transport[2][2];
                if (i->first->filename == "main_campus")
                {
                    transport[0][0] = i->first->time_first_dijkstra(i->second, 0);
                    transport[0][1] = (i + 1)->first->time_first_dijkstra(83, (i + 1)->second);
                    transport[1][0] = i->first->time_first_dijkstra(i->second, 116);
                    transport[1][1] = (i + 1)->first->time_first_dijkstra(88, (i + 1)->second);
                }
                else
                {
                    transport[0][0] = i->first->time_first_dijkstra(i->second, 83);
                    transport[0][1] = (i + 1)->first->time_first_dijkstra(0, (i + 1)->second);
                    transport[1][0] = i->first->time_first_dijkstra(i->second, 88);
                    transport[1][1] = (i + 1)->first->time_first_dijkstra(116, (i + 1)->second);
                }

                if (transport[0][0].time + transport[0][1].time < transport[1][0].time + transport[1][1].time)
                {
                    tmp.routes.push_back(transport[0][0]);
                    tmp.routes.push_back(transport[0][1]);
                }
                else
                {
                    tmp.routes.push_back(transport[1][0]);
                    tmp.routes.push_back(transport[1][1]);
                }
            }
        }
        for (auto i = tmp.routes.begin(); i < tmp.routes.end(); i++)
        {
            tmp.distance += i->distance;
            tmp.time += i->time;
        }
        time_first = tmp;
    }

    {
        multi_routes tmp;
        for (auto i = des.begin(); i < des.end() - 1; i++)
        {
            if (i->first == (i + 1)->first)
                tmp.routes.push_back(i->first->bike_allowed_dijkstra(i->second, (i + 1)->second));
            else
            {
                route_info transport[2][2];
                if (i->first->filename == "main_campus")
                {
                    transport[0][0] = i->first->bike_allowed_dijkstra(i->second, 0);
                    transport[0][1] = (i + 1)->first->bike_allowed_dijkstra(83, (i + 1)->second);
                    transport[1][0] = i->first->bike_allowed_dijkstra(i->second, 116);
                    transport[1][1] = (i + 1)->first->bike_allowed_dijkstra(88, (i + 1)->second);
                }
                else
                {
                    transport[0][0] = i->first->bike_allowed_dijkstra(i->second, 83);
                    transport[0][1] = (i + 1)->first->bike_allowed_dijkstra(0, (i + 1)->second);
                    transport[1][0] = i->first->bike_allowed_dijkstra(i->second, 88);
                    transport[1][1] = (i + 1)->first->bike_allowed_dijkstra(116, (i + 1)->second);
                }

                if (transport[0][0].time + transport[0][1].time < transport[1][0].time + transport[1][1].time)
                {
                    tmp.routes.push_back(transport[0][0]);
                    tmp.routes.push_back(transport[0][1]);
                }
                else
                {
                    tmp.routes.push_back(transport[1][0]);
                    tmp.routes.push_back(transport[1][1]);
                }
            }
        }
        for (auto i = tmp.routes.begin(); i < tmp.routes.end(); i++)
        {
            tmp.distance += i->distance;
            tmp.time += i->time;
        }
        bike_allowed = tmp;
    }
}

void Player::move()
{
    update();
    for (auto j = now_routes.routes.begin(); j < now_routes.routes.end(); j++)
    {
        now_routes.now = &*j;
        if(now_routes.now->on != now_on)
            *log << QTime::currentTime().toString("hh:mm:ss:zzz") << " > 玩家 " << now_on->name << "->" << now_routes.now->on->name << Qt::endl;
        now_on = now_routes.now->on;
        for (auto i = j->edges.begin(); i < j->edges.end(); i++)
        {
            if (now_routes.canceled)
                break;
            emit moving();
            pos_number = now_on->vertices[(*i)->from].number;
            pos_x = now_on->vertices[(*i)->from].pos_x;
            pos_y = now_on->vertices[(*i)->from].pos_y;
            now_routes.now->now = *i;
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
            for (auto k = 0; k < distance; k++)
            {
                if (now_routes.canceled)
                    break;
                if (now_routes.now->is_riding && now_routes.now->now->is_bike_allowed)
                    sleep(map_ratio * 1.0 / (now_routes.now->now->congestion * walk_speed * speedfactor / 60 * ride_multiplier) * 1000);
                else
                    sleep(map_ratio * 1.0 / (now_routes.now->now->congestion * walk_speed * speedfactor / 60) * 1000);
                pos_x += x_move;
                pos_y += y_move;
                update();
                *log << QTime::currentTime().toString("hh:mm:ss:zzz") << " > 玩家 [" << now_on->name << "(" << pos_x - x_move << "," << pos_y - y_move << ")]";
                *log << " -> [" << now_on->name << "(" << pos_x << "," << pos_y << ")]" << Qt::endl;
            }
            pos_number = now_on->vertices[(*i)->to].number;
            pos_x = now_on->vertices[(*i)->to].pos_x;
            pos_y = now_on->vertices[(*i)->to].pos_y;
            if(now_on->vertices[pos_number].name != "Crossing")
                *log << QTime::currentTime().toString("hh:mm:ss:zzz") << " > 玩家到达" + now_on->vertices[pos_number].name << "[" << now_on->name << "(" << pos_x << "," << pos_y << ")]" << Qt::endl;
        }
        emit moving();
    }
    now_routes.visable = false;
    now_routes.canceled = false;
    update();
}

void Player::show_route(multi_routes *route)
{
    route->visable = true;
    update();
}

void Player::hide_route(multi_routes *route)
{
    route->visable = false;
    update();
}

QVector<route_info> Player::checkSurrounding()
{
    QVector<route_info> surrounding;
    for(auto i = now_on->vertices.begin(); i < now_on->vertices.end(); i++)
    {
        if(i->name != "Crossing" && get_distance(pos_x, pos_y, i->pos_x, i->pos_y) * map_ratio <= 200 && i->number != pos_number)
            surrounding.push_back(now_on->distance_first_dijkstra(pos_number, i->number));
    }
    std::sort(surrounding.begin(), surrounding.end());
    return surrounding;
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
        for (auto i = distance_first.routes.begin(); i < distance_first.routes.end(); i++)
            if (i->on->isVisible())
                for (auto j = i->edges.begin(); j < i->edges.end(); j++)
                    painter.drawLine(QPointF(i->on->vertices[(*j)->from].pos_x * my_ratio + my_drift,
                                             i->on->vertices[(*j)->from].pos_y * my_ratio + my_drift),
                                     QPointF(i->on->vertices[(*j)->to].pos_x * my_ratio + my_drift,
                                             i->on->vertices[(*j)->to].pos_y * my_ratio + my_drift));
    if (time_first.visable)
        for (auto i = time_first.routes.begin(); i < time_first.routes.end(); i++)
            if (i->on->isVisible())
                for (auto j = i->edges.begin(); j < i->edges.end(); j++)
                    painter.drawLine(QPointF(i->on->vertices[(*j)->from].pos_x * my_ratio + my_drift,
                                             i->on->vertices[(*j)->from].pos_y * my_ratio + my_drift),
                                     QPointF(i->on->vertices[(*j)->to].pos_x * my_ratio + my_drift,
                                             i->on->vertices[(*j)->to].pos_y * my_ratio + my_drift));
    if (bike_allowed.visable)
        for (auto i = bike_allowed.routes.begin(); i < bike_allowed.routes.end(); i++)
            if (i->on->isVisible())
                for (auto j = i->edges.begin(); j < i->edges.end(); j++)
                    painter.drawLine(QPointF(i->on->vertices[(*j)->from].pos_x * my_ratio + my_drift,
                                             i->on->vertices[(*j)->from].pos_y * my_ratio + my_drift),
                                     QPointF(i->on->vertices[(*j)->to].pos_x * my_ratio + my_drift,
                                             i->on->vertices[(*j)->to].pos_y * my_ratio + my_drift));
    if (now_routes.visable)
        for (auto i = now_routes.routes.begin(); i < now_routes.routes.end(); i++)
            if (i->on->isVisible())
                for (auto j = i->edges.begin(); j < i->edges.end(); j++)
                    painter.drawLine(QPointF(i->on->vertices[(*j)->from].pos_x * my_ratio + my_drift,
                                             i->on->vertices[(*j)->from].pos_y * my_ratio + my_drift),
                                     QPointF(i->on->vertices[(*j)->to].pos_x * my_ratio + my_drift,
                                             i->on->vertices[(*j)->to].pos_y * my_ratio + my_drift));
    QPixmap player_img;
    player_img.load("../data/me.png");
    if(now_on->isVisible())
        painter.drawPixmap(pos_x * my_ratio + my_drift - 20, pos_y * my_ratio + my_drift - 30, 40, 40, player_img);
}