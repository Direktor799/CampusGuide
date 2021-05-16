#include "player.h"

Player::Player(QTextStream *textstream, QWidget *parent) : QWidget(parent)
{
    lower();
    pos_number = 3;
    pos_x = 0;
    pos_y = 0;
    speedfactor = 6;
    waiting = false;
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
    des.insert(des.begin(), qMakePair(now_on, pos_number));
    for (int iter = 0; iter < 3; iter++)
    {
        multi_routes new_routes;
        for (auto i = des.begin(); i < des.end() - 1; i++)
        {
            if (i->first == (i + 1)->first)
            {
                new_routes.routes.push_back(i->first->dijkstra(i->second, (i + 1)->second, strat(iter)));
                new_routes.distance += new_routes.routes.back().distance;
                new_routes.time += new_routes.routes.back().time;
            }
            else
            {
                route_info subway[4], bus[4];
                double cost_of_subway = 0, cost_of_bus = 0;
                if (i->first->filename == "main_campus")
                {
                    subway[0] = i->first->dijkstra(i->second, 152, strat(iter));
                    subway[3] = (i + 1)->first->dijkstra(89, (i + 1)->second, strat(iter));
                    bus[0] = i->first->dijkstra(i->second, 153, strat(iter));
                    bus[3] = (i + 1)->first->dijkstra(90, (i + 1)->second, strat(iter));
                }
                else
                {
                    subway[0] = i->first->dijkstra(i->second, 89, strat(iter));
                    subway[3] = (i + 1)->first->dijkstra(152, (i + 1)->second, strat(iter));
                    bus[0] = i->first->dijkstra(i->second, 90, strat(iter));
                    bus[3] = (i + 1)->first->dijkstra(153, (i + 1)->second, strat(iter));
                }
                QVector<route_info> subway_tmp = i->first->cross_campus(new_routes.time + subway[0].time, by_subway);
                subway[1] = subway_tmp[0];
                subway[2] = subway_tmp[1];
                QVector<route_info> bus_tmp = i->first->cross_campus(new_routes.time + bus[0].time, by_bus);
                bus[1] = bus_tmp[0];
                bus[2] = bus_tmp[1];
                if (strat(iter) == distance_first)
                    for (int j = 0; j < 4; j++)
                    {
                        cost_of_subway += subway[j].distance;
                        cost_of_bus += bus[j].distance;
                    }
                else
                    for (int j = 0; j < 4; j++)
                    {
                        cost_of_subway += subway[j].time;
                        cost_of_bus += bus[j].time;
                    }
                if (cost_of_subway < cost_of_bus)
                    for (int j = 0; j < 4; j++)
                        new_routes.routes.push_back(subway[j]);
                else
                    for (int j = 0; j < 4; j++)
                        new_routes.routes.push_back(bus[j]);
                for (auto wtf = new_routes.routes.end() - 4; wtf < new_routes.routes.end(); wtf++)
                {
                    new_routes.distance += wtf->distance;
                    new_routes.time += wtf->time;
                }
            }
        }
        routes_with_strat[iter] = new_routes;
    }
}

void Player::move()
{
    update();
    for (auto j = now_routes.routes.begin(); j < now_routes.routes.end(); j++)
    {
        if (now_routes.canceled)
            break;
        if (j->on == nullptr)
        {
            waiting = true;
            update();
            sleep(j->time * 60 * 1000, speedfactor, now_routes.canceled);
            waiting = false;
            update();
            j++;
            if (((now_on->filename == "main_campus" && pos_number == 153)) || (now_on->filename == "shahe_campus" && pos_number == 90)) //如果在乘车处
                emit playAnimation(by_bus);
            else
                emit playAnimation(by_subway);
            *now_on->time_ptr = now_on->time_ptr->addSecs(j->time * 60);
            emit changeMapFrom(now_on);
            continue;
        }
        now_routes.now = &*j;
        if (now_routes.now->on != now_on)
            *log << QTime::currentTime().toString("hh:mm:ss:zzz") << " > 玩家 " << now_on->name << "->" << now_routes.now->on->name << Qt::endl;
        now_on = now_routes.now->on;
        for (auto i = j->edges.begin(); i < j->edges.end(); i++)
        {
            pos_number = now_on->vertices[(*i)->from].number;
            pos_x = now_on->vertices[(*i)->from].pos_x;
            pos_y = now_on->vertices[(*i)->from].pos_y;
            now_routes.now->now = *i;
            if (now_routes.canceled)
                break;
            emit moving();
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
                if (now_routes.now->is_riding && now_routes.now->now->is_bike_allowed)
                    sleep(map_ratio * 1.0 / (now_routes.now->now->congestion * walk_speed / 60 * ride_multiplier) * 1000, speedfactor, now_routes.canceled);
                else
                    sleep(map_ratio * 1.0 / (now_routes.now->now->congestion * walk_speed / 60) * 1000, speedfactor, now_routes.canceled);
                if (now_routes.canceled)
                    break;
                pos_x += x_move;
                pos_y += y_move;
                update();
                *log << QTime::currentTime().toString("hh:mm:ss:zzz") << " > 玩家 [" << now_on->name << "(" << pos_x - x_move << "," << pos_y - y_move << ")]";
                *log << " -> [" << now_on->name << "(" << pos_x << "," << pos_y << ")]" << Qt::endl;
            }
            if (now_routes.canceled)
                break;
            pos_number = now_on->vertices[(*i)->to].number;
            pos_x = now_on->vertices[(*i)->to].pos_x;
            pos_y = now_on->vertices[(*i)->to].pos_y;
            if (now_on->vertices[pos_number].name != "Crossing")
                *log << QTime::currentTime().toString("hh:mm:ss:zzz") << " > 玩家到达" + now_on->vertices[pos_number].name << "[" << now_on->name << "(" << pos_x << "," << pos_y << ")]" << Qt::endl;
        }
        emit moving();
    }
    now_routes.visable = false;
    now_routes.canceled = false;
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
    emit moving();
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
    for (auto i = now_on->vertices.begin(); i < now_on->vertices.end(); i++)
    {
        if (i->name != "Crossing" && get_distance(pos_x, pos_y, i->pos_x, i->pos_y) * map_ratio <= 200 && i->number != pos_number)
            surrounding.push_back(now_on->dijkstra(pos_number, i->number, distance_first));
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
    for (int iter = 0; iter < 3; iter++)
        if (routes_with_strat[iter].visable)
            for (auto i = routes_with_strat[iter].routes.begin(); i < routes_with_strat[iter].routes.end(); i++)
                if (i->on != nullptr && i->on->isVisible())
                    for (auto j = i->edges.begin(); j < i->edges.end(); j++)
                        painter.drawLine(QPointF(i->on->vertices[(*j)->from].pos_x * my_ratio + my_drift,
                                                 i->on->vertices[(*j)->from].pos_y * my_ratio + my_drift),
                                         QPointF(i->on->vertices[(*j)->to].pos_x * my_ratio + my_drift,
                                                 i->on->vertices[(*j)->to].pos_y * my_ratio + my_drift));
    if (now_routes.visable)
        for (auto i = now_routes.routes.begin(); i < now_routes.routes.end(); i++)
            if (i->on != nullptr && i->on->isVisible())
                for (auto j = i->edges.begin(); j < i->edges.end(); j++)
                    painter.drawLine(QPointF(i->on->vertices[(*j)->from].pos_x * my_ratio + my_drift,
                                             i->on->vertices[(*j)->from].pos_y * my_ratio + my_drift),
                                     QPointF(i->on->vertices[(*j)->to].pos_x * my_ratio + my_drift,
                                             i->on->vertices[(*j)->to].pos_y * my_ratio + my_drift));
    QPixmap player_img;
    player_img.load("../data/me.png");
    if (now_on->isVisible())
    {
        painter.drawPixmap(pos_x * my_ratio + my_drift - 20, pos_y * my_ratio + my_drift - 30, 40, 40, player_img);
        if (waiting)
        {
            if (((now_on->filename == "main_campus" && pos_number == 153)) || (now_on->filename == "shahe_campus" && pos_number == 90)) //如果在乘车处
                player_img.load("../data/waiting_bus.png");
            else
                player_img.load("../data/waiting_subway.png");
            painter.drawPixmap(pos_x * my_ratio + my_drift + 10, pos_y * my_ratio + my_drift - 100, 100, 100, player_img);
        }
    }
}