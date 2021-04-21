#include "mainwindow.h"
Player *me;
Map *main_campus, *shahe_campus, *now_show;
QPushButton *map_switch_btn, *move_cancel_btn, *add_btn, *delete_btn;
QDateTime *vtime;
QLabel *time_display;
RouteLabel *distance_first_display, *time_first_display, *bike_allowed_display;
vector<QComboBox*> comboboxs;

void MainWindow::add_combobox()
{
    QPoint pos = comboboxs.back()->pos() + QPoint(0, 30);
    comboboxs.push_back(new QComboBox(this));
    comboboxs.back()->move(pos);
    comboboxs.back()->setFixedSize(100, 30);
    comboboxs.back()->addItems(now_show->list);
    comboboxs.back()->show();
    add_btn->move(add_btn->pos() + QPoint(0, 30));
    delete_btn->move(delete_btn->pos() + QPoint(0, 30));
    delete_btn->setEnabled(true);
    if(comboboxs.size() == 5)
        add_btn->setEnabled(false);
}

void MainWindow::delete_combobox()
{
    delete comboboxs.back();
    comboboxs.pop_back();
    add_btn->move(add_btn->pos() - QPoint(0, 30));
    delete_btn->move(delete_btn->pos() - QPoint(0, 30));
    add_btn->setEnabled(true);
    if(comboboxs.size() == 1)
        delete_btn->setEnabled(false);
}

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
    for (auto i = comboboxs.begin(); i < comboboxs.end(); i++)
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

    main_campus = new Map("main_campus", this);
    now_show = main_campus;
    shahe_campus = new Map("shahe_campus", this);
    shahe_campus->hide();
    for (auto i = shahe_campus->bllist.begin(); i < shahe_campus->bllist.end(); i++)
        (*i)->hide();
    me = new Player(this);
    me->now_on = main_campus;

    main_campus->stackUnder(me);
    shahe_campus->stackUnder(me);

    for (auto i = shahe_campus->bllist.begin(); i < shahe_campus->bllist.end(); i++)
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

    for (auto i = main_campus->vertices.begin(); i < main_campus->vertices.end(); i++)
    {
        if (i->name != "Crossing")
        {
            BuildingLabel *bl = new BuildingLabel(i->name, i->pos_x, i->pos_y, this);
            connect(bl, BuildingLabel::hover_in, bl, BuildingLabel::choose);
            connect(bl, BuildingLabel::hover_out, bl, BuildingLabel::unchoose);
            main_campus->bllist.push_back(bl);
        }
    }

    comboboxs.push_back(new QComboBox(this));
    comboboxs.back()->move(1100, 0);
    comboboxs.back()->setFixedSize(100, 30);
    comboboxs.back()->addItems(main_campus->list);
    for(auto i = main_campus->bllist.begin(); i < main_campus->bllist.end(); i++)
        connect(*i, &BuildingLabel::clicked, comboboxs.back(), &QComboBox::setCurrentText);
    //connect((*i), &QComboBox::currentTextChanged, this, &MainWindow::route_calcu);

    add_btn = new QPushButton("+", parent);
    add_btn->move(1100, 30);
    add_btn->setFixedSize(50, 30);
    connect(add_btn, &QPushButton::clicked, this, &MainWindow::add_combobox);

    delete_btn = new QPushButton("-", parent);
    delete_btn->move(1150, 30);
    delete_btn->setFixedSize(50, 30);
    delete_btn->setEnabled(false);
    connect(delete_btn, &QPushButton::clicked, this, &MainWindow::delete_combobox);
}

MainWindow::~MainWindow()
{
    delete ui;
}
