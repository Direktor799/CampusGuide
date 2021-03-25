#ifndef ROUTE_H
#define ROUTE_H

#include <QLabel>
#include "map.h"

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
    void clicked();
    void hover_in(route_info* route);
    void hover_out(route_info* route);
};

#endif // ROUTE_H