#include "map.h"
#include <QLabel>
#include <iostream>
Map::Map(QString fn, QString n, QDateTime *vtime, QWidget *parent) : QWidget(parent)
{
    resize(1080, 672);
    filename = fn;
    name = n;
    json data, j;
    std::ifstream("../data/" + filename.toStdString() + ".json") >> data;
    j = data["vertices"];
    for (auto i = j.begin(); i < j.end(); i++)
    {
        vertex tmp;
        tmp.number = (*i)["number"];
        tmp.name = QString::asprintf(std::string((*i)["name"]).c_str());
        tmp.pos_x = (*i)["pos_x"];
        tmp.pos_y = (*i)["pos_y"];
        vertices.push_back(tmp);
    }
    j = data["edges"];
    for (auto i = j.begin(); i < j.end(); i++)
    {
        edge tmp;
        tmp.from = (*i)["from"];
        tmp.to = (*i)["to"];
        tmp.congestion = (*i)["congestion"];
        tmp.is_bike_allowed = (*i)["is_bike_allowed"];
        tmp.length = map_ratio * get_length(vertices[tmp.from].pos_x, vertices[tmp.from].pos_y, vertices[tmp.to].pos_x, vertices[tmp.to].pos_y);
        edges.push_back(tmp);
    }
    for (auto i = edges.begin(); i < edges.end(); i++)
    {
        vertices[i->from].adjlist.push_back(*i);
        int tmp = i->from;
        i->from = i->to;
        i->to = tmp;
        vertices[i->from].adjlist.push_back(*i);
    }
    vertices_size = vertices.size();
    list << "";
    for (auto i = vertices.begin(); i < vertices.end(); i++)
        if (i->name != "Crossing")
            list << i->name;
    std::sort(list.begin(), list.end(), string_less);

    //读入校车时刻表
    json bus_data;
    std::ifstream("../data/" + filename.toStdString() + "bus.json") >> bus_data;
    bus.resize(5);
    for (int k = 0; k < 5; k++)
    {
        json week;
        std::string week_num = "";
        week_num += char(k + '0');
        week = bus_data[week_num];
        for (auto i = week.begin(); i < week.end(); i++)
        {
            bus_time_table tmp;
            tmp.start_time = QTime::fromString(QString::asprintf(std::string((*i)["start"]).c_str()), "hh:mm");
            tmp.arrival_time = QTime::fromString(QString::asprintf(std::string((*i)["arrive"]).c_str()), "hh:mm");
            bus[k].push_back(tmp);
        }
    }
    map_time_ptr = vtime;
}

route_info Map::distance_first_dijkstra(int src, int des)
{
    std::priority_queue<dij_tmp, QVector<dij_tmp>, distance_first_cmp> q;
    QVector<int> dis(vertices.size(), INT32_MAX);
    QVector<double> time(vertices.size(), INT32_MAX);
    QVector<edge *> _path(vertices.size(), NULL);
    QVector<edge *> route;
    dis[src] = 0;
    time[src] = 0;
    for (auto i = vertices[src].adjlist.begin(); i < vertices[src].adjlist.end(); i++)
        q.push({&*i, i->length, i->length / (i->congestion * walk_speed)});
    while (dis[des] == INT32_MAX)
    {
        dij_tmp tmp = q.top();
        q.pop();
        if (dis[tmp.edge_ptr->to] != INT32_MAX)
            continue;
        dis[tmp.edge_ptr->to] = tmp.total_dis;
        time[tmp.edge_ptr->to] = tmp.total_time;
        _path[tmp.edge_ptr->to] = tmp.edge_ptr;
        for (auto i = vertices[tmp.edge_ptr->to].adjlist.begin(); i < vertices[tmp.edge_ptr->to].adjlist.end(); i++)
            if (dis[i->to] == INT32_MAX)
                q.push({&*i, i->length + dis[i->from], i->length / (i->congestion * walk_speed) + time[i->from]});
    }
    for (int i = des; i != src; i = _path[i]->from)
        route.push_back(_path[i]);
    std::reverse(route.begin(), route.end());
    route_info ret;
    ret.distance = dis[des];
    ret.time = time[des];
    ret.on = this;
    ret.edges = route;
    return ret;
}

route_info Map::time_first_dijkstra(int src, int des)
{
    std::priority_queue<dij_tmp, QVector<dij_tmp>, time_first_cmp> q;
    QVector<int> dis(vertices.size(), INT32_MAX);
    QVector<double> time(vertices.size(), INT32_MAX);
    QVector<edge *> _path(vertices.size(), NULL);
    QVector<edge *> route;
    dis[src] = 0;
    time[src] = 0;
    for (auto i = vertices[src].adjlist.begin(); i < vertices[src].adjlist.end(); i++)
        q.push({&*i, i->length, i->length / (i->congestion * walk_speed)});
    while (dis[des] == INT32_MAX)
    {
        dij_tmp tmp = q.top();
        q.pop();
        if (dis[tmp.edge_ptr->to] != INT32_MAX)
            continue;
        dis[tmp.edge_ptr->to] = tmp.total_dis;
        time[tmp.edge_ptr->to] = tmp.total_time;
        _path[tmp.edge_ptr->to] = tmp.edge_ptr;
        for (auto i = vertices[tmp.edge_ptr->to].adjlist.begin(); i < vertices[tmp.edge_ptr->to].adjlist.end(); i++)
            if (dis[i->to] == INT32_MAX)
                q.push({&*i, i->length + dis[i->from], i->length / (i->congestion * walk_speed) + time[i->from]});
    }
    for (int i = des; i != src; i = _path[i]->from)
        route.push_back(_path[i]);
    std::reverse(route.begin(), route.end());
    route_info ret;
    ret.distance = dis[des];
    ret.time = time[des];
    ret.on = this;
    ret.edges = route;
    return ret;
}

route_info Map::bike_allowed_dijkstra(int src, int des)
{
    std::priority_queue<dij_tmp, QVector<dij_tmp>, time_first_cmp> q;
    QVector<int> dis(vertices.size(), INT32_MAX);
    QVector<double> time(vertices.size(), INT32_MAX);
    QVector<edge *> _path(vertices.size(), NULL);
    QVector<edge *> route;
    dis[src] = 0;
    time[src] = 0;
    for (auto i = vertices[src].adjlist.begin(); i < vertices[src].adjlist.end(); i++)
    {
        if (i->is_bike_allowed)
            q.push({&*i, i->length, i->length / (i->congestion * walk_speed * ride_multiplier)});
        else
            q.push({&*i, i->length, i->length / (i->congestion * walk_speed)});
    }
    while (dis[des] == INT32_MAX)
    {
        dij_tmp tmp = q.top();
        q.pop();
        if (dis[tmp.edge_ptr->to] != INT32_MAX)
            continue;
        dis[tmp.edge_ptr->to] = tmp.total_dis;
        time[tmp.edge_ptr->to] = tmp.total_time;
        _path[tmp.edge_ptr->to] = tmp.edge_ptr;
        for (auto i = vertices[tmp.edge_ptr->to].adjlist.begin(); i < vertices[tmp.edge_ptr->to].adjlist.end(); i++)
            if (dis[i->to] == INT32_MAX)
            {
                if (i->is_bike_allowed)
                    q.push({&*i, i->length + dis[i->from], i->length / (i->congestion * walk_speed * ride_multiplier) + time[i->from]});
                else
                    q.push({&*i, i->length + dis[i->from], i->length / (i->congestion * walk_speed) + time[i->from]});
            }
    }
    for (int i = des; i != src; i = _path[i]->from)
        route.push_back(_path[i]);
    std::reverse(route.begin(), route.end());
    route_info ret;
    ret.distance = dis[des];
    ret.time = time[des];
    ret.on = this;
    ret.edges = route;
    ret.is_riding = true;
    return ret;
}

void Map::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPixmap pix;
    pix.load("../data/" + filename + ".png");
    painter.drawPixmap(0, 0, pix);
}

route_info Map::cross_campus(bool flag)
{
    route_info cur;
    cur.distance = 22400;
    if (!flag) //flag为0表示地铁,10分钟一趟，最早一班是6：00,最晚是11：50
    {
        cur.time = 10 - double(map_time_ptr->time().minute() % 10 + map_time_ptr->time().second()) / 60; //算出距离下一个整十分钟要多久
        if (map_time_ptr->time().hour() >= 0 && map_time_ptr->time().hour() < 6)                         //如果当前地铁没车，加上要等的小时
            cur.time += 60 * (6 - map_time_ptr->time().hour());
        cur.time += 50; //模拟两个地铁站之间50分钟
    }
    else
    {
        int weekday = map_time_ptr->date().dayOfWeek() - 1;
        if (weekday == 5 || weekday == 6 || (weekday == 4 && map_time_ptr->time() > bus[4].back().start_time)) //如果已经错过周五最后一班车或者在周六日
        {
            cur.time = double(map_time_ptr->time().secsTo(bus[0][0].start_time)) / 60 + 24 * 60 * (7 - weekday);
        }
        else
        {
            if (map_time_ptr->time() < bus[weekday].back().start_time) //如果当天还有车
                for (int i = 0; i < bus[weekday].size(); i++)
                {
                    if (bus[weekday][i].start_time > map_time_ptr->time())
                    {
                        cur.time = double(map_time_ptr->time().secsTo(bus[weekday][i].start_time)) / 60;
                        break;
                    }
                }
            else
                cur.time = double(map_time_ptr->time().secsTo(bus[weekday + 1][0].start_time)) / 60 + 24 * 60;
        }
    }
    return cur;
}