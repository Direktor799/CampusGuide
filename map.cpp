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
        tmp.number = (*i)["number"];
        tmp.from = (*i)["from"];
        tmp.to = (*i)["to"];
        tmp.congestion = (*i)["congestion"];
        tmp.is_bike_allowed = (*i)["is_bike_allowed"];
        tmp.length = abs(vertices[tmp.from].pos_x - vertices[tmp.to].pos_x) + abs(vertices[tmp.from].pos_y - vertices[tmp.to].pos_y);
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
    for(auto i = vertices.begin(); i < vertices.end(); i++)
        if(i->name != "Crossing")
            list << QString::fromStdString(i->name);
    std::sort(list.begin(), list.end(), string_less);
}

pair<int, vector<int> > Map::dijkstra_dis(int src, int des)
{
    priority_queue<edge, vector<edge>, pq_cmp> q;
    vector<int> dis(vertices.size(), INT32_MAX);
    vector<int> _path(vertices.size(), -1);
    vector<int> route;
    dis[src] = 0;
    for(auto i = vertices[src].adjlist.begin(); i < vertices[src].adjlist.end(); i++)
        q.push(*i);
    while(dis[des] == INT32_MAX)
    {
        auto tmp = q.top();
        q.pop();
        if(dis[tmp.to] != INT32_MAX)
            continue;
        dis[tmp.to] = tmp.length;
        _path[tmp.to] = tmp.number;
        for(auto i = vertices[tmp.to].adjlist.begin(); i < vertices[tmp.to].adjlist.end(); i++)
            if(dis[i->to] == INT32_MAX)
            {
                auto dij = *i;
                dij.length += dis[dij.from];
                q.push(dij);
            }
    }
    for(auto i = des; i != src; i = edges[_path[i]].from + edges[_path[i]].to - i)
        route.push_back(_path[i]);
    reverse(route.begin(), route.end());
    return make_pair(dis[des], route);
}

void Map::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPixmap pix;
    pix.load(QString::fromStdString(filename + ".png"));
    painter.drawPixmap(0,0,pix);
    QPen pen;
    QString text;

    pen.setWidth(5);
    pen.setColor(QColor(255,0,0));
    painter.setPen(pen);
    for(auto i = passed; i < route.second.end(); i++)
        painter.drawLine(QPointF(vertices[edges[*i].from].pos_x*my_ratio+my_drift,
                vertices[edges[*i].from].pos_y*my_ratio+my_drift),
                QPointF(vertices[edges[*i].to].pos_x*my_ratio+my_drift,
                vertices[edges[*i].to].pos_y*my_ratio+my_drift));
    pen.setColor(QColor(25,25,25));
    painter.setPen(pen);
    for(auto i = route.second.begin(); i < passed; i++)
        painter.drawLine(QPointF(vertices[edges[*i].from].pos_x*my_ratio+my_drift,
                vertices[edges[*i].from].pos_y*my_ratio+my_drift),
                QPointF(vertices[edges[*i].to].pos_x*my_ratio+my_drift,
                vertices[edges[*i].to].pos_y*my_ratio+my_drift));

    pen.setWidth(1);
    pen.setColor(QColor(0,0,0));
    painter.setPen(pen);
    QFont font = painter.font();
    font.setBold(true);
    painter.setFont(font);
    QFontMetrics fm(font);
    for(auto i = vertices.begin(); i < vertices.end(); i++)
    {
        if(i->name != "Crossing")
        {
            text = text.asprintf("%s", i->name.c_str());
            QRectF rec = fm.boundingRect(text);
            QPainterPath path;
            path.addRoundedRect(QRectF(i->pos_x * my_ratio + my_drift - rec.width() / 2 - 2, i->pos_y * my_ratio + my_drift - rec.height() / 2, rec.width() + 4, rec.height()), 5, 5);
            painter.drawPath(path);
            painter.fillPath(path, QColor(255,255,255,210));
            painter.drawText(i->pos_x * my_ratio + my_drift - rec.width() / 2, i->pos_y * my_ratio + my_drift + rec.height() / 2 - 3, text);
        }
    }
}
