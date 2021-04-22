#include "labels.h"

RouteLabel::RouteLabel(multi_routes *tmp, QString string, QWidget *parent) : QLabel(parent)
{
    text = string;
    routes = tmp;
    hover = false;
    enable = true;
    setStyleSheet("border:1px solid black;");
    hide();
}

void RouteLabel::enterEvent(QEnterEvent *ev)
{
    if (!hover)
    {
        hover = true;
        emit hover_in(routes);
    }
    QLabel::enterEvent(ev);
};

void RouteLabel::leaveEvent(QEvent *ev)
{
    if (hover)
    {
        hover = false;
        emit hover_out(routes);
    }
    QLabel::leaveEvent(ev);
};

void RouteLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev)
    if(enable)
        emit clicked(routes);
}

void RouteLabel::display()
{
    setText("<div style = 'font-weight:bold;'>" + text + "</div><br>" + QString::number(routes->distance) + "米   约" + QString::number(ceil(routes->time)) + "分钟");
    adjustSize();
    show();
}

BuildingLabel::BuildingLabel(QString s, int x, int y, QWidget *parent) : QLabel(parent)
{
    hover = false;
    name = s;
    pos_x = x;
    pos_y = y;
    QFont font = this->font();
    font.setBold(true);
    setFont(font);
    QFontMetrics fm(font);
    QRectF rec = fm.boundingRect(name);
    setAutoFillBackground(true);
    setStyleSheet("border-radius: 5px; border:1px solid black; background-color:rgba(255,255,255,150);");
    raise();
    show();
    setFont(font);
    setText(name);
    adjustSize();
    move(pos_x * my_ratio + my_drift - rec.width() / 2 - 5, pos_y * my_ratio + my_drift - rec.height() / 2 - 1);
}

void BuildingLabel::enterEvent(QEnterEvent *ev)
{
    if (!hover)
    {
        hover = true;
        raise();
        setStyleSheet("border-radius: 5px; border:2px solid black; background-color:rgba(255,255,255,255);");
    }
    QLabel::enterEvent(ev);
};

void BuildingLabel::leaveEvent(QEvent *ev)
{
    if (hover)
    {
        hover = false;
        setStyleSheet("border-radius: 5px; border:1px solid black; background-color:rgba(255,255,255,150);");
    }
    QLabel::leaveEvent(ev);
};

void BuildingLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev)
    emit clicked(name);
}