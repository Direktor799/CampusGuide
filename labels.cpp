#include "labels.h"

RouteLabel::RouteLabel(route_info *tmp, QWidget *parent) : QLabel(parent)
{
    route = tmp;
    hover = false;
    setStyleSheet("border:1px solid black;");
    hide();
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

BuildingLabel::BuildingLabel(string s, int x, int y, QWidget *parent) : QLabel(parent)
{
    hover = false;
    name = s;
    pos_x = x;
    pos_y = y;
    QString text = QString::asprintf("%s", name.c_str());
    QFont font = this->font();
    font.setBold(true);
    setFont(font);
    QFontMetrics fm(font);
    QRectF rec = fm.boundingRect(text);
    setAutoFillBackground(true);
    setStyleSheet("border-radius: 5px; border:1px solid black; background-color:rgba(255,255,255,150);");
    raise();
    show();
    setFont(font);
    setText(text);
    adjustSize();
    move(pos_x * my_ratio + my_drift - rec.width() / 2 - 5, pos_y * my_ratio + my_drift - rec.height() / 2 - 1);
}

void BuildingLabel::choose()
{
    raise();
    setStyleSheet("border-radius: 5px; border:2px solid black; background-color:rgba(255,255,255,255);");
}

void BuildingLabel::unchoose()
{
    setStyleSheet("border-radius: 5px; border:1px solid black; background-color:rgba(255,255,255,150);");
}

void BuildingLabel::enterEvent(QEnterEvent *ev)
{
    if (!hover)
    {
        hover = true;
        emit hover_in();
    }
    QLabel::enterEvent(ev);
};

void BuildingLabel::leaveEvent(QEvent *ev)
{
    if (hover)
    {
        hover = false;
        emit hover_out();
    }
    QLabel::leaveEvent(ev);
};

void BuildingLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev)
    emit clicked(QString::asprintf("%s", name.c_str()));
}