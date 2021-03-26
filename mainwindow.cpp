#include "mainwindow.h"
QComboBox* des;
Player* me;
Map* main_campus, *shahe_campus;
QPushButton* map_switch_btn, *route_calcu_btn;
QDateTime* vtime;
QLabel* time_display;
RouteLabel* distance_first_display, *time_first_display;

void MainWindow::route_calcu()
{
    string s = des->currentText().toStdString();
    for(auto i = me->now_on->vertices.begin(); i < me->now_on->vertices.end(); i++)
        if(i->name == s)
        {
            me->navigation(i - me->now_on->vertices.begin());
            break;
        }

    distance_first_display->setText("<div style = 'font-weight:bold;'>最短距离：</div><br>" + QString::number(me->distance_first.distance * map_ratio)  + "米   约" + QString::number(ceil(me->distance_first.time * map_ratio)) + "分钟");
    distance_first_display->adjustSize();
    distance_first_display->show();

    time_first_display->setText("<div style = 'font-weight:bold;'>最短时间：</div><br>" + QString::number(me->time_first.distance * map_ratio)  + "米   约" + QString::number(ceil(me->time_first.time * map_ratio)) + "分钟");
    time_first_display->adjustSize();
    time_first_display->show();
}

void MainWindow::map_switch()
{
    if(!main_campus->isHidden())
    {
        main_campus->hide();
        shahe_campus->show();
        des->clear();
        des->addItems(shahe_campus->list);
        map_switch_btn->setText("切换至本部地图");
    }
    else
    {
        shahe_campus->hide();
        main_campus->show();
        des->clear();
        des->addItems(main_campus->list);
        map_switch_btn->setText("切换至沙河地图");
    }
    if(me->now_on->isHidden())
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

    me = new Player(this);
    me->now_on = main_campus;

    des = new QComboBox(this);
    des->move(1100, 0);
    des->setFixedSize(150, 30);
    des->addItems(main_campus->list);

    map_switch_btn = new QPushButton("切换至沙河地图",this);
    map_switch_btn->move(1300, 0);
    connect(map_switch_btn, &QPushButton::clicked, this, &MainWindow::map_switch);

    route_calcu_btn = new QPushButton("查询路线", this);
    route_calcu_btn->move(1100, 30);
    connect(route_calcu_btn, &QPushButton::clicked, this, &MainWindow::route_calcu);

    distance_first_display = new RouteLabel(&me->distance_first, this);
    distance_first_display->setStyleSheet("border:1px solid black;");
    distance_first_display->move(1100, 200);
    distance_first_display->hide();
    connect(distance_first_display, &RouteLabel::hover_in, me, &Player::show_route);
    connect(distance_first_display, &RouteLabel::hover_out, me, &Player::hide_route);
    connect(distance_first_display, &RouteLabel::clicked, me, &Player::move);

    time_first_display = new RouteLabel(&me->time_first, this);
    time_first_display->setStyleSheet("border:1px solid black;");
    time_first_display->move(1100, 300);
    time_first_display->hide();
    connect(time_first_display, &RouteLabel::hover_in, me, &Player::show_route);
    connect(time_first_display, &RouteLabel::hover_out, me, &Player::hide_route);
    connect(time_first_display, &RouteLabel::clicked, me, &Player::move);

    QTimer* timer = new QTimer(this);
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

