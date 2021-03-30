#ifndef MAP_H
#define MAP_H

#include <QWidget>
#include <QPainter>
#include <fstream>
#include <queue>
#include "json.hpp"
#include "functions.h"
#include "labels.h"
using json = nlohmann::json;

class Map : public QWidget
{
    Q_OBJECT
public:
    QString filename;
    int vertices_size;
    vector<vertex> vertices;
    vector<edge> edges;
    QStringList list;
    vector<BuildingLabel *> bllist;
    Map(QString s, QWidget *parent = nullptr);
    route_info distance_first_dijkstra(int src, int des);
    route_info time_first_dijkstra(int src, int des);
    route_info bike_allowed_dijkstra(int src, int des);

protected:
    void paintEvent(QPaintEvent *);

signals:
};

struct route_info
{
    int distance;
    double time;
    bool is_riding = false;
    bool visable = false;
    bool canceled = false;
    Map *on;
    edge *now = nullptr;
    vector<edge *> edges;
};

#endif // MAP_H