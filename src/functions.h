#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <QString>
#include <QTime>
#include <QApplication>
#include <QPushButton>
#include <QVector>
#include <QTest>

const int walk_speed = 80;
const int ride_multiplier = 3;
const int my_ratio = 24;
const int my_drift = 12;
const int map_ratio = 18;

enum strat
{
    distance_first = 0,
    time_first = 1,
    bike_allowed = 2
};

enum transport
{
    by_subway = 0,
    by_bus = 1
};

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
    QString name;
    int pos_x, pos_y;
    QVector<edge> adjlist;
};

struct dij_tmp
{
    edge *edge_ptr;
    int total_dis;
    double total_time;
};

class distance_first_cmp
{
public:
    bool operator()(dij_tmp a, dij_tmp b)
    {
        if (a.total_dis > b.total_dis)
            return true;
        if (a.total_dis == b.total_dis && a.total_time > b.total_time)
            return true;
        if (a.total_dis == b.total_dis && a.total_time == b.total_time && a.edge_ptr->from > b.edge_ptr->from)
            return true;
        if (a.total_dis == b.total_dis && a.total_time == b.total_time && a.edge_ptr->from == b.edge_ptr->from && a.edge_ptr->to > b.edge_ptr->to)
            return true;
        return false;
    }
};

class time_first_cmp
{
public:
    bool operator()(dij_tmp a, dij_tmp b)
    {
        if (a.total_time > b.total_time)
            return true;
        if (a.total_time == b.total_time && a.edge_ptr->from > b.edge_ptr->from)
            return true;
        if (a.total_time == b.total_time && a.edge_ptr->from == b.edge_ptr->from && a.edge_ptr->to > b.edge_ptr->to)
            return true;
        return false;
    }
};

struct route_info;

struct multi_routes
{
    int distance = 0;
    double time = 0;
    bool visable = false;
    bool canceled = false;
    route_info *now = nullptr;
    QVector<route_info> routes;
};

struct bus_time
{
    int day_of_week;
    QTime start_time;
    QTime arrival_time;
};

bool string_less(const QString &s1, const QString &s2);
void sleep(int msec, int &speedfactor, bool &cancel);
void _tinysleep100ms();
int get_length(int a_pos_x, int a_pos_y, int b_pos_x, int b_pos_y);
int get_distance(int a_pos_x, int a_pos_y, int b_pos_x, int b_pos_y);
#endif // FUNCTIONS_H
