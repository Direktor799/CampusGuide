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

const int walk_speed = 60;
const int ride_multiplier = 3;
const int my_ratio = 24;
const int my_drift = 12;
const int map_ratio = 18;

struct edge
{
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

struct dij_tmp
{
    edge* edge_ptr;
    int total_dis;
    double total_time;
};

class distance_first_cmp
{
public:
    bool operator()(dij_tmp a, dij_tmp b)
    {
        if(a.total_dis > b.total_dis)
            return true;
        if(a.total_dis == b.total_dis && a.total_time > b.total_time)
            return true;
        if(a.total_dis == b.total_dis && a.total_time == b.total_time&& a.edge_ptr->from > b.edge_ptr->from)
            return true;
        if(a.total_dis == b.total_dis && a.total_time == b.total_time&& a.edge_ptr->from == b.edge_ptr->from && a.edge_ptr->to > b.edge_ptr->to)
            return true;
        return false;
    }
};

class time_first_cmp
{
public:
    bool operator()(dij_tmp a, dij_tmp b)
    {
        if(a.total_time > b.total_time)
            return true;
        if(a.total_time == b.total_time && a.edge_ptr->from > b.edge_ptr->from)
            return true;
        if(a.total_time == b.total_time && a.edge_ptr->from == b.edge_ptr->from && a.edge_ptr->to > b.edge_ptr->to)
            return true;
        return false;
    }
};

class Map;

struct route_info
{
    int distance;
    double time;
    bool is_riding = false;
    bool visable = false;
    bool canceled = false;
    Map* on;
    edge* now = nullptr;
    vector<edge*> edges;
};

class Map : public QWidget
{
    Q_OBJECT
public:
    string filename;
    int vertices_size;
    vector<vertex> vertices;
    vector<edge> edges;
    QStringList list;
    Map(string s, QWidget *parent = nullptr);
    route_info distance_first_dijkstra(int src, int des);
    route_info time_first_dijkstra(int src, int des);
    route_info bike_allowed_dijkstra(int src, int des);
protected:
    void paintEvent(QPaintEvent *);

signals:

};
#endif // MAP_H
