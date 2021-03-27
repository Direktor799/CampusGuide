#include "map.h"

Map::Map(string s, QWidget *parent) : QWidget(parent)
{
    resize(1080, 672);
    filename = s;
    json data, j;
    ifstream(filename + ".json") >> data;
    j = data["vertices"];
    for(auto i = j.begin(); i < j.end(); i++)
    {
        vertex tmp;
        tmp.number = (*i)["number"];
        tmp.name = (*i)["name"];
        tmp.pos_x = (*i)["pos_x"];
        tmp.pos_y = (*i)["pos_y"];
        vertices.push_back(tmp);
    }
    j = data["edges"];
    for(auto i = j.begin(); i < j.end(); i++)
    {
        edge tmp;
        tmp.from = (*i)["from"];
        tmp.to = (*i)["to"];
        tmp.congestion = (*i)["congestion"];
        tmp.is_bike_allowed = (*i)["is_bike_allowed"];
        tmp.length = get_length(vertices[tmp.from].pos_x, vertices[tmp.from].pos_y, vertices[tmp.to].pos_x, vertices[tmp.to].pos_y);
        edges.push_back(tmp);
    }
    for(auto i = edges.begin(); i < edges.end(); i++)
    {
        vertices[i->from].adjlist.push_back(*i);
        int tmp = i->from;
        i->from = i->to;
        i->to = tmp;
        vertices[i->from].adjlist.push_back(*i);
    }
    vertices_size = vertices.size();
    list << "";
    for(auto i = vertices.begin(); i < vertices.end(); i++)
        if(i->name != "Crossing")
            list << QString::fromStdString(i->name);
    std::sort(list.begin(), list.end(), string_less);
}

route_info Map::distance_first_dijkstra(int src, int des)
{
    priority_queue<dij_tmp, vector<dij_tmp>, distance_first_cmp> q;
    vector<int> dis(vertices.size(), INT32_MAX);
    vector<double> time(vertices.size(), INT32_MAX);
    vector<edge*> _path(vertices.size(), NULL);
    vector<edge*> route;
    dis[src] = 0;
    time[src] = 0;
    for(auto i = vertices[src].adjlist.begin(); i < vertices[src].adjlist.end(); i++)
        q.push({&*i, i->length, i->length / i->congestion / walk_speed});
    while(dis[des] == INT32_MAX)
    {
        dij_tmp tmp = q.top();
        q.pop();
        if(dis[tmp.edge_ptr->to] != INT32_MAX)
            continue;
        dis[tmp.edge_ptr->to] = tmp.total_dis;
        time[tmp.edge_ptr->to] = tmp.total_time;
        _path[tmp.edge_ptr->to] = tmp.edge_ptr;
        for(auto i = vertices[tmp.edge_ptr->to].adjlist.begin(); i < vertices[tmp.edge_ptr->to].adjlist.end(); i++)
            if(dis[i->to] == INT32_MAX)
                q.push({&*i, i->length + dis[i->from], i->length / i->congestion / walk_speed + time[i->from]});
    }
    for(int i = des; i != src; i = _path[i]->from)
        route.push_back(_path[i]);
    reverse(route.begin(), route.end());
    route_info ret;
    ret.distance = dis[des];
    ret.time = time[des];
    ret.on = this;
    ret.edges = route;
    return ret;
}

route_info Map::time_first_dijkstra(int src, int des)
{
    priority_queue<dij_tmp, vector<dij_tmp>, time_first_cmp> q;
    vector<int> dis(vertices.size(), INT32_MAX);
    vector<double> time(vertices.size(), INT32_MAX);
    vector<edge*> _path(vertices.size(), NULL);
    vector<edge*> route;
    dis[src] = 0;
    time[src] = 0;
    for(auto i = vertices[src].adjlist.begin(); i < vertices[src].adjlist.end(); i++)
        q.push({&*i, i->length, i->length / i->congestion / walk_speed});
    while(dis[des] == INT32_MAX)
    {
        dij_tmp tmp = q.top();
        q.pop();
        if(dis[tmp.edge_ptr->to] != INT32_MAX)
            continue;
        dis[tmp.edge_ptr->to] = tmp.total_dis;
        time[tmp.edge_ptr->to] = tmp.total_time;
        _path[tmp.edge_ptr->to] = tmp.edge_ptr;
        for(auto i = vertices[tmp.edge_ptr->to].adjlist.begin(); i < vertices[tmp.edge_ptr->to].adjlist.end(); i++)
            if(dis[i->to] == INT32_MAX)
                q.push({&*i, i->length + dis[i->from], i->length / i->congestion / walk_speed + time[i->from]});
    }
    for(int i = des; i != src; i = _path[i]->from)
        route.push_back(_path[i]);
    reverse(route.begin(), route.end());
    route_info ret;
    ret.distance = dis[des];
    ret.time = time[des];
    ret.on = this;
    ret.edges = route;
    return ret;
}

void Map::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPixmap pix;
    pix.load(QString::fromStdString(filename + ".png"));
    painter.drawPixmap(0,0,pix);
    QPen pen;
    pen.setWidth(1);
    pen.setColor(QColor(0,0,0));
    painter.setPen(pen);
    QFont font = painter.font();
    font.setBold(true);
    painter.setFont(font);
    for(auto i = vertices.begin(); i < vertices.end(); i++)
    {
        if(i->name != "Crossing")
        {
            QString text = QString::asprintf("%s", i->name.c_str());
            QFontMetrics fm(font);
            QRectF rec = fm.boundingRect(text);
            QPainterPath path;
            path.addRoundedRect(QRectF(i->pos_x * my_ratio + my_drift - rec.width() / 2 - 2, i->pos_y * my_ratio + my_drift - rec.height() / 2, rec.width() + 4, rec.height()), 5, 5);
            painter.drawPath(path);
            painter.fillPath(path, QColor(255,255,255,210));
            painter.drawText(i->pos_x * my_ratio + my_drift - rec.width() / 2, i->pos_y * my_ratio + my_drift + rec.height() / 2 - 3, text);
        }
    }
}
