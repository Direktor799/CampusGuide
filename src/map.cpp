#include "map.h"
#include <QLabel>
<<<<<<< HEAD
Map::Map(QString fn, QString n, QDateTime *vtime, QWidget *parent) : QWidget(parent)
=======

Map::Map(QString fn, QString n, QWidget *parent) : QWidget(parent)
>>>>>>> parent of fd42980 (cross_campus_bus)
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

<<<<<<< HEAD
    //读入校车时刻表
    json bus_data;
    std::ifstream("../data/" + filename.toStdString() + "bus.json") >> bus_data;
    bus.resize(7);
    for (int k = 0; k < 7; k++)
    {
        json week;
        std::string week_num = "";
        week_num += char(k + '0' + 1);
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
=======
>>>>>>> parent of fd42980 (cross_campus_bus)
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
<<<<<<< HEAD
}

QVector<route_info> Map::cross_campus(bool flag)
{
    QVector<route_info> result;
    route_info wait;
    wait.distance = 0;
    wait.time = 0;
    route_info cross;
    cross.distance = 0;
    cross.time = 0;
    if (!flag) //flag为0表示地铁,10分钟一趟，最早一班是6：00,最晚是11：50
    {
        wait.time = 10 - double(map_time_ptr->time().minute() % 10 + map_time_ptr->time().second()) / 60; //算出距离下一个整十分钟要多久
        if (map_time_ptr->time().hour() >= 0 && map_time_ptr->time().hour() < 6)                         //如果当前地铁没车，加上要等的小时
            wait.time += 60 * (6 - map_time_ptr->time().hour());
        wait.distance = 0;
        cross.time = 50; //模拟两个地铁站之间50分钟
        cross.distance = 22400;
    }
    else
    {
        int today = map_time_ptr->date().dayOfWeek() - 1;
        int next_bus_gap = 0;
        while(bus[(today + next_bus_gap) % 7].size() == 0 || (!next_bus_gap && map_time_ptr->time() > bus[today].back().start_time))//如果当天没车，转至有车的一天
        {
            next_bus_gap++;
        };
        if(!next_bus_gap)
            for (int i = 0; i < bus[today].size(); i++)
            {
                if (bus[today][i].start_time > map_time_ptr->time())
                {
                    wait.time = double(map_time_ptr->time().secsTo(bus[today][i].start_time)) / 60;
                    cross.time = double(bus[today][i].start_time.secsTo(bus[today][i].arrival_time));
                    break;
                }
            }
        else
        {   wait.time = double(map_time_ptr->time().secsTo(bus[(today + next_bus_gap) % 7][0].start_time)) / 60 + 24 * 60 * next_bus_gap;
            cross.time = double(bus[(today + next_bus_gap) % 7][0].start_time.secsTo(bus[(today + next_bus_gap) % 7][0].arrival_time));
        }
    }
    result.push_back(wait);
    result.push_back(cross);
    return result;
=======
>>>>>>> parent of fd42980 (cross_campus_bus)
}