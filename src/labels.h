#ifndef LABELS_H
#define LABELS_H

#include <QLabel>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include "functions.h"
class RouteLabel : public QLabel
{
    Q_OBJECT
public:
    multi_routes *routes;
    QString text;
    bool hover;
    strat strategy;
    RouteLabel(multi_routes *tmp, QString string, strat s, QWidget *parent = nullptr);
    void display();

protected:
    virtual void mouseReleaseEvent(QMouseEvent *ev);
    virtual void enterEvent(QEnterEvent *ev);
    virtual void leaveEvent(QEvent *ev);
signals:
    void clicked(multi_routes *route, strat s);
    void hover_in(multi_routes *route);
    void hover_out(multi_routes *route);
};

class BuildingLabel : public QLabel
{
    Q_OBJECT
public:
    QString name;
    int pos_x;
    int pos_y;
    bool hover;
    BuildingLabel(QString s, int x, int y, QWidget *parent = nullptr);

protected:
    virtual void mouseReleaseEvent(QMouseEvent *ev);
    virtual void enterEvent(QEnterEvent *ev);
    virtual void leaveEvent(QEvent *ev);
signals:
    void clicked(QString text);
};

class AnimationLabel : public QLabel
{
    Q_OBJECT
public:
    QPropertyAnimation *filter_animation;
    QPropertyAnimation *background_animation;
    QPropertyAnimation *bus_animation;
    QPropertyAnimation *subway_animation;
    AnimationLabel(QWidget *parent = nullptr);
    void play(transport by);
};

#endif // LABELS_H