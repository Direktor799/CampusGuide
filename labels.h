#ifndef LABELS_H
#define LABELS_H

#include <QLabel>
#include "functions.h"
class RouteLabel : public QLabel
{
    Q_OBJECT
public:
    route_info *route;
    bool hover;

    RouteLabel(route_info *tmp, QWidget *parent = nullptr);

protected:
    virtual void mouseReleaseEvent(QMouseEvent *ev);
    virtual void enterEvent(QEnterEvent *ev);
    virtual void leaveEvent(QEvent *ev);
signals:
    void clicked(route_info* route);
    void hover_in(route_info* route);
    void hover_out(route_info* route);
};

class BuildingLabel : public QLabel
{
    Q_OBJECT
public:
    string name;
    int pos_x;
    int pos_y;
    bool hover;

    BuildingLabel(string s, int x, int y, QWidget *parent = nullptr);
    void choose();
    void unchoose();
protected:
    virtual void mouseReleaseEvent(QMouseEvent *ev);
    virtual void enterEvent(QEnterEvent *ev);
    virtual void leaveEvent(QEvent *ev);
signals:
    void clicked(QString text);
    void hover_in();
    void hover_out();
};
#endif // LABELS_H