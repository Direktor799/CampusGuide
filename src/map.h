#ifndef MAP_H
#define MAP_H

#include <QWidget>
#include <QPainter>
#include <queue>
#include <fstream>
#include "json.hpp"
#include "functions.h"
#include "labels.h"
using json = nlohmann::json;

class Map : public QWidget
{
    Q_OBJECT
public:
    QString filename;
    QString name;
    int vertices_size;
    QVector<vertex> vertices;
    QVector<edge> edges;
    QStringList list;
    QVector<BuildingLabel *> bllist;
    Map(QString fn, QString n, QWidget *parent = nullptr);
    route_info distance_first_dijkstra(int src, int des);
    route_info time_first_dijkstra(int src, int des);
    route_info bike_allowed_dijkstra(int src, int des);
<<<<<<< HEAD
    QVector<route_info> cross_campus(bool flag);
    QDateTime *map_time_ptr;
=======
>>>>>>> parent of fd42980 (cross_campus_bus)

protected:
    void paintEvent(QPaintEvent *);

signals:
};

struct route_info
{
    int distance;
    double time;
    bool is_riding = false;
    Map *on = nullptr;
    edge *now = nullptr;
    QVector<edge *> edges;

    bool operator < (route_info x)
    {
        if(distance < x.distance)
            return true;
        else
            return false;
    };
};

#endif // MAP_H