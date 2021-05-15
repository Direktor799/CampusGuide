#include "map.h"
#include <QLabel>

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

    json bus_data, day_data;
    std::ifstream("../data/" + filename.toStdString() + "_bus.json") >> bus_data;
    for (int week = 0; week < 7; week++)
    {
        std::string week_index;
        week_index += week + '0';
        day_data = bus_data[week_index];
        for (auto i = day_data.begin(); i < day_data.end(); i++)
        {
            bus_time tmp;
            tmp.day_of_week = week;
            tmp.start_time = QTime::fromString(QString::asprintf(std::string((*i)["start"]).c_str()), "hh:mm");
            tmp.arrival_time = QTime::fromString(QString::asprintf(std::string((*i)["arrival"]).c_str()), "hh:mm");
            bus_time_list.push_back(tmp);
        }
    }
    time_ptr = vtime;
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

route_info Map::dijkstra(int src, int des, strat type)
{
    route_info tmp;
    if (type == distance_first)
        tmp = distance_first_dijkstra(src, des);
    else if (type == time_first)
        tmp = time_first_dijkstra(src, des);
    else if (type == bike_allowed)
        tmp = bike_allowed_dijkstra(src, des);
    return tmp;
}

void Map::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPixmap pix;
    pix.load("../data/" + filename + ".png");
    painter.drawPixmap(0, 0, pix);
}