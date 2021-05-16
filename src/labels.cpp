#include "labels.h"

RouteLabel::RouteLabel(multi_routes *tmp, QString string, strat s, QWidget *parent) : QLabel(parent)
{
    text = string;
    routes = tmp;
    strategy = s;
    hover = false;
    setStyleSheet("border:1px solid black;");
    hide();
}

void RouteLabel::enterEvent(QEnterEvent *ev)
{
    if (!hover)
    {
        hover = true;
        setStyleSheet("border:2px solid black;");
        emit hover_in(routes);
    }
    QLabel::enterEvent(ev);
};

void RouteLabel::leaveEvent(QEvent *ev)
{
    if (hover)
    {
        hover = false;
        setStyleSheet("border:1px solid black;");
        emit hover_out(routes);
    }
    QLabel::leaveEvent(ev);
};

void RouteLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev)
    emit clicked(routes, strategy);
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

AnimationLabel::AnimationLabel(QWidget *parent) : QLabel(parent)
{
    resize(1080, 672);

    QLabel *background = new QLabel(this);
    QPixmap background_pic(1080, 300);
    background_pic.fill(QColor("White"));
    background->setPixmap(background_pic);
    background->move(0, 150);

    QLabel *bus = new QLabel(this);
    QPixmap bus_pic;
    bus_pic.load("../data/bus.png");
    bus->setPixmap(bus_pic);
    bus->adjustSize();
    bus->move(QPoint(-400, 150));
    bus_animation = new QPropertyAnimation(bus, "pos");
    bus_animation->setDuration(1500);
    bus_animation->setStartValue(QPoint(-400, 150));
    bus_animation->setEndValue(QPoint(1100, 150));

    QLabel *subway = new QLabel(this);
    QPixmap subway_pic;
    subway_pic.load("../data/subway.png");
    subway->setPixmap(subway_pic);
    subway->adjustSize();
    subway->move(QPoint(-2400, 250));
    subway_animation = new QPropertyAnimation(subway, "pos");
    subway_animation->setDuration(1500);
    subway_animation->setStartValue(QPoint(-2400, 250));
    subway_animation->setEndValue(QPoint(2000, 250));
    hide();
}

void AnimationLabel::play(transport by)
{
    raise();
    show();
    update();
    if (by == by_bus)
        bus_animation->start();
    else if (by == by_subway)
        subway_animation->start();
    QTest::qWait(1500);
    hide();
}