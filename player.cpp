#include "player.h"

Player::Player(QWidget *parent) : QWidget(parent)
{
    pos_number = 0;
    pos_x = 0;
    pos_y = 0;
    resize(1080, 672);
};

bool Player::is_on_vertex()
{
    for (auto i = now_on->vertices.begin(); i < now_on->vertices.end(); i++)
        if (i->pos_x == pos_x && i->pos_y == pos_y)
        {
            pos_number = i->number;
            return true;
        }
    return false;
}

void Player::teleport(int des)
{
    int distance = fabs(now_on->vertices[des].pos_x - pos_x) + fabs(now_on->vertices[des].pos_y - pos_y);
    int x_move = 0;
    int y_move = 0;
    if (fabs(now_on->vertices[des].pos_x - pos_x) == 0)
    {
        if (now_on->vertices[des].pos_y - pos_y > 0)
            y_move = 1;
        else
            y_move = -1;
    }
    else
    {
        if (now_on->vertices[des].pos_x - pos_x > 0)
            x_move = 1;
        else
            x_move = -1;
    }
    for (auto i = 0; i < distance; i++)
    {
        pos_x += x_move;
        pos_y += y_move;
        now_on->update();
        sleep(100);
    }
    pos_number = now_on->vertices[des].number;
    pos_x = now_on->vertices[des].pos_x;
    pos_y = now_on->vertices[des].pos_y;
}

void Player::navigation(int des)
{
    if (is_on_vertex())
        distance_first = now_on->dijkstra(pos_number, des);
    move();
}

void Player::move()
{
    for (auto i = distance_first.edges.begin(); i < distance_first.edges.end(); i++)
        teleport((*i)->to);
}

void Player::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPen pen;
    pen.setWidth(5);
    pen.setColor(QColor(25, 25, 25));
    painter.setPen(pen);
    for (auto i = distance_first.edges.begin(); i < distance_first.edges.end(); i++)
        painter.drawLine(QPointF(now_on->vertices[(*i)->from].pos_x * my_ratio + my_drift,
                                 now_on->vertices[(*i)->from].pos_y * my_ratio + my_drift),
                         QPointF(now_on->vertices[(*i)->to].pos_x * my_ratio + my_drift,
                                 now_on->vertices[(*i)->to].pos_y * my_ratio + my_drift));

    QPixmap pix;
    pix.load("me.png");
    painter.drawPixmap(pos_x * my_ratio + my_drift - 20, pos_y * my_ratio + my_drift - 30, 40, 40, pix);
};
