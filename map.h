#ifndef MAP_H
#define MAP_H

#include <QWidget>
#include <QPainter>
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
    bool operator()(edge a, edge b)
    {
        if(a.length > b.length)
            return true;
        if(a.length == b.length && a.number > b.number)
            return true;
        return false;
    }
};

class Map : public QWidget
{
    Q_OBJECT
public:
    string filename;
    vector<vertex> vertices;
    vector<edge> edges;
    pair<int, vector<int> > route;
    vector<int>::iterator passed;
    QStringList list;

    Map(string s, QWidget *parent = nullptr);
    pair<int, vector<int> > dijkstra_dis(int src, int des);
protected:
    void paintEvent(QPaintEvent *);

signals:

};
#endif // MAP_H
