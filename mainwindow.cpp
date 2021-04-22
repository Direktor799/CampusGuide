#include "mainwindow.h"
Player *me;
Map *main_campus, *shahe_campus, *now_show;
QPushButton *map_switch_btn, *move_cancel_btn, *add_btn, *delete_btn, *route_calcu_btn;
QDateTime *vtime;
QLabel *time_display;
RouteLabel *distance_first_display, *time_first_display, *bike_allowed_display;
DesWidget *deswidget;

void MainWindow::move_cancel()
{
    me->now_routes.canceled = true;
    route_calcu();
}

void MainWindow::move_switch(multi_routes *routes)
{
    me->now_routes = *routes;
    distance_first_display->enable = false;
    time_first_display->enable = false;
    bike_allowed_display->enable = false;
    move_cancel_btn->show();
    me->move();
    deswidget->clear();
    route_calcu();
    move_cancel_btn->hide(); 
    distance_first_display->enable = true;
    time_first_display->enable = true;
    bike_allowed_display->enable = true;
}

void MainWindow::route_calcu()
{
    bool is_valid = false;
    vector<pair<Map*, int> > des;
    for (auto i = deswidget->deslist.begin(); i < deswidget->deslist.end(); i++)
        for (auto j = (*i)->map->vertices.begin(); j < (*i)->map->vertices.end(); j++)
            if (j->name == (*i)->combobox->currentText())
            {
                is_valid = true;
                des.push_back(make_pair((*i)->map, j - (*i)->map->vertices.begin()));
                break;
            }
    me->navigation(des);
    if (is_valid)
    {
        distance_first_display->display();
        time_first_display->display();
        bike_allowed_display->display();
    }
    else
    {
        distance_first_display->hide();
        time_first_display->hide();
        bike_allowed_display->hide();
    }
}

void MainWindow::map_switch()
{
    if (!main_campus->isHidden())
    {
        main_campus->hide();
        for (auto i = main_campus->bllist.begin(); i < main_campus->bllist.end(); i++)
            (*i)->hide();
        now_show = shahe_campus;
        shahe_campus->show();
        for (auto i = shahe_campus->bllist.begin(); i < shahe_campus->bllist.end(); i++)
            (*i)->show();
        map_switch_btn->setText("切换至本部地图");
    }
    else
    {
        shahe_campus->hide();
        for (auto i = shahe_campus->bllist.begin(); i < shahe_campus->bllist.end(); i++)
            (*i)->hide();
        now_show = main_campus;
        main_campus->show();
        for (auto i = main_campus->bllist.begin(); i < main_campus->bllist.end(); i++)
            (*i)->show();
        map_switch_btn->setText("切换至沙河地图");
    }
    if (me->now_on->isHidden())
        me->hide();
    else
        me->show();
}

void MainWindow::timer_update()
{
    *vtime = vtime->addSecs(1);
    time_display->setText(vtime->toString("yyyy-MM-dd hh:mm:ss ddd"));
    time_display->update();
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("CampusGuide");
    setFixedSize(1600, 672);

    deswidget = new DesWidget(this);

    main_campus = new Map("main_campus", this);
    for (auto i = main_campus->vertices.begin(); i < main_campus->vertices.end(); i++)
    {
        if (i->name != "Crossing")
        {
            BuildingLabel *bl = new BuildingLabel(i->name, i->pos_x, i->pos_y, this);
            connect(bl, &BuildingLabel::clicked, deswidget, &DesWidget::setComboBox);
            main_campus->bllist.push_back(bl);
        }
    }
    now_show = main_campus;

    shahe_campus = new Map("shahe_campus", this);
    for (auto i = shahe_campus->vertices.begin(); i < shahe_campus->vertices.end(); i++)
    {
        if (i->name != "Crossing")
        {
            BuildingLabel *bl = new BuildingLabel(i->name, i->pos_x, i->pos_y, this);
            connect(bl, &BuildingLabel::clicked, deswidget, &DesWidget::setComboBox);
            shahe_campus->bllist.push_back(bl);
        }
    }
    shahe_campus->hide();
    for (auto i = shahe_campus->bllist.begin(); i < shahe_campus->bllist.end(); i++)
        (*i)->hide();

    me = new Player(this);
    me->now_on = main_campus;
    main_campus->stackUnder(me);
    shahe_campus->stackUnder(me);

    map_switch_btn = new QPushButton("切换至沙河地图", this);
    map_switch_btn->move(1300, 0);
    connect(map_switch_btn, &QPushButton::clicked, this, &MainWindow::map_switch);

    move_cancel_btn = new QPushButton("停止导航", this);
    move_cancel_btn->move(1200, 150);
    move_cancel_btn->hide();
    connect(move_cancel_btn, &QPushButton::clicked, this, &MainWindow::move_cancel);

    route_calcu_btn = new QPushButton("计算路线", this);
    route_calcu_btn->move(1300, 100);
    connect(route_calcu_btn, &QPushButton::clicked, this, &MainWindow::route_calcu);

    distance_first_display = new RouteLabel(&me->distance_first, "最短距离", this);
    distance_first_display->move(1100, 120);
    connect(distance_first_display, &RouteLabel::hover_in, me, &Player::show_route);
    connect(distance_first_display, &RouteLabel::hover_out, me, &Player::hide_route);
    connect(distance_first_display, &RouteLabel::clicked, this, &MainWindow::move_switch);

    time_first_display = new RouteLabel(&me->time_first, "最短时间", this);
    time_first_display->move(1100, 160);
    connect(time_first_display, &RouteLabel::hover_in, me, &Player::show_route);
    connect(time_first_display, &RouteLabel::hover_out, me, &Player::hide_route);
    connect(time_first_display, &RouteLabel::clicked, this, &MainWindow::move_switch);

    bike_allowed_display = new RouteLabel(&me->bike_allowed, "骑车最短时间", this);
    bike_allowed_display->move(1100, 200);
    connect(bike_allowed_display, &RouteLabel::hover_in, me, &Player::show_route);
    connect(bike_allowed_display, &RouteLabel::hover_out, me, &Player::hide_route);
    connect(bike_allowed_display, &RouteLabel::clicked, this, &MainWindow::move_switch);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::timer_update);
    timer->start(166);

    vtime = new QDateTime;
    *vtime = QDateTime::currentDateTime();

    time_display = new QLabel(this);
    time_display->move(1100, 650);
    QFont font;
    font.setBold(true);
    time_display->setFont(font);
    time_display->setText("当前时间：" + vtime->toString("yyyy-MM-dd hh:mm:ss ddd"));
    time_display->adjustSize();
}

MainWindow::~MainWindow()
{
    delete ui;
}

DesComboBox::DesComboBox(Map* m, QWidget *parent) : QWidget(parent)
{
    map = m;
    combobox = new QComboBox(this);
    label = new QLabel(this);
    combobox->addItems(map->list);
    if(m->filename == "main_campus")
        label->setText("本部");
    else
        label->setText("沙河");
    combobox->move(label->pos() + QPoint(30, 0));
    label->move(label->pos() + QPoint(0, 3));
    combobox->setFixedSize(100, 25);
    resize(130, 25);
}

void DesWidget::addComboBox()
{
    if(deslist.size() >= 5)
        return;
    QPoint pos;
    if(!deslist.empty())
    {
        pos = deslist.back()->pos() + QPoint(0, 30);
        deslist.back()->setDisabled(true);
    }
    deslist.push_back(new DesComboBox(now_show, this));
    deslist.back()->move(pos);
    deslist.back()->show();
    delete_btn->setEnabled(true);
    if(deslist.size() >= 5)
        add_btn->setDisabled(true);

    add_btn->move(add_btn->pos() + QPoint(0, 30));
    delete_btn->move(delete_btn->pos() + QPoint(0, 30));
}

void DesWidget::deleteComboBox()
{
    delete deslist.back();
    deslist.pop_back();
    if(!deslist.empty())
        deslist.back()->setEnabled(true);
    add_btn->setEnabled(true);
    if(deslist.empty())
        delete_btn->setDisabled(true);

    add_btn->move(add_btn->pos() - QPoint(0, 30));
    delete_btn->move(delete_btn->pos() - QPoint(0, 30));
}

void DesWidget::setComboBox(QString text)
{
    addComboBox();
    deslist.back()->combobox->setCurrentText(text);
}

void DesWidget::clear()
{
    while(!deslist.empty())
        deleteComboBox();
}

DesWidget::DesWidget(QWidget *parent) : QWidget(parent)
{
    move(1100, 0);
    add_btn = new QPushButton("+", this);
    add_btn->move(30, 0);
    add_btn->setFixedSize(50, 30);
    connect(add_btn, &QPushButton::clicked, this, &DesWidget::addComboBox);

    delete_btn = new QPushButton("-", this);
    delete_btn->move(80, 0);
    delete_btn->setFixedSize(50, 30);
    connect(delete_btn, &QPushButton::clicked, this, &DesWidget::deleteComboBox);

    resize(130, 500);
}