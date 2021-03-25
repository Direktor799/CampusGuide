#ifndef MAP_H
#define MAP_H

#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <vector>
#include <string>
#include <fstream>
#include <queue>
#include "json.hpp"
#include "functions.h"
using namespace std;
using json = nlohmann::json;

const int ideal_speed = 12;
const int my_ratio = 24;
const int my_drift = 12;

struct edge
{
    int number;
    int from;
    int to;
    int length;
    double congestion;
    bool is_bike_allowed;
};

struct vertex
{
    int number;
    string name;
    int pos_x, pos_y;
    vector<edge> adjlist;
};

class pq_cmp
{
public:
    bool operator()(edge* a, edge* b)
    {
        if(a->length > b->length)
            return true;
        if(a->length == b->length && a->number > b->number)
            return true;
        return false;
    }
};

class Map;

struct route_info
{
    int distance;
    double time;
    bool is_riding;
    Map* on;
    vector<edge*> edges;
};

class Map : public QWidget
{
    Q_OBJECT
public:
    string filename;
    vector<vertex> vertices;
    vector<edge> edges;
    QStringList list;
    Map(string s, QWidget *parent = nullptr);
    route_info dijkstra(int src, int des);
protected:
    void paintEvent(QPaintEvent *);

signals:

};
#endif // MAP_H
