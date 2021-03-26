#include "route_label.h"

RouteLabel::RouteLabel(route_info *tmp, QWidget *parent) : QLabel(parent)
{
    route = tmp;
    hover = false;
    setAttribute(Qt::WA_Hover, true);
}

void RouteLabel::enterEvent(QEnterEvent *ev)
{
    if (!hover)
    {
        hover = true;
        emit hover_in(route);
    }
    QLabel::enterEvent(ev);
};

void RouteLabel::leaveEvent(QEvent *ev)
{
    if (hover)
    {
        hover = false;
        emit hover_out(route);
    }
    QLabel::leaveEvent(ev);
};

void RouteLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev)
    emit clicked(route);
}