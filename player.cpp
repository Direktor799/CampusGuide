#include "player.h"

Player::Player(QWidget *parent) : QWidget(parent)
{
    number = 0;
    pos_x = 0;
    pos_y = 0;
    resize(1080,672);
};

bool Player::is_on_vertex()
{
    for(auto i = now_on->vertices.begin(); i < now_on->vertices.end(); i++)
        if(i->pos_x == pos_x && i->pos_y == pos_y)
        {
            number = i->number;
            return true;
        }
    return false;
}

void Player::teleport(int des)
{
    int distance = fabs(now_on->vertices[des].pos_x - pos_x) + fabs(now_on->vertices[des].pos_y - pos_y);
    int x_move = 0;
    int y_move = 0;
    if(fabs(now_on->vertices[des].pos_x - pos_x) == 0)
    {
        if(now_on->vertices[des].pos_y - pos_y > 0)
           y_move = 1;
        else
            y_move = -1;

    }
    else
    {
        if(now_on->vertices[des].pos_x - pos_x > 0)
           x_move = 1;
        else
            x_move = -1;
    }
    for(auto i = 0; i < distance; i++)
    {
       pos_x += x_move;
       pos_y += y_move;
       now_on->update();
       sleep(100);
    }
    number = now_on->vertices[des].number;
    pos_x = now_on->vertices[des].pos_x;
    pos_y = now_on->vertices[des].pos_y;
}

void Player::navigation(int des, int strategy)
{
    if(is_on_vertex())
        now_on->route = now_on->dijkstra_dis(number, des);
    else
    {
        pair<int, vector<int> > side1 = now_on->dijkstra_dis(now_on->edges[number].from, des);
        pair<int, vector<int> > side2 = now_on->dijkstra_dis(now_on->edges[number].to, des);
        if(side1.first > side2.first)
            side1 = side2;
        now_on->route = side1;
        now_on->route.second.insert(now_on->route.second.begin(), number);
    }
    move();
}

void Player::move()
{
    for(auto i = now_on->route.second.begin(); i < now_on->route.second.end(); i++)
    {
        now_on->passed = i + 1;
        teleport(now_on->edges[*i].from + now_on->edges[*i].to - number);
    }
    now_on->route.first = 0;
    now_on->route.second.resize(0);
    now_on->passed = now_on->route.second.begin();
}

void Player::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPixmap pix;
    pix.load("me.png");
    painter.drawPixmap(pos_x*my_ratio+my_drift-20,pos_y*my_ratio+my_drift-30,40,40,pix);
};
