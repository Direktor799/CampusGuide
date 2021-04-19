#include "mainwindow.h"
Player *me;
Map *main_campus, *shahe_campus;
QPushButton *map_switch_btn, *move_cancel_btn;
QDateTime *vtime;
QLabel *time_display;
RouteLabel *distance_first_display, *time_first_display, *bike_allowed_display;

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
    route_calcu();
    move_cancel_btn->hide();
    distance_first_display->enable = true;
    time_first_display->enable = true;
    bike_allowed_display->enable = true;
}

void MainWindow::route_calcu()
{
    bool is_valid = false;
    vector<int> des;
    for (auto i = me->now_on->comboboxs.begin(); i < me->now_on->comboboxs.end(); i++)
        for (auto j = me->now_on->vertices.begin(); j < me->now_on->vertices.end(); j++)
            if (j->name == (*i)->currentText())
            {
                is_valid = true;
                des.push_back(j - me->now_on->vertices.begin());
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
        for (auto i = main_campus->comboboxs.begin(); i < main_campus->comboboxs.end(); i++)
            (*i)->hide();
        for (auto i = main_campus->bllist.begin(); i < main_campus->bllist.end(); i++)
            (*i)->hide();
        main_campus->add_btn->hide();
        main_campus->delete_btn->hide();

        shahe_campus->show();
        for (auto i = shahe_campus->comboboxs.begin(); i < shahe_campus->comboboxs.end(); i++)
            (*i)->show();
        for (auto i = shahe_campus->bllist.begin(); i < shahe_campus->bllist.end(); i++)
            (*i)->show();
        shahe_campus->add_btn->show();
        shahe_campus->delete_btn->show();
        map_switch_btn->setText("切换至本部地图");
    }
    else
    {
        shahe_campus->hide();
        for (auto i = shahe_campus->comboboxs.begin(); i < shahe_campus->comboboxs.end(); i++)
            (*i)->hide();
        for (auto i = shahe_campus->bllist.begin(); i < shahe_campus->bllist.end(); i++)
            (*i)->hide();
        shahe_campus->add_btn->hide();
        shahe_campus->delete_btn->hide();
        
        main_campus->show();
        for (auto i = main_campus->comboboxs.begin(); i < main_campus->comboboxs.end(); i++)
            (*i)->show();
        for (auto i = main_campus->bllist.begin(); i < main_campus->bllist.end(); i++)
            (*i)->show();
        main_campus->add_btn->show();
        main_campus->delete_btn->show();
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

    main_campus = new Map("main_campus", this);
    shahe_campus = new Map("shahe_campus", this);
    shahe_campus->hide();
    for (auto i = shahe_campus->bllist.begin(); i < shahe_campus->bllist.end(); i++)
        (*i)->hide();
    shahe_campus->add_btn->hide();
    shahe_campus->delete_btn->hide();
    for (auto i = main_campus->comboboxs.begin(); i < main_campus->comboboxs.end(); i++)
        connect((*i), &QComboBox::currentTextChanged, this, &MainWindow::route_calcu);
    for (auto i = shahe_campus->comboboxs.begin(); i < shahe_campus->comboboxs.end(); i++)
        connect((*i), &QComboBox::currentTextChanged, this, &MainWindow::route_calcu);
    me = new Player(this);
    me->now_on = main_campus;

    main_campus->stackUnder(me);
    shahe_campus->stackUnder(me);

    for (auto i = shahe_campus->bllist.begin(); i < shahe_campus->bllist.end(); i++)
        (*i)->hide();
    for (auto i = shahe_campus->comboboxs.begin(); i < shahe_campus->comboboxs.end(); i++)
        (*i)->hide();

    map_switch_btn = new QPushButton("切换至沙河地图", this);
    map_switch_btn->move(1300, 0);
    connect(map_switch_btn, &QPushButton::clicked, this, &MainWindow::map_switch);

    move_cancel_btn = new QPushButton("停止导航", this);
    move_cancel_btn->move(1200, 150);
    move_cancel_btn->hide();
    connect(move_cancel_btn, &QPushButton::clicked, this, &MainWindow::move_cancel);

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
