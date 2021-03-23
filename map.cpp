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
    //map
    QPixmap pix;
    pix.load(QString::fromStdString(filename + ".png"));
    painter.drawPixmap(0,0,pix);
    QPen pen;
    QString text;
    //routes
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
    //name
    painter.setPen(pen);
    QFont font = painter.font();
    font.setBold(true);
    QFontMetrics fm(font);
    painter.setFont(font);
    for(auto i = vertices.begin(); i < vertices.end(); i++)
    {
        if(i->name != "Crossing")
        {
            text = text.asprintf("%s", i->name.c_str());
            QRectF rec = fm.boundingRect(text);
            painter.fillRect(QRectF(i->pos_x * my_ratio + my_drift - rec.width() / 2, i->pos_y * my_ratio + my_drift - rec.height() / 2, rec.width(), rec.height()), Qt::white);
            painter.drawText(i->pos_x * my_ratio + my_drift - rec.width() / 2, i->pos_y * my_ratio + my_drift + rec.height() / 2 - 3, text);
        }
    }
//    //edges
//    for(auto i = edges.begin(); i < edges.end(); i++)
//        painter.drawLine(QPointF(vertices[i->from].pos_x*my_ratio+my_drift,
//                         vertices[i->from].pos_y*my_ratio+my_drift),
//                QPointF(vertices[i->to].pos_x*my_ratio+my_drift,
//                vertices[i->to].pos_y*my_ratio+my_drift));
//    //vertices
//    pen.setWidth(10);
//    pen.setColor(QColor(0,0,0));
//    painter.setPen(pen);
//    for(auto i = vertices.begin(); i < vertices.end(); i++)
//        painter.drawEllipse(i->pos_x*my_ratio+my_drift-4,i->pos_y*my_ratio+my_drift-4,10,10);
//    //numbers
//    pen.setColor(QColor(255,0,0));
//    pen.setWidth(2);
//    painter.setPen(pen);
//    for(auto i = main_campus_vertices.begin(); i < main_campus_vertices.end(); i++)
//    {
//        text = text.asprintf("%d", i->number);
//        painter.drawText(i->pos_x*my_ratio+my_drift-6,i->pos_y*my_ratio+my_drift+2,text);
//    }
//    //distants
//    pen.setColor(QColor(0,255,0));
//    painter.setPen(pen);
//    for(auto i = main_campus_edges.begin(); i < main_campus_edges.end(); i++)
//    {
//        text = text.asprintf("%d", i->length);
//        painter.drawText((main_campus_vertices[i->from].pos_x + main_campus_vertices[i->to].pos_x)/2*my_ratio+my_drift,
//                         (main_campus_vertices[i->from].pos_y + main_campus_vertices[i->to].pos_y)/2*my_ratio+my_drift, text);
//    }
}
